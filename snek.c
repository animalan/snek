/////////////////////////////////////////////// LIB
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

/////////////////////////////////////////////// LIB


/////////////////////////////////////////////// I/O
#define LED_BASE                     0xFF200000
// 9 - 0 Red LEDs

#define KEY_BASE                     0xff200050
// DATA REG
// DIRECTION REG
// INTERRUPT MASK REG
// EDGE CAPTURE REG

#define SW_BASE                      0xFF200040
// 9 - 0 SWITCHES

// 7 Seg Display
#define ADDR_7SEG1 ((volatile long *) 0xFF200020)
#define ADDR_7SEG2 ((volatile long *) 0xFF200030)

// Pointers
volatile int* pLED = (int*) LED_BASE;
volatile int* pKEY = (int*) KEY_BASE;
volatile int* pSW =  (int*) SW_BASE;

///////////////////////////////////////////////


/////////////////////////////////////////////// AUDIO
#define AUDIO_BASE                   0xFF203040
// CONTROL      WI RI ... CW CR WE RE
// FIFOSPACE    WSLC WSRC RALC RARC
// LEFT DATA
// RIGHT DATA






///////////////////////////////////////////////




/////////////////////////////////////////////// VGA
#define PIXEL_BUFFER                 0xFF203020

#define WIDTH  320
#define HEIGHT 240

#define RED   0xF800
#define WHITE 0xFFFF

#define BLACK 0
#define CLEAR BLACK

#define SECOND 10000000

int pixel_buffer_start; // global variable
volatile int* pixel_ctrl_ptr = (int*) PIXEL_BUFFER;

// Prototypes
void wait_for_vsync();
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void delay (int duration);
///////////////////////////////////////////////





////////////////////////////////////////// PS/2
#define PS2_BASE  0xFF200100 // Address required for DE1-SOC

// Arrow Keys
#define LEFT_KEY  0x6B
#define DOWN_KEY  0x72
#define RIGHT_KEY 0x74
#define UP_KEY    0x75
#define BREAK     0xF0

volatile int* pPS2 = (int*) PS2_BASE;

//////////////////////////////////////////


////////////////////////////////////////// GAME CONSTANTS

#define TRUE  1
#define FALSE 0

#define GAME_WIDTH  50
#define GAME_HEIGHT GAME_WIDTH
#define DELAY 900000

#define MAX_SNAKE_LENGTH GAME_WIDTH * GAME_WIDTH
#define STARTING_LENGTH 1
#define SCALE 1
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
};

struct cell snake[MAX_SNAKE_LENGTH];

// int snake[MAX_SNAKE_LENGTH] = {0};

//////////////////////////////////////////

void input()
{
	unsigned char byte1 = 0, byte2 = 0, byte3 = 0;
	int previousKey = byte2;
    int PS2_data, RVALID;

	// Handle key input via polling.

	PS2_data = *(pPS2);              // read the Data register in the PS/2 port
    RVALID = (PS2_data & 0x8000);    // extract the RVALID field

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

		//printf("LEFT KEY\n");

		dirX = -1;
		dirY =  0;
	}
	else if (byte3 == RIGHT_KEY && acceptInput)
	{
		acceptInput = FALSE;

		//printf("RIGHT KEY\n");

		dirX =  1;
		dirY =  0;
	}
	else if (byte3 == UP_KEY && acceptInput)
	{
		acceptInput = FALSE;

		// printf("UP KEY\n");

		dirX =  0; // Set direction
		dirY = -1;
	}
	else if (byte3 == DOWN_KEY && acceptInput)
	{
		acceptInput = FALSE;

		//printf("DOWN KEY\n");

		dirX =  0;
		dirY =  1;
	}
}

void delay (int duration)
{
	while(duration > 0) {duration--;}
}

void wait_for_vsync()
{
    *pixel_ctrl_ptr = 1;
    int status = *(pixel_ctrl_ptr + 3);

    while ((status & 0x01) != 0) { status = *(pixel_ctrl_ptr + 3); }
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
        int duration =  delay;
        while(duration > 0) {duration--;}
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
        plot_pixel_delay(startX + (LENGTH-1), startY + i, COLOUR, delay);
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



int main(void)
{

    *ADDR_7SEG1 = 0x00000006; 
    *ADDR_7SEG2 = 0;

    while(true) {}

    // Wait for v-sync before writing to pixel buffer.
    wait_for_vsync();

    pixel_buffer_start = *pixel_ctrl_ptr;

    // Clear screen.
    clear_screen();

	srand(time(NULL));
    
	int randX = rand() % (RANDOM_RANGE + 1);
	int randY = 0;

	plot_pixel(randX, randY, RED);
	
	// Start with 1 block by default (adjust for debugging).
	snakeLength = STARTING_LENGTH;
	for (int i=0; i < snakeLength; ++i){ snake[i].active; }
    
	while (TRUE)
    {            
        input();
		printf("X: %d   Y: %d \n", headX, headY);


        // borderBuilder(0, 0, 20, WHITE, 100000);
        // borderBuilder(0, 0, 20, RED, 0);
        // while(true);


		// Boundary checks.
		if (headX + dirX < 0 || headX + dirX > GAME_WIDTH) 
        {
            dirX = 0;
        }

		if (headY + dirY < 0 || headY + dirY > GAME_HEIGHT)
        {
            dirY = 0;
        }


		headX += dirX;
		headY += dirY;

		snake[0].x = headX;
		snake[0].y = headY;

        
        // Check for body intersection.
        // Should i = 2? 
        for (int i = 2; i < snakeLength; i++) 
        {     
            if (snake[i].x == snake[0].x && snake[i].y == snake[0].y)
            {
                while(true)
                {
                    // printf("%d(%d, %d) %d(%d, %d)", i, snake[i].x, snake[i].y);
                }
            }
        }

		// Avoid snake from collapsing into itself.
		if (dirX != 0 || dirY != 0)
		{
			// Update the position of snake body.
			for (int i = snakeLength - 1; i > 0; --i)
			{
				snake[i].x = snake[i-1].x;
				snake[i].y = snake[i-1].y;
			}
		}

		// Draw snake
		for (int i = 0; i < snakeLength; i++)
		{  
			boxBuilder(snake[i].x, snake[i].y, SCALE, WHITE); 
		}


		int duration = DELAY;
		while(duration > 0) {duration--;}

		// Clear after delay.
		for (int i = 0; i < snakeLength; i++) 
		{ 
			boxBuilder(snake[i].x, snake[i].y, SCALE, CLEAR); 
		}

		// Colliding with food.
		if (headX == randX && headY == randY)
		{
			// printf("EATEN\n");

			// randX = rand() % (GAME_WIDTH + 1);
			randX = rand() % (RANDOM_RANGE + 1);

			randY = 1;
            
            // Increase snake length.
			snakeLength++; 			            
			snake[snakeLength-1].active = TRUE; // Set cell to active.
            
            score = snakeLength - 1;

            *pLED = score;
		}

        // Draw food.
        boxBuilder(randX, randY, SCALE, RED); 

		// If body intersects itself

		

			
		// printf("Length: %d\n", snakeLength);
	}
}

// PS2 Interrupts // in the DE1-SOC docs
// Maze Generation
// Scaling -- DONE ?
// Double buffering
// Better random number generation
// Build frame.
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








































/////////////////////////////////////////////////// MISC SNIPPETS

// Mouse.
// if ( (byte2 == 0xAA) && (byte3 == 0x00) )
// {
    // *(PS2_ptr) = 0xF4;
// }
