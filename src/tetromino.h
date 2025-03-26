#ifndef _TETROMINO_H
#define _TETROMINO_H

#include <raylib.h>
#include <stdlib.h>

typedef struct {
  // Tetromino types: I O T S Z J L
  char type;
  // U D L R
  char orientation;
  int x, y;
} Tetromino;

Tetromino tetromino_spawn() {
  char types[7] = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};
  char type = types[rand() % 7];
  return (Tetromino){type, 'U', 4, 0};
}

Color tetromino_get_color(char tetromino_type) {
  Color color = BLUE;

  switch (tetromino_type) {
  case 'I':
    color = BLUE;
    break;
  case 'O':
    color = YELLOW;
    break;
  case 'T':
    color = MAGENTA;
    break;
  case 'S':
    color = RED;
    break;
  case 'Z':
    color = GREEN;
    break;
  case 'J':
    color = PINK;
    break;
  case 'L':
    color = ORANGE;
    break;
  }

  return color;
}

#endif //_TETROMINO_H
