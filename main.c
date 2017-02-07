#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEAD 0
#define ALIVE 1

#define DEAD_CHAR '.'
#define ALIVE_CHAR '#'

WINDOW *create_window(int height, int width, int starty, int startx);
bool **create_grid(int height, int width);

int main(int argc, char *argv[])
{
    /* initialize ncurses */
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, 1);

    // TODO: allow user to choose size of display window
    int height = 50;
    int width = 50;

    int starty = (getmaxy(stdscr) - height) / 2;
    int startx = (getmaxx(stdscr) - width) / 2;

    WINDOW *life_window = create_window(height, width, starty, startx);

    /* create and initialize the grid */
    bool **grid = create_grid(height, width);
    bool **buf = create_grid(height, width);

    /* sample oscillator to make sure grid logic is working */
    int width_mid = width / 2;
    int height_mid = height / 2;

    grid[height_mid][width_mid-1] = ALIVE;
    grid[height_mid][width_mid] = ALIVE;
    grid[height_mid][width_mid+1] = ALIVE;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (grid[i][j] == DEAD)
                mvwaddch(life_window, i, j, DEAD_CHAR);
            else
                mvwaddch(life_window, i, j, ALIVE_CHAR);
        }
    }

    int ch;
    while(1) {
        if ((ch = getch()) == 'q' || ch == 'Q')
            break;
        wrefresh(life_window);
    }

    endwin();
    delwin(life_window);
    free(grid);
    free(buf);
    return 0;
}

/* create a new curses window centered on stdscr */
WINDOW *create_window(int height, int width, int starty, int startx)
{
    WINDOW * win = newwin(height, width, starty, startx);
    box(win, 0, 0);
    wrefresh(win);
    return win;
}

/* allocate memory for a two-dimensional array to be used as a grid */
bool **create_grid(int height, int width)
{
    bool **grid = malloc(height * sizeof(bool *));
    for (int i = 0; i < height; ++i) {
        grid[i] = malloc(width * sizeof(bool *));
        for (int j = 0; j < width; ++j)
            grid[i][j] = false;
    }

    return grid;
}
