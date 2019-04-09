#include <stdio.h> /* printf */
#include <stdlib.h> /* qsort */
#include <math.h> /* pow */ 

#include "knight_tour.h"
#include "bit_array.h"

/* U = up, D = down, R = right, L = left */
typedef enum MOVES_ENUMS
{
	MOVE_UL,
	MOVE_UR,
	MOVE_RU,
	MOVE_RD,
	MOVE_DR,
	MOVE_DL,
	MOVE_LD,
	MOVE_LU,
	MOVES_ENUMS_COUNT
} moves_t;

typedef struct sq_data sq_data_t;
typedef struct board_info board_info_t;

/* functions to calculate knight's moves */
typedef sq_t (*move_func_t)(sq_t sq);

struct sq_data
{
	sq_t sq;
	/* all 8 optional next squars, INCLUDING NON-VALID ONES */
	sq_data_t *optional_moves[MOVES_ENUMS_COUNT];
	/* squares which may be visited set to 0, others set to 1 */
	bit_arr_t unique_visits_map; 
	/* calculated only when considering visiting square */
	int num_of_valid_moves_for_next_sq; 
};

struct board_info
{
	bit_arr_t general_visits_map;
	int arr_order_of_visits[N][N];
};

static sq_data_t bad_sq_data = {
							   		{-1, -1}, 
									{NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL}, 
									FULL_BIT_ARR, 
									-1 
							   }; 
	

/*****************************************************************
auxiliary functions
*****************************************************************/

static status_t RecursiveKnight(sq_data_t arr_sq_data[N][N],
								board_info_t board_info,
								sq_data_t cur_sq_data);

/* initialization functions */
static void InitMoveFuncs();
static void InitSquaresData();

/* moving functions */
static sq_t MoveUL(sq_t sq);
static sq_t MoveUR(sq_t sq);
static sq_t MoveRU(sq_t sq);
static sq_t MoveRD(sq_t sq);
static sq_t MoveDR(sq_t sq);
static sq_t MoveDL(sq_t sq);
static sq_t MoveLD(sq_t sq);
static sq_t MoveLU(sq_t sq);

/* other functions */
static int IsInBoundsOfBoard(sq_t sqare);
static int WasVisited(sq_t square, bit_arr_t bit_arr);
static size_t GetBitIdx(sq_t sqare);
static sq_data_t GetSqData(sq_data_t arr_sq_data[N][N], sq_t square);
static void PrintOrderOfVisits(board_info_t board_info);
static int CompareSqDatas(const void *external_sq_data,
				   const void *internal_sq_data);


/*******************************************************************/

status_t DrawKnightTour(sq_t sq)
{
	static int is_first_run = 1;
	static move_func_t arr_move_funcs[MOVES_ENUMS_COUNT] = {0};
	static sq_data_t arr_sq_data[N][N] = {0};

	status_t status = DEAD_END;
	sq_data_t sq_data = {0};
	board_info_t board_info = {0};
	board_info.general_visits_map = EMPTY_BIT_ARR;

	if (is_first_run)
	{
		InitMoveFuncs(arr_move_funcs);
		InitSquaresData(arr_sq_data, arr_move_funcs);
		is_first_run = 0;
	}
	
	sq_data = GetSqData(arr_sq_data, sq);
	status = RecursiveKnight(arr_sq_data, board_info, sq_data);

	return (status);
}

/**********/
static status_t RecursiveKnight(sq_data_t arr_sq_data[N][N],
								board_info_t board_info,
								sq_data_t cur_sq_data)
{
	size_t num_of_plays_so_far = 0;
	sq_t cur_sq = cur_sq_data.sq;
	sq_t optional_next_sq = {0};
	int num_of_valid_moves_for_current_sq = 0;
	int num_of_non_valid_moves_for_current_sq = 0;
	size_t i = 0;
	sq_data_t current_optional_moves[MOVES_ENUMS_COUNT] = {0};

	/* mark current square as visited */
	board_info.general_visits_map =
		BitArraySetOn(board_info.general_visits_map, GetBitIdx(cur_sq));
	/* the number of bits on indicats how many plays were played so far */
	num_of_plays_so_far = BitArrayCountOn(board_info.general_visits_map);
	/* sets the order of visit in the array to be printed eventually */
	board_info.arr_order_of_visits[cur_sq.r][cur_sq.c] = num_of_plays_so_far;

	/* board is full - success! */
	if (FULL_BIT_ARR == board_info.general_visits_map)
	{
		PrintOrderOfVisits(board_info);
		return (COMPLETE);
	}

	/* fill the array with 8 next optional squares, calculate how many
	next-next optional moves each of them has */
	for (i = 0; i < MOVES_ENUMS_COUNT; ++i)
	{
		int num_of_valid_moves_for_next_sq = -1; 

		/* optional moves for each square in board were already calculated
		at initialization */
		current_optional_moves[i] = *cur_sq_data.optional_moves[i];
		optional_next_sq = current_optional_moves[i].sq;

		/* check wether optional_next_sq is valid or not */
		if ((IsInBoundsOfBoard(optional_next_sq)) &&
		   (!WasVisited(optional_next_sq, board_info.general_visits_map)))
		{
			bit_arr_t unique_visits_map = EMPTY_BIT_ARR;
			bit_arr_t combined_visits_map = EMPTY_BIT_ARR;
			
			/* unique_visits_map for each square in board were
			already calculated at initialization */
			unique_visits_map = current_optional_moves[i].unique_visits_map;

			/* creates a new visits map for the optional next square (considers the information in the general map and the square's unique map) to evaluate which squares may be visited in the next-next move.
			THE VALID BITS IN THE NEW MAP WOULD BE SET ON */
			combined_visits_map =
				((board_info.general_visits_map ^ FULL_BIT_ARR) &
				(unique_visits_map ^ FULL_BIT_ARR));

			/* calculate how many valid moves optional_next_sq would have
			(using the combined visits map).
			if not valid - the default (-1, means non-valid) remains */
			num_of_valid_moves_for_next_sq = BitArrayCountOn
											 (combined_visits_map);
			++num_of_valid_moves_for_current_sq;
		}

		current_optional_moves[i].num_of_valid_moves_for_next_sq =
			num_of_valid_moves_for_next_sq;	
	}

	/* sort the array by the num_of_valid_moves_for_next_sq,
	from lowest to highest, thus the squares with the least options
	would be visited first (non-valid squares would be located
	in the very beginning of the array, and would be skipped) */
	qsort(current_optional_moves, MOVES_ENUMS_COUNT,
		  sizeof(sq_data_t), CompareSqDatas);

	/* make sure the next loop would skip the non-valid squares,
	and start from the first valid one */
	num_of_non_valid_moves_for_current_sq =
		MOVES_ENUMS_COUNT - num_of_valid_moves_for_current_sq;

	/* invoke the function recursively for each valid possible next square,
	in case of success - appropriate value is returned */
	for (i = num_of_non_valid_moves_for_current_sq; i < MOVES_ENUMS_COUNT; ++i)
	{
		status_t status = DEAD_END;
		sq_data_t optional_next_sq_data = GetSqData(arr_sq_data,
										  current_optional_moves[i].sq);

		status = RecursiveKnight(arr_sq_data, board_info,
								 optional_next_sq_data);
		if (COMPLETE == status)
		{
			return (COMPLETE);
		}
	}

	/* none of the possible next squares led to success */
	return (DEAD_END);
}




/**********/
static int IsInBoundsOfBoard(sq_t sq)
{
	return ((sq.r >= 0) && (sq.r < N) && (sq.c >= 0) && (sq.c < N));
}

/**********/
static int WasVisited(sq_t square, bit_arr_t bit_arr)
{
	size_t idx = GetBitIdx(square);

	return (BitArrayGetBit(bit_arr, idx));
}

/**********/
static sq_data_t GetSqData(sq_data_t arr_sq_data[N][N], sq_t square)
{
	return(arr_sq_data[square.r][square.c]);
}

/**********/
static size_t GetBitIdx(sq_t sq)
{
	return(sq.r * N + sq.c);
}

/**********/
static void PrintOrderOfVisits(board_info_t board_info)
{
	int r = 0;
	int c = 0;
	
	for (r = 0; r < N; ++r)
	{
		for (c = 0; c < N; ++c)
		{
			printf("%d\t", board_info.arr_order_of_visits[r][c]);
		}
		printf("\n\n\n");
	}
}



/* moving functions */

static sq_t MoveUL(sq_t sq)
{
	sq.r -= 2;
	sq.c -= 1;

	return (sq);
}

/**********/
static sq_t MoveUR(sq_t sq)
{
	sq.r -= 2;
	sq.c += 1;

	return (sq);
}

/**********/
static sq_t MoveRU(sq_t sq)
{
	sq.r -= 1;
	sq.c += 2;

	return (sq);
}

/**********/
static sq_t MoveRD(sq_t sq)
{
	sq.r += 1;
	sq.c += 2;

	return (sq);
}

/**********/
static sq_t MoveDR(sq_t sq)
{
	sq.r += 2;
	sq.c += 1;

	return (sq);
}

/**********/
static sq_t MoveDL(sq_t sq)
{
	sq.r += 2;
	sq.c -= 1;

	return (sq);
}

/**********/
static sq_t MoveLD(sq_t sq)
{
	sq.r += 1;
	sq.c -= 2;

	return (sq);
}

/**********/
static sq_t MoveLU(sq_t sq)
{
	sq.r -= 1;
	sq.c -= 2;

	return (sq);
}


/* sorting functions */

static int CompareSqDatas(const void *external_sq_data,
						  const void *internal_sq_data) 
{
	return (((sq_data_t *)external_sq_data)->num_of_valid_moves_for_next_sq -
			((sq_data_t *)internal_sq_data)->num_of_valid_moves_for_next_sq);
}


/*initialization functions */

static void InitMoveFuncs(move_func_t arr_move_funcs[])
{
	arr_move_funcs[MOVE_UL] = MoveUL;
	arr_move_funcs[MOVE_UR] = MoveUR;
	arr_move_funcs[MOVE_RU] = MoveRU;
	arr_move_funcs[MOVE_RD] = MoveRD;
	arr_move_funcs[MOVE_DR] = MoveDR;
	arr_move_funcs[MOVE_DL] = MoveDL;
	arr_move_funcs[MOVE_LD] = MoveLD;
	arr_move_funcs[MOVE_LU] = MoveLU;
}

/**********/
static void InitSquaresData(sq_data_t arr_sq_data[N][N],
							const move_func_t arr_move_funcs[])
{
	size_t r = 0;
	size_t c = 0;
	int func_idx = 0;

	for (r = 0; r < N; ++r)
	{
		for (c = 0; c < N; ++c)
		{
			bit_arr_t unique_visits_map = FULL_BIT_ARR;
			sq_t cur_sq = {0};
			cur_sq.r = r;
			cur_sq.c = c;
			arr_sq_data[r][c].sq = cur_sq;
			/* would be calculated only when considering visiting square */
			arr_sq_data[r][c].num_of_valid_moves_for_next_sq = 0;
			
			/* set the optional next squares:
			invoke each of the move functions with the current square,
			and set the returned value (the next square) in the array */
			for (func_idx = 0; func_idx < MOVES_ENUMS_COUNT; ++func_idx)
			{
				sq_t next_sq = arr_move_funcs[func_idx](cur_sq);
				/* check wether the next square is valid or not */
				if (IsInBoundsOfBoard(next_sq))
				{
					/* setting a bit off means it's an optional next-square */
					size_t idx_to_set_off = GetBitIdx(next_sq);
					unique_visits_map = BitArraySetOff(unique_visits_map,
												   	   idx_to_set_off);
					/* store the pointer to the data of the optional next square in the array */
					arr_sq_data[r][c].optional_moves[func_idx] =
						&(arr_sq_data[next_sq.r][next_sq.c]);
				}
				/* if not valid - store a pointer to bad_sq_data */
				else
				{
					arr_sq_data[r][c].optional_moves[func_idx] = &bad_sq_data;
				}
			}
			
			arr_sq_data[r][c].unique_visits_map = unique_visits_map;
		}
	}
}

/**********/
