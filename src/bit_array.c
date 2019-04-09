#include <assert.h> /* assert */

#include "bit_array.h"

#define ONE ((bit_arr_t)1)

#define BITS_IN_BYTE 8
#define BYTES_IN_BIT_ARRAY (sizeof(bit_arr_t))
#define BITS_IN_BIT_ARRAY (BYTES_IN_BIT_ARRAY * BITS_IN_BYTE)
#define OPTIONS_IN_BYTE (256)

static void InitCountedBitsTable(unsigned char counted_bits_table[]);


/*******************************************************************/

bit_arr_t BitArraySetOn(bit_arr_t bit_arr, size_t bit_idx)
{
    bit_arr_t mask = BitArrayFlipBit(EMPTY_BIT_ARR, bit_idx);

    assert(bit_idx < BITS_IN_BIT_ARRAY);
    
    return (bit_arr | mask); 
}

/**********/
bit_arr_t BitArraySetOff(bit_arr_t bit_arr, size_t bit_idx)
{
    bit_arr_t mask = BitArrayFlipBit(FULL_BIT_ARR, bit_idx);

    assert(bit_idx < BITS_IN_BIT_ARRAY);
    
    return (bit_arr & mask); 
}

/**********/
bit_arr_t BitArraySetBit(bit_arr_t bit_arr, size_t bit_idx,  int value)
{
    int value_of_bit = BitArrayGetBit(bit_arr, bit_idx);
    bit_arr_t mask = ((bit_arr_t)(value ^ value_of_bit)) << bit_idx;

    assert(bit_idx < BITS_IN_BIT_ARRAY);

    return (bit_arr ^ mask);
}

/**********/
size_t BitArrayBasicCountOn(bit_arr_t bit_arr)
{
    size_t on_bits_ctr = 0;
    
    while (bit_arr > 0)
    {
        bit_arr &= bit_arr - 1;
        ++on_bits_ctr;    
    }
    
    return (on_bits_ctr);
}

/**********/
size_t BitArrayCountOff(bit_arr_t bit_arr)
{
    return (BITS_IN_BIT_ARRAY - BitArrayCountOn(bit_arr));
}

/**********/
size_t BitArrayCountOn(bit_arr_t bit_arr)
{
    static int is_first_run = 1;
    static unsigned char counted_bits_table[OPTIONS_IN_BYTE] = {0};
    bit_arr_t mask = 0x00000000000000FF;
    size_t on_bits_ctr = 0;

    if (is_first_run)
    {
        InitCountedBitsTable(counted_bits_table);
        is_first_run = 0;
    }

    while(bit_arr > 0)
    { 
        on_bits_ctr += counted_bits_table[bit_arr & mask];
        bit_arr >>= BITS_IN_BYTE;    
    }
    
    return (on_bits_ctr);   
}

/**********/
static void InitCountedBitsTable(unsigned char counted_bits_table[])
{
    int i = 0;
  
    for (i = 0; i < OPTIONS_IN_BYTE; ++i)
    {
        counted_bits_table[i] = BitArrayBasicCountOn(i);
    }
}

/**********/
bit_arr_t BitArrayBasicMirrorBits(bit_arr_t bit_arr)
{
    size_t i = 0;
    bit_arr_t result = 0;
    
    for (i = 0; i < BITS_IN_BIT_ARRAY; ++i)
    {
        result <<= 1;
        result |= (bit_arr & 1);
        bit_arr >>= 1;
    }
    
    return (result);   
}

/**********/
bit_arr_t BitArrayRotateLeft(bit_arr_t bit_arr, size_t rotations_num)
{
    bit_arr_t mask = FULL_BIT_ARR;
    
    rotations_num %= BITS_IN_BIT_ARRAY;
    
    mask <<= (BITS_IN_BIT_ARRAY - rotations_num);
    mask &= bit_arr;
    mask >>= (BITS_IN_BIT_ARRAY - rotations_num);
    
    bit_arr <<= rotations_num;
    
    return (bit_arr |= mask);
}

/**********/
bit_arr_t BitArrayRotateRight(bit_arr_t bit_arr, size_t rotations_num)
{
    bit_arr_t mask = FULL_BIT_ARR;
    
    rotations_num %= BITS_IN_BIT_ARRAY;
    
    mask >>= (BITS_IN_BIT_ARRAY - rotations_num);
    mask &= bit_arr;
    mask <<= (BITS_IN_BIT_ARRAY - rotations_num);
    
    bit_arr >>= rotations_num;
    
    return (bit_arr |= mask);
}

/**********/
int BitArrayGetBit(bit_arr_t bit_arr, size_t bit_idx)
{
    bit_arr_t mask = ONE << bit_idx;
    
    assert(bit_idx < BITS_IN_BIT_ARRAY);
    
    return((bit_arr & mask) > 0);
}

/**********/
bit_arr_t BitArrayFlipBit(bit_arr_t bit_arr, size_t bit_idx)
{
    bit_arr_t mask = ONE << bit_idx;
    
    assert(bit_idx < BITS_IN_BIT_ARRAY);
    
    return (bit_arr ^ mask);
}

