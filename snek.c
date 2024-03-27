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

// Pointers
volatile int* pLED = (int*) LED_BASE;
volatile int* pKEY = (int*) KEY_BASE;
volatile int* pSW =  (int*) SW_BASE;

///////////////////////////////////////////////


/////////////////////////////////////////////// AUDIO
#define AUDIO_BASE			         0xFF203040
// CONTROL      WI RI ... CW CR WE RE
// FIFOSPACE    WSLC WSRC RALC RARC
// LEFT DATA
// RIGHT DATA






///////////////////////////////////////////////




/////////////////////////////////////////////// VGA
#define PIXEL_BUFFER                 0xFF203020

#define WIDTH  320
#define HEIGHT 240

int pixel_buffer_start; // global variable
volatile int* pixel_ctrl_ptr = (int*) PIXEL_BUFFER;


// Prototypes
void wait_for_vsync();
void clear_screen();

///////////////////////////////////////////////





////////////////////////////////////////// PS/2
#define PS2_BASE  0xFF200108

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



//////////////////////////////////////////

int main(void)
{

	unsigned char byte1 = 0, byte2 = 0, byte3 = 0;
    int PS2_data, RVALID;

	// Wait for v-sync before writing to pixel buffer.
    wait_for_vsync();

	pixel_buffer_start = *pixel_ctrl_ptr;

	// Clear screen.
    clear_screen();

	while (TRUE)
	{
		PS2_data = *(pPS2);	// read the Data register in the PS/2 port

		RVALID = (PS2_data & 0x8000);	// extract the RVALID field

		/* always save the last three bytes received */
		if (RVALID != 0)
		{
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
		}

		// Display last byte on Red LEDs

		// Key Detection.
		if (byte2 == LEFT_KEY)
		{
			printf("LEFT KEY\n");
		}
		else if (byte2 == RIGHT_KEY)
		{
			printf("RIGHT KEY\n");
		}
		else if (byte2 == UP_KEY)
		{
			printf("UP KEY\n");
		}
		else if (byte2 == DOWN_KEY)
		{
			printf("DOWN KEY\n");
		}
		else if (byte2 == BREAK)
		{
			printf("BREAK\n");
		}
	}

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









































/////////////////////////////////////////////////// MISC SNIPPETS

// Mouse.
// if ( (byte2 == 0xAA) && (byte3 == 0x00) )
// {
	// *(PS2_ptr) = 0xF4;
// }


