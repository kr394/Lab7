#ifndef TETRISDISPLAY_H
#define TETRISDISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "display.h"

#define DRAW_SIZE   5

#define NUM_COLS   10
#define NUM_ROWS   20
#define BORDER_WIDTH    12
#define BORDER_HEIGHT   22

#define LINE_COLOR     DISPLAY_CYAN
#define T_COLOR        DISPLAY_MAGENTA
#define SQUARE_COLOR   DISPLAY_YELLOW
#define L_COLOR        0xFD00
#define L_INV_COLOR    DISPLAY_BLUE
#define Z_COLOR        DISPLAY_RED
#define Z_INV_COLOR    DISPLAY_GREEN
#define EDGE_COLOR     DIDPLAY_GRAY

#define SHAPE_SIZE   4

typedef struct {
  uint16_t color;
  uint8_t x_pos;
  uint8_t y_pos;
  bool filled;
} box;

typedef struct {
  uint16_t color;
  box centerBox;
  box* boxes;
} shape;

void tetrisDisplay_init();

void tetrisDisplay_makeShape(shape* activeShape);

void tetrisDisplay_drawShape(shape* activeShape);

void tetrisDisplay_moveShape(shape* activeShape);

void tetrisDisplay_rotateShape(shape* activeShape);

void tetrisDisplay_drawBox(box* activeBox);

void tetrisDisplay_findCollision(shape* activeShape);

#endif /* TETRISDISPLAY_H */
