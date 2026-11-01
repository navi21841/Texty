#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define NORMAL 0
#define ESCAPE 1
#define CSI 2

struct termios orig;

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
            printf("Up Key\r\n");
            break;
        case 'B':
            printf("Down Key\r\n");
            break;
        case 'C':
            printf("Right Key\r\n");
            break;
        case 'D':
            printf("Left Key\r\n");
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
    readChar();
    disableRawMode();
    return 0;
}
