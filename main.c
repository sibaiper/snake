#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_SIZE 20
#define LIGHTER_RED                                                            \
  (Color) { 252, 146, 154, 255 }

typedef struct SnakeSegment {
  int x;
  int y;
} SnakeSegment;

typedef enum Direction { RIGHT, LEFT, UP, DOWN } Direction;

typedef struct Game {
  SnakeSegment snake[256];
  int snakeLength;
  Direction currentDirection;
  bool isGameOver;
  int foodX, foodY;
  float updateTime;
  float timer;
  int playerScore;
  int lastPlayerScore;
  bool isScreenshotTaken;
} Game;

void InitGame(Game *game);
void ResetGame(Game *game);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void HandleInput(Game *game);
void MoveSnake(Game *game);
void CheckCollisions(Game *game);
void CheckFood(Game *game);
void PlaceFood(Game *game);
void GrowSnake(Game *game);
void DisplayScore(int score);
void DrawGameOverText(int score);

int main() {
  Game game;
  InitGame(&game);

  while (!WindowShouldClose()) {
    UpdateGame(&game);
    DrawGame(&game);
  }

  CloseWindow();

  return 0;
}

void InitGame(Game *game) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");

  SetTargetFPS(30);

  ResetGame(game);
}

void ResetGame(Game *game) {
  game->snakeLength = 3;
  game->snake[0] = (SnakeSegment){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
  game->snake[1] =
      (SnakeSegment){SCREEN_WIDTH / 2 - GRID_SIZE, SCREEN_HEIGHT / 2};
  game->snake[2] =
      (SnakeSegment){SCREEN_WIDTH / 2 - 2 * GRID_SIZE, SCREEN_HEIGHT / 2};

  game->currentDirection = RIGHT;
  game->isGameOver = false;
  game->updateTime = 0.1f;
  game->timer = 0.0f;
  game->playerScore = 0;
  game->lastPlayerScore = 0;
  game->isScreenshotTaken = false;

  PlaceFood(game);
}

void UpdateGame(Game *game) {
  if (game->isGameOver) {
    if (!game->isScreenshotTaken) {
      TakeScreenshot("screenshot.png");
      game->isScreenshotTaken = true;
    }
    if (IsKeyPressed(KEY_R)) {
      ResetGame(game);
    }
    return;
  }

  HandleInput(game);
  game->timer += GetFrameTime();

  if (game->timer >= game->updateTime) {
    MoveSnake(game);
    CheckCollisions(game);
    CheckFood(game);
    game->timer = 0.0f;
  }
}

void DrawGame(Game *game) {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  // draw main grid
  for (int i = 0; i < SCREEN_WIDTH; i += GRID_SIZE) {    // x
    for (int j = 0; j < SCREEN_HEIGHT; j += GRID_SIZE) { // y
      if (i == game->foodX) {
        DrawRectangle(i, j, GRID_SIZE, GRID_SIZE, LIGHTER_RED);
      } else if (j == game->foodY) {
        DrawRectangle(i, j, GRID_SIZE, GRID_SIZE, LIGHTER_RED);
      } else {
        DrawRectangleLines(i, j, GRID_SIZE, GRID_SIZE, LIGHTGRAY);
      }
    }
  }

  // draw game over
  if (game->isGameOver) {
    DrawGameOverText(game->lastPlayerScore);
  } else {
    DrawRectangle(game->foodX, game->foodY, GRID_SIZE, GRID_SIZE, RED);

    for (int i = 0; i < game->snakeLength; i++) {
      DrawRectangle(game->snake[i].x, game->snake[i].y, GRID_SIZE, GRID_SIZE,
                    BLUE);
    }

    // draw snake eyes
    DrawCircle(game->snake[0].x + (GRID_SIZE / 2) - 5,
               game->snake[0].y + (GRID_SIZE / 2) - 5, 2, BLACK);
    DrawCircle(game->snake[0].x + (GRID_SIZE / 2) + 5,
               game->snake[0].y + (GRID_SIZE / 2) + 5, 2, BLACK);

    DisplayScore(game->playerScore);
  }

  EndDrawing();
}

void HandleInput(Game *game) {
  if ((IsKeyPressed(KEY_UP) || IsKeyPressed(87)) &&
      game->currentDirection != DOWN)
    game->currentDirection = UP;
  if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(83)) &&
      game->currentDirection != UP)
    game->currentDirection = DOWN;
  if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(65)) &&
      game->currentDirection != RIGHT)
    game->currentDirection = LEFT;
  if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(68)) &&
      game->currentDirection != LEFT)
    game->currentDirection = RIGHT;
}

void MoveSnake(Game *game) {
  for (int i = game->snakeLength - 1; i > 0; i--) {
    game->snake[i] = game->snake[i - 1];
  }

  switch (game->currentDirection) {
  case RIGHT:
    game->snake[0].x += GRID_SIZE;
    break;
  case LEFT:
    game->snake[0].x -= GRID_SIZE;
    break;
  case UP:
    game->snake[0].y -= GRID_SIZE;
    break;
  case DOWN:
    game->snake[0].y += GRID_SIZE;
    break;
  }
}

void CheckCollisions(Game *game) {
  if (game->snake[0].x < 0 || game->snake[0].x >= SCREEN_WIDTH ||
      game->snake[0].y < 0 || game->snake[0].y >= SCREEN_HEIGHT) {
    game->isGameOver = true;
    game->lastPlayerScore = game->playerScore;
  }

  for (int i = 1; i < game->snakeLength; i++) {
    if (game->snake[0].x == game->snake[i].x &&
        game->snake[0].y == game->snake[i].y) {
      game->isGameOver = true;
      game->lastPlayerScore = game->playerScore;
    }
  }
}

void CheckFood(Game *game) {
  if (game->snake[0].x == game->foodX && game->snake[0].y == game->foodY) {
    game->playerScore++;
    GrowSnake(game);
    PlaceFood(game);

    if (game->updateTime > 0.05f) {
      game->updateTime -= 0.0015f;
    }
  }
}

void PlaceFood(Game *game) {
  game->foodX = (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
  game->foodY = (rand() % (SCREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE;
}

void GrowSnake(Game *game) {
  game->snakeLength++;
  game->snake[game->snakeLength - 1] = game->snake[game->snakeLength - 2];
}

void DisplayScore(int score) {
  char buffer[20];
  sprintf(buffer, "Score: %d", score);
  DrawText(buffer, 20, 20, 24, RED);
}

void DrawGameOverText(int score) {
  char buffer[80];
  sprintf(buffer, "Game Over! Score: %d. Press R to Restart", score);
  int text_width = MeasureText(buffer, 30);
  DrawText(buffer, SCREEN_WIDTH / 2 - text_width / 2, SCREEN_HEIGHT / 2 - 10,
           30, RED);
}
