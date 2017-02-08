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

int count_live_neighbors(bool **grid, int y, int x);
void print_frame(WINDOW *win, bool **grid);

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

    /* main loop */
    int ch;
    while(1) {
        if ((ch = getch()) == 'q' || ch == 'Q')
            break;

        print_frame(life_window, grid);
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

/* check the value of the eight cells adjacent to the one at (y,x) and return
 * the number of live cells */
int count_live_neighbors(bool **grid, int y, int x)
{
    int live_neighbors = 0;

    for (int row = y-1; row <= y+1; ++row) {
        for (int col = x-1; col <= x+1; ++col) {
            if (row == y && col == x)   /* ignore the given cell */
                continue;
            if (grid[row][col] == ALIVE)
                ++live_neighbors;
        }
    }
    return live_neighbors;
}

/* write the contents of grid to win and display win to the user */
void print_frame(WINDOW *win, bool **grid)
{
    int height, width;
    werase(win);
    getmaxyx(win, height, width);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (grid[i][j] == ALIVE)
                mvwaddch(win, i, j, ALIVE_CHAR);
            else
                mvwaddch(win, i, j, DEAD_CHAR);
        }
    }
    wrefresh(win);
}
