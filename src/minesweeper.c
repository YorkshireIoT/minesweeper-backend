#include "minesweeper.h"
#include <stdbool.h>
#include <stdio.h>

/** Flags whether the board is currently initialised or not */
static bool is_init = false;
/** Counts the number of squares which have been revealed */
static uint32_t shown_points = 0;

/**
 * @brief Checks if a given coordinate is valid.
 *
 * @param coordinate    [in]    The coordinate to check.
 * @param max_value     [in]    The maximum allowed value (exclusive).
 * @param name          [in]    Friendly name for the coordinate to print out.
 * @return true     Coordinate is valid.
 * @return false    Coordinate is invalid.
 */
static bool check_coordinate(minesweeper_coordinate coordinate,
                             minesweeper_coordinate max_value,
                             const char *name) {
  bool is_valid = true;

  if (coordinate >= max_value) {
    is_valid = false;
    printf("The %s coordinate '%u' is greater than the max allowed value %u.\n",
           name, coordinate, max_value);
  }

  return is_valid;
}

/**
 * @brief Checks if the given point is valid.
 *
 * @param point [in]    The point to check.
 * @return true     The point is valid.
 * @return false    The point is invalid.
 */
static bool check_point(MINESWEEPER_POINT *point) {
  if (check_coordinate(point->x, MINESWEEPER_BOARD_WIDTH, "X") &&
      check_coordinate(point->y, MINESWEEPER_BOARD_HEIGHT, "Y")) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief Changes the specified hidden or flagged point to its revealed
 * equivalent.
 *
 * @param grid  [in/out]    The grid containing the state of all points.
 * @param point [in]        The point to show
 */
static void show_point(
    MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT],
    MINESWEEPER_POINT *point) {
  minesweeper_coordinate x = point->x;
  minesweeper_coordinate y = point->y;
  switch (grid[x][y]) {
  case MINESWEEPER_STATE_MINE_HIDDEN:
  /* Fallthrough, flagging has no effect on the underlying state */
  case MINESWEEPER_STATE_MINE_FLAGGED:
    grid[x][y] = MINESWEEPER_STATE_MINE_SHOWN;
    break;
  case MINESWEEPER_STATE_CLEAR_HIDDEN:
  /* Fallthrough, flagging has no effect on the underlying state */
  case MINESWEEPER_STATE_CLEAR_FLAGGED:
    grid[x][y] = MINESWEEPER_STATE_CLEAR_SHOWN;
    shown_points++;
    break;
  case MINESWEEPER_STATE_MINE_SHOWN:
  case MINESWEEPER_STATE_CLEAR_SHOWN:
    /* Nothing to do, already shown */
    break;
  default:
    printf("INTERNAL ERROR: Unknown state '%u' at point (%u, %u)\n", grid[x][y],
           x, y);
    break;
  }
}

/**
 * @brief Changes any hidden or flagged points to their revealed equivalent.
 *
 * @param grid  [in/out]    The grid containing the state of all points.
 * @param point [in]        The point to show
 */
static void show_all(
    MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT]) {
  for (minesweeper_coordinate x = 0; x < MINESWEEPER_BOARD_WIDTH; x++) {
    for (minesweeper_coordinate y = 0; y < MINESWEEPER_BOARD_HEIGHT; y++) {
      MINESWEEPER_POINT point = {x, y};
      show_point(grid, &point);
    }
  }
}

/**
 * @brief Pick a clear (not mine) point by revealing it and all adjacent points
 * to it.
 *
 * @param grid  [in/out]    The grid containing the state of all points.
 * @param point [in]        The point to pick.
 */
static void pick_clear_point(
    MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT],
    MINESWEEPER_POINT *point) {
  /* Show the point, and the 8 adjacent squares */
  for (int i = -1; i <= 1; i++) {
    /* If it's out of bounds, don't try update it */
    minesweeper_coordinate x = (minesweeper_coordinate)(point->x + i);
    if (x < MINESWEEPER_BOARD_WIDTH) {
      for (int j = -1; j <= 1; j++) {
        /* If it's out of bounds, don't try update it */
        minesweeper_coordinate y = (minesweeper_coordinate)(point->y + j);
        if (y < MINESWEEPER_BOARD_HEIGHT) {
          MINESWEEPER_POINT p = {x, y};
          show_point(grid, &p);
        }
      }
    }
  }
}

/**
 * @brief Pick a point on the grid and update the grid state with the proper
 * effects
 *
 * @param grid  [in/out]    The grid containing the state of all points.
 * @param point [in]        The point to pick.
 */
static MINESWEEPER_RESULT pick_point(
    MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT],
    MINESWEEPER_POINT *point) {
  MINESWEEPER_RESULT result = MINESWEEPER_RESULT_SUCCESS;
  minesweeper_coordinate x = point->x;
  minesweeper_coordinate y = point->y;
  MINESWEEPER_STATE state = grid[x][y];

  switch (state) {
  case MINESWEEPER_STATE_MINE_HIDDEN:
    /* Fallthrough, flagging has no effect on the underlying state */
  case MINESWEEPER_STATE_MINE_FLAGGED:
    printf("Oh no, point (%u, %u) was a mine :(\n", x, y);
    show_all(grid);
    result = MINESWEEPER_RESULT_LOSE;
    break;
  case MINESWEEPER_STATE_MINE_SHOWN:
  case MINESWEEPER_STATE_CLEAR_SHOWN:
    printf("This shouldn't happen. You've clicked on an already visible square "
           "at point (%u, %u)\n",
           x, y);
    result = MINESWEEPER_RESULT_UNKNOWN_ERR;
    break;
  case MINESWEEPER_STATE_CLEAR_HIDDEN:
    /* Fallthrough, flagging has no effect on the underlying state */
  case MINESWEEPER_STATE_CLEAR_FLAGGED:
    printf("Phew, point (%u, %u) is clear.\n", x, y);
    pick_clear_point(grid, point);
    /* If all non-mine points are now visible, the player has won */
    if (shown_points == (MINESWEEPER_BOARD_SIZE - MINESWEEPER_MINE_COUNT)) {
      result = MINESWEEPER_RESULT_WIN;
    } else {
      result = MINESWEEPER_RESULT_SUCCESS;
    }
    break;
  default:
    printf("INTERNAL ERROR: Unknown state '%u' at point (%u, %u)\n", grid[x][y],
           x, y);
    break;
  }

  return result;
}

/**
 * @brief Flag a point on the grid
 *
 * @param grid  [in/out]    The grid containing the state of all points.
 * @param point [in]        The point to flag.
 */
static MINESWEEPER_RESULT flag_point(
    MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT],
    MINESWEEPER_POINT *point) {
  MINESWEEPER_RESULT result = MINESWEEPER_RESULT_SUCCESS;
  minesweeper_coordinate x = point->x;
  minesweeper_coordinate y = point->y;
  MINESWEEPER_STATE state = grid[x][y];

  switch (state) {
  case MINESWEEPER_STATE_MINE_HIDDEN:
    grid[x][y] = MINESWEEPER_STATE_MINE_FLAGGED;
    result = MINESWEEPER_RESULT_SUCCESS;
    break;
  case MINESWEEPER_STATE_CLEAR_HIDDEN:
    grid[x][y] = MINESWEEPER_STATE_CLEAR_FLAGGED;
    result = MINESWEEPER_RESULT_SUCCESS;
    break;
  case MINESWEEPER_STATE_MINE_SHOWN:
  case MINESWEEPER_STATE_CLEAR_SHOWN:
    printf("This shouldn't happen. You can't flag an already visible square "
           "(%u, %u)\n",
           x, y);
    result = MINESWEEPER_RESULT_UNKNOWN_ERR;
    break;
  case MINESWEEPER_STATE_MINE_FLAGGED:
  case MINESWEEPER_STATE_CLEAR_FLAGGED:
    printf("This shouldn't happen. You can't flag an already flagged square "
           "(%u, %u)\n",
           x, y);
    result = MINESWEEPER_RESULT_UNKNOWN_ERR;
    break;
  default:
    printf("INTERNAL ERROR: Unknown state '%u' at point (%u, %u)\n", grid[x][y],
           x, y);
    break;
  }

  return result;
}

MINESWEEPER_RESULT minesweeper_reset(
    MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT],
    MINESWEEPER_POINT mines[MINESWEEPER_MINE_COUNT]) {
  /* Set to success by default, will be updated accordingly otherwise */
  MINESWEEPER_RESULT result = MINESWEEPER_RESULT_SUCCESS;
  is_init = false;
  shown_points = 0;

  /* Initialise the grid to its default initial value, clear and hidden */
  for (minesweeper_coordinate x = 0; x < MINESWEEPER_BOARD_WIDTH; x++) {
    for (minesweeper_coordinate y = 0; y < MINESWEEPER_BOARD_HEIGHT; y++) {
      grid[x][y] = MINESWEEPER_STATE_CLEAR_HIDDEN;
    }
  }
  /* Now try to populate the given mines */
  for (uint16_t i = 0; i < MINESWEEPER_MINE_COUNT; i++) {
    /* Check each of the supplied mine coordinates would fit on the grid */
    if (!check_point(&mines[i])) {
      result = MINESWEEPER_RESULT_OUT_OF_BOUNDS;
      printf("Invalid coordinates for user supplied mine number %d\n", i);
      break;
    } else {
      /* Check there isn't already a mine there */
      if (grid[mines[i].x][mines[i].y] != MINESWEEPER_STATE_MINE_HIDDEN) {
        /* All good, set the mine on the grid */
        grid[mines[i].x][mines[i].y] = MINESWEEPER_STATE_MINE_HIDDEN;
      } else {
        result = MINESWEEPER_RESULT_OUT_OF_BOUNDS;
        printf("Duplicate mine point supplied for point (%u, %u).\n",
               mines[i].x, mines[i].y);
        break;
      }
    }
  }

  /* If successful, set the is initialised variable to true */
  if (MINESWEEPER_RESULT_SUCCESS == result) {
    is_init = true;
  }

  return result;
}

MINESWEEPER_RESULT minesweeper_pick(
    MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT],
    MINESWEEPER_POINT *point) {
  /* Set to success by default, will be updated accordingly otherwise */
  MINESWEEPER_RESULT result = MINESWEEPER_RESULT_SUCCESS;

  if (is_init) {
    /* Check the supplied grid point is valid */
    if (check_point(point)) {
      result = pick_point(grid, point);
    } else {
      printf("Invalid supplied user point.\n");
      result = MINESWEEPER_RESULT_OUT_OF_BOUNDS;
    }
  } else {
    printf(
        "Board not yet initialised. Please call minesweeper_reset() first.\n");
    result = MINESWEEPER_RESULT_NOT_INIT;
  }

  return result;
}

MINESWEEPER_RESULT minesweeper_flag(
    MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT],
    MINESWEEPER_POINT *point) {
  /* Set to success by default, will be updated accordingly otherwise */
  MINESWEEPER_RESULT result = MINESWEEPER_RESULT_SUCCESS;

  if (is_init) {
    /* Check the supplied grid point is valid */
    if (check_point(point)) {
      result = flag_point(grid, point);
    } else {
      printf("Invalid supplied user point.\n");
      result = MINESWEEPER_RESULT_OUT_OF_BOUNDS;
    }
  } else {
    printf(
        "Board not yet initialised. Please call minesweeper_reset() first.\n");
    result = MINESWEEPER_RESULT_NOT_INIT;
  }

  return result;
}