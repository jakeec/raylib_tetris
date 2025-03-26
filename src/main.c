#include "logger.h"
#include "tetromino.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define PADDING 10
#define ROWS 20
#define COLS 10
#define CELLSIZE 20

int main(void) {
  srand(time(0));
  SetTraceLogCallback(CustomRaylibLogCallback);

  InitWindow(640, 480, "Tetris");

  SetTargetFPS(60);

  double fall_speed = 0.35;
  double move_speed = 0.25;
  double last_fall_update = GetTime();
  double last_move_update = GetTime();

  Tetromino active = tetromino_spawn();
  Tetromino next = tetromino_spawn();

  char grid[ROWS][COLS] = {0};

  bool move_right = false;
  bool move_left = false;
  bool fall_faster = false;

  bool running = true;
  bool paused = false;
  while (running) {
    if (WindowShouldClose()) {
      running = false;
      break;
    }

    if (paused) {
      if (IsKeyPressed(KEY_P)) {
        paused = false;
      }
    } else {
      if (IsKeyPressed(KEY_P)) {
        paused = true;
      }
    }

    if (IsKeyDown(KEY_RIGHT)) {
      move_right = true;
    }

    if (IsKeyDown(KEY_LEFT)) {
      move_left = true;
    }

    fall_faster = IsKeyDown(KEY_DOWN);

    if (!paused) {
      double now = GetTime();
      if (now - last_move_update > move_speed) {
        last_move_update = now;

        if (move_right) {
          active.x++;
          move_right = false;
        }

        if (move_left) {
          active.x--;
          move_left = false;
        }

        if (active.x > COLS - 1) {
          active.x = COLS - 1;
        }

        if (active.x < 0) {
          active.x = 0;
        }
      }

      // Run interval updates
      double final_fall_speed = fall_faster ? fall_speed / 2 : fall_speed;
      if (now - last_fall_update > final_fall_speed) {
        last_fall_update = now;

        // Collision
        if (active.y >= ROWS - 1) {
          active.y = ROWS - 1;
          grid[active.y][active.x] = active.type;
          active = next;
          next = tetromino_spawn();
        }

        active.y++;
      }
    }

    BeginDrawing();
#define BACKGROUND CLITERAL(Color){0x2E, 0x30, 0x3E, 0xFF} // 2E303E
    ClearBackground(BACKGROUND);

    // Draw the grid
    DrawRectangleLines(PADDING, PADDING, CELLSIZE * COLS, CELLSIZE * ROWS,
                       BROWN);

    for (int x = 0, y = 0; y < ROWS; x = (x + 1) % COLS, x == 0 && y++) {
      DrawRectangleLines(PADDING + (x * CELLSIZE), PADDING + (y * CELLSIZE),
                         CELLSIZE, CELLSIZE, GRAY);
    }

    // Draw placed tetrominoes.
    for (int x = 0, y = 0; y < ROWS; x = (x + 1) % COLS, x == 0 && y++) {
      char cell = grid[y][x];
      if (cell == '\0')
        continue;

      Color color = tetromino_get_color(cell);

      DrawRectangle(PADDING + (x * CELLSIZE), PADDING + (y * CELLSIZE),
                    CELLSIZE, CELLSIZE, color);
    }

    // Draw active tetromino.
    DrawRectangle(PADDING + (active.x * CELLSIZE),
                  PADDING + (active.y * CELLSIZE), CELLSIZE, CELLSIZE,
                  tetromino_get_color(active.type));

    EndDrawing();
  }
}