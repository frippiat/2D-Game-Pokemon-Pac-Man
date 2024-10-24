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


