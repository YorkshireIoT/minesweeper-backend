#include <stdio.h>
#include "minesweeper.h"

MINESWEEPER_STATE grid[MINESWEEPER_BOARD_WIDTH][MINESWEEPER_BOARD_HEIGHT] = {0u};

MINESWEEPER_POINT mines[MINESWEEPER_MINE_COUNT] = {
    {0, 0},
    {1, 1},
    {2, 2},
    {3, 3},
    {4, 4},
    {5, 5},
    {6, 6},
    {7, 7},
    {7, 6},
    {7, 5},
};

/**
 * @brief Prints all points in the grid to the console.
 * 
 */
static void show_grid(void) {
    for (minesweeper_coordinate y = MINESWEEPER_BOARD_HEIGHT - 1; y < MINESWEEPER_BOARD_HEIGHT; y--) {
        for (minesweeper_coordinate x = 0; x < MINESWEEPER_BOARD_WIDTH; x++) {
            switch (grid[x][y]) {
                case MINESWEEPER_STATE_MINE_HIDDEN:
                case MINESWEEPER_STATE_CLEAR_HIDDEN:
                    printf(" [?] ");
                    break;
                case MINESWEEPER_STATE_MINE_FLAGGED:
                case MINESWEEPER_STATE_CLEAR_FLAGGED:
                    printf(" [F] ");
                    break;
                case MINESWEEPER_STATE_MINE_SHOWN:
                    printf(" [!] ");
                    break;
                case MINESWEEPER_STATE_CLEAR_SHOWN:
                    printf(" [ ] ");
                    break;
                default:
                    printf("INTERNAL ERROR: Unknown state '%u' at point (%u, %u)\n", grid[x][y], x, y);
                    break;
            }
        }
        puts("");
    }
}

int main(void) {
    puts("Welcome to Minesweeper CMD Line.\n");
    int x = 0;
    int y = 0;

    MINESWEEPER_RESULT result = minesweeper_reset(grid, mines);
    if (MINESWEEPER_RESULT_SUCCESS != result) {
        printf("Got error code: %u\n Exiting...", result);
        return -1;
    }

    while (1) {
        puts("Pick a point (P) or flag (F)?");
        char choice;
        scanf("%c", &choice);
        if (choice != 'P' && choice != 'F') {
            continue;
        }
        printf("X: ");
        scanf("%d", &x);
        printf("Y: ");
        scanf("%d", &y);
        MINESWEEPER_POINT point = {x, y};
        if (choice == 'P') {
            result = minesweeper_pick(grid, &point);
        } else {
            result = minesweeper_flag(grid, &point);
        }
        switch (result) {
            case MINESWEEPER_RESULT_SUCCESS:
                show_grid();
                break;
            case MINESWEEPER_RESULT_LOSE:
                show_grid();
                puts("You lose.");
                return 0;
            case MINESWEEPER_RESULT_WIN:
                show_grid();
                puts("You Win!");
                return 0;
            default:
                printf("Got error code: %u\n", result);
                break;
        }
    }

    return 0;
}