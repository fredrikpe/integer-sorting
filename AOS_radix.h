

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
  Integer Radix LSD sort. Stable and out-of-place.
  ---Parameters---
  
  vector[] - Pointer to the orginal array of integers
  size     - Size of the array to sort
 
 ---List of optimizations implemented---
  1 - Use of powers of 2 for the expoents and bucket size in order to use
      shift and bitwise operations (expoent = 8 in order to sort 1 byte per iteration).
      This works a lot like the American Flag algorithm used to sort strings.
  2 - Small preliminary check of the initial unsorted array to determine
      number of bytes to sort. Special useful in randomly shuffled arrays.
  3 - The indexes of the buckets express the amount of elements of that respective
      index in the original array. There is also a array of pointers so that
      each pointer has the adress in the helper array where the given offset
      should start.
   
  4 - As there are only 4 iterations at max (for a 32 bit integer at least),
      instead of copying the whole helper array to the original at the end of 
      each iteration, the algorithm switches the purpose of these two arrays
      in order to reduce copying overhead (eventually correcting this at the
      end if it stops in a even number of bytes).
  5 - The algorithms is the same even if the original array contains integers
      of different signs. The only thing we have to do at the end is starting
      from the negative numbers instead (in case of different signs). This task
      has a relatively small overhead.
  6 - Neglecting the shift operation when sorting the first byte (equals >> 0).
      
  Adding to the main structural optimizations of the algorithm, there are a
  number of microoptimizations to further improve performance. These include
  the use (and abuse!) of macros, traversing the array with pointers instead of
  indexes, use of standard C functions like memcpy to efficiently copy arrays
  segments, prefix increments rather than sufix (if possible), registers, etc
 */
void int_radix_sort(register int vector[], register const int size) {

    /* Support for variable sized integers without overflow warnings */
    const int MAX_UINT__ = ((((1 << ((sizeof(int) << 3) - 2)) - 1) << 1) + 1);
    const int LAST_EXP__ = (sizeof(int) - 1) << 3;
    
    /* Define std preliminary, constrain and expression to check if all bytes are sorted */
#define PRELIMINARY__ 100
#define CONSTRAIN__(a, b, n) ((n) < (a) ? (a) : ((n) > (b) ? (b) : (n)))
#define MISSING_BITS__ exp < (sizeof(int) << 3) && (max >> exp) > 0
    /* Check for biggest integer in [a, b[ array segment */
#define LOOP_MAX__(a, b)				\
    for(s = &vector[a], k = &vector[b]; s < k; ++s) {	\
	if(*s > max || *s < exp) {			\
	    if(*s > max)  {				\
		max = *s;				\
	    } else{					\
		exp = *s;				\
	    }						\
	}						\
    }
    
    /* b = helper array pointer ; s, k and i = array iterators */
    /* exp = bits sorted, max = maximun range in array         */
    /* point = array of pointers to the helper array           */
    register int *b, *s, *k;
    register int exp = *vector;
    register int max = exp;
    int i, *point[0x100];
    
    /* Set preliminary according to size */
    const int preliminary = (size > PRELIMINARY__) ? PRELIMINARY__ : (size >> 3);
    
    /* If we found a integer with more than 24 bits in preliminar, */
    /* will have to sort all bytes either way, so max = MAX_UINT__ */
    LOOP_MAX__(1, preliminary);
    if(CONSTRAIN__(0, MAX_UINT__, (unsigned int)(max - exp)) <= (MAX_UINT__ >> 7)) {	
	LOOP_MAX__(preliminary, size);
    }
    max = CONSTRAIN__(0, MAX_UINT__, (unsigned int)(max - exp));
    exp = 0;
    
    /* Helper array initialization */
    b = (int *)malloc(sizeof(int) * size);
    
    /* Core algorithm: for a specific byte, fill the buckets array, */
    /* rearrange the array and reset the initial array accordingly. */
#define BYTE_IS_ODD__ ((exp >> 3) & 1)
#define SORT_BYTE__(vec, bb, shift)					\
    int bucket[0x100] = {0};						\
    register unsigned char *n, *m = (unsigned char *)(&vec[size]);	\
    for(n = (unsigned char *)(vec) + (exp >> 3);			\
	n < m; n += sizeof(int)) {					\
	++bucket[*n];							\
    }									\
    s = bb;								\
    if(exp == LAST_EXP__) {						\
	for(i = 128; i < 0x100; s += bucket[i++]) {			\
	    point[i] = s;						\
	}								\
	for(i = 0; i < 128; s += bucket[i++]) {				\
	    point[i] = s;						\
	}								\
    } else {								\
	for(i = 0; i < 0x100; s += bucket[i++]) {			\
	    point[i] = s;						\
	}								\
    }									\
    for(s = vec, k = &vec[size]; s < k; ++s) {				\
	*point[(*s shift) & 0xFF]++ = *s;				\
    }									\
    exp += 8;

    /* Sort each byte (if needed) */
    while(MISSING_BITS__) {
	if(exp) {
	    if(BYTE_IS_ODD__) {
		SORT_BYTE__(b, vector, >> exp);
	    } else {
		SORT_BYTE__(vector, b, >> exp);
	    }
	} else {
	    SORT_BYTE__(vector, b, );
	}
    }

    /* In case the array has both negative and positive integers, find the      */
    /* index of the first negative integer and put it in the start of the array */
    if(exp != (LAST_EXP__ + 8) && (((*vector ^ vector[size - 1]) < 0 && !BYTE_IS_ODD__) ||
				   ((*b ^ b[size - 1]) < 0 && BYTE_IS_ODD__))) {
	int offset = size - 1;
    	int tminusoff;

	if(!BYTE_IS_ODD__)  {
	    for(s = vector, k = &vector[size]; s < k && *s >= 0; ++s) { }
	    offset = s - vector;

	    tminusoff = size - offset;

	    if(offset < tminusoff) {
		memcpy(b, vector, sizeof(int) * offset);
		memcpy(vector, vector + offset, sizeof(int) * tminusoff);
		memcpy(vector + tminusoff, b, sizeof(int) * offset);
	    } else {
		memcpy(b, vector + offset, sizeof(int) * tminusoff);
		memmove(vector + tminusoff, vector, sizeof(int) * offset);
		memcpy(vector, b, sizeof(int) * tminusoff);
	    }
	} else {
	    for(s = b, k = &b[size]; s < k && *s >= 0; ++s) { }
	    offset = s - b;

	    tminusoff = size - offset;

	    memcpy(vector, b + offset, sizeof(int) * tminusoff);
	    memcpy(vector + tminusoff, b, sizeof(int) * (size - tminusoff));	
	}

    } else if(BYTE_IS_ODD__) {
	memcpy(vector, b, sizeof(int) * size);
    }
    
    /* Free helper array */
    free(b);
    
    /* Undefine function scoped macros for eventual later use */
#undef PRELIMINARY__
#undef CONSTRAIN__
#undef MISSING_BITS__
#undef LOOP_MAX__
#undef SORT_BYTE__
#undef BYTE_IS_ODD__
    
}
