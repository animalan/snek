/////////////////////////////////////////////////////////////////////////////////////////////////// LIB

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////////////////////////// LIB

/////////////////////////////////////////////////////////////////////////////////////////////////// I/O

///////////////////////////////////////// 9 - 0 Red LEDs
#define LED_BASE 0xFF200000
volatile int *pLED = (int *)LED_BASE;

// Pushbuttons (Keys)

// DATA REG
// DIRECTION REG
// INTERRUPT MASK REG
// EDGE CAPTURE REG
/////////////////////////////////////////

///////////////////////////////////////// 9 - 0 Red LEDs
#define KEY_BASE 0xff200050
volatile int *pKEY = (int *)KEY_BASE;
///////////////////////////////////////// 9 - 0 Red LEDs

///////////////////////////////////////// 9 - 0 Switches
#define SW_BASE 0xFF200040
volatile int *pSW = (int *)SW_BASE;
/////////////////////////////////////////

///////////////////////////////////////// 7 Seg Display
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
/////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////// AUDIO
#define AUDIO_BASE 0xFF203040
// CONTROL      WI RI ... CW CR WE RE
// FIFOSPACE    WSLC WSRC RALC RARC
// LEFT DATA
// RIGHT DATA
///////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////// VGA
#define PIXEL_BUFFER 0xFF203020

#define WIDTH 320
#define HEIGHT 240


#define CLEAR BLACK
#define SHADING_OFFSET 10305

#define SECOND 10000000

int pixel_buffer_start; // global variable
volatile int *pixel_ctrl_ptr = (int*) PIXEL_BUFFER;

short int Buffer1[HEIGHT][512]; // Front buffer 240 rows, 512 (320 + padding) columns
short int Buffer2[HEIGHT][512]; // Back buffer

// Prototypes
void wait_for_vsync();
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void delay(int duration);

///////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////// PS/2
#define PS2_BASE 0xFF200100 // Address required for DE1-SOC

// Arrow Keys
#define LEFT_KEY 0x6B
#define DOWN_KEY 0x72
#define RIGHT_KEY 0x74
#define UP_KEY 0x75
#define BREAK 0xF0


volatile int *pPS2 = (int *)PS2_BASE;
////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////// Interrupts

// IRQ (Interrupt Request) Numbers
// Interval timer                   0
// Pushbutton switch parallel port  1
// Second Interval timer            2
// Audio port                       6
// PS/2 port                        7
// JTAG port                        8
// IrDA port                        9
// Serial port                     10
// JP1 Expansion parallel port     11
// JP2 Expansion parallel port     12
// PS/2 port dual                  23

#define NIOS2_READ_STATUS(dest)   do { dest = __builtin_rdctl(0); } while (0)
#define NIOS2_WRITE_STATUS(src)   do {__builtin_wrctl(0, src); } while (0)
#define NIOS2_READ_ESTATUS(dest)  do { dest = __builtin_rdctl(1); } while (0)
#define NIOS2_READ_BSTATUS(dest)  do { dest = __builtin_rdctl(2); } while (0)
#define NIOS2_READ_IENABLE(dest)  do { dest = __builtin_rdctl(3); } while (0)
#define NIOS2_WRITE_IENABLE(src)  do {__builtin_wrctl(3, src); } while (0)
#define NIOS2_READ_IPENDING(dest) do { dest = __builtin_rdctl(4); } while (0)
#define NIOS2_READ_CPUID(dest)    do { dest = __builtin_rdctl(5); } while (0)


// ISR
void PS2_ISR(void);


// IRH
void interrupt_handler(void)
{
    // Pending interrupts (to-be0handled)
    int ipending;
    NIOS2_READ_IPENDING(ipending);

    if (ipending & 0b10000000) { PS2_ISR(); }
    return; // Ignore all other interrupts
}


void the_reset(void) __attribute__((section(".reset")));
void the_reset(void)
{
    asm(".set noat");      /* Instruct the assembler NOT to use reg at (r1) as
                            * a temp register for performing optimizations */
    asm(".set nobreak");   /* Suppresses a warning message that says that
                            * some debuggers corrupt regs bt (r25) and ba
                            * (r30)
                            */
    asm("movia r2, main"); // Call the C language main program
    asm("jmp r2");
}

void the_exception(void) __attribute__((section(".exceptions")));
void the_exception(void)
{
    asm("subi sp, sp, 128");
    asm("stw et, 96(sp)");
    asm("rdctl et, ctl4");
    asm("beq et, r0, SKIP_EA_DEC"); // Interrupt is not external
    asm("subi ea, ea, 4");          /* Must decrement ea by one instruction
                                     * for external interupts, so that the
                                     * interrupted instruction will be run */
    asm("SKIP_EA_DEC:");
    asm("stw r1, 4(sp)"); // Save all registers
    asm("stw r2, 8(sp)");
    asm("stw r3, 12(sp)");
    asm("stw r4, 16(sp)");
    asm("stw r5, 20(sp)");
    asm("stw r6, 24(sp)");
    asm("stw r7, 28(sp)");
    asm("stw r8, 32(sp)");
    asm("stw r9, 36(sp)");
    asm("stw r10, 40(sp)");
    asm("stw r11, 44(sp)");
    asm("stw r12, 48(sp)");
    asm("stw r13, 52(sp)");
    asm("stw r14, 56(sp)");
    asm("stw r15, 60(sp)");
    asm("stw r16, 64(sp)");
    asm("stw r17, 68(sp)");
    asm("stw r18, 72(sp)");
    asm("stw r19, 76(sp)");
    asm("stw r20, 80(sp)");
    asm("stw r21, 84(sp)");
    asm("stw r22, 88(sp)");
    asm("stw r23, 92(sp)");
    asm("stw r25, 100(sp)"); // r25 = bt (skip r24 = et, because it is saved
    // above)
    asm("stw r26, 104(sp)"); // r26 = gp
    // skip r27 because it is sp, and there is no point in saving this
    asm("stw r28, 112(sp)"); // r28 = fp
    asm("stw r29, 116(sp)"); // r29 = ea
    asm("stw r30, 120(sp)"); // r30 = ba
    asm("stw r31, 124(sp)"); // r31 = ra
    asm("addi fp, sp, 128");
    asm("call interrupt_handler"); // Call the C language interrupt handler
    asm("ldw r1, 4(sp)");          // Restore all registers
    asm("ldw r2, 8(sp)");
    asm("ldw r3, 12(sp)");
    asm("ldw r4, 16(sp)");
    asm("ldw r5, 20(sp)");
    asm("ldw r6, 24(sp)");
    asm("ldw r7, 28(sp)");
    asm("ldw r8, 32(sp)");
    asm("ldw r9, 36(sp)");
    asm("ldw r10, 40(sp)");
    asm("ldw r11, 44(sp)");
    asm("ldw r12, 48(sp)");
    asm("ldw r13, 52(sp)");
    asm("ldw r14, 56(sp)");
    asm("ldw r15, 60(sp)");
    asm("ldw r16, 64(sp)");
    asm("ldw r17, 68(sp)");
    asm("ldw r18, 72(sp)");
    asm("ldw r19, 76(sp)");
    asm("ldw r20, 80(sp)");
    asm("ldw r21, 84(sp)");
    asm("ldw r22, 88(sp)");
    asm("ldw r23, 92(sp)");
    asm("ldw r24, 96(sp)");
    asm("ldw r25, 100(sp)"); // r25 = bt
    asm("ldw r26, 104(sp)"); // r26 = gp
    // skip r27 because it is sp, and we did not save this on the stack
    asm("ldw r28, 112(sp)"); // r28 = fp
    asm("ldw r29, 116(sp)"); // r29 = ea
    asm("ldw r30, 120(sp)"); // r30 = ba
    asm("ldw r31, 124(sp)"); // r31 = ra
    asm("addi sp, sp, 128");
    asm("eret");
}


///////////////////////////////////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////////////////////// GAME CONSTANTS

#define RED        0xF800
#define ORANGE     0xfd00
#define YELLOW     0xff45
#define GREEN      0x0707
#define BLUE       0x2d7f
#define LAVENDER   0x83b3
#define PINK       0xfbb4
#define PEACH      0xfe55
#define WHITE      0xff9c
#define LIGHTGREY  0xc618
#define DARKGREY   0x62aa
#define BROWN      0xaa87
#define DARKGREEN  0x042a
#define DARKPURPLE 0x792a
#define DARKBLUE   0x216a


int entryX = 0;
int entryY = 0;

int exitX = 0;
int exitY = 0;

#define WHITE 0xFFFF

#define BLACK 0




#define SECOND 50000000
#define TRUE 1
#define FALSE 0

#define GAME_WIDTH 20
#define GAME_HEIGHT GAME_WIDTH
#define DELAY 900000

#define MAX_SNAKE_LENGTH GAME_WIDTH *GAME_WIDTH
#define STARTING_LENGTH 2
#define SCALE 9
#define RANDOM_RANGE GAME_WIDTH

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
///////////////////////////////////////////////////////////////////////////////////////////////////////

int headX = 0;
int headY = 0;

int dirX = 0;
int dirY = 0;

// int acceptInput = TRUE;
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

// RGB565
// 5 Bits of Red, 6 Bits of Green, 5 Bits of Blue


////////////////////////////////////////////////////////////////////////////////////////////////

// Assets
char letter[26][5][3];
char number[10][5][3];
unsigned int fruits[5][9][9];
unsigned int fruit_color[5];
unsigned int snake_head_red[9][9];
unsigned int snake_body_red[9][9];

////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////

void input();

void wait_for_vsync();
void clear_screen(const int COLOUR);
void plot_pixel(int x, int y, short int line_color);
void plot_pixel_delay(int x, int y, short int line_color, int delay);

// Draw functions
void drawAnimationSq(int startX, int startY, const int LENGTH, const int COLOUR, const int delay);
void drawLine(int x0, int y0, int x1, int y1, int line_color);
void drawBox(int startX, int startY, const int LENGTH, const int COLOUR, int OFFSET_X, int OFFSET_Y);
void drawBorder(int startX, int startY, const int LENGTH, const int COLOUR, const int delay);
void drawFruit (int startX, int startY, const int LENGTH, unsigned int fruit[9][9], int offset);
void drawSnake(int startX, int startY, const int LENGTH, unsigned int snake[9][9], int directionX, int directionY, int offset, int color);

void clearFruit(int startX, int startY, const int LENGTH, int color);

bool isReadEmpty();
void swap(int *x, int *y);
void displayHex(int number);

// Maze
void path(int currX, int currY);
void generateMaze();
void drawMaze();

// Text
void twrite(char* text, int x, int y, int size, int COLOUR, int typeDuration);


#define MAZE_SIZE 10
int maze[MAZE_SIZE][MAZE_SIZE];
int mazeData[MAZE_SIZE * 2 + 1][MAZE_SIZE * 2 + 1];


int dir[4][2];

void drawCircle(const int center_x, const int center_y, const int radius, const int COLOUR)
{
    if (radius < 0) {return;}

    int x = 0;
    int y = radius;

    int d = 3 - 2 * radius;

    plot_pixel(center_x + x, center_y + y, COLOUR);
    plot_pixel(center_x - x, center_y + y, COLOUR);
    plot_pixel(center_x + x, center_y - y, COLOUR);
    plot_pixel(center_x - x, center_y - y, COLOUR);
    plot_pixel(center_x + y, center_y + x, COLOUR);
    plot_pixel(center_x - y, center_y + x, COLOUR);
    plot_pixel(center_x + y, center_y - x, COLOUR);
    plot_pixel(center_x - y, center_y - x, COLOUR);

    while (y >= x)
    {

        x++;

        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
        {
            d = d + 4 * x + 6;
        }

        plot_pixel(center_x + x, center_y + y, COLOUR);
        plot_pixel(center_x - x, center_y + y, COLOUR);
        plot_pixel(center_x + x, center_y - y, COLOUR);
        plot_pixel(center_x - x, center_y - y, COLOUR);
        plot_pixel(center_x + y, center_y + x, COLOUR);
        plot_pixel(center_x - y, center_y + x, COLOUR);
        plot_pixel(center_x + y, center_y - x, COLOUR);
        plot_pixel(center_x - y, center_y - x, COLOUR);

    }
}

struct frame
{
    int explosionRadius;
    int foodX;
    int foodY;
    bool offsetEven;
};


struct frame prevFrame;    // last frame.
struct frame prevFrameTwo; // frame before last frame.
struct frame currFrame;

int frameCount = 0;
#define EXPLOSION_FRAME_RATE 45
#define SNAKE_FRAME_RATE 15

#define REFRESH_RATE 60
#define EXPLOSION_RADIUS 25

bool drawExplosion = false;

int explosionX = 0;
int explosionY = 0;

int main(void)
{

    *(pPS2 + 1) = 0x1;     // Turn on PS2 interrupt.
    NIOS2_WRITE_IENABLE(0b10000011); // IRQ Enable.
    NIOS2_WRITE_STATUS(1); // Enable Nios II to accept interrupts.

    animationRadius = sqrt((240 * 240) + (240 * 240)) ;
    frame++;

    // Clear buffers.
    // Place front buffer at the back, wait for swap.
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1;
    wait_for_vsync();
    // Now clear both buffers.
	pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(BLACK);
    // Place back buffer at the back.
	*(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    clear_screen(BLACK);

    // Generate random seed.
    srand(2444);

    // pixel_buffer_start = *(pixel_ctrl_ptr);
    // drawBox(0, 0, 10, RED, 0, 0);


    // while(true);
    // Init food.
    bool foodFound = true;
    int foodX = 0;
    int foodY = 0;

    // Store last two previous frame information.
    struct cell prev_snake     [MAX_SNAKE_LENGTH];
    struct cell prev_snake_two [MAX_SNAKE_LENGTH];


    // Start with 1 block snake by default.
    snakeLength = STARTING_LENGTH;

    // Set snake colour based on fruit.
    for (int i = 0; i < snakeLength; ++i) { snake[i].colour = fruit_color[fruitIdx]; }

    // Border animation on front buffer.
    pixel_buffer_start = *(pixel_ctrl_ptr);
    drawBorder(TOP_LEFT_X - 1, TOP_LEFT_Y - 1, (GAME_WIDTH + 1) * (SCALE) + 2, RED, 100);
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);

    currFrame.explosionRadius = 0;
    prevFrame.explosionRadius = 0;
    prevFrameTwo.explosionRadius = 0;

    // generateMaze();
    // drawMaze();

    headX = 0;
    headY = 0;

    snake[0].x = headX;
    snake[0].y = headY;

    // Game loop.
    while (TRUE)
    {

        // for(int i =0; i < SECOND/100; ++i);

        // drawCircle(CENTER_X, CENTER_Y, r, CLEAR);


        // Store
        // Clear
        // Update
        // Draw

        /////////////////////////////////////////////////////////////////////////////////////////////////// STORE
        // Store last two frames for double buffering.
		for (int i = 0; i < snakeLength; i++)
		{
            // Prev Frame 2 <-- Prev Frame
            prev_snake_two[i].x      = prev_snake[i].x;
            prev_snake_two[i].y      = prev_snake[i].y;
            prev_snake_two[i].active = prev_snake[i].active;
            prev_snake_two[i].dirX   = prev_snake[i].dirX;
            prev_snake_two[i].dirY   = prev_snake[i].dirY;


            // Prev Frame 1 <-- Current Frame
            prev_snake[i].x      = snake[i].x;
            prev_snake[i].y      = snake[i].y;
            prev_snake[i].active = snake[i].active;
            prev_snake[i].dirX   = snake[i].dirX;
            prev_snake[i].dirY   = snake[i].dirY;

		}

        prevFrameTwo.explosionRadius = prevFrame.explosionRadius;
        prevFrame.explosionRadius = currFrame.explosionRadius;

        prevFrameTwo.foodX = prevFrame.foodX;
        prevFrame.foodX = currFrame.foodX;

        prevFrameTwo.foodY = prevFrame.foodY;
        prevFrame.foodY = currFrame.foodY;

        prevFrameTwo.offsetEven = prevFrame.offsetEven;
        prevFrame.offsetEven = currFrame.offsetEven;


        ///////////////////////////////////////////////////////////////////////////////////////// STORE

        ///////////////////////////////////////////////////////////////////////////////////////// CLEAR

        // Clear current food.
        clearFruit(prevFrameTwo.foodX, prevFrameTwo.foodY + prevFrameTwo.offsetEven, SCALE, CLEAR);

        offsetEven = !offsetEven;
        offsetOdd  = !offsetOdd;


        // Clear current frame (using information from penultimate frame)

        // Clear snake
        for (int i = 0; i < snakeLength; i++)
        {
            drawBox(prev_snake_two[i].x, prev_snake_two[i].y, SCALE, CLEAR, TOP_LEFT_X, TOP_LEFT_Y);
        }


        if (drawExplosion)
        {

        if (currFrame.explosionRadius < (EXPLOSION_RADIUS + 2))
        {
            drawCircle(explosionX, explosionY, prevFrameTwo.explosionRadius, CLEAR);

            if (frameCount % (REFRESH_RATE / EXPLOSION_FRAME_RATE) == 0)
            {
                currFrame.explosionRadius++;
            }


            if (currFrame.explosionRadius < EXPLOSION_RADIUS)
            {
                drawCircle(explosionX, explosionY, currFrame.explosionRadius, fruits[fruitIdx]);
            }
            else
            {
                drawCircle(explosionX, explosionY, currFrame.explosionRadius, CLEAR);

                if (currFrame.explosionRadius == EXPLOSION_RADIUS + 2){
                    drawExplosion = false;
                    currFrame.explosionRadius = 0;
                    currFrame.explosionRadius = 0;
                    prevFrame.explosionRadius = 0;
                    prevFrameTwo.explosionRadius = 0;
                }
                // while(true){}
            }
        }
        }
        else
        {
            explosionX = foodX * SCALE + TOP_LEFT_X;
            explosionY = foodY * SCALE + TOP_LEFT_Y;
        }


        // drawMaze();



        // drawCircle(CENTER_X, CENTER_Y, prevFrame.explosionRadius, CLEAR);

        // drawCircle(CENTER_X, CENTER_Y, prevFrameTwo.explosionRadius - 5, YELLOW);


        ////////////////////////////////////////////////////////////////////////////////////// CLEAR


        // drawCircle(CENTER_X, CENTER_Y, prevFrameTwo.explosionRadius, BLACK);
        // wait_for_vsync();





        // drawCircle(CENTER_X, CENTER_Y, prevFrameTwo.explosionRadius, CLEAR);



        // drawCircle(CENTER_X, CENTER_Y, currFrame.explosionRadius - 5, WHITE);


        // Draw Center Mark
        plot_pixel(CENTER_X, CENTER_Y, WHITE);

        // Draw game border.
        drawBorder(TOP_LEFT_X - 1, TOP_LEFT_Y - 1, (GAME_WIDTH + 1) * (SCALE) + 2, RED, 0);

        // Plot food item.
        if (foodFound)
        {
            foodFound = false;
            // Generate new food position.
            foodX = rand() % (RANDOM_RANGE + 1);
            foodY = rand() % (RANDOM_RANGE + 1);
            // Generate new food.
            fruitIdx = rand() % 5;
        }

        // foodX = exitX;
        // foodY = exitY;

        // Draw food.
        drawFruit(foodX, foodY, SCALE, fruits[fruitIdx], offsetEven);

        // Poll for input.
        // input();
        // printf("X: %d   Y: %d \n", headX, headY);

        // Boundary checks.
        if (headX + dirX < 0 || headX + dirX > GAME_WIDTH){ dirX = 0;}
        if (headY + dirY < 0 || headY + dirY > GAME_WIDTH){ dirY = 0;}

        printf("%d\n-- %d\n", headX + dirX, mazeData[headY][headX]);

        // if (mazeData[headY + dirY][headX + dirX] == 0 )
        // {

        // }
        // else
        if (frameCount % (REFRESH_RATE / SNAKE_FRAME_RATE) == 0)
        {
            // Move head
            headX += dirX;
            headY += dirY;

            // Store head position and direction.
            snake[0].x    = headX;
            snake[0].y    = headY;
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
                    snake[i].x    = snake[i - 1].x;    // Position
                    snake[i].y    = snake[i - 1].y;

                    snake[i].dirX = snake[i - 1].dirX; // Direction
                    snake[i].dirY = snake[i - 1].dirY;
                }
            }
        }

        // Draw snake
        for (int i = 0; i < snakeLength; i++){ drawBox(snake[i].x, snake[i].y, SCALE, WHITE, TOP_LEFT_X, TOP_LEFT_Y); }

         // Colliding with food.
        if (headX == foodX && headY == foodY)
        {
            drawExplosion = true;

            showAnimation = true;

            foodFound = true;

            // animationX = headX;
            // animationY = headY;

            // Increase snake length.
            snakeLength++;
            snake[snakeLength - 1].active = TRUE; // Set cell to active.
            snake[snakeLength - 1].colour = fruit_color[fruitIdx];

            // Clear score text.
            char* text;
            sprintf(text, "%d", score);
			twrite(text, 0, 0, 3, BLACK, 0);

            // Update score.
            score = snakeLength - 1;

            // Display score on HEX displays and LEDs.
            *pLED = score;
            displayHex(score);

			sprintf(text, "%d", score);
			twrite(text, 0, 0, 3, WHITE, 0);
        }


        // // Clear snake.
        // for (int i = 0; i < snakeLength; i++)
        // {
        //     drawBox(snake[i].x, snake[i].y, SCALE, CLEAR, TOP_LEFT_X, TOP_LEFT_Y);
        // }



        // Delay.
        // for (int i = 0; i < 1000000; ++i);

        // Wait for buffer swap to set write buffer.

        wait_for_vsync(); // swap every 1/60s
		pixel_buffer_start = *(pixel_ctrl_ptr + 1);


        frameCount = (frameCount + 1) % 60; // 0 - 59

        // drawCircle(CENTER_X, CENTER_Y, currFrame.explosionRadius, CLEAR);
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

///////////////////////////////////////////////////////////////////////////////////////////////// MISC SNIPPETS

// Mouse.
// if ( (byte2 == 0xAA) && (byte3 == 0x00) )
// {
// *(PS2_ptr) = 0xF4;
// }



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




int dir[4][2] =
{
        {0, -1}, // North
        {1, 0},  // East
        {0, 1},  // South
        {-1, 0}  // West
};

// void input()
// {
//     unsigned char byte1 = 0, byte2 = 0, byte3 = 0;
//     int previousKey = byte2;
//     int PS2_data, RVALID;

//     // Handle key input via polling.

//     PS2_data = *(pPS2);           // read the Data register in the PS/2 port
//     RVALID = (PS2_data & 0x8000); // extract the RVALID field

//     if (RVALID != 0)
//     {
//         byte3 = PS2_data & 0xFF;
//     }

//     if (byte3 == BREAK)
//     {
//         acceptInput = TRUE; // Wait for a break before accepting input.
//     }
//     else if (byte3 == LEFT_KEY && acceptInput)
//     {
//         acceptInput = FALSE;


//         // printf("LEFT KEY\n");

//         dirX = -1;
//         dirY = 0;
//     }
//     else if (byte3 == RIGHT_KEY && acceptInput)
//     {
//         acceptInput = FALSE;


//         // printf("RIGHT KEY\n");

//         dirX = 1;
//         dirY = 0;
//     }
//     else if (byte3 == UP_KEY && acceptInput)
//     {
//         acceptInput = FALSE;


//         // printf("UP KEY\n");

//         dirX = 0; // Set direction
//         dirY = -1;
//     }
//     else if (byte3 == DOWN_KEY && acceptInput)
//     {
//         acceptInput = FALSE;


//         // printf("DOWN KEY\n");

//         dirX = 0;
//         dirY = 1;
//     }
// }

// Check if READ FIFO is empty.
bool isReadEmpty()
{
    return ((*(pPS2) & 0xFFFF0000) == 0);
}

void displayHex(int number)
{
    // Max number exceeded.
    if (number > 999999){ return; }

    int offsetSeg0 = 0;
    int offsetSeg1 = 0;

    int hexDataSeg0 = 0;
    int hexDataSeg1 = 0;

    int hexDigit = 0;
    int digit = 0;

    while (number > 0)
    {
        digit = number % 10;

             if (digit == 0){ hexDigit = HEX_ZERO; }
        else if (digit == 1){ hexDigit = HEX_ONE; }
        else if (digit == 2){ hexDigit = HEX_TWO; }
        else if (digit == 3){ hexDigit = HEX_THREE; }
        else if (digit == 4){ hexDigit = HEX_FOUR; }
        else if (digit == 5){ hexDigit = HEX_FIVE;}
        else if (digit == 6){ hexDigit = HEX_SIX; }
        else if (digit == 7){ hexDigit = HEX_SEVEN; }
        else if (digit == 8){ hexDigit = HEX_EIGHT; }
        else if (digit == 9){ hexDigit = HEX_NINE; }

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
            offsetSeg0++;
        }

        number /= 10;
    }

    // Write to hex displays.
    *ADDR_7SEG0 = hexDataSeg0; // HEX2 - HEX0
    *ADDR_7SEG1 = hexDataSeg1; // HEX5 - HEX3
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

// void drawMBox(int startX, int startY, const int LENGTH, const int COLOUR, int OFFSET_X, int OFFSET_Y)
// {
//     // Draw a (LENGTH x LENGTH) Box.
//     for (int j = 0; j < LENGTH; ++j)
//     {
//         for (int k = 0; k < LENGTH; ++k)
//         {
//             plot_pixel(startX   + k + OFFSET_X, startY + j + OFFSET_Y, COLOUR);
//         }
//     }
// }

void drawBox(int startX, int startY, const int LENGTH, const int COLOUR, int OFFSET_X, int OFFSET_Y)
{
    // Draw a (LENGTH x LENGTH) Box.
    for (int j = 0; j < LENGTH; ++j)
    {
        for (int k = 0; k < LENGTH; ++k)
        {
            plot_pixel(startX * LENGTH  + k + OFFSET_X, startY * LENGTH  + j + OFFSET_Y, COLOUR);
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

void drawBorder(int startX, int startY, const int LENGTH, const int COLOUR, const int delay)
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


void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

void drawLine(int x0, int y0, int x1, int y1, int line_color)
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

            // Draw line
            int x0 = currX * 2, y0 = currY * 2;
            int x1 = newX * 2, y1 = newY * 2;

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
                    mazeData[x + 1][y + 1] = 1;
                }
                else
                {
                    mazeData[y + 1][x + 1] = 1;
                }

                error = error + deltay;

                if (error > 0)
                {
                    y = y + y_step;
                    error = error - deltax;
                }
            }


            // printf("%d, %d \n", newX, newY);
            path(newX, newY);
        }
    }

    return;
}


void generateMaze()
{
    for (int i = 0; i < MAZE_SIZE; ++i)
    {
        for (int j = 0; j < MAZE_SIZE; ++j)
        {
            maze[i][j] = 0;
            mazeData[i][j] = 0;
        }
    }

    path(0, 0);

    // Generate entry

    bool notFound = true;

    for (int i = 0; i < 2; i++)
    {
        int openingX = -1;
        int openingY = -1;

        while (notFound)
        {
            int entry = (rand() % (MAZE_SIZE * 2 + 1));
            int side  = (rand() % 4);

            int dirX = 0;
            int dirY = 0;

            if (side == 0)
            {
                openingX = 0;
                dirX = 1;
            }
            else if (side == 1)
            {
                openingX = MAZE_SIZE * 2;
                dirX = -1;
            }
            else if (side == 2)
            {
                openingY = 0;
                dirY = 1;
            }
            else
            {
                openingY = MAZE_SIZE * 2;
                dirY = -1;
            }

            if (openingX == -1) {openingX = entry;}
            else {openingY = entry;}

            if (mazeData[openingY + dirY][openingX + dirX] == 0)
            {
                notFound = true;
            }
            else
            {
                mazeData[openingY][openingX] = 1;
                notFound = false;
            }
        }


        if (i == 0)
        {
            entryX = openingX;
            entryY = openingY;
        }
        else if (i == 1)
        {
            exitX = openingX;
            exitY = openingY;
        }

        notFound = true;
    }
}


void drawMaze()
{
    for (int y = 0; y < (MAZE_SIZE * 2 + 1); y++)
    {
        for (int x = 0; x < (MAZE_SIZE * 2 + 1); x++)
        {
            if (mazeData[y][x] == 1)
            {
                drawBox(x, y, SCALE, DARKGREY, TOP_LEFT_X, TOP_LEFT_Y);
            }
            else
            {
                drawBox(x, y, SCALE, ORANGE, TOP_LEFT_X, TOP_LEFT_Y);
            }
        }
    }
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


void twrite(char* text, int x, int y, int size, int COLOUR, int typeDuration)
{
	for (int i = 0; i < strlen(text) ; i++)
	{

		for (int j = 0; j < 5; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
                if ('a' <= text[i] && text[i] <= 'z')
                {
                    if (letter[text[i] - 'a'][j][k] == 1)
                    {
                        drawBox(x + k, y + j, size, COLOUR, 0, 0);
                    }
                }
                else if ('0' <= text[i] && text[i] <= '9')
                {
                    if (number[text[i] - '0'][j][k] == 1)
                    {
                        drawBox(x + k, y + j, size, COLOUR, 0, 0);
                    }
                }

			}
		}

        if(typeDuration > 0)
        {
            int duration = typeDuration;
            while (duration > 0) { duration--; }
        }

		// Update spacing
		x += 4;
	}
}

void clear_screen(const int COLOUR)
{
    volatile short int *one_pixel_address;

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1); // Do not cast.
            *one_pixel_address = COLOUR;
        }
    }
}

void wait_for_vsync()
{
    *pixel_ctrl_ptr = 1;
    int status = *(pixel_ctrl_ptr + 3);
    while ((status & 0x01) != 0){ status = *(pixel_ctrl_ptr + 3); }
}

char letter[26][5][3] =
{
    //a
    {
        {0, 0, 0},
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
        {1, 0, 1}
    },

    //b
    {
        {0, 0, 0},
        {1, 1, 0},
        {1, 1, 0},
        {1, 0, 1},
        {1, 1, 1}
    },

    //c
    {
        {0, 0, 0},
        {1, 1, 1},
        {1, 0, 0},
        {1, 0, 0},
        {1, 1, 1}
    },

    //d
    {
        {0, 0, 0},
        {1, 1, 0},
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 0}
    },

    //e
    {
        {0, 0, 0},
        {1, 1, 1},
        {1, 1, 0},
        {1, 0, 0},
        {1, 1, 1}
    },

    //f
    {
        {0, 0, 0},
        {1, 1, 1},
        {1, 1, 0},
        {1, 0, 0},
        {1, 0, 0}
    },

    //g
    {
        {0, 0, 0},
        {1, 1, 1},
        {1, 0, 0},
        {1, 0, 1},
        {1, 1, 1}
    },

    //h
    {
        {0, 0, 0},
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {1, 0, 1}
    },

    //i
    {
        {0, 0, 0},
        {1, 1, 1},
        {0, 1, 0},
        {0, 1, 0},
        {1, 1, 1}
    },

    //j
    {
        {0, 0, 0},
        {1, 1, 1},
        {0, 1, 0},
        {0, 1, 0},
        {1, 1, 0}
    },

    //k
    {
        {0, 0, 0},
        {1, 0, 1},
        {1, 1, 0},
        {1, 0, 1},
        {1, 0, 1}
    },

    //l
    {
        {0, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 1, 1}
    },

    //m
    {
        {0, 0, 0},
        {1, 1, 1},
        {1, 1, 1},
        {1, 0, 1},
        {1, 0, 1}
    },

    //n
    {
        {0, 0, 0},
        {1, 1, 0},
        {1, 0, 1},
        {1, 0, 1},
        {1, 0, 1}
    },

    // o
    {
        {0, 0, 0},
        {0, 1, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 0}
    },

    // -
    {
        {0, 0, 0},
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
        {1, 0, 0}
    },

    // q
    {
        {0, 0, 0},
        {0, 1, 0},
        {1, 0, 1},
        {1, 1, 0},
        {0, 1, 1}
    },

    // r
    {
        {0, 0, 0},
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 0},
        {1, 0, 1}
    },

    // s
    {
        {0, 0, 0},
        {0, 1, 1},
        {1, 0, 0},
        {0, 0, 1},
        {1, 1, 0}
    },

    // t
    {
        {0, 0, 0},
        {1, 1, 1},
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0}
    },

    // u
    {
        {0, 0, 0},
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 1, 1}
    },

    // v
    {
        {0, 0, 0},
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 1, 0}
    },

    // w
    {
        {0, 0, 0},
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {1, 1, 1}
    },

    // x
    {
        {0, 0, 0},
        {1, 0, 1},
        {0, 1, 0},
        {1, 0, 1},
        {1, 0, 1}
    },

    // y
    {
        {0, 0, 0},
        {1, 0, 1},
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1}
    },

    // z
    {
        {0, 0, 0},
        {1, 1, 1},
        {0, 0, 1},
        {1, 0, 0},
        {1, 1, 1}
    }
};

char number[10][5][3] =
{
//0
{
    {1, 1, 1},
    {1, 0, 1},
    {1, 0, 1},
    {1, 0, 1},
    {1, 1, 1}
},
//1
{
    {1, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
    {1, 1, 1}
},
//2
{
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1}
},
//3
{
    {1, 1, 1},
    {0, 0, 1},
    {0, 1, 1},
    {0, 0, 1},
    {1, 1, 1}
},
//4
{
    {1, 0, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 1}
},
//5
{
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1}
},
//6
{
    {1, 0, 0},
    {1, 0, 0},
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1}
},
//7
{
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1}
},
//8
{
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1}
},
//9
{
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 1}
}


};

unsigned int fruits[5][9][9] =
{
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





unsigned int fruit_color[5] =
{
     0xFBE0, 0x1C27,  0xE8E4, 0xF720, 0x9806
};

// int snake[MAX_SNAKE_LENGTH] = {0};
/////////////////////////////////////// SPRITES

unsigned int snake_body_red[9][9] =
{
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

unsigned int snake_head_red[9][9] =
{
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


void PS2_ISR(void)
{
    printf("%x\n", *pPS2 & 0xFF);

    unsigned char byte3 = 0;

    int PS2_data = *(pPS2);           // read the Data register in the PS/2 port
    int RVALID = (PS2_data & 0x8000); // extract the RVALID field
                                        // Read to clear FIFO
    if (RVALID != 0)
    {
        byte3 = PS2_data & 0xFF;
    }

    if (byte3 == BREAK)
    {
        printf("BREAK\n");
    }
    if (byte3 == LEFT_KEY)
    {
        dirX = -1;
        dirY = 0;

        printf("LEFT\n");
    }
    else if (byte3 == RIGHT_KEY)
    {

        dirX = 1;
        dirY = 0;

        printf("RIGHT\n");
    }
    else if (byte3 == UP_KEY)
    {
        dirX = 0; // Set direction
        dirY = -1;

        printf("UP\n");
    }
    else if (byte3 == DOWN_KEY)
    {
        dirX = 0;
        dirY = 1;
    }

    return;
}

// Mutliple fruits, better randomization
