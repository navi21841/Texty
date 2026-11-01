#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define NORMAL 0
#define ESCAPE 1
#define CSI 2

struct termios orig;
struct editorConfig
{
    int rows;
    int col;
    int cx;
    int cy;
};
struct editorConfig E;
int state = NORMAL;
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig) == -1)
    {
        perror("tcgetattr");
    }
    struct termios raw = orig;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON | IGNCR | INLCR | PARMRK);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag &= ~(CSIZE | PARENB);
    raw.c_cflag |= CS8;
    raw.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw))
    {
        perror("tcsetattr");
    }
}

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &orig))
    {
        perror("tcsetattr");
    }
}

void getWindowSize(){
    struct winsize ws;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
    {
        perror(("GetWindowSize"));
        exit(1);
    }
    E.rows = ws.ws_row;
    E.col = ws.ws_col;
}

void clearscreen()
{
    printf("\x1b[2J\x1b[H");
    fflush(stdout);
}

void moveCursor(int row , int col){
    printf("\x1b[%d;%dH", row + 1, col + 1);
    fflush(stdout);
}


void processKeypress(char c)
{
    switch (state)
    {
    case NORMAL:
        if ((unsigned char)c == 27)
        {
            /* code */
            state = ESCAPE;
        }
        else
        {
            printf("\r\nOrdinary key: %c\r\n", c);
        }
        break;
    case ESCAPE:
        if (c == '[')
        {
            state = CSI;
        }
        else
        {
            state = NORMAL;
        }
        break;
    case CSI:
        switch (c)
        {
        case 'A':
            // printf("Up Key\r\n");
            if (E.cy > 0)
            {
                E.cy--;
                moveCursor(E.cy, E.cx);
            }
            break;
        case 'B':
            // printf("Down Key\r\n");
            if (E.cy < E.rows - 1)
            {
                E.cy++;
                moveCursor(E.cy, E.cx);

            }
            break;
        case 'C':
            // printf("Right Key\r\n");
            if (E.cx < E.col - 1)
            {
                E.cx++;
                moveCursor(E.cy, E.cx);

            }
            break;
        case 'D':
            // printf("Left Key\r\n");
            if (E.cx > 0)
            {
                E.cx--;
                moveCursor(E.cy, E.cx);

            }
            break;
        default:
            printf("Unknown Key\r\n");
        }
        state = NORMAL;
        break;
    }
}
void readChar()
{
    char c;
    ssize_t n;
    while (1)
    {
        n = read(STDIN_FILENO, &c, 1);

        if (n == 1)
        {
            if (c == 'q')
            {
                /* code */
                break;
            }
            // printf("%u\r\n", (unsigned char)c);
            processKeypress(c);
            fflush(stdout);
        }
    }
    fflush(stdout);
}

int main()
{
    enableRawMode();
    getWindowSize();
    clearscreen();
    readChar();
    disableRawMode();
    return 0;
}
