#include "../Unity/src/unity.h"
#include "../include/minesweeper.h"
#include <stdbool.h>

MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT] = {0u};

MINESWEEPER_POINT mines[MINESWEEPER_MINE_COUNT];

MINESWEEPER_POINT winning_points[] = {
    {1, 7}, {4, 7}, {6, 7},
    {0, 4}, {3, 4}, {5, 4}, {7, 4},
    {0, 1}, {3, 1}, {5, 1}, {7, 1}
};

/* Grid should look like this (! is a mine)
 [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  [!] 
 [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  [!]  [!] 
 [ ]  [ ]  [ ]  [ ]  [ ]  [!]  [ ]  [!] 
 [ ]  [ ]  [ ]  [ ]  [!]  [ ]  [ ]  [ ] 
 [ ]  [ ]  [ ]  [!]  [ ]  [ ]  [ ]  [ ] 
 [ ]  [ ]  [!]  [ ]  [ ]  [ ]  [ ]  [ ] 
 [ ]  [!]  [ ]  [ ]  [ ]  [ ]  [ ]  [ ] 
 [!]  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  [ ] 
 */
void setUp(void) {
    MINESWEEPER_POINT point;
    for (uint8_t i = 0; i < MINESWEEPER_BOARD_WIDTH; i++) {
        point.x = i;
        point.y = i;
        mines[i] = point;
    }
    point.y = 6;
    mines[8] = point;
    point.y = 5;
    mines[9] = point;
}

void tearDown(void)
{
}

void test_mine_placement(void) {
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    for (minesweeper_coordinate x = 0; x < MINESWEEPER_BOARD_WIDTH; x++) {
        for (minesweeper_coordinate y = 0; y < MINESWEEPER_BOARD_HEIGHT; y++) {
            bool is_mine = false;
            for (uint16_t i = 0; i < MINESWEEPER_MINE_COUNT; i++) {
                if (mines[i].x == x && mines[i].y == y) {
                    is_mine = true;
                }
            }
            if (is_mine) {
                TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_MINE_HIDDEN, grid[x][y]);
            } else {
                TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_CLEAR_HIDDEN, grid[x][y]);
            }
        }
    }
}

void test_bad_mine(void) {
    MINESWEEPER_POINT point = {MINESWEEPER_BOARD_WIDTH, MINESWEEPER_BOARD_HEIGHT};
    mines[MINESWEEPER_MINE_COUNT-1] = point;
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_OUT_OF_BOUNDS, result);
}

void test_bad_point(void) {
    MINESWEEPER_POINT point = {MINESWEEPER_BOARD_WIDTH, MINESWEEPER_BOARD_HEIGHT};
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    result = minesweeper_flag(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_OUT_OF_BOUNDS, result);
}

void test_bad_flag(void) {
    MINESWEEPER_POINT point = {MINESWEEPER_BOARD_WIDTH, MINESWEEPER_BOARD_HEIGHT};
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    result = minesweeper_pick(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_OUT_OF_BOUNDS, result);
}

void test_duplicate_pick(void) {
    MINESWEEPER_POINT point = {0, 5};
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    result = minesweeper_pick(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    result = minesweeper_pick(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_UNKNOWN_ERR, result);
}

void test_duplicate_flag(void) {
    MINESWEEPER_POINT point = {0, 5};
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    result = minesweeper_flag(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    result = minesweeper_flag(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_UNKNOWN_ERR, result);
}

void test_flag_visible(void) {
    MINESWEEPER_POINT point = {0, 5};
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    result = minesweeper_pick(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    result = minesweeper_flag(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_UNKNOWN_ERR, result);
}

void test_uninitialised(void) {
    MINESWEEPER_POINT point = {0, 0};
    MINESWEEPER_RESULT result = minesweeper_pick(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_NOT_INIT, result);
    result = minesweeper_flag(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_NOT_INIT, result);
}

void test_duplicate_mine(void) {
    MINESWEEPER_POINT point = {6, 6};
    mines[MINESWEEPER_MINE_COUNT-1] = point;
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_OUT_OF_BOUNDS, result);
}

void test_pick_clear_middle_point(void) {
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    MINESWEEPER_POINT point = {3, 4};
    result = minesweeper_pick(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);

    MINESWEEPER_POINT revealed_clear[] = {
        {2, 5}, {3, 5}, {4, 5},
        {2, 4}, {3, 4},
        {2, 3}, {4, 3},
    };

    MINESWEEPER_POINT revealed_mines[] = {
        {3, 3}, {4, 4}
    };

    for (minesweeper_coordinate x = 0; x < MINESWEEPER_BOARD_WIDTH; x++) {
        for (minesweeper_coordinate y = 0; y < MINESWEEPER_BOARD_HEIGHT; y++) {
            bool is_mine = false;
            bool is_revealed_clear = false;
            bool is_revealed_mine = false;
            for (uint16_t i = 0; i < MINESWEEPER_MINE_COUNT; i++) {
                if (mines[i].x == x && mines[i].y == y) {
                    is_mine = true;
                }
            }

            for (uint16_t i = 0; i < (sizeof(revealed_clear)/sizeof(MINESWEEPER_POINT)); i++) {
                if (revealed_clear[i].x == x && revealed_clear[i].y == y) {
                    is_revealed_clear = true;
                }
            }

            for (uint16_t i = 0; i < (sizeof(revealed_mines)/sizeof(MINESWEEPER_POINT)); i++) {
                if (revealed_mines[i].x == x && revealed_mines[i].y == y) {
                    is_revealed_mine = true;
                }
            }
            
            if (is_mine) {
                if (is_revealed_mine) {
                    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_MINE_SHOWN, grid[x][y]);
                } else {
                    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_MINE_HIDDEN, grid[x][y]);
                }
            } else {
                if (is_revealed_clear) {
                    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_CLEAR_SHOWN, grid[x][y]);
                } else {
                    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_CLEAR_HIDDEN, grid[x][y]);
                }
            }
        }
    }
}

void test_pick_clear_edge_point(void) {
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    MINESWEEPER_POINT point = {0, 2};
    result = minesweeper_pick(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);

    MINESWEEPER_POINT revealed_clear[] = {
        {0, 3}, {1, 3},
        {0, 2}, {1, 2},
        {0, 1},
    };

    MINESWEEPER_POINT revealed_mines[] = {
        {1, 1}
    };

    for (minesweeper_coordinate x = 0; x < MINESWEEPER_BOARD_WIDTH; x++) {
        for (minesweeper_coordinate y = 0; y < MINESWEEPER_BOARD_HEIGHT; y++) {
            bool is_mine = false;
            bool is_revealed_clear = false;
            bool is_revealed_mine = false;
            for (uint16_t i = 0; i < MINESWEEPER_MINE_COUNT; i++) {
                if (mines[i].x == x && mines[i].y == y) {
                    is_mine = true;
                }
            }

            for (uint16_t i = 0; i < (sizeof(revealed_clear)/sizeof(MINESWEEPER_POINT)); i++) {
                if (revealed_clear[i].x == x && revealed_clear[i].y == y) {
                    is_revealed_clear = true;
                }
            }

            for (uint16_t i = 0; i < (sizeof(revealed_mines)/sizeof(MINESWEEPER_POINT)); i++) {
                if (revealed_mines[i].x == x && revealed_mines[i].y == y) {
                    is_revealed_mine = true;
                }
            }
            
            if (is_mine) {
                if (is_revealed_mine) {
                    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_MINE_SHOWN, grid[x][y]);
                } else {
                    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_MINE_HIDDEN, grid[x][y]);
                }
            } else {
                if (is_revealed_clear) {
                    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_CLEAR_SHOWN, grid[x][y]);
                } else {
                    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_CLEAR_HIDDEN, grid[x][y]);
                }
            }
        }
    }
}

void test_pick_mine(void) {
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    MINESWEEPER_POINT point = {2, 2};
    result = minesweeper_pick(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_LOSE, result);
}

void test_flag_mine(void) {
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    MINESWEEPER_POINT point = {2, 2};
    result = minesweeper_flag(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_MINE_FLAGGED, grid[2][2]);
}

void test_flag_clear(void) {
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    MINESWEEPER_POINT point = {2, 3};
    result = minesweeper_flag(grid, &point);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_STATE_CLEAR_FLAGGED, grid[2][3]);
}

void test_win_game(void) {
    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
    for (uint8_t i = 0; i < (sizeof(winning_points)/sizeof(MINESWEEPER_POINT)); i++) {
        result = minesweeper_pick(grid, &winning_points[i]);
        if (i == ((sizeof(winning_points)/sizeof(MINESWEEPER_POINT))-1)) {
            TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_WIN, result);
        } else {
            TEST_ASSERT_EQUAL_UINT(MINESWEEPER_RESULT_SUCCESS, result);
        }
    }
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_uninitialised);
    RUN_TEST(test_mine_placement);
    RUN_TEST(test_bad_mine);
    RUN_TEST(test_bad_point);
    RUN_TEST(test_bad_flag);
    RUN_TEST(test_duplicate_mine);
    RUN_TEST(test_duplicate_pick);
    RUN_TEST(test_duplicate_flag);
    RUN_TEST(test_flag_visible);
    RUN_TEST(test_pick_clear_middle_point);
    RUN_TEST(test_pick_clear_edge_point);
    RUN_TEST(test_pick_mine);
    RUN_TEST(test_flag_mine);
    RUN_TEST(test_flag_clear);
    RUN_TEST(test_win_game);
    return UNITY_END();
}