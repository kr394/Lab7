#ifndef TETRISDISPLAY_H
#define TETRISDISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "display.h"

#define DRAW_SIZE   11

#define NUM_COLS   12
#define NUM_ROWS   22

#define LINE_COLOR     DISPLAY_CYAN
#define T_COLOR        DISPLAY_MAGENTA
#define SQUARE_COLOR   DISPLAY_YELLOW
#define L_COLOR        0xFD00
#define L_INV_COLOR    DISPLAY_BLUE
#define Z_COLOR        DISPLAY_RED
#define Z_INV_COLOR    DISPLAY_GREEN
#define EDGE_COLOR     DISPLAY_GRAY

#define SHAPE_SIZE   4

typedef struct box{
  uint16_t color;
  uint8_t x_pos;
  uint8_t y_pos;
  bool filled;
} Box;

typedef struct shape{
  uint16_t color;
  Box centerBox;
  Box* boxes;
} Shape;

void tetrisDisplay_init();

void tetrisDisplay_makeShape(Shape* activeShape);

void tetrisDisplay_drawShape(Shape* activeShape);

void tetrisDisplay_moveShape(Shape* activeShape);

void tetrisDisplay_rotateShape(Shape* activeShape);

void tetrisDisplay_drawBox(Box* activeBox);

void tetrisDisplay_findCollision(Shape* activeShape);

#endif /* TETRISDISPLAY_H */
