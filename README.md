# 2D Pokémon Pac-Man: Catch Them All in C

## Collaborator: Roel Boussier

### Context

This project was created for the course Computer Programming in the Bachelor of Science in Engineering (Computer Science Engineering), under the supervision of Prof. Dr. Ir. Filip De Turck.

---

## Introduction

### 1.1 Project Goal

The objective of this project is to implement a game named **"Catch Them All"** in C. This project must be completed in pairs and aims to cover various aspects of software development. This includes not only programming but also utilizing the Visual Studio Code editor, integrating existing libraries, and collaborating efficiently through Git. The project accounts for 6 out of 20 points for the Programming course.

### 1.2 Game Programming

Game programming differs from traditional application programming in several ways. Key aspects include:

#### 1.2.1 Game Loop

The game loop is the core component of the game, ensuring it runs smoothly, regardless of user input. Unlike traditional software (like word processors), which responds to specific user actions, a game loop continuously processes events. Below is a pseudocode example of a generic game loop:

while user does not exit do
    Capture user input
    Run (simple) AI
    Move objects
    Resolve collisions
    Render graphics
end while



This sequence involves checking for user input, executing game logic and AI, moving objects, resolving collisions, and rendering the graphics. Game loops can be further refined as development progresses and can be implemented differently depending on the target platform.

#### 1.2.2 Rendering Graphics

Visual presentation is crucial in games. Smooth visuals enhance the gaming experience. Key points regarding frame rates include:

- Higher frames per second (FPS) lead to smoother motion.
- Smaller differences between consecutive frames also contribute to smoothness.

In this project, we utilize the open-source game programming library **Allegro**. Essential graphical functionalities (like initialization and user input handling) are provided, requiring familiarity with the GUI code to integrate these functions correctly into the game logic.

#### 1.2.3 Collision Detection

Collision detection identifies interactions between objects. In each game loop iteration, it’s vital to check for collisions before redrawing the frame. This project will focus on 2D collision detection.

---

## 2 Task Overview

The aim is to develop a game where players capture multiple **Pikachu** characters. The player navigates through a forest, viewing the game from above. Pikachus move along predetermined paths, and players can use **Pokeballs** to capture them. However, Pikachus can eliminate the player on contact. To progress to the next level, all Pikachus must be captured, requiring strategic placement of Pokeballs. Players can also use Pokeballs to remove obstacles but must be cautious, as they are vulnerable to their own Pokeballs. The game ends if the player captures themselves.

An initial impression of the game is shown in Figure 2. The following sections detail various game components and specify the rules.

---

## 3 Implementation

### 3.1 Game World

#### 3.1.1 Tiles and Coordinates

The game world can be represented in two dimensions, moving horizontally along the x-axis and vertically along the y-axis. Each object is assigned a coordinate (x,y), where the x-coordinate increases to the right, and the y-coordinate increases downwards. For simplicity, the world is divided into tiles, each with a fixed size defined by the macro constant **TILE_SIZE**. Each tile can contain an object (e.g., a Pokeball or bonus item).

#### 3.1.2 Level Information

The `LevelInfo` struct defines level properties through various parameters:

- **width**: The level's width (in tiles)
- **height**: The level's height (in tiles)
- **level_nr**: The level number
- **fill_ratio**: A number between 0 and 1 that determines the percentage of the level filled with catchable obstacles
- **nr_of_pikachus**: The number of Pikachus present
- **spawn_strong_pikachu**: Indicates if an extra strong Pikachu is in the level
- **bonus_spawn_ratio**: A number between 0 and 1 determining the chance of a bonus appearing where a catchable obstacle or Pikachu was captured

Instances of `LevelInfo` are generated using the `generate_level_info` function, specified in **level.h**. Levels are generated randomly but influenced by the level number, allowing for increasing difficulty.

### 3.2 Game Objects

#### 3.2.1 Actors

##### 3.2.1.1 Player
The player is represented by the `Player` struct with the following fields:

##### 3.2.1.2 Pikachu
Pikachu is represented by the `Pikachu` struct with the following fields:

#### 3.2.2 Level Objects

##### 3.2.2.1 Obstacles
Obstacles are represented by the `Obstacle` struct with the following fields:
- **type:** (see Entity)
- **x:** x-coordinate of the obstacle

##### 3.2.2.2 Pokeballs and Capture Attempts
A Pokeball is represented by the `Pokeball` struct with the following fields:
- **type:** (see Entity)
- **is_strong:** (see 3.2.1.2), indicating it can survive multiple attempts.

##### 3.2.2.3 Bonuses
A bonus is represented by the `Bonus` struct with the following fields:
Three bonus types (enum `BONUS_TYPE`):
- **EXTRA_POWER:** increases the strength of the player's Pokeballs
- **EXTRA_POKEBALL:** gives the player an extra Pokeball
- **FREEZE_PIKACHUS:** freezes the Pikachu

### 3.3 Game Loop
The game loop's basic structure is outlined. The game's status is maintained in a `Game` struct containing:
- **level:** information about the game world (see 3.1)
- **player:** the player (see 3.2.1.1)
- **pikachus:** an array of existing Pikachu (see 3.2.1.2)
- **pikachus_left:** number of uncaptured Pikachu
- **state:** current game phase (started, game over, level completed, etc.)
- **input:** stores input information
- **score:** current score

Score increments occur when the player captures a Pikachu:
- For an extra strong Pikachu, the score increases by `STRONG_PIKACHU_SCORE`.
- For a regular Pikachu, the score increases by `PIKACHU_SCORE` minus the GUI timer, but not below zero.

The game loop waits for events from Allegro by calling `gui_game_event()`. Three events are defined in `util.h`: 
- **TimerEvent:** triggers when a new frame is needed.
- **DisplayCloseEvent:** when the display is closed.
- **KeyDownEvent:** when a key is pressed or released.

#### 3.3.1 Input Handling
The first step is to check player input using the `check_game_input` method, which processes input via GUI methods and stores it in the `Input` struct. The `KEY_DOWN` event provides a positive integer representing pressed keys. Each key has a specific bit value:
- UP = 1, DOWN = 2, RIGHT = 4, LEFT = 8, SPACE = 16, EXIT = 32.

Example: DOWN + RIGHT = 6 = 000110. A bitwise AND helps identify which keys are pressed. Use the `&` operator for this.

#### 3.3.2 Status Update
The main step is updating the game status. The `update_game` method calls:
- `do_player_movement(Game* game)`: Executes player movement, considering obstacles.
- `do_pikachu_ai(Game* game)`: Executes Pikachu movements and checks for collisions with the player.
- `process_bonus_items(Game* game)`: Checks if the player is on a bonus and executes the action.
- `process_pokeballs(Game* game)`: Updates Pokeball timers and performs capture attempts.

#### 3.3.3 Rendering
The `render_game` method draws the current game state using GUI methods.

### 3.4 Playability
Playability is crucial beyond functional aspects. Consider:
- Smooth controls: Automatically take the next available turn when multiple keys are pressed.
- Looser collision detection: A Pikachu partially in a tile should not immediately be marked as caught.

### 3.5 Highscores
Maintain a highscores array in a file. Implement the following operations from `highscores.h`:
- `void load_highscores(HighScoreTable* highscores)`: Reads highscores from a file, using dynamic allocation within limits set by `MAX_HIGHSCORE_ENTRIES`.
- `void check_highscore_entry(HighScoreTable* highscores, int score)`: Checks if the score is a new highscore. If so, prompts for a name and updates the table.
- `void save_highscores(HighScoreTable* highscores)`: Saves the highscores table to a file as binary data, only if changed. Call these functions appropriately, and display highscores on game over.

### 4 Files
Some code is provided in the designated repository. Modify files as needed for a functional solution. Below are the files included:

#### 4.1 catch_them_all.h
Contains game definitions: constants, enumerations, and structs for the player, Pikachu, and level entities.

#### 4.2 game.h
Specifies a game as a struct and operations such as initializing, moving the player, and rendering.

#### 4.3 game.c
Implement the empty functions and adjust the game loop for proper termination.

#### 4.4 gui.h/c
Specifications and implementation of the GUI, handling communication with Allegro.

#### 4.5 util.h
Contains information about generated events. Modifications are not expected.

#### 4.6 highscores.h
Specifies operations for managing the game's highscores.

#### 4.7 highscores.c
Implement the empty functions.

#### 4.8 level.h
Specifies a level as a struct and applicable operations.

#### 4.9 level.c
Implement the empty functions.

#### 4.10 main.c
Main implementation is available; modifications are allowed.



