#ifndef __KNIGHT_TOUR_H__
#define __KNIGHT_TOUR_H__

#define N 8

typedef enum STATUS_ENUMS
{
	COMPLETE,
	DEAD_END,
	STATUS_ENUMS_COUNT
} status_t;

typedef struct sq /* square */
{
	int r; /* row */
	int c; /* column */
} sq_t;

status_t DrawKnightTour(sq_t sq);


#endif /* __KNIGHT_TOUR_H__ */

