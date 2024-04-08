# Snek

### Introduction:
Snek is a feature-rich variant of the classic game, Snake. It is written is C/Assembly for the NIOS II Processor in the DE1-SoC FPGA. 

### How to Play.
Arrow keys (UP, DOWN, LEFT, RIGHT) for snake movement. Move on top of the fruit to "eat" it. The snake cannot move in the reverse direction (i.e if the snake is moving up, the next move must be a right or left. It cannot move down and overlap itself). 
Arrow Keys to navigate menus. Esc to leave menu. Enter to select menu options.
The game is over if the snake intersects / overlaps itself.

### Features 
**1. PS2 Keyboard Controls**
Polling-based Keyboard controls are implemented for controlling the snake's movement and navigating game menus. Interrupt-based Keyboard controls are available as well (however less reliable).

**2. Complex Game FSM**
Snek contains multiple game menus (states of the FSM). This includes a main menu, a settings menu, an achievement menu, a game over menu and a game pause menu. 

**3. Double Buffering**
Snek utilizes double buffering for animations for smoother animations and better responsiveness.

**4. In-game Dynamic Animations**
Snek involves both dynamic, programmed animations and sprite-based animations. Animations include:
- Ripple effect: When the snake eats a fruit, the tiles change to the fruit colour and "ripple" in the direction of the snake.
- Explosion effects: When the snake eats a fruit, an explosion effect occurs. This ultizes Midpoint circle / Bresenham's Circle algorithm.
- Game Border Drawing: On startup, the game border is drawn.
- Bobbing Fruit: Fruits bob up and down.
- Slithering snake: The snake body segements move side to side, creating a slithering effect.
These animations are all double buffered and run at different frames rates, allowing for different animation speeds to occur simultaneously.

**5. Custom Sprites**
- Custom, original sprites were designed for this game. This includes:
- Snake Sprite (head and body sprites). Body colour is adjusted programmically.
- 10+ Food Sprites
- Power-up Sprites
- Text: The PICO-8 Mono Font was ported to C-arrays and used for Snek.
  
**5. Grayscale Mode**
Snek enters "grayscale mode" when the snake eats a rock. The game become monochrome and all colours are converted into grayscale equivalents. Values are computed beforehand (preloaded) to improve game performance as floating point operations are expensive and cause latency.

**6. Maze Mode and Dynamic Generation**
If the snake eats the maze powerup, the user must navigate the snake through a maze to the fruit. This maze generated randomly and dynamically using a classic depth first search, backtracking algorithm.

**7. Sound Effects**
Sound effects are played when keys are pressed (or any event happens like game over ) to change FSM state and also when the snake eats the food. 

**9. Hex Display and LED score counters**
Hex and LED displays are used as player score counters.


### Controls 
Arrow keys (UP, DOWN, LEFT, RIGHT) for snake movement and navigate menus
Enter to select menu option.


### Attribution Table


| Feature                                             | Attribution                                                                                   |
|-----------------------------------------------------|-----------------------------------------------------------------------------------------------|
| Integrated PS2 Keyboard for Movement Controls       | Abhishek, Kovid |
| Double buffering| Abhishek |
| Preloading Optimization | Abhishek, Kovid |
| Complex Game Finite State Machine (FSM)| Kovid                        |
| Dynamic Maze Generation (DFS)  |      Abhishek                  |
| Explosion Effects  |      Abhishek                  |
| Movement logic  |      Abhishek, Kovid                  |
| Score counter  |      Abhishek                  |
| Level indicator + progress bar  |      Abhishek                  |
| Food animation | Kovid|
| Graphics / Drawing functions (drawLine, drawCircle, drawBox, drawText) | Abhishek |
|Game over menu| Kovid|
|Settings Menu | Kovid|
|Acheivement Menu| Kovid|
|Resume Game, save old game state functionality| Kovid|
|Start menu | Kovid, Abhishek |
| Grayscale, RGB and HEX conversions | Abhishek, Kovid |
| Tile ripple animation   								| Abhishek |                    
| Custom Sprites               					  | Kovid                            |
| Sound Effects                           |Kovid                    |
| Polling Based Key Handling            |  Abhishek        |
| Interupt Based Key Handling            |  Abhishek        |
| Hex Display Interface | Abhishek 
| LED Display                                     |Abhishek                      |
|Particle Effects |Abhishek, Kovid|
