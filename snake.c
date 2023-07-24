#include <Windows.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 50
#define CELL_SIZE 10

static int score=0;
HFONT hFont;
static int fontSize = 30;

struct Snake
{
    int x, y;
    int bodyX[GRID_SIZE * GRID_SIZE];
    int bodyY[GRID_SIZE * GRID_SIZE];
};

struct Food
{
    int x, y;
};

struct Snake snake;
struct Food food;
int direction;
int gameOver;
int length = 4;

void SetupGame()
{
    snake.x = GRID_SIZE / 2;
    snake.y = GRID_SIZE / 2;

    food.x = rand() % 30;
    food.y = rand() % 30;

    direction = 1;
    gameOver = 0;
}

void HandleInput()
{
    if (GetAsyncKeyState(VK_UP) && direction != 2)
        direction = 0;
    else if (GetAsyncKeyState(VK_RIGHT) && direction != 3)
        direction = 1;
    else if (GetAsyncKeyState(VK_DOWN) && direction != 0)
        direction = 2;
    else if (GetAsyncKeyState(VK_LEFT) && direction != 1)
        direction = 3;
}

void Update()
{
    switch (direction)
    {
        case 0: snake.y--; break;
        case 1: snake.x++; break;
        case 2: snake.y++; break;
        case 3: snake.x--; break;
    }

    // Increase the snake's length

    if (snake.x == food.x && snake.y == food.y)
    {
        length++;
        score+=10;

        // Move the food to a new random position
        food.x = rand() % 30;
        food.y = rand() % 20;
    }

    // Check for collisions with the window boundaries
    if (snake.x < 0 || snake.x >= GRID_SIZE-3 || snake.y < 0 || snake.y >= GRID_SIZE-5)
    {
        gameOver = 1;
    }

    // Check for collisions with the snake's body
    for (int i = 1; i < length; i++)
    {
        if (snake.x == snake.bodyX[i] && snake.y == snake.bodyY[i])
        {
            gameOver = 1;
            break;
        }
    }

    // Move the rest of the snake's body
    for (int i = length - 1; i > 0; i--)
    {
        snake.bodyX[i] = snake.bodyX[i - 1];
        snake.bodyY[i] = snake.bodyY[i - 1];
    }

    // Update the position of the snake's head in the body array
    snake.bodyX[0] = snake.x;
    snake.bodyY[0] = snake.y;
}

void Draw(HDC hdc)
{
    // Clear the window
    RECT rect;
    GetClientRect(GetConsoleWindow(), &rect);
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

    SetDCBrushColor(hdc, RGB(39, 43, 45));
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(DC_BRUSH));

    // Create a green pen for the snake
    HPEN hSnakePen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    HBRUSH hSnakeBrush = CreateSolidBrush(RGB(0, 255, 0));


    // Select the green pen and brush into the device context
    HPEN hOldPen = (HPEN)SelectObject(hdc, hSnakePen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hSnakeBrush);


    // Draw the snake's body

    for (int i = 0; i < length; i++)
    {
        Rectangle(hdc,
                  snake.bodyX[i] * CELL_SIZE, snake.bodyY[i] * CELL_SIZE,
                  (snake.bodyX[i] + 1) * CELL_SIZE, (snake.bodyY[i] + 1) * CELL_SIZE);
    }

    // Clean up the resources for the snake
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hSnakePen);
    DeleteObject(hSnakeBrush);

    // Draw the food

    // Create a red pen and brush for the food
    HPEN hFoodPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HBRUSH hFoodBrush = CreateSolidBrush(RGB(255, 128, 128));

    // Select the red pen and brush into the device context
    hOldPen = (HPEN)SelectObject(hdc, hFoodPen);
    hOldBrush = (HBRUSH)SelectObject(hdc, hFoodBrush);

    SetDCPenColor(hdc, RGB(255, 255, 0)); // Yellow color
    SetDCBrushColor(hdc, RGB(255, 255, 0));

    Rectangle(
              hdc,
              food.x * CELL_SIZE,
              food.y * CELL_SIZE,
              (food.x + 1) * CELL_SIZE,
              (food.y + 1) * CELL_SIZE);


              hFont = CreateFont(
                    fontSize,  // Height of the font
                    0,         // Width of the font (0 for default)
                    0,         // Angle of escapement (0 for default)
                    0,         // Orientation angle (0 for default)
                    FW_NORMAL, // Font weight (FW_NORMAL, FW_BOLD, etc.)
                    FALSE,     // Italic (TRUE or FALSE)
                    FALSE,     // Underline (TRUE or FALSE)
                    FALSE,     // Strikeout (TRUE or FALSE)
                    DEFAULT_CHARSET, // Character set identifier (DEFAULT_CHARSET for default)
                    OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY,
                    DEFAULT_PITCH | FF_DONTCARE,
                    "Arial"    // Font name (use "Arial" or any other font name)
                );

                    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);


                SetBkMode(hdc, TRANSPARENT); // Set the background mode to transparent for the text
                SetTextColor(hdc, RGB(255, 0, 0)); // Set the text color to black

                char scoreText[20];
                snprintf(scoreText, sizeof(scoreText), "Score: %d", score); // Convert the score to a string
                TextOut(hdc, 200, 430, scoreText, strlen(scoreText));

                SelectObject(hdc, hOldFont);
                DeleteObject(hFont);

}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    srand(time(NULL));

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SnakeWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    // Create the window
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate the center position for the window
    int windowWidth = GRID_SIZE * CELL_SIZE;
    int windowHeight = GRID_SIZE * CELL_SIZE;
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;

    // Create the window
    HWND hwnd = CreateWindowEx(
                               0,
                               "SnakeWindowClass",
                               "Snake Game",
                               WS_OVERLAPPEDWINDOW,
                               windowX,
                               windowY,
                               windowWidth,
                               windowHeight,
                               NULL,
                               NULL,
                               hInstance,
                               NULL
                               );
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    SetupGame();

    // Game loop
    MSG msg;
    while (!gameOver)
    {
        // Handle input
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                gameOver = 1;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Update game state
        HandleInput();
        Update();

        // Render
        HDC hdc = GetDC(hwnd);
        Draw(hdc);
        ReleaseDC(hwnd, hdc);

        Sleep(80);
    }

    if(gameOver){
        printf("You loose..");
        printf("\nYour score : %d",score);
    }

    return 0;
}
