#include "buttons.h"
#include "display.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

#define BUTTONS_GPIO_DATA_OFFSET 0
#define BUTTONS_GPIO_TRI_OFFSET 4

#define RECTANGLE_HEIGHT DISPLAY_HEIGHT / 2
#define RECTANGLE_WIDTH DISPLAY_WIDTH / 4

#define BTN0_POSITION DISPLAY_WIDTH * 3 / 4
#define BTN1_POSITION DISPLAY_WIDTH / 2
#define BTN2_POSITION DISPLAY_WIDTH / 4
#define BTN3_POSITION 0

#define TEXT_SIZE 2
#define TEXT_YPOS DISPLAY_HEIGHT * 14 / 64
#define BTN0_TEXT_XPOS BTN0_POSITION + RECTANGLE_WIDTH * 15 / 64
#define BTN1_TEXT_XPOS BTN1_POSITION + RECTANGLE_WIDTH * 15 / 64
#define BTN2_TEXT_XPOS BTN2_POSITION + RECTANGLE_WIDTH * 15 / 64
#define BTN3_TEXT_XPOS BTN3_POSITION + RECTANGLE_WIDTH * 15 / 64
#define BTN0_TEXT "BTN0"
#define BTN1_TEXT "BTN1"
#define BTN2_TEXT "BTN2"
#define BTN3_TEXT "BTN3"

#define ALL_INPUT 0x00
#define ALL_PRESSED 0x0F

// Reads the register which resides a provided offset away from the push
// buttons' base address. The value of "offset" tells the function how far from
// the base address the desired register lies. The function returns the 32-bit
// value that is held in the register.
int32_t buttons_readGpioRegister(int32_t offset) {
  return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

// Writes to the register which resides a provided offset away from the push
// buttons' base address. The value of "offset" tells the function where the
// register lies relative to the base address. The value of "value" tells the
// function what should be written to the register.
void buttons_writeGpioRegister(int32_t offset, uint32_t value) {
  Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}

// Initializes the buttons by setting the Tri-state buffer register to input
// mode (all 0's), and then checking they're at 0. The function returns
// BUTTONS_INIT_STATUS_OK if it was successful (value of 1), and
// BUTTONS_INIT_STATUS_FAIL (0) otherwise.
int32_t buttons_init() {
  buttons_writeGpioRegister(BUTTONS_GPIO_TRI_OFFSET, ALL_INPUT);
  if (!buttons_readGpioRegister(BUTTONS_GPIO_TRI_OFFSET)) {
    return BUTTONS_INIT_STATUS_OK;
  } else
    return BUTTONS_INIT_STATUS_FAIL;
}

// Calls the above function buttons_readGpioRegister() while passing the correct
// offset for the buttons' register.
int32_t buttons_read() {
  return buttons_readGpioRegister(BUTTONS_GPIO_DATA_OFFSET);
}

// Runs a test for the buttons. When a button is pressed, a box on the LCD
// screen appears until all buttons are simulatneously pressed.
void buttons_runTest() {
  // Initializes the buttons and returns an error and ends the test if the
  // buttons register is not set to be input.
  if (!buttons_init()) {
    printf("Button Test Failed: Buttons not set as input\n");
    return;
  }
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  printf("Buttons and display successfully initiated.\n");

  uint8_t buttons_value;
  uint8_t last_buttons_value = 0xFF;
  // Read the buttons register and loops until all the buttons are pressed.
  while (((buttons_value = buttons_read()) & ALL_PRESSED) != ALL_PRESSED) {
    // Only changes the LCD screen if the current buttons combination is
    // different from the last time the display changed.
    if (buttons_value != last_buttons_value) {
      // Checks to see that BTN0 is pressed. Draws its rectangle if so, clears
      // it if not.
      if (buttons_value & BUTTONS_BTN0_MASK) {
        display_fillRect(BTN0_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                         DISPLAY_YELLOW);
        display_setCursor(BTN0_TEXT_XPOS, TEXT_YPOS);
        display_setTextColor(DISPLAY_BLACK);
        display_setTextSize(TEXT_SIZE);
        display_println(BTN0_TEXT);
      } else
        display_fillRect(BTN0_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                         DISPLAY_BLACK);
      // Checks to see that BTN1 is pressed. Draws its rectangle if so, clears
      // it if not.
      if (buttons_value & BUTTONS_BTN1_MASK) {
        display_fillRect(BTN1_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                         DISPLAY_GREEN);
        display_setCursor(BTN1_TEXT_XPOS, TEXT_YPOS);
        display_setTextColor(DISPLAY_BLACK);
        display_setTextSize(TEXT_SIZE);
        display_println(BTN1_TEXT);
      } else
        display_fillRect(BTN1_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                         DISPLAY_BLACK);
      // Checks to see that BTN2 is pressed. Draws its rectangle if so, clears
      // it if not.
      if (buttons_value & BUTTONS_BTN2_MASK) {
        display_fillRect(BTN2_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                         DISPLAY_RED);
        display_setCursor(BTN2_TEXT_XPOS, TEXT_YPOS);
        display_setTextColor(DISPLAY_WHITE);
        display_setTextSize(TEXT_SIZE);
        display_println(BTN2_TEXT);
      } else
        display_fillRect(BTN2_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                         DISPLAY_BLACK);
      // Checks to see that BTN3 is pressed. Draws its rectangle if so, clears
      // it if not.
      if (buttons_value & BUTTONS_BTN3_MASK) {
        display_fillRect(BTN3_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                         DISPLAY_BLUE);
        display_setCursor(BTN3_TEXT_XPOS, TEXT_YPOS);
        display_setTextColor(DISPLAY_WHITE);
        display_setTextSize(TEXT_SIZE);
        display_println(BTN3_TEXT);
      } else
        display_fillRect(BTN3_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                         DISPLAY_BLACK);
      last_buttons_value = buttons_value;
    }
  }
  display_fillRect(BTN0_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                   DISPLAY_BLACK);
  display_fillRect(BTN1_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                   DISPLAY_BLACK);
  display_fillRect(BTN2_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                   DISPLAY_BLACK);
  display_fillRect(BTN3_POSITION, 0, RECTANGLE_WIDTH, RECTANGLE_HEIGHT,
                   DISPLAY_BLACK);
}
