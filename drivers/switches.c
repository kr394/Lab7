#include "switches.h"
#include "leds.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

#define SWITCHES_GPIO_DATA_OFFSET 0
#define SWITCHES_GPIO_TRI_OFFSET 4

#define LEDS_PRINT_FAILED_STATUS 1

#define ALL_INPUT 0x00
#define ALL_OFF 0x00
#define ALL_ON 0x0F

// Reads the register which resides a provided offset away from the slide
// switches' base address. The value of "offset" tells the function how far from
// the base address the desired register lies. The function returns the 32-bit
// value that is held in the register.
int32_t switches_readGpioRegister(int32_t offset) {
  return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

// Writes to the register which resides a provided offset away from the slide
// switches' base address. The value of "offset" tells the function where the
// register lies relative to the base address. The value of "value" tells the
// function what should be written to the register.
void switches_writeGpioRegister(int32_t offset, uint32_t value) {
  Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}

// Initializes the switches by setting the Tri-state buffer register to input
// mode (all 0's), and then checking they're at 0. The function returns
// SWITCHES_INIT_STATUS_OK if it was successful (value of 1), and
// SWITCHES_INIT_STATUS_FAIL (0) otherwise.
int32_t switches_init() {
  switches_writeGpioRegister(SWITCHES_GPIO_TRI_OFFSET, ALL_INPUT);
  // Checks to make sure the Tri-state buffer register is all 0's. If so,
  // return 1. Otherwise, return 0.
  if (!switches_readGpioRegister(SWITCHES_GPIO_TRI_OFFSET)) {
    return SWITCHES_INIT_STATUS_OK;
  } else
    return SWITCHES_INIT_STATUS_FAIL;
}

// Calls the above function switches_readGpioRegister() while passing the
// correct offset for the switches' register.
int32_t switches_read() {
  return switches_readGpioRegister(SWITCHES_GPIO_DATA_OFFSET);
}

// Runs the test for the switches. When a switch is on, the corresponding LED
// turns on until all switches are flipped on.
void switches_runTest() {
  // Initializes the switches, and reports an error and terminates the test if
  // the tri-state buffer register is not set to 0
  if (!switches_init()) {
    printf("Switch Test Failed: Switches not set as input\n");
    return;
  }
  leds_init(LEDS_PRINT_FAILED_STATUS);
  printf("Switches and LEDs successfully initiated.\n");

  uint8_t switch_value;

  // Continuously read the switches register and checks if all the switches are
  // on. Until they are, the value of the LED register is set to the value in
  // the switches register.
  while ((switch_value = switches_read()) != ALL_ON) {
    leds_write(switch_value);
  }
  leds_write(ALL_OFF);
}
