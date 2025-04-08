#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

using namespace std;

bool gameOver;
const int width = 20;
const int height = 20;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

void Setup()
{
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
}

void Draw()
{
    system("clear");

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width + 2; j++)
        {
            if (j == 0 || j == width + 1)
                cout << "#";
            else if (i == y && j == x + 1)
                cout << "O";
            else if (i == fruitY && j == fruitX + 1)
                cout << "$";
            else
            {
                bool print = false;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j - 1 && tailY[k] == i)
                    {
                        cout << "o";
                        print = true;
                        break;
                    }
                }
                if (!print)
                    cout << " ";
            }
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    cout << "Score: " << score << endl;
    if (dir == STOP)
        cout << "Press W/A/S/D to start..." << endl;
}

bool kbhit()
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return true;
    }

    return false;
}

char getInput()
{
    return getchar();
}

void Input()
{
    if (kbhit())
    {
        char ch = getInput();
        switch (ch)
        {
        case 'a':
            if (dir != RIGHT) dir = LEFT;
            break;
        case 'd':
            if (dir != LEFT) dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN) dir = UP;
            break;
        case 's':
            if (dir != UP) dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}

void Logic()
{
    if (dir == STOP)
        return;

    if (x == fruitX && y == fruitY)
    {
        score++;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
    }

    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;

    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir)
    {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }

    // teleport przez ściany
    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    // kolizja z ogonem
    for (int i = 0; i < nTail; i++)
    {
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;
    }
}

int main()
{
    srand(time(0));
    Setup();

    const useconds_t delay = 150000;  // 150 ms — prędkość węża

    while (!gameOver)
    {
        Draw();
        Input();
        Logic();
        usleep(delay);
    }

    cout << "Game Over! Final Score: " << score << endl;
    return 0;
}
