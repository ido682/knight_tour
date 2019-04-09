#ifndef __BIT_ARRAY_H__
#define __BIT_ARRAY_H__

#include <stddef.h> /* size_t */

typedef size_t bit_arr_t;

#define EMPTY_BIT_ARR ((bit_arr_t)0)
#define FULL_BIT_ARR (~EMPTY_BIT_ARR)


/*******************************************************************/

bit_arr_t BitArraySetBit(bit_arr_t bit_arr, size_t bit_idx, int set_to);

bit_arr_t BitArraySetOn(bit_arr_t bit_arr, size_t bit_idx);

bit_arr_t BitArraySetOff(bit_arr_t bit_arr, size_t bit_idx);

size_t BitArrayCountOff(bit_arr_t bit_arr);

size_t BitArrayBasicCountOn(bit_arr_t bit_arr);

/* better performance than BitArrayBasicCountOn() */   
size_t BitArrayCountOn(bit_arr_t bit_arr);

bit_arr_t BitArrayBasicMirrorBits(bit_arr_t bit_arr);

bit_arr_t BitArrayRotateLeft(bit_arr_t bit_arr, size_t rotations_num);

bit_arr_t BitArrayRotateRight(bit_arr_t bit_arr, size_t rotations_num);

int BitArrayGetBit(bit_arr_t bit_arr, size_t bit_idx);

bit_arr_t BitArrayFlipBit(bit_arr_t bit_arr, size_t bit_idx);
   
         
#endif /* __BIT_ARRAY_H__ */

