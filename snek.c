/////////////////////////////////////////////// LIB
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

/////////////////////////////////////////////// LIB

/////////////////////////////////////////////// I/O
#define LED_BASE 0xFF200000
// 9 - 0 Red LEDs

#define KEY_BASE 0xff200050
// DATA REG
// DIRECTION REG
// INTERRUPT MASK REG
// EDGE CAPTURE REG

#define SW_BASE 0xFF200040
// 9 - 0 SWITCHES

// 7 Seg Display
#define ADDR_7SEG0 ((volatile long *)0xFF200020)
#define ADDR_7SEG1 ((volatile long *)0xFF200030)

#define HEX_ZERO 0b0111111  //
#define HEX_ONE 0b0000110   // 1
#define HEX_TWO 0b1011011   // 2
#define HEX_THREE 0b1001111 // 3
#define HEX_FOUR 0b1100110  // 4
#define HEX_FIVE 0b1101101  // 5
#define HEX_SIX 0b1111101   // 6
#define HEX_SEVEN 0b0000111 // 7
#define HEX_EIGHT 0b1111111 // 8
#define HEX_NINE 0b1101111  // 9
#define HEX_EMPTY 0b0000000 // 9

// Pointers
volatile int *pLED = (int *)LED_BASE;
volatile int *pKEY = (int *)KEY_BASE;
volatile int *pSW = (int *)SW_BASE;

///////////////////////////////////////////////

/////////////////////////////////////////////// AUDIO
#define AUDIO_BASE 0xFF203040
// CONTROL      WI RI ... CW CR WE RE
// FIFOSPACE    WSLC WSRC RALC RARC
// LEFT DATA
// RIGHT DATA

///////////////////////////////////////////////

/////////////////////////////////////////////// VGA
#define PIXEL_BUFFER 0xFF203020

#define WIDTH 320
#define HEIGHT 240

#define RED 0xF800
#define WHITE 0xFFFF

#define BLACK 0
#define CLEAR BLACK

#define SECOND 10000000

int pixel_buffer_start; // global variable
volatile int *pixel_ctrl_ptr = (int *)PIXEL_BUFFER;

// Prototypes
void wait_for_vsync();
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void delay(int duration);
///////////////////////////////////////////////

////////////////////////////////////////// PS/2
#define PS2_BASE 0xFF200100 // Address required for DE1-SOC

// Arrow Keys
#define LEFT_KEY 0x6B
#define DOWN_KEY 0x72
#define RIGHT_KEY 0x74
#define UP_KEY 0x75
#define BREAK 0xF0

volatile int *pPS2 = (int *)PS2_BASE;

//////////////////////////////////////////

////////////////////////////////////////// GAME CONSTANTS

#define TRUE 1
#define FALSE 0

#define GAME_WIDTH 50
#define GAME_HEIGHT GAME_WIDTH
#define DELAY 900000

#define MAX_SNAKE_LENGTH GAME_WIDTH *GAME_WIDTH
#define STARTING_LENGTH 5
#define SCALE 6
#define RANDOM_RANGE 5

int headX = 0;
int headY = 0;

int dirX = 0;
int dirY = 0;

int acceptInput = TRUE;
int snakeLength = 1;

int score = 0;

struct cell
{
    int x;
    int y;
    int active;
    int dirX;
    int dirY;
};

struct cell snake[MAX_SNAKE_LENGTH];

// int snake[MAX_SNAKE_LENGTH] = {0};
/////////////////////////////////////// SPRITES
unsigned int snake_head_red[6][6] = {
    {
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0x0262,
    },
    {
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0x0841,
        0xE8E4,
        0x0262,
    },
    {
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0xFD16,
    },
    {
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0xFD16,
    },
    {
        0xE8E4,
        0xE8E4,
        0x0841,
        0xE8E4,
        0xE8E4,
        0x0262,
    },
    {
        0xe8e4,
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0xE8E4,
        0x0262,
    },

};

unsigned int snake_body_red[6][6] = {
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262}, {0x0262, 0xFF80, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262}, {0x0262, 0xE8E4, 0xFF80, 0xE8E4, 0xE8E4, 0x0262}, {0x0262, 0xE8E4, 0xE8E4, 0xFF80, 0xE8E4, 0x0262}, {0x0262, 0xE8E4, 0xE8E4, 0xE8E4, 0xFF80, 0x0262}, {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262}

};
//////////////////////////////////////////

void input()
{
    unsigned char byte1 = 0, byte2 = 0, byte3 = 0;
    int previousKey = byte2;
    int PS2_data, RVALID;

    // Handle key input via polling.

    PS2_data = *(pPS2);           // read the Data register in the PS/2 port
    RVALID = (PS2_data & 0x8000); // extract the RVALID field

    if (RVALID != 0)
    {
        byte3 = PS2_data & 0xFF;
    }

    if (byte3 == BREAK)
    {
        acceptInput = TRUE; // Wait for a break before accepting input.
    }
    else if (byte3 == LEFT_KEY && acceptInput)
    {
        acceptInput = FALSE;
        if (!dirY) return; //to avoid going back in the opposite direction

        // printf("LEFT KEY\n");

        dirX = -1;
        dirY = 0;
    }
    else if (byte3 == RIGHT_KEY && acceptInput)
    {
        acceptInput = FALSE;
         if (!dirY) return; //to avoid going back in the opposite direction

        // printf("RIGHT KEY\n");

        dirX = 1;
        dirY = 0;
    }
    else if (byte3 == UP_KEY && acceptInput)
    {
        acceptInput = FALSE;
         if (!dirX) return; //to avoid going back in the opposite direction

        // printf("UP KEY\n");

        dirX = 0; // Set direction
        dirY = -1;
    }
    else if (byte3 == DOWN_KEY && acceptInput)
    {
        acceptInput = FALSE;
         if (!dirX) return; //to avoid going back in the opposite direction

        // printf("DOWN KEY\n");

        dirX = 0;
        dirY = 1;
    }
}

void delay(int duration)
{
    while (duration > 0)
    {
        duration--;
    }
}

void wait_for_vsync()
{
    *pixel_ctrl_ptr = 1;
    int status = *(pixel_ctrl_ptr + 3);

    while ((status & 0x01) != 0)
    {
        status = *(pixel_ctrl_ptr + 3);
    }
}

void clear_screen()
{
    volatile short int *one_pixel_address;

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1); // Do not cast.
            *one_pixel_address = 0;
        }
    }
}

void plot_pixel(int x, int y, short int line_color)
{
    short int *one_pixel_address;
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
    *one_pixel_address = line_color;
}

void plot_pixel_delay(int x, int y, short int line_color, int delay)
{
    short int *one_pixel_address;
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
    *one_pixel_address = line_color;
    int duration = delay;
    while (duration > 0)
    {
        duration--;
    }
}

void boxBuilder(int startX, int startY, const int LENGTH, const int COLOUR)
{
    // Draw a (LENGTH x LENGTH) Box.
    for (int j = 0; j < LENGTH; ++j)
    {
        for (int k = 0; k < LENGTH; ++k)
        {
            plot_pixel(startX * LENGTH + k, startY * LENGTH + j, COLOUR);
        }
    }
}

void drawSnake(int startX, int startY, const int LENGTH, unsigned int snake[6][6], int directionX, int directionY)
{

    // flip sprites based on direction
    if (directionX == -1)
    { // flip the sprites by 180 degrees (left direction)
        for (int j = 0; j < LENGTH; ++j)
        {
            for (int k = 0; k < LENGTH; ++k)
            {
                if (snake[j][k] == 0x0262)
                {
                    continue;
                }
                plot_pixel(startX * LENGTH + k, startY * LENGTH + j, snake[LENGTH - 1 - j][LENGTH - 1 - k]);
            }
        }
    }
    else if (directionX == 1)
    {
        // Draw a (LENGTH x LENGTH) Box.
        for (int j = 0; j < LENGTH; ++j)
        {
            for (int k = 0; k < LENGTH; ++k)
            {
                if (snake[j][k] == 0x0262)
                {
                    continue;
                }
                plot_pixel(startX * LENGTH + k, startY * LENGTH + j, snake[j][k]);
            }
        }
    }
    else if (directionY == -1) //90 degree anticlockwise rotation of sprite
    {
        // Draw a (LENGTH x LENGTH) Box.
        for (int j = 0; j < LENGTH; ++j)
        {
            for (int k = 0; k < LENGTH; ++k)
            {
                if (snake[j][k] == 0x0262)
                {
                    continue;
                }
                plot_pixel(startX * LENGTH + k, startY * LENGTH + j, snake[k][LENGTH - 1 - j]);
            }
        }
    }
    else if (directionY == 1) //90 degree anticlockwise rotation of sprite
    {
        // Draw a (LENGTH x LENGTH) Box.
        for (int j = 0; j < LENGTH; ++j)
        {
            for (int k = 0; k < LENGTH; ++k)
            {
                if (snake[j][k] == 0x0262)
                {
                    continue;
                }
                plot_pixel(startX * LENGTH + k, startY * LENGTH + j, snake[LENGTH - 1 - k][j]);
            }
        }
    }
    else{
        // Draw a (LENGTH x LENGTH) Box.
        for (int j = 0; j < LENGTH; ++j)
        {
            for (int k = 0; k < LENGTH; ++k)
            {
                if (snake[j][k] == 0x0262)
                {
                    continue;
                }
                plot_pixel(startX * LENGTH + k, startY * LENGTH + j, snake[j][k]);
            }
        }
    
    }
}

void borderBuilder(int startX, int startY, const int LENGTH, const int COLOUR, const int delay)
{
    // Draw a (LENGTH x LENGTH) outline.

    // Top -->
    for (int i = 0; i < LENGTH; ++i)
    {
        plot_pixel_delay(startX + i, startY, COLOUR, delay);
    }

    // Right V
    for (int i = 0; i < LENGTH; ++i)
    {
        plot_pixel_delay(startX + (LENGTH - 1), startY + i, COLOUR, delay);
    }

    // Bottom <--
    for (int i = 0; i < LENGTH; ++i)
    {
        plot_pixel_delay(startX + (LENGTH - 1) - i, startY + (LENGTH - 1), COLOUR, delay);
    }

    // Left ^
    for (int i = 0; i < LENGTH; ++i)
    {
        plot_pixel_delay(startX, startY + (LENGTH - 1) - i, COLOUR, delay);
    }
}

// Check if READ FIFO is empty.
bool isReadEmpty()
{
    return ((*(pPS2) & 0xFFFF0000) == 0);
}

void displayHex(int number)
{
    // Max exceeded.
    if (number > 999999)
    {
        return;
    }

    int offsetSeg0 = 0;
    int offsetSeg1 = 0;

    int hexDataSeg0 = 0;
    int hexDataSeg1 = 0;

    int hexDigit = 0;
    int digit = 0;

    while (number > 0)
    {
        digit = number % 10;

        printf("%d\n", digit);

        if (digit == 0)
        {
            hexDigit = HEX_ZERO;
        }
        else if (digit == 1)
        {
            hexDigit = HEX_ONE;
        }
        else if (digit == 2)
        {
            hexDigit = HEX_TWO;
        }
        else if (digit == 3)
        {
            hexDigit = HEX_THREE;
        }
        else if (digit == 4)
        {
            hexDigit = HEX_FOUR;
        }
        else if (digit == 5)
        {
            hexDigit = HEX_FIVE;
        }
        else if (digit == 6)
        {
            hexDigit = HEX_SIX;
        }
        else if (digit == 7)
        {
            hexDigit = HEX_SEVEN;
        }
        else if (digit == 8)
        {
            hexDigit = HEX_EIGHT;
        }
        else if (digit == 9)
        {
            hexDigit = HEX_NINE;
        }

        // Max 6 digits (6 hex displays)
        // ADDR_7SEG1, ADDR_7SEG1, ADDR_7SEG0, ADDR_7SEG0, ADDR_7SEG0, ADDR_7SEG0
        // First 2 on ADDR_7SEG1, Last 4 on ADDR_7SEG0
        if (offsetSeg0 > 3)
        {
            hexDataSeg1 += hexDigit << (8 * offsetSeg1);
            offsetSeg1++;
        }
        else
        {
            hexDataSeg0 += hexDigit << (8 * offsetSeg0);
            *ADDR_7SEG0 = hexDataSeg0;
            // while(true) {}

            offsetSeg0++;
        }

        number /= 10;
    }

    // Write to hex displays.
    *ADDR_7SEG0 = hexDataSeg0; // HEX2 - HEX0
    *ADDR_7SEG1 = hexDataSeg1; // HEX5 - HEX3

    // hexDataSeg1 += HEX_EMPTY << (8 * offsetSeg1);
    // printf("%d \n", number);
}

// struct mazeCell  =
// {
//     bool UP = false;
//     bool DOWN = false;
//     bool RIGHT = false;
//     bool LEFT = false;
// }

#define MAZE_SIZE 10

int maze[MAZE_SIZE][MAZE_SIZE];

// {x,y}
int dir[4][2] =
    {
        {0, -1}, // North
        {1, 0},  // East
        {0, 1},  // South
        {-1, 0}  // West
};

// if (!maze[currY][currX].UP)
// {
//     newX = currX;
//     newY = currY + 1;
// }
// else if (!maze[currY][currX].DOWN)
// {
//     newX = currX;
//     newY = currY - 1;
// }
// else if (!maze[currY][currX].RIGHT)
// {
//     newX = currX + 1;
//     newY = currY;
// }
// else if (!maze[currY][currX].LEFT)
// {
//     newX = currX - 1;
//     newY = currY;
// }
// else
// {
//     return;
// }

const int UP = 0;
const int RIGHT = 1;
const int DOWN = 2;
const int LEFT = 3;

void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

void draw_line(int x0, int y0, int x1, int y1, int line_color)
{
    int is_steep = (int)abs(y1 - y0) > (int)abs(x1 - x0);

    if (is_steep)
    {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }

    if (x0 > x1)
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax / 2);
    int y = y0;
    int y_step = 1;

    if (y0 < y1)
    {
        y_step = 1;
    }
    else
    {
        y_step = -1;
    }

    for (int x = x0; x <= x1; x++)
    {
        if (is_steep)
        {
            plot_pixel(y, x, line_color);
        }
        else
        {
            plot_pixel(x, y, line_color);
        }

        error = error + deltay;

        if (error > 0)
        {
            y = y + y_step;
            error = error - deltax;
        }
    }
}

// DFS (Recursive Backtracking) Maze Generation
void path(int currX, int currY)
{
    int dirOrder = 0;

    for (int i = 0; i < 4; ++i)
    {
        bool newDirFound = false;
        int randDir;

        while (!newDirFound)
        {
            randDir = rand() % 4;

            if ((dirOrder & (1 << randDir)) == 0)
            {
                dirOrder |= (1u << (randDir));
                newDirFound = true;
            }
        }

        printf("%d\n", randDir);
        int newX = currX + dir[randDir][0];
        int newY = currY + dir[randDir][1];

        if ((0 <= newY && newY < MAZE_SIZE) && (0 <= newX && newX < MAZE_SIZE) && (maze[newY][newX] == 0))
        {
            // Current cell has been visited.
            maze[currY][currX] = 1;

            // // Set ith bit to 1.
            // maze[currY][currX] |= (1u << i);

            // if (i == UP)
            // {
            //     maze[newY][newX] |= (1u << DOWN);
            // }
            // else if (i == DOWN)
            // {
            //     maze[newY][newX] |= (1u << DOWN);
            // }
            // else if (i == RIGHT)
            // {
            //     maze[newY][newX] |= (1u << LEFT);

            // }
            // else if (i == LEFT)
            // {
            //     maze[newY][newX] |= (1u << RIGHT);
            // }
            // plot_pixel(newX * 1, newY * 1, WHITE);

            // for (int x = currX; x != newX * 2; x+= dir[randDir][0])
            // {
            //     plot_pixel(x, newY, WHITE);
            // }

            draw_line(currX * 2, currY * 2, newX * 2, newY * 2, WHITE);

            // plot_pixel(newX * 2, newY * 2, WHITE);
            printf("%d, %d \n", newX, newY);
            path(newX, newY);
        }
    }

    return;
}

void drawMaze()
{
    for (int i = 0; i < MAZE_SIZE; ++i)
    {
        for (int j = 0; j < MAZE_SIZE; ++j)
        {
            maze[i][j] = 0;
        }
    }

    for (int y = 0; y < 20; ++y)
    {
        for (int x = 0; x < 20; ++x)
        {
            plot_pixel(x * 2, y * 2, RED);
        }
    }

    path(0, 0);
}

int main(void)
{
    // Wait for v-sync before writing to pixel buffer.
    wait_for_vsync();

    pixel_buffer_start = *pixel_ctrl_ptr;

    // Clear screen.
    clear_screen();

    // Generate random seed.
    srand(2444);

    // Plot food item.
    int randX = rand() % (RANDOM_RANGE + 1);
    int randY = 0;
    plot_pixel(randX, randY, RED);

    // Start with 1 block snake by default
    // (adjust STARTING_LENGTH for debugging).
    snakeLength = STARTING_LENGTH;
    for (int i = 0; i < snakeLength; ++i)
    {
        snake[i].active;
    }

    // Game loop.
    while (TRUE)
    {
        // Poll for input.
        input();

        printf("X: %d   Y: %d \n", headX, headY);

        // Boundary checks.
        if (headX + dirX < 0 || headX + dirX > GAME_WIDTH)
        {
            dirX = 0;
        }
        if (headY + dirY < 0 || headY + dirY > GAME_HEIGHT)
        {
            dirY = 0;
        }

        // Move head
        headX += dirX;
        headY += dirY;
        snake[0].x = headX;
        snake[0].y = headY;
        snake[0].dirX = dirX;
        snake[0].dirY = dirY;

        // Check for body intersection.
        // Avoid snake from collapsing into itself.

        // Should i = 2?
        //   for (int i = 2; i < snakeLength; i++)
        // {

        //   if (snake[i].x == snake[0].x && snake[i].y == snake[0].y)
        // {
        //   while(true)
        // {
        // printf("%d(%d, %d) %d(%d, %d)", i, snake[i].x, snake[i].y);
        //}
        //}
        //}

        // Update positions of the rest of snake body.
        if (dirX != 0 || dirY != 0)
        {
            for (int i = snakeLength - 1; i > 0; --i)
            {
                snake[i].x = snake[i - 1].x;
                snake[i].y = snake[i - 1].y;
                snake[i].dirX = snake[i - 1].dirX;
                snake[i].dirY = snake[i - 1].dirY;
            }
        }

        // Draw snake
        // for (int i = 0; i < snakeLength; i++){ boxBuilder(snake[i].x, snake[i].y, SCALE, WHITE); }
        for (int i = 0; i < snakeLength; i++)
        {
            if (i == 0 || i == 1)
            {
                drawSnake(headX, headY, SCALE, snake_head_red, snake[i].dirX, snake[i].dirY);
            }
            else
            {
                drawSnake(snake[i].x, snake[i].y, SCALE, snake_body_red, snake[i].dirX, snake[i].dirY);
            }
        }

        // Delay before clearing snake.
        int duration = DELAY;
        while (duration > 0)
        {
            duration--;
        }

        // Clear snake after delay.
        for (int i = 0; i < snakeLength; i++)
        {
            boxBuilder(snake[i].x, snake[i].y, SCALE, CLEAR);
        }

        // Colliding with food.
        if (headX == randX && headY == randY)
        {

            // Generate new food position.
            randX = rand() % (RANDOM_RANGE + 1);
            randY = 1;

            // Increase snake length.
            snakeLength++;
            snake[snakeLength - 1].active = TRUE; // Set cell to active.

            // Update score.
            score = snakeLength - 1;

            // Display score on HEX displays and LEDs.
            *pLED = score;
            displayHex(score);
        }

        // Draw current food.
        boxBuilder(randX, randY, SCALE, RED);
    }
}

// PS2 Interrupts // in the DE1-SOC docs
// Maze Generation
// Scaling -- DONE ?
// Double buffering
// Better random number generation
// Build border -- DONE.
// Border waves.
// Maze generation.
// Rotation (rotate game 90 deg every X seconds).
// Check if read FIFO is full. -- DONE.
// Boss Mode.
// Invert mode.
// Translate board.
// Platformer
// Left Shift Back Register for random number generation.
// LED score counter
// Settings
// Audio
// Sprite Animation?
// Floor animation
// Random Seeding with Switches?

/////////////////////////////////////////////////// MISC SNIPPETS

// Mouse.
// if ( (byte2 == 0xAA) && (byte3 == 0x00) )
// {
// *(PS2_ptr) = 0xF4;
// }
