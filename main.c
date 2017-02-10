#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#define DEAD 0
#define ALIVE 1

#define DEAD_CHAR ' '
#define ALIVE_CHAR '#'

#define MAX_WIDTH 1000

WINDOW *create_window(int height, int width, int starty, int startx);
bool **create_grid(int height, int width);
void set_initial_state(bool **grid, int height, int width, FILE *fp);

int count_live_neighbors(bool **grid, int y, int x, int height, int width);
void print_frame(WINDOW *win, bool **grid);
void step(bool **grid, bool **buf, int height, int width);
void buf_swap(bool **grid, bool **buf, int height, int width);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Error: not enough arguments\n");
        exit(1);
    }

    FILE *init_file = fopen(argv[1], "r");
    if (init_file == NULL) {
        printf("Error opening input file\n");
        exit(2);
    }

    /* initialize ncurses */
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, 1);

    char dimensions[MAX_WIDTH];
    int main_window_height, main_window_width, steps;

    fgets(dimensions, MAX_WIDTH, init_file);
    dimensions[strlen(dimensions)-1] = '\0';  /* remove newline left by fgets */
    sscanf(dimensions, "%d %d %d", &steps, &main_window_width, &main_window_height);

    /* starting coordinates for the display window */
    int starty = (getmaxy(stdscr) - main_window_height) / 2;
    int startx = (getmaxx(stdscr) - main_window_width) / 2;

    WINDOW *main_window = create_window(main_window_height, main_window_width, starty, startx);
    bool **grid = create_grid(main_window_height, main_window_width);
    bool **buf = create_grid(main_window_height, main_window_width);

    set_initial_state(grid, main_window_height, main_window_width, init_file);
    fclose(init_file);

    /* main loop */
    int ch;
    struct timespec tm = {0, 200000000L};
    while(1) {
        if ((ch = getch()) == 'q' || ch == 'Q')
            break;
        print_frame(main_window, grid);
        step(grid, buf, main_window_height, main_window_width);
        buf_swap(grid, buf, main_window_height, main_window_width);
        nanosleep(&tm, NULL);
    }

    endwin();
    delwin(main_window);
    free(grid);
    free(buf);
}

/* create a new ncurses window, display it, and return a pointer to it */
WINDOW *create_window(int height, int width, int starty, int startx)
{
    WINDOW * win = newwin(height, width, starty, startx);
    box(win, 0, 0);
    wrefresh(win);
    return win;
}

/* allocate memory for a two-dimensional array and return a double pointer to it */
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

/* read fp and set grid to match */
void set_initial_state(bool **grid, int height, int width, FILE *fp)
{
    char line[width];

    /* if formatted properly, the grid should start at line 3 in the input file,
     * so we'll skip the first two lines */
    rewind(fp);
    fgets(line, MAX_WIDTH, fp);
    fgets(line, MAX_WIDTH, fp);

    int row = 0;
    while (row < height && fgets(line, MAX_WIDTH, fp)) {
        for (int col = 0; col < width; ++col)
            line[col] == ALIVE_CHAR ? (grid[row][col] = ALIVE) : (grid[row][col] = DEAD);
        ++row;
    }
}

/* check the value of the eight cells adjacent to the one at (y,x) and return
 * the number of live cells */
int count_live_neighbors(bool **grid, int y, int x, int height, int width)
{
    int live_neighbors = 0;

    for (int row = y-1; row <= y+1; ++row) {
        for (int col = x-1; col <= x+1; ++col) {
            if (row == y && col == x)   /* ignore the given cell */
                continue;

            /* we need to add the height/width and divide to account for cells
             * at the edge of the grid and get a "wrapping" effect */
            if (grid[(row + height) % height][(col + width) % width] == ALIVE)
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

void step(bool **grid, bool **buf, int height, int width)
{
    int live_neighbors;
    bool cell;

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            cell = grid[row][col];
            live_neighbors = count_live_neighbors(grid, row, col, height, width);

            if (cell == ALIVE && live_neighbors < 2)
                buf[row][col] = DEAD;
            else if (cell == ALIVE && (live_neighbors == 2 || live_neighbors == 3))
                buf[row][col] = ALIVE;
            else if (cell == ALIVE && live_neighbors > 3)
                buf[row][col] = DEAD;
            else if (cell == DEAD && live_neighbors == 3)
                buf[row][col] = ALIVE;
        }
    }
}

/* copy the contents of buf into grid and clear buf */
void buf_swap(bool **grid, bool **buf, int height, int width)
{
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            grid[row][col] = buf[row][col];
            buf[row][col] = DEAD;
        }
    }
}
