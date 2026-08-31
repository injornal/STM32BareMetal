#include <stdint.h>
#include <stdlib.h>

// register address
#define RCC_BASE 0x40021000
#define GPIOC_BASE 0x40011000
#define RCC_APB2ENR *(volatile uint32_t*)(RCC_BASE + 0x18)
#define GPIOC_CRH *(volatile uint32_t*)(GPIOC_BASE + 0x04)
#define GPIOC_ODR *(volatile uint32_t*)(GPIOC_BASE + 0x0C)

// bit fields
#define RCC_IOPCEN (1 << 4)
#define GPIOC13 (1UL << 13)

void init_clock(void);
void turn_on_led(void);
void turn_off_led(void);
void blink_led(void);
void error(void);

int main(void) {
  init_clock();
  blink_led();

  int* state = (int*)malloc(sizeof(int));
  if (state == NULL) {
    error();
  }
  *state = 1234;
  if (*state == 1234) {
    blink_led();
  } else {
    error();
  }

  return EXIT_SUCCESS;
}

void init_clock(void) {
  RCC_APB2ENR |= RCC_IOPCEN;
  GPIOC_CRH &= 0xFF0FFFFF;
  GPIOC_CRH |= 0x00200000;
}

void turn_on_led(void) { GPIOC_ODR &= ~GPIOC13; }

void turn_off_led(void) { GPIOC_ODR |= GPIOC13; }

void blink_led(void) {
  turn_on_led();
  for (volatile uint32_t i = 0; i < 50000; ++i);
  turn_off_led();
  for (volatile uint32_t i = 0; i < 50000; ++i);
}

void error(void) {
  init_clock();
  while (1) {
    blink_led();
  }
}
