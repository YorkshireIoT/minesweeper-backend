#include <stdint.h>

/**
 * @brief Describes all the states a single point in the grid can be in.
 * 
 */
typedef enum {
    MINESWEEPER_STATE_MINE_HIDDEN,      /*! Mine, but hidden */
    MINESWEEPER_STATE_MINE_SHOWN,       /*! A discovered mine */
    MINESWEEPER_STATE_MINE_FLAGGED,     /*! A hidden mine that's been flagged. */
    MINESWEEPER_STATE_CLEAR_HIDDEN,     /*! A hidden clear point (i.e. no mine) */
    MINESWEEPER_STATE_CLEAR_SHOWN,      /*! A discovered clear point */
    MINESWEEPER_STATE_CLEAR_FLAGGED,    /*! A hidden clear point that's been flagged */
} MINESWEEPER_STATE;

/**
 * @brief Lists the possible return codes for the public functions.
 * 
 */
typedef enum {
    MINESWEEPER_RESULT_SUCCESS,         /*! Success */
    MINESWEEPER_RESULT_OUT_OF_BOUNDS,   /*! Supplied parameter out of bounds */
    MINESWEEPER_RESULT_NOT_INIT,        /*! Game has not yet been initialised */
    MINESWEEPER_RESULT_UNKNOWN_ERR,     /*! Unknown error */
    MINESWEEPER_RESULT_LOSE,            /*! User has clicked on a mine and lost */
    MINESWEEPER_RESULT_WIN,             /*! User has cleared all spaces without clicking on a mine */
} MINESWEEPER_RESULT;

/* Sets the game pragmatics */
#define MINESWEEPER_BOARD_HEIGHT    (8u)
#define MINESWEEPER_BOARD_WIDTH     (8u)
#define MINESWEEPER_BOARD_SIZE      (MINESWEEPER_BOARD_HEIGHT*MINESWEEPER_BOARD_WIDTH)
#define MINESWEEPER_MINE_COUNT      (10u)

typedef uint8_t minesweeper_coordinate;

/**
 * @brief Describes a cartesian point
 * 
 */
typedef struct {
    minesweeper_coordinate x;
    minesweeper_coordinate y;
} MINESWEEPER_POINT;

/**
 * @brief Resets and initialises the game.
 * 
 * @param grid  [in/out]    A 2D array which stores the game state and is updated by the module.
 *                          Must be initialised and memory managed by the user.
 * @param mines [in]        A list of points describing the location of the mines.
 * @return MINESWEEPER_RESULT The return code.
 */
MINESWEEPER_RESULT minesweeper_reset(MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT],
    MINESWEEPER_POINT mines[MINESWEEPER_MINE_COUNT]);

/**
 * @brief Picks a point within the grid.
 * 
 * @param grid  [in/out]    The 2D grid describing the game state.
 * @param point [in]        The point to choose.
 * @return MINESWEEPER_RESULT The return code.
 */
MINESWEEPER_RESULT minesweeper_pick(MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT],
    MINESWEEPER_POINT *point);

/**
 * @brief Flags a point within the grid.
 * 
 * @param grid  [in/out]    The 2D grid describing the game state.
 * @param point [in]        The point to flag.
 * @return MINESWEEPER_RESULT The return code.
 */
MINESWEEPER_RESULT minesweeper_flag(MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT],
    MINESWEEPER_POINT *point);
