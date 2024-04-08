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
Sound effects are played when the 
**8. Custom Sprites**
**9. Hex Display and LED score counters**
**10. Double Buffering**



**5. PS2 Keyboard Controls:**
**6. PS2 Keyboard Controls:**
**7. PS2 Keyboard Controls:**
**8. PS2 Keyboard Controls:**
**9. PS2 Keyboard Controls:**

One of the key features of our implementation is the integration of a PS2 keyboard for both movement controls and menu navigation. Players can use the arrow keys for controlling the snake's movement and navigating through various menu options, adding convenience and familiarity to the gameplay experience.

**2. Integrated Fairly Complex Finite State Machine (FSM):**
To manage the game's logic efficiently, we implemented a fairly complex finite state machine (FSM). The FSM handles various game states such as menu navigation, gameplay, pause menu, game over, etc. This ensures smooth transitions between different phases of the game.

**3. Dynamically Generated Game Maps, Particle Effects, and Bitmapped Text:**
Our implementation features dynamically generated game maps to provide players with unique and challenging gameplay experiences. We have different game modes like maze (dynamically generated using DFS) and classic. Additionally, we incorporated particle effects to add visual appeal to the game environment. Furthermore, bitmapped text is utilized for displaying text elements such as scores, game over messages, and menu options, ensuring clarity and readability on the display.

**4. Dynamic Animations:**
To make the gameplay more engaging, we included dynamic animations for various game elements such as the snake's movement, food consumption, particle effects and menu selection. These animations enhance the visual aesthetics of the game and contribute to a more immersive gaming experience.

**5. 100% Original Sprites:**
All sprites used in the game, including the snake, food, obstacles,powerups and other game elements, are original.

**6. Sound Effects:**
Sound effects are integrated into the game to provide auditory feedback to players. These sound effects include, eating sounds when the snake consumes food, selection sound when something is selected, and various other audio cues that enhance the overall gaming experience.

**7. Smooth Game Control:**
Efforts were made to ensure smooth and responsive game control, allowing players to maneuver the snake with precision and ease. This was achieved through optimized code and efficient handling of user input, resulting in a seamless gaming experience.

### Additional Algorithms and Features:

**Circle Drawing Algorithm:**
For rendering circular elements such as the snake's body segments or food, we implemented a Bresenham's Circle Drawing Algorithm. This algorithm efficiently calculates the points on the circumference of a circle and allows for smooth and accurate rendering of circular shapes on the display.

**Line Drawing Algorithm:**
To draw straight lines for various game elements such as the game grid or obstacles, we utilized Bresenham's Line Drawing Algorithm. This algorithm efficiently calculates the pixels required to draw a straight line between two points and ensures crisp and clear rendering of lines on the display.

**Interrupt and Polling Based Key Handling:**
Key input from the PS2 keyboard is handled using a combination of interrupt-based and polling-based techniques. Interrupts are utilized to detect keypress events, while polling is used to continuously monitor the keyboard for input during gameplay. This hybrid approach ensures that key input is promptly detected and processed without causing delays in the game.

**Hex Display and LEDs:**
In addition to the graphical display, a hex display is used to provide additional visual feedback to the player, such as displaying the current score. The LED display is updated in real-time to reflect if the ps2 fifo is nearly filled or not to avoid player from overflowing the fifo.

**Manipulating Color Values Algorithm:**
For implementing visual effects such as particle effects or dynamic animations with changing colors, we developed algorithms to manipulate color values in real-time. Like switch greyscale and monochromatic themes.


### Controls 
Arrow keys (UP, DOWN, LEFT, RIGHT) for snake movement.
Arrow Keys to navigate menus. Esc to leave menu. Enter to select menu option.

### Conclusion:
In conclusion, our implementation of the Snake game on an FPGA platform with integrated features such as PS2 keyboard support, complex FSM, dynamically generated game elements, original sprites, sound effects, smooth game control, and additional algorithms for rendering and visual effects, demonstrates the versatility and potential of FPGA-based systems for game development. The incorporation of these advanced features enhances the gameplay experience and showcases the capabilities of FPGA technology in creating immersive gaming environments.


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
