#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "sound.h"
#include "tetromino.h"

#define WINWIDTH      680
#define WINHEIGHT     480
#define TARGETFPS     60
#define TITLE         "Tetris"
#define ROWS          22
#define COLS          10
#define CELLSIZE      WINWIDTH / 34
#define PADDING       CELLSIZE
#define FONTSIZELARGE WINWIDTH / 22
#define FONTSIZEMED   WINWIDTH / 24
#define SPEEDBOOST    10
#define NEXT_COUNT    4
#define BACKGROUND    CLITERAL(Color){0x2E, 0x30, 0x3E, 0xFF}

#define MAX_SAMPLES            512
#define MAX_SAMPLES_PER_UPDATE 4096

void draw_tetromino_cell(int px, int py, int x, int y, char tetromino_type,
                         bool shadow);
void draw_tetromino_preview_cell(int px, int py, int x, int y,
                                 char tetromino_type);
void draw_grid_tetromino(Tetromino *tetromino);
void draw_grid_tetromino_preview(Tetromino *tetromino);
void draw_tetromino(Tetromino *tetromino, int px, int py);
void draw_tetromino_3d(Tetromino *tetromino);
bool check_tetromino_collision(Tetromino *tetromino, char grid[ROWS][COLS]);

int main(void) {
  srand(time(0));
  SetTraceLogCallback(CustomRaylibLogCallback);
  InitWindow(WINWIDTH, WINHEIGHT, "🔊 "TITLE);
  SetTargetFPS(TARGETFPS);

  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);

  Wave bwoop_wave   = GenBwoopWave(300.0f, 200.0f, 0.25f, 44100);
  Sound sound_bwoop = LoadSoundFromWave(bwoop_wave);
  UnloadWave(bwoop_wave);

  Wave instant_drop_wave   = GenBwoopWave(400.0f, 100.0f, 0.5f, 44100);
  Sound sound_instant_drop = LoadSoundFromWave(instant_drop_wave);
  UnloadWave(instant_drop_wave);

  float angle  = 0.0f;
  float radius = 7.0f;
  Camera camera;
  camera.position   = (Vector3){0.0f, 1.0f, 10.0f};
  camera.target     = (Vector3){0.0f, 1.0f, 0.0f};
  camera.up         = (Vector3){0.0f, 2.0f, 0.0f};
  camera.fovy       = 40.0f;
  camera.projection = CAMERA_PERSPECTIVE;

restart:

  double last_fall_update = GetTime();
  double last_move_update = GetTime();

  Tetromino active               = tetromino_spawn();
  TetrominoController controller = {(float)active.x};
  Tetromino next[NEXT_COUNT]     = {0};
  for (int i = 0; i < NEXT_COUNT; i++) {
    next[i] = tetromino_spawn();
  }

  char grid[ROWS][COLS] = {0};

  double move_speed    = 0.125;
  int level            = 1;
  int lines_cleared    = 0;
  size_t score         = 0;
  size_t display_score = 0;

  bool move_right   = false;
  bool move_left    = false;
  bool fall_faster  = false;
  bool rotate_cw    = false;
  bool instant_drop = false;

  bool running   = true;
  bool paused    = false;
  bool game_over = false;
  bool muted     = false;

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

    if (game_over) {
      display_score = score;
      if (IsKeyPressed(KEY_R)) {
        goto restart;
      }
    }

    if (IsKeyPressed(KEY_UP)) {
      rotate_cw = true;
    }

    if (IsKeyPressed(KEY_SPACE)) {
      instant_drop = true;
    }

    if (IsKeyPressed(KEY_M)) {
      muted = !muted;
      if (muted) {
        SetWindowTitle("🔈 "TITLE);
      } else {
        SetWindowTitle("🔊 "TITLE);
      }
    }

    fall_faster = IsKeyDown(KEY_DOWN);

    if (display_score < score) {
      display_score += 1;
    }

    double now = GetTime();

    if (game_over || paused) goto render;

    if (rotate_cw) {
      Tetromino clone = tetromino_clone(&active);
      tetromino_rotate_cw(&clone);
      bool collision = check_tetromino_collision(&clone, grid);
      if (!collision) {
        active = clone;
      }
      rotate_cw = false;
    }

    {
      float x_before = controller.x;
      if (IsKeyPressed(KEY_RIGHT)) {
        controller.x = roundf(controller.x) + 1;
      } else if (IsKeyDown(KEY_RIGHT)) {
        controller.x += move_speed;
      }

      if (IsKeyPressed(KEY_LEFT)) {
        controller.x = roundf(controller.x) - 1;
      } else if (IsKeyDown(KEY_LEFT)) {
        controller.x -= move_speed;
      }

      Tetromino clone = tetromino_clone(&active);
      // Update clone.
      clone.x = (int)roundf(controller.x);

      // Check if move was valid.
      bool collision = check_tetromino_collision(&clone, grid);
      if (!collision) {
        active = clone;
      } else {
        controller.x = x_before;
      }
    }

    double fall_speed = powf((0.8 - ((level - 1) * 0.007)), level - 1);
    double final_fall_speed =
        fall_faster ? fall_speed / SPEEDBOOST : fall_speed;
    if (now - last_fall_update > final_fall_speed || instant_drop) {
      if (!muted) {
        if (instant_drop)
          PlaySound(sound_instant_drop);
        else
          PlaySound(sound_bwoop);
      }

      last_fall_update = now;

      // Collision
      Tetromino clone = tetromino_clone(&active);
      clone.y++;
      bool collision = check_tetromino_collision(&clone, grid);

      while (instant_drop && !collision) {
        clone.y++;
        collision = check_tetromino_collision(&clone, grid);
      }
      if (instant_drop) {
        active = clone;
        active.y--;
      }

      if (collision) {
        TetrominoLayout layout       = tetromino_get_absolute_layout(&active);
        grid[layout.a.y][layout.a.x] = active.type;
        grid[layout.b.y][layout.b.x] = active.type;
        grid[layout.c.y][layout.c.x] = active.type;
        grid[layout.d.y][layout.d.x] = active.type;
        active                       = next[0];
        controller.x                 = (float)active.x;
        bool collision               = check_tetromino_collision(&active, grid);
        if (collision) {
          game_over = true;
          continue;
        }
        for (int i = 0; i < NEXT_COUNT - 1; i++) {
          next[i] = next[i + 1];
        }
        next[NEXT_COUNT - 1] = tetromino_spawn();
      } else {
        active.y++;
      }

      instant_drop = false;

      // Check for completed lines.
      int full_lines[ROWS] = {false};
      int full_line_count  = 0;
      for (size_t y = 0; y < ROWS; y++) {
        bool full_line = true;
        for (size_t x = 0; x < COLS; x++) {
          if (grid[y][x] == '\0') {
            full_line = false;
          }
        }
        if (full_line) {
          full_lines[y] = true;
          full_line_count++;
        }
      }

      // Delete completed lines.
      for (int i = 0; i < ROWS; i++) {
        if (full_lines[i]) {
          for (int x = 0; x < COLS; x++) {
            grid[i][x] = '\0';
          }
        }
      }

      // Shift lines down.
      for (int y = 0; y < ROWS; y++) {
        if (full_lines[y]) {
          // Shift all lines above down.
          for (int ya = y; ya > 0; ya--) {
            for (int x = 0; x < COLS; x++) {
              grid[ya][x] = grid[ya - 1][x];
            }
          }
        }
      }

      // Award points.
      // TODO Move points to defines.
      switch (full_line_count) {
        case 1:
          score += 100;
          break;
        case 2:
          score += 300;
          break;
        case 3:
          score += 500;
          break;
        case 4:
          score += 800;
          break;
      }

      lines_cleared += full_line_count;
      level = (lines_cleared / 10) + 1;
    }

  render:
    BeginDrawing();
    ClearBackground(BACKGROUND);

    // Make camera circle the tetromino.
    angle += 0.01f;
    camera.position.x = radius * cos(angle);
    camera.position.z = radius * sin(angle);
    // Draw background.
    BeginMode3D(camera);
    draw_tetromino_3d(&active);
    EndMode3D();

    DrawRectangle(0, 0, WINWIDTH, WINHEIGHT, (Color){0, 0, 0, 200});

    // Draw the grid
    DrawRectangleLines(PADDING, PADDING, CELLSIZE * COLS, CELLSIZE * ROWS,
                       BROWN);

#ifdef GRID
    // Draw grid cell outlines.
    for (int x = 0, y = 0; y < ROWS; x = (x + 1) % COLS, x == 0 && y++) {
      DrawRectangleLines(PADDING + (x * CELLSIZE), PADDING + (y * CELLSIZE),
                         CELLSIZE, CELLSIZE, (Color){155, 155, 155, 30});
    }
#endif

    // Draw score.
    char points_text[100];
    sprintf(points_text, "SCORE: %d", display_score);
    DrawText(points_text, WINWIDTH / 2, 125, FONTSIZELARGE,
             (Color){0, 0, 0, 150});
    DrawText(points_text, WINWIDTH / 2, 120, FONTSIZELARGE, WHITE);

    if (!game_over && !paused) {
      char level_text[10];
      sprintf(level_text, "LEVEL: %d", level);
      DrawText(level_text, WINWIDTH / 2, 175, FONTSIZEMED,
               (Color){0, 0, 0, 150});
      DrawText(level_text, WINWIDTH / 2, 170, FONTSIZEMED, WHITE);
      char lines_cleared_text[10];
      sprintf(lines_cleared_text, "LINES: %d", lines_cleared);
      DrawText(lines_cleared_text, WINWIDTH / 2, 215, FONTSIZEMED,
               (Color){0, 0, 0, 150});
      DrawText(lines_cleared_text, WINWIDTH / 2, 210, FONTSIZEMED, WHITE);
    }

    if (game_over) {
      char *game_over_text = "Game over.";
      char *restart_text   = "Press R to restart.";
      DrawText(game_over_text, WINWIDTH / 2, 175, FONTSIZELARGE,
               (Color){0, 0, 0, 150});
      DrawText(game_over_text, WINWIDTH / 2, 170, FONTSIZELARGE, WHITE);
      DrawText(restart_text, WINWIDTH / 2, 225, FONTSIZELARGE,
               (Color){0, 0, 0, 150});
      DrawText(restart_text, WINWIDTH / 2, 220, FONTSIZELARGE, WHITE);
    }

    if (paused) {
      char *paused_text = "Paused.";
      char *resume_text = "Press P to resume.";
      DrawText(paused_text, WINWIDTH / 2, 175, FONTSIZELARGE,
               (Color){0, 0, 0, 150});
      DrawText(paused_text, WINWIDTH / 2, 170, FONTSIZELARGE, WHITE);
      DrawText(resume_text, WINWIDTH / 2, 225, FONTSIZELARGE,
               (Color){0, 0, 0, 150});
      DrawText(resume_text, WINWIDTH / 2, 220, FONTSIZELARGE, WHITE);
    }

    // Draw active tetromino.
    draw_grid_tetromino(&active);

    // Draw placed tetrominoes.
    for (int x = 0, y = 0; y < ROWS; x = (x + 1) % COLS, x == 0 && y++) {
      char cell = grid[y][x];
      if (cell == '\0') continue;

      draw_tetromino_cell(PADDING, PADDING, x, y, cell, true);
    }

    // Draw preview.
    Tetromino preview = tetromino_clone(&active);
    bool collision    = false;
    while (!collision) {
      preview.y++;
      collision = check_tetromino_collision(&preview, grid);
    }
    preview.y--;
    draw_grid_tetromino_preview(&preview);

    // Draw grid border.
    for (int x = 0; x < COLS + 24; x++) {
      draw_tetromino_cell(0, 0, x, 0, '\0', false);
      draw_tetromino_cell(0, (ROWS + 1) * CELLSIZE, x, 0, '\0', false);
    }

    for (int y = 1; y < ROWS + 1; y++) {
      draw_tetromino_cell(0, y * CELLSIZE, 0, 0, '\0', false);
      draw_tetromino_cell((COLS + 1) * CELLSIZE, y * CELLSIZE, 0, 0, '\0',
                          false);
      draw_tetromino_cell((COLS + 23) * CELLSIZE, y * CELLSIZE, 0, 0, '\0',
                          false);
    }

    // Draw next tetrominoes.
    for (int i = 0; i < NEXT_COUNT; i++) {
      draw_tetromino(&next[i], (14 * CELLSIZE) + (i * CELLSIZE * 5),
                     CELLSIZE * 2);
    }

    EndDrawing();
  }

  CloseAudioDevice();
  CloseWindow();
}

void draw_tetromino_cell(int px, int py, int x, int y, char tetromino_type,
                         bool shadow) {
  Color color = tetromino_get_color(tetromino_type);
  int cx      = px + (x * CELLSIZE);
  int cy      = py + (y * CELLSIZE);

  // Shadow
  int s = 4;  // Shadow size.
  if (shadow)
    DrawRectangle(cx + s, cy + s, CELLSIZE, CELLSIZE, (Color){0, 0, 0, 40});

  int b = 4;  // Bevel size
  DrawRectangle(cx, cy, CELLSIZE, CELLSIZE, color);
  DrawRectangle(cx, cy, CELLSIZE - b, CELLSIZE - b,
                (Color){255, 255, 255, 100});
  DrawRectangle(cx + b, cy + b, CELLSIZE - b, CELLSIZE - b,
                (Color){0, 0, 0, 40});
  DrawRectangle(cx + b, cy + b, CELLSIZE - 2 * b, CELLSIZE - 2 * b, color);
  DrawRectangleLines(cx, cy, CELLSIZE, CELLSIZE, BLACK);
}

void draw_tetromino_preview_cell(int px, int py, int x, int y,
                                 char tetromino_type) {
  Color color = tetromino_get_color(tetromino_type);
  int cx      = px + (x * CELLSIZE);
  int cy      = py + (y * CELLSIZE);

  DrawRectangleLines(cx, cy, CELLSIZE, CELLSIZE, color);
}

void draw_grid_tetromino(Tetromino *tetromino) {
  TetrominoLayout layout = tetromino_get_absolute_layout(tetromino);
  draw_tetromino_cell(PADDING, PADDING, layout.a.x, layout.a.y, tetromino->type,
                      true);
  draw_tetromino_cell(PADDING, PADDING, layout.b.x, layout.b.y, tetromino->type,
                      true);
  draw_tetromino_cell(PADDING, PADDING, layout.c.x, layout.c.y, tetromino->type,
                      true);
  draw_tetromino_cell(PADDING, PADDING, layout.d.x, layout.d.y, tetromino->type,
                      true);
}

void draw_grid_tetromino_preview(Tetromino *tetromino) {
  TetrominoLayout layout = tetromino_get_absolute_layout(tetromino);
  draw_tetromino_preview_cell(PADDING, PADDING, layout.a.x, layout.a.y,
                              tetromino->type);
  draw_tetromino_preview_cell(PADDING, PADDING, layout.b.x, layout.b.y,
                              tetromino->type);
  draw_tetromino_preview_cell(PADDING, PADDING, layout.c.x, layout.c.y,
                              tetromino->type);
  draw_tetromino_preview_cell(PADDING, PADDING, layout.d.x, layout.d.y,
                              tetromino->type);
}

void draw_tetromino(Tetromino *tetromino, int px, int py) {
  TetrominoLayout layout = tetromino_get_layout(tetromino);
  draw_tetromino_cell(px, py, layout.a.x, layout.a.y, tetromino->type, true);
  draw_tetromino_cell(px, py, layout.b.x, layout.b.y, tetromino->type, true);
  draw_tetromino_cell(px, py, layout.c.x, layout.c.y, tetromino->type, true);
  draw_tetromino_cell(px, py, layout.d.x, layout.d.y, tetromino->type, true);
}

void draw_tetromino_3d(Tetromino *tetromino) {
  TetrominoLayout layout = tetromino_get_layout_up(tetromino);
  Color color            = tetromino_get_color(tetromino->type);
  float ox = 0.0, oy = 0.0;

  Vector3 pos_a = {ox + layout.a.x, oy + layout.a.y, 0.0};
  DrawCube(pos_a, 1.0f, 1.0f, 1.0f, color);
  DrawCubeWires(pos_a, 1.0f, 1.0f, 1.0f, BLACK);

  Vector3 pos_b = {ox + layout.b.x, oy + layout.b.y, 0.0};
  DrawCube(pos_b, 1.0f, 1.0f, 1.0f, color);
  DrawCubeWires(pos_b, 1.0f, 1.0f, 1.0f, BLACK);

  Vector3 pos_c = {ox + layout.c.x, oy + layout.c.y, 0.0};
  DrawCube(pos_c, 1.0f, 1.0f, 1.0f, color);
  DrawCubeWires(pos_c, 1.0f, 1.0f, 1.0f, BLACK);

  Vector3 pos_d = {ox + layout.d.x, oy + layout.d.y, 0.0};
  DrawCube(pos_d, 1.0f, 1.0f, 1.0f, color);
  DrawCubeWires(pos_d, 1.0f, 1.0f, 1.0f, BLACK);
}

// TODO Decide whether to prevent invalid moves or use nudge
// strategies.
bool check_tetromino_collision(Tetromino *tetromino, char grid[ROWS][COLS]) {
  TetrominoLayout l = tetromino_get_absolute_layout(tetromino);

  if (l.a.x >= COLS || l.b.x >= COLS || l.c.x >= COLS || l.d.x >= COLS)
    return true;
  if (l.a.x < 0 || l.b.x < 0 || l.c.x < 0 || l.d.x < 0) return true;
  if (grid[l.a.y][l.a.x] != '\0') return true;
  if (grid[l.b.y][l.b.x] != '\0') return true;
  if (grid[l.c.y][l.c.x] != '\0') return true;
  if (grid[l.d.y][l.d.x] != '\0') return true;
  if (l.a.y >= ROWS || l.b.y >= ROWS || l.c.y >= ROWS || l.d.y >= ROWS)
    return true;

  return false;
}