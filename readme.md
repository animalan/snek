# Snake Game

### Introduction:
The Snake game is a classic arcade game that has been enjoyed by players for decades. In this report, we will discuss the implementation of the Snake game on an FPGA platform using the C programming language. Our implementation includes several advanced features aimed at enhancing the gaming experience and demonstrating the capabilities of FPGA-based systems.

### Features 
**1. Integrated PS2 Keyboard for Movement Controls:**
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
Arrow keys for snake movement.
Esc, Enter and arrow keys to navigate menus.

### Conclusion:
In conclusion, our implementation of the Snake game on an FPGA platform with integrated features such as PS2 keyboard support, complex FSM, dynamically generated game elements, original sprites, sound effects, smooth game control, and additional algorithms for rendering and visual effects, demonstrates the versatility and potential of FPGA-based systems for game development. The incorporation of these advanced features enhances the gameplay experience and showcases the capabilities of FPGA technology in creating immersive gaming environments.


### Attribution Table


| Feature                                             | Attribution                                                                                   |
|-----------------------------------------------------|-----------------------------------------------------------------------------------------------|
| Integrated PS2 Keyboard for Movement Controls       | Abhishek, Kovid                                                                     |
| Integrated Fairly Complex Finite State Machine (FSM)| Kovid                        |
| Dynamically Generated Game Maps and Bitmapped Text  |      Abhishek                  |
| Dynamic Animations   								| Kovid, Abhishek                 |                    
| Original Sprites               					  | Kovid                            |
| Sound Effects                                       |Kovid                    |
| Circle Drawing Algorithm                            | Abhishek                   |
| Line Drawing Algorithm                              | Abhishkek                  |
| Interrupt and Polling Based Key Handling            |  Abhishek        |
| Hex and LED Display                                     |Abhishek                      |
| Manipulating Color Values Algorithm                 |Kovid, Abhishek                    |
|Particle Effects |Abhishek, Kovid|
