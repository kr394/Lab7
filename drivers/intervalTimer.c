#include "intervalTimer.h"
#include "utils.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

#define OFFSET_TCSR0 0x00
#define OFFSET_TLR0 0x04
#define OFFSET_TCR0 0x08
#define OFFSET_TCSR1 0x10
#define OFFSET_TLR1 0x14
#define OFFSET_TCR1 0x18

#define CASC_BIT                                                               \
  11 // These determine how far a 1 or 0 must be shifted to correspond with the
     // bit named
#define ENT_BIT 07
#define LOAD_BIT 05
#define UDT_BIT 01

#define ALL_OFF 0x00
#define HIGH 0x1
#define LOW 0x0

#define TIMER_FREQUENCY                                                        \
  (XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ *                                            \
   1.0) // Copies the clock frequency as a double
#define LONG_INT_SHIFT                                                         \
  32 // Used when concatenating the two 32 bit timer registers into a 64 bit
     // variable

#define ONE_SECOND_DELAY 1000
#define TEN_SECOND_DELAY 10000
#define FORTY_FIVE_SECOND_DELAY 45000

// This function reads the value in a memory address.
// The inputs are the base address of the timer, and an offset away from that
// address.
int32_t intervalTimer_readGpioRegister(int32_t baseAddress, int32_t offset) {
  return Xil_In32(baseAddress + offset);
}

// This function writes a given value into the memory address passed in.
// The inputs are the base address of the timer, an offset away from that base
// address, and the value to be written to the register.
void intervalTimer_writeGpioRegister(int32_t baseAddress, int32_t offset,
                                     uint32_t value) {
  Xil_Out32(baseAddress + offset, value);
}

// This is a helper function which determines the base address of the desired
// timer. The input is any of the three timer numbers defined in the .h file.
// The output is the base address of that particular timer.
uint32_t intervalTimer_determineBaseaddress(uint32_t timerNumber) {
  uint32_t timerBase;
  // Determines the base address of the passed in timer number
  if (timerNumber == INTERVAL_TIMER_TIMER_0) {
    timerBase = XPAR_AXI_TIMER_0_BASEADDR;
  } else if (timerNumber == INTERVAL_TIMER_TIMER_1) {
    timerBase = XPAR_AXI_TIMER_1_BASEADDR;
  } else if (timerNumber == INTERVAL_TIMER_TIMER_2) {
    timerBase = XPAR_AXI_TIMER_2_BASEADDR;
  }
  return timerBase;
}

// This function is called whenever you want to reuse an interval timer.
// For example, say the interval timer has been used in the past, the user
// will call intervalTimer_reset() prior to calling intervalTimer_start().
// timerNumber indicates which timer should reset.
void intervalTimer_reset(uint32_t timerNumber) {
  uint32_t timerBase = intervalTimer_determineBaseaddress(timerNumber);

  intervalTimer_stop(timerNumber);
  // Set the counters to value 0 by setting load bit high in registers TCSR0 and
  // TCSR1
  intervalTimer_writeGpioRegister(timerBase, OFFSET_TCSR0, HIGH << LOAD_BIT);
  intervalTimer_writeGpioRegister(timerBase, OFFSET_TCSR1, HIGH << LOAD_BIT);
  // Clear the load bit so the counters can function correctly
  intervalTimer_writeGpioRegister(timerBase, OFFSET_TCSR0, ALL_OFF);
  intervalTimer_writeGpioRegister(timerBase, OFFSET_TCSR1, ALL_OFF);
  // Set the cascade bit high in register TCSR0
  intervalTimer_writeGpioRegister(timerBase, OFFSET_TCSR0, HIGH << CASC_BIT);
}

// You must initialize the timers before you use them the first time.
// It is generally only called once but should not cause an error if it
// is called multiple times.
// timerNumber indicates which timer should be initialized.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber) {
  uint32_t timerBase = intervalTimer_determineBaseaddress(timerNumber);

  // Clear both of the control registers TCSR0 and TCSR1 - This also defines the
  // counters as up-counters (UDT bit set to 0)
  intervalTimer_writeGpioRegister(timerBase, OFFSET_TCSR0, ALL_OFF);
  intervalTimer_writeGpioRegister(timerBase, OFFSET_TCSR1, ALL_OFF);
  // Write 0 to registers TLR0 and TLR1
  intervalTimer_writeGpioRegister(timerBase, OFFSET_TLR0, ALL_OFF);
  intervalTimer_writeGpioRegister(timerBase, OFFSET_TLR1, ALL_OFF);
  // Use intervalTimer_reset function to load the TLR registers into the TCR
  // registers, then set the cascade bit high in register TCSR0
  intervalTimer_reset(timerNumber);

  // Return OK status if both the counter registers TCR0 and TCR1 are at 0, and
  // only the CASC bit is high in TCSR0. Otherwise, return fail
  if ((intervalTimer_readGpioRegister(timerBase, OFFSET_TCR0) == ALL_OFF) &&
      (intervalTimer_readGpioRegister(timerBase, OFFSET_TCR1) == ALL_OFF)) {
    if (intervalTimer_readGpioRegister(timerBase, OFFSET_TCSR0) ==
        HIGH << CASC_BIT) {
      return INTERVAL_TIMER_STATUS_OK;
    }
  }
  return INTERVAL_TIMER_STATUS_FAIL;
}

// This is a convenience function that initializes all interval timers.
// Simply calls intervalTimer_init() on all timers.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_initAll() {
  // Only returns INTERVAL_TIMER_STATUS_OK if all three initializations are
  // successful
  if (intervalTimer_init(INTERVAL_TIMER_TIMER_0)) {
    if (intervalTimer_init(INTERVAL_TIMER_TIMER_1)) {
      if (intervalTimer_init(INTERVAL_TIMER_TIMER_2)) {
        return INTERVAL_TIMER_STATUS_OK;
      }
    }
  }
  return INTERVAL_TIMER_STATUS_FAIL;
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber) {
  // Determines the correct timer and sets the enable bit ENT0 high without
  // disrupting other values in register TCSR0
  uint32_t timerBase = intervalTimer_determineBaseaddress(timerNumber);
  intervalTimer_writeGpioRegister(
      timerBase, OFFSET_TCSR0,
      intervalTimer_readGpioRegister(timerBase, OFFSET_TCSR0) |
          (HIGH << ENT_BIT));
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber) {
  // Determines the correct timer and sets the enable bit ENT0 low without
  // disrupting other values in register TCSR0
  uint32_t timerBase = intervalTimer_determineBaseaddress(timerNumber);
  intervalTimer_writeGpioRegister(
      timerBase, OFFSET_TCSR0,
      intervalTimer_readGpioRegister(timerBase, OFFSET_TCSR0) &
          ~(HIGH << ENT_BIT));
}

// Convenience function for intervalTimer_reset().
// Simply calls intervalTimer_reset() on all timers.
void intervalTimer_resetAll() {
  intervalTimer_reset(INTERVAL_TIMER_TIMER_0);
  intervalTimer_reset(INTERVAL_TIMER_TIMER_1);
  intervalTimer_reset(INTERVAL_TIMER_TIMER_2);
}

// Runs a test on a single timer as indicated by the timerNumber argument.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_test(uint32_t timerNumber) {
  uint32_t timerBase = intervalTimer_determineBaseaddress(timerNumber);

  printf("Testing Timer %d\n", timerNumber);
  if (!intervalTimer_init(timerNumber))
    return INTERVAL_TIMER_STATUS_FAIL;
  printf("Initialized Timer %d. Value in register: %d\n", timerNumber,
         intervalTimer_readGpioRegister(timerBase, OFFSET_TCR0));

  printf("Starting Timer\n");
  intervalTimer_start(timerNumber);
  utils_msDelay(ONE_SECOND_DELAY);
  intervalTimer_stop(timerNumber);
  printf("Timer Stopped. Value in register: %d\n",
         intervalTimer_readGpioRegister(timerBase, OFFSET_TCR0));

  printf("Resetting Timer\n");
  intervalTimer_reset(timerNumber);
  printf("Timer Reset. Value in register: %d\n",
         intervalTimer_readGpioRegister(timerBase, OFFSET_TCR0));

  printf("Next test will start timer, run for 1 second, stop timer, print "
         "value, run again for 10 seconds, stop and print value.\n");
  intervalTimer_start(timerNumber);
  utils_msDelay(ONE_SECOND_DELAY);
  intervalTimer_stop(timerNumber);
  printf("Value in register after 1 second: %d\n",
         intervalTimer_readGpioRegister(timerBase, OFFSET_TCR0));
  printf("Run time in seconds: %f\n",
         intervalTimer_getTotalDurationInSeconds(timerNumber));
  intervalTimer_start(timerNumber);
  utils_msDelay(TEN_SECOND_DELAY);
  intervalTimer_stop(timerNumber);
  printf("Value in register after 10 more seconds: %d\n",
         intervalTimer_readGpioRegister(timerBase, OFFSET_TCR0));
  printf("Run time in seconds: %f\n",
         intervalTimer_getTotalDurationInSeconds(timerNumber));

  printf("Resetting Timer\n");
  intervalTimer_reset(timerNumber);

  printf("Last test will run timer for 45 seconds (enough time to cascade into "
         "second timer register).\n");
  intervalTimer_start(timerNumber);
  utils_msDelay(FORTY_FIVE_SECOND_DELAY);
  printf("Value in upper register: %d\n",
         intervalTimer_readGpioRegister(timerBase, OFFSET_TCR1));
  printf("Value in lower register: %d\n",
         intervalTimer_readGpioRegister(timerBase, OFFSET_TCR0));
  printf("Run time in seconds: %f\n",
         intervalTimer_getTotalDurationInSeconds(timerNumber));

  printf("Timer %d Test Complete. Resetting Timer\n", timerNumber);
  intervalTimer_reset(timerNumber);
  return INTERVAL_TIMER_STATUS_OK;
}

// Convenience function that invokes test on all interval timers.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_testAll() {
  // Only returns INTERVAL_TIMER_STATUS_OK if all timer tests are successful.
  if (intervalTimer_test(INTERVAL_TIMER_TIMER_0)) {
    if (intervalTimer_test(INTERVAL_TIMER_TIMER_1)) {
      if (intervalTimer_test(INTERVAL_TIMER_TIMER_2)) {
        return INTERVAL_TIMER_STATUS_OK;
      }
    }
  }
  return INTERVAL_TIMER_STATUS_FAIL;
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  uint32_t timerBase = intervalTimer_determineBaseaddress(timerNumber);

  uint32_t lower32;
  uint32_t upper32;
  // Reads values from counters as described in documentation. Reads upper32
  // first, then lower32, then, if upper32 changed, tries again until upper32
  // doesn't change during the reading process.
  do {
    upper32 = intervalTimer_readGpioRegister(timerBase, OFFSET_TCR1);
    lower32 = intervalTimer_readGpioRegister(timerBase, OFFSET_TCR0);
  } while (intervalTimer_readGpioRegister(timerBase, OFFSET_TCR1) != upper32);

  // Concatenate the upper and lower 32 bits into a single 64 bit variable,
  // then performs floating point division to determine runtime in seconds
  uint64_t registerValue = upper32;
  registerValue = (registerValue << LONG_INT_SHIFT) | lower32;
  double timerTotal = (double)registerValue / TIMER_FREQUENCY;
  return timerTotal;
}
