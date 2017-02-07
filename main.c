#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define DEAD 0
#define ALIVE 1

#define DEAD_CHAR '.'
#define ALIVE_CHAR '#'

int main(int argc, char *argv[])
{
    /* initialize ncurses */
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, 1);

    int win_width, win_height;
    getmaxyx(stdscr, win_height, win_width);

    /* create and initialize the grid */
    int **grid = malloc(win_height * sizeof(int *));
    for (int i = 0; i < win_height; ++i)
        grid[i] = malloc(win_width * sizeof(int *));

    for (int i = 0; i < win_height; ++i) {
        for (int j = 0; j < win_width; ++j)
            grid[i][j] = 0;
    }

    /* sample oscillator to make sure grid logic is working */
    int width_mid = win_width / 2;
    int height_mid = win_height / 2;

    grid[height_mid][width_mid-1] = ALIVE;
    grid[height_mid][width_mid] = ALIVE;
    grid[height_mid][width_mid+1] = ALIVE;

    for (int i = 0; i < win_height; ++i) {
        for (int j = 0; j < win_width; ++j) {
            if (grid[i][j] == DEAD)
                mvaddch(i, j, DEAD_CHAR);
            else
                mvaddch(i, j, ALIVE_CHAR);
        }
    }

    int ch;
    while(1) {
        if ((ch = getch()) == 'q' || ch == 'Q')
            break;
    }

    endwin();
    free(grid);
    return 0;
}
