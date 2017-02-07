#include <ncurses.h>

int main(int argc, char *argv[])
{
    /* initialize ncurses */
    initscr();
    cbreak();
    noecho();

    getch();

    endwin();
    return 0;
}
