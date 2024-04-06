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
#define SHADING_OFFSET 10305

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

#define GAME_WIDTH 20
#define GAME_HEIGHT GAME_WIDTH
#define DELAY 900000

#define MAX_SNAKE_LENGTH GAME_WIDTH *GAME_WIDTH
#define STARTING_LENGTH 2
#define SCALE 9
#define RANDOM_RANGE 5
	
// Center point
#define CENTER_X  (int) WIDTH / 2
#define CENTER_Y  (int) HEIGHT / 2

// Offset center point for drawing border.
// Game border can be completely defined by two points
// (TOP_LEFT_X, TOP_LEFT_Y) (BOTTOM_RIGHT_X, bottomRightY)
#define TOP_LEFT_X    (int) (CENTER_X - (GAME_WIDTH / 2) * SCALE)
#define TOP_LEFT_Y    (int) (CENTER_Y - (GAME_HEIGHT / 2) * SCALE)
        
#define BOTTOM_RIGHT_X  (int) (CENTER_X + (GAME_WIDTH / 2) * SCALE)
#define BOTTOM_RIGHT_Y  (int) (CENTER_Y + (GAME_HEIGHT / 2) * SCALE)

int headX = 0;
int headY = 0;

int dirX = 0;
int dirY = 0;

int acceptInput = TRUE;
int snakeLength = 1;
int frame = 0;
bool offsetEven = true;
bool offsetOdd = false;
int score = 0;
int fruitIdx = 1;

double workingRadius = 0;
double animationRadius;
bool showAnimation = false;
double animationX = 0;
double animationY = 0;

struct cell
{
    int x;
    int y;
    int active;
    int dirX;
    int dirY;
    unsigned short int colour;
};

struct cell snake[MAX_SNAKE_LENGTH];

unsigned int fruits[5][9][9] = {
   {
    {0x0262, 0x0262, 0xBB01, 0xFBE0, 0x2589, 0xFBE0, 0x0262, 0x0262, 0x0262},
    {0x0262, 0xBB01, 0xFBE0, 0x2589, 0x2589, 0x2589, 0xFBE0, 0x0262, 0x0262},
    {0xBB01, 0xFBE0, 0xFBE0, 0xFBE0, 0x2589, 0xFBE0, 0xFBE0, 0xFBE0, 0x0262},
    {0xBB01, 0xFBE0, 0xFBE0, 0xFBE0, 0xFBE0, 0xFBE0, 0xF58E, 0xFBE0, 0x0262},
    {0xBB01, 0xFBE0, 0xFBE0, 0xFBE0, 0xFBE0, 0xFBE0, 0xF58E, 0xFBE0, 0x0262},
    {0xBB01, 0xFBE0, 0xFBE0, 0xFBE0, 0xFBE0, 0xF58E, 0xFBE0, 0xFBE0, 0x0262},
    {0x0262, 0xBB01, 0xFBE0, 0xFBE0, 0xFBE0, 0xFBE0, 0xFBE0, 0x0262, 0x0262},
    {0x0262, 0x0262, 0xBB01, 0xBB01, 0xFBE0, 0xFBE0, 0x0262, 0x0262, 0x0262},
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262},
},
{
    {0x1C27, 0xF720, 0xE8E4, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262},
    {0x1C27, 0xF720, 0xE8E4, 0xE8E4, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262},
    {0x1C27, 0xF720, 0xE8E4, 0x0000, 0xE8E4, 0x0262, 0x0262, 0x0262, 0x0262},
    {0x1C27, 0xF720, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262, 0x0262, 0x0262 },
    {0x1C27, 0xF720, 0xE8E4, 0x0000, 0xE8E4, 0x0000, 0xE8E4, 0x0262, 0x0262},
    {0x1C27, 0xF720, 0xF720, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262},
    {0x0262, 0x2589, 0xF720, 0xF720, 0xF720, 0xF720, 0xF720, 0xF720, 0x0262},
    {0x0262, 0x0262, 0x2589, 0x2589, 0x2589, 0x2589, 0x2589, 0x2589, 0x0262},
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262},
    
},
{
    {0x0262, 0x0262, 0x1345, 0x1C27, 0x1C27, 0x1C27, 0x0262, 0x0262, 0x0262},
    {0x0262, 0xC0A3, 0x1345, 0xE8E4, 0xE8E4, 0x1C27, 0xE8E4, 0x0262, 0x0262},
    {0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262},
    {0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xFD16, 0xE8E4, 0xE8E4, 0x0262},
    {0x0262, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xFD16, 0xE8E4, 0x0262, 0x0262},
    {0x0262, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262, 0x0262},
    {0x0262, 0x0262, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262, 0x0262, 0x0262},
    {0x0262, 0x0262, 0x0262, 0xC0A3, 0xE8E4, 0x0262, 0x0262, 0x0262, 0x0262},
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262},
},
{
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x9AC7, 0x9AC7, 0x0262, 0x0262},
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0xF720, 0xF720, 0xBD40, 0x0262},
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0xF720, 0xF720, 0xBD40, 0x0262},
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0xF720, 0xF720, 0xBD40, 0x0262 },
    {0x0262, 0x0262, 0x0262, 0x0262, 0xF720, 0xF720, 0xBD40, 0xBD40, 0x0262},
    {0x0262, 0x0262, 0x0262, 0xF720, 0xF720, 0xBD40, 0xBD40, 0x0262, 0x0262},
    {0x0262, 0x0262, 0xF720, 0xF720, 0xBD40, 0xBD40, 0x0262, 0x0262, 0x0262},
    {0x0262, 0xF720, 0xF720, 0xBD40, 0xBD40, 0x0262, 0x0262, 0x0262, 0x0262},
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262},
},
{
    {0x0262, 0x0262, 0x0262, 0x9AC7, 0x0262, 0x2589, 0x0262, 0x0262, 0x0262 },
    {0x0262, 0x8846, 0x7825, 0x9AC7, 0x2589, 0x7825, 0xB889, 0x0262, 0x0262},
    {0x8846, 0x8846, 0x9806, 0xB889, 0xB889, 0x8846, 0x9806, 0x9806, 0x0262},
    {0x8846, 0x9806, 0x9806, 0x9806, 0x9806, 0x9806, 0xF1AE, 0x9806, 0x0262},
    {0x8846, 0x9806, 0x9806, 0x9806, 0x9806, 0xF1AE, 0xF1AE, 0x9806, 0x0262},
    {0x8846, 0x8846, 0x9806, 0x9806, 0x9806, 0xF1AE, 0xF1AE, 0x9806, 0x0262},
    {0x0262, 0x8846, 0x7825, 0x7825, 0x9806, 0x9806, 0x9806, 0x0262, 0x0262},
    {0x0262, 0x0262, 0x7825, 0x7825, 0x7825, 0x7825, 0x0262, 0x0262, 0x0262},
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262},
}

};

unsigned int fruit_color[5] = {
     0xFBE0, 0x1C27,  0xE8E4, 0xF720, 0x9806
};
// int snake[MAX_SNAKE_LENGTH] = {0};
/////////////////////////////////////// SPRITES
unsigned int snake_body_red[9][9] = {
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, },
    {0x0262, 0x0262, 0x0262, 0xC0A3, 0xC0A3, 0xC0A3, 0x0262, 0x0262, 0x0262, },
    {0x0262, 0x0262, 0xC0A3, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262, 0x0262, },
    {0x0262, 0xC0A3, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262, },
    {0x0262, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262, },
    {0x0262, 0xC0A3, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262, },
    {0x0262, 0x0262, 0xC0A3, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262, 0x0262, },
    {0x0262, 0x0262, 0x0262, 0xC0A3, 0xC0A3, 0xC0A3, 0x0262, 0x0262, 0x0262, },
    {0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, 0x0262, },
};

unsigned int snake_head_red[9][9] = {
     {0x0262, 0x0262, 0xC0A3, 0xC0A3, 0xC0A3, 0xC0A3, 0xC0A3, 0x0262, 0x0262, },
    {0x0262, 0xC0A3, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262, },
    {0xC0A3, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0841, 0x0841, 0xE8E4, },
    {0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, },
    {0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, },
    {0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0000, 0x0841, 0xE8E4, },
    {0xC0A3, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, },
    {0x0262, 0xC0A3, 0xC0A3, 0xC0A3, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0262, },
    {0x0262, 0x0262, 0xC0A3, 0xC0A3, 0xC0A3, 0xC0A3, 0xC0A3, 0x0262, 0x0262, },
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
       

        // printf("LEFT KEY\n");

        dirX = -1;
        dirY = 0;
    }
    else if (byte3 == RIGHT_KEY && acceptInput)
    {
        acceptInput = FALSE;
       

        // printf("RIGHT KEY\n");

        dirX = 1;
        dirY = 0;
    }
    else if (byte3 == UP_KEY && acceptInput)
    {
        acceptInput = FALSE;
        

        // printf("UP KEY\n");

        dirX = 0; // Set direction
        dirY = -1;
    }
    else if (byte3 == DOWN_KEY && acceptInput)
    {
        acceptInput = FALSE;
       

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

void boxBuilder(int startX, int startY, const int LENGTH, const int COLOUR, int OFFSET_X, int OFFSET_Y)
{
    // Draw a (LENGTH x LENGTH) Box.
    for (int j = 0; j < LENGTH; ++j)
    {
        for (int k = 0; k < LENGTH; ++k)
        {
            plot_pixel(startX * LENGTH + k + OFFSET_X, startY * LENGTH + j + OFFSET_Y, COLOUR);
        }
    }
}

void drawSnake(int startX, int startY, const int LENGTH, unsigned int snake[9][9], int directionX, int directionY, int offset, int color)
{

    // flip sprites based on direction
    if (directionX == -1)
    { // flip the sprites by 180 degrees (left direction)
        for (int j = 0; j < LENGTH; ++j)
        {
            for (int k = 0; k < LENGTH; ++k)
            {
                if (snake[LENGTH - 1 - j][LENGTH - 1 - k] == 0x0262)
                {
                    continue;
                }
                if (snake[LENGTH - 1 - j][LENGTH - 1 - k] ==  0xE8E4)
                {
                    plot_pixel(startX * LENGTH + k + TOP_LEFT_X, startY * LENGTH + j + offset + TOP_LEFT_Y, color);
                }
                else if (snake[LENGTH - 1 - j][LENGTH - 1 - k] == 0xC0A3){
                    plot_pixel(startX * LENGTH + k + TOP_LEFT_X, startY * LENGTH + j + offset + TOP_LEFT_Y, color-SHADING_OFFSET);
                }
                else
                {
                    plot_pixel(startX * LENGTH + k + TOP_LEFT_X, startY * LENGTH + j + offset + TOP_LEFT_Y, snake[LENGTH - 1 - j][LENGTH - 1 - k]);
                }
              //  plot_pixel(startX * LENGTH + k , startY * LENGTH + j + offset, snake[LENGTH - 1 - j][LENGTH - 1 - k]);
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
                if (snake[j][k] ==  0xE8E4)
                {
                    plot_pixel(startX * LENGTH + k + TOP_LEFT_X, startY * LENGTH + j + offset + TOP_LEFT_Y, color);
                }
                else if (snake[j][k] == 0xC0A3){
                    plot_pixel(startX * LENGTH + k + TOP_LEFT_X, startY * LENGTH + j + offset + TOP_LEFT_Y, color-SHADING_OFFSET);
                }
                else
                {
                    plot_pixel(startX * LENGTH + k + TOP_LEFT_X, startY * LENGTH + j + offset + TOP_LEFT_Y, snake[j][k]);
                }
                //plot_pixel(startX * LENGTH + k, startY * LENGTH + j + offset,  snake[j][k]);
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
                if (snake[k][LENGTH - 1 - j] == 0x0262)
                {
                    continue;
                }
                if (snake[k][LENGTH - 1 - j] ==  0xE8E4)
                {
                    plot_pixel(startX * LENGTH + k + offset + TOP_LEFT_X, startY * LENGTH + j + TOP_LEFT_Y, color);
                }
                else if (snake[k][LENGTH - 1 - j] == 0xC0A3){
                    plot_pixel(startX * LENGTH + k + offset + TOP_LEFT_X, startY * LENGTH + j + TOP_LEFT_Y, color-SHADING_OFFSET);
                }
                else
                {
                    plot_pixel(startX * LENGTH + k + offset + TOP_LEFT_X, startY * LENGTH + j + TOP_LEFT_Y, snake[k][LENGTH - 1 - j]);
                }
                //plot_pixel(startX * LENGTH + k + offset, startY * LENGTH + j , snake[k][LENGTH - 1 - j]);
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
                if (snake[LENGTH - 1 - k][j] == 0x0262)
                {
                    continue;
                }
                if (snake[LENGTH - 1 - k][j]==  0xE8E4)
                {
                    plot_pixel(startX * LENGTH + k + offset + TOP_LEFT_X, startY * LENGTH + j + TOP_LEFT_Y, color);
                }
                else if (snake[LENGTH - 1 - k][j] == 0xC0A3){
                    plot_pixel(startX * LENGTH + k + offset + TOP_LEFT_X, startY * LENGTH + j + TOP_LEFT_Y, color-SHADING_OFFSET);
                }
                else
                {
                    plot_pixel(startX * LENGTH + k + offset + TOP_LEFT_X, startY * LENGTH + j + TOP_LEFT_Y, snake[LENGTH - 1 - k][j]);
                }
                //plot_pixel(startX * LENGTH + k +offset, startY * LENGTH + j , snake[LENGTH - 1 - k][j]);
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
                plot_pixel(startX * LENGTH + k + TOP_LEFT_X, startY * LENGTH + j + TOP_LEFT_Y, snake[j][k]);
            }
        }
    
    }
}

void drawFruit (int startX, int startY, const int LENGTH, unsigned int fruit[9][9], int offset)
{
    // Draw a (LENGTH x LENGTH) Box.
    for (int j = 0; j < LENGTH; ++j)
    {
        for (int k = 0; k < LENGTH; ++k)
        {
            if (fruit[j][k] == 0x0262)
            {
                continue;
            }
            plot_pixel(startX * LENGTH + k + TOP_LEFT_X, startY * LENGTH + j +offset + TOP_LEFT_Y, fruit[j][k]);
        }
    }
}

void clearFruit(int startX, int startY, const int LENGTH, int color)
{
    // Draw a (LENGTH x LENGTH) Box.
    for (int j = 0; j < LENGTH; ++j)
    {
        for (int k = 0; k < LENGTH; ++k)
        {
            plot_pixel(startX * LENGTH + k + TOP_LEFT_X, startY * LENGTH + j + TOP_LEFT_Y, color);
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

void drawAnimationSq(int startX, int startY, const int LENGTH, const int COLOUR, const int delay)
{
    // Draw a (LENGTH x LENGTH) outline.

    // Top -->
    for (int i = 0; i < LENGTH; ++i)
    {
        if (startX + i < 240 && startY < 240 && startX + i > 0 && startY > 0)
        {
            plot_pixel_delay(startX + i, startY, COLOUR, delay);
        }
       // plot_pixel_delay(startX + i, startY, COLOUR, delay);
    }

    // Right V
    for (int i = 0; i < LENGTH; ++i)
    {
        if (startX + (LENGTH - 1) < 240 && startY + i < 240 && startX + (LENGTH - 1) > 0 && startY + i > 0 )
        {
            plot_pixel_delay(startX + (LENGTH - 1), startY + i, COLOUR, delay);
        }
       // plot_pixel_delay(startX + (LENGTH - 1), startY + i, COLOUR, delay);
    }

    // Bottom <--
    for (int i = 0; i < LENGTH; ++i)
    {
        if (startX + (LENGTH - 1) - i < 240 && startY + (LENGTH - 1) < 240 && startX + (LENGTH - 1) - i > 0 && startY + (LENGTH - 1) > 0)
        {
            plot_pixel_delay(startX + (LENGTH - 1) - i, startY + (LENGTH - 1), COLOUR, delay);
        }
       // plot_pixel_delay(startX + (LENGTH - 1) - i, startY + (LENGTH - 1), COLOUR, delay);
    }

    // Left ^
    for (int i = 0; i < LENGTH; ++i)
    {
        if (startX < 240 && startY + (LENGTH - 1) - i < 240 && startX > 0 && startY + (LENGTH - 1) - i > 0)
        {
            plot_pixel_delay(startX, startY + (LENGTH - 1) - i, COLOUR, delay);
        }
      //  plot_pixel_delay(startX, startY + (LENGTH - 1) - i, COLOUR, delay);
    }
}

int main(void)
{
    // Wait for v-sync before writing to pixel buffer.
    animationRadius = sqrt((240 * 240) + (240 * 240)) ;
    frame ++;
    wait_for_vsync();

    pixel_buffer_start = *pixel_ctrl_ptr;

    // Clear screen.
    clear_screen();
    
    // Generate random seed.
    srand(2444);
	
    // Draw game border
    borderBuilder(TOP_LEFT_X - 1, TOP_LEFT_Y - 1, (GAME_WIDTH + 1) * (SCALE) + 2, RED, 0);

    // Plot food item.
    int randX = rand() % (RANDOM_RANGE + 1);
    int randY = 0;
    drawFruit(randX, randY, SCALE, fruits[fruitIdx], offsetEven);

    // Start with 1 block snake by default
    // (adjust STARTING_LENGTH for debugging).
    snakeLength = STARTING_LENGTH;
    for (int i = 0; i < snakeLength; ++i)
    {
        snake[i].colour = fruit_color[fruitIdx];
    }

    // Game loop.
    while (TRUE)
    {
        // Center Mark
        plot_pixel(CENTER_X, CENTER_Y, WHITE);

        // Poll for input.
        input();

        printf("X: %d   Y: %d \n", headX, headY);

        // Boundary checks.
        if (headX + dirX < 0 || headX + dirX > GAME_WIDTH){ dirX = 0;}
        if (headY + dirY < 0 || headY + dirY > GAME_WIDTH){ dirY = 0;}

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
        drawFruit(randX, randY, SCALE, fruits[fruitIdx],offsetEven);
        // Draw snake
        for (int i = 0; i < snakeLength; i++){ boxBuilder(snake[i].x, snake[i].y, SCALE, WHITE, TOP_LEFT_X, TOP_LEFT_Y); }
        
        
        // DRAW SNAKE SPRITES
        // for (int i = 0; i < snakeLength; i++)
        // {
        //     if (i == 0 || i == 1)
        //     {
        //         drawSnake(headX, headY, SCALE, snake_head_red, snake[i].dirX, snake[i].dirY, 0 , snake[i].colour);

        //     }
        //     else if (i%2 ==0){
        //         drawSnake(snake[i].x, snake[i].y, SCALE, snake_body_red, snake[i].dirX, snake[i].dirY, offsetEven , snake[i].colour);
        //     }
        //     else{
        //         drawSnake(snake[i].x, snake[i].y, SCALE, snake_body_red, snake[i].dirX, snake[i].dirY, offsetOdd, snake[i].colour);
        //     }
           
        // }

           
        

         // Colliding with food.
        if (headX == randX && headY == randY)
        {   
            showAnimation = true;
            animationX = headX;
            animationY = headY;
            //drawAnimationSquare(headX * SCALE, headY * SCALE, animationRadius, RED);
            // Generate new food position.
            randX = rand() % (RANDOM_RANGE + 1);
            randY = 1;
            fruitIdx = rand() % 5;

            // Increase snake length.
            snakeLength++;
            snake[snakeLength - 1].active = TRUE; // Set cell to active.
            snake[snakeLength - 1].colour = fruit_color[fruitIdx];

            // Update score.
            score = snakeLength - 1;

            // Display score on HEX displays and LEDs.
            *pLED = score;
            displayHex(score);
        }
        // Delay before clearing snake.
        int duration = DELAY;
        while (duration > 0) { duration--; }



        //  for (int i = 0; i < snakeLength; i++)
        // {
        //     if (i == 0 || i == 1)
        //     {
        //         drawSnake(headX, headY, SCALE, snake_head_red, snake[i].dirX, snake[i].dirY, 0 , snake[i].colour);
        //     }
        //     else if (i%2 ==0){
        //         drawSnake(snake[i].x, snake[i].y, SCALE, snake_body_red, snake[i].dirX, snake[i].dirY, offsetEven , snake[i].colour);
        //     }
        //     else{
        //         drawSnake(snake[i].x, snake[i].y, SCALE, snake_body_red, snake[i].dirX, snake[i].dirY, offsetOdd, snake[i].colour);
        //     }
           
        // }

        // Clear snake after delay.
       for (int i = 0; i < snakeLength; i++)
       {
           boxBuilder(snake[i].x, snake[i].y, SCALE, CLEAR, TOP_LEFT_X, TOP_LEFT_Y);
       }

     
      

       
     //  clearAnimationSquare(headX * SCALE, headY * SCALE, animationRadius);

        // Draw current food.
        clearFruit(randX, randY+offsetEven, SCALE, CLEAR);
       
      
       
            offsetOdd = !offsetOdd;
            offsetEven = !offsetEven;
     
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
