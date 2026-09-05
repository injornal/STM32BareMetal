#include <stdint.h>

#include "timer.h"

// register address
#define RCC_BASE 0x40021000
#define GPIOC_BASE 0x40011000
#define RCC_APB2ENR *(volatile uint32_t*)(RCC_BASE + 0x18)
#define GPIOC_CRH *(volatile uint32_t*)(GPIOC_BASE + 0x04)
#define GPIOC_ODR *(volatile uint32_t*)(GPIOC_BASE + 0x0C)

// bit fields
#define RCC_IOPCEN (1 << 4)
#define GPIOC13 (1UL << 13)

#define BLINK_SLOW 1000
#define BLINK_FAST 200

#define ASM_WFI __asm__ __volatile__("wfi")

extern volatile uint32_t systick_counter;

void init_clock(void);
void turn_on_led(void);
void turn_off_led(void);
void toggle_led(void);
void blink_led(uint32_t wait_ms);
void error(void);

int main(void) {
  init_clock();
  set_timer(TIM2, BLINK_SLOW);
  set_irq_handler(TIM2, toggle_led);
  while (1) {
    ASM_WFI;
  }
}

void init_clock(void) {
  RCC_APB2ENR |= RCC_IOPCEN;
  GPIOC_CRH &= 0xFF0FFFFF;
  GPIOC_CRH |= 0x00200000;
}

void turn_on_led(void) { GPIOC_ODR &= ~GPIOC13; }

void turn_off_led(void) { GPIOC_ODR |= GPIOC13; }

void toggle_led(void) { GPIOC_ODR ^= GPIOC13; }

void blink_led(uint32_t wait_ms) {
  turn_on_led();
  for (volatile uint32_t i = 0; i < 8000u * wait_ms; ++i) {
  }
  turn_off_led();
  for (volatile uint32_t i = 0; i < 8000u * wait_ms; ++i) {
  }
}

void error(void) {
  init_clock();
  while (1) {
    blink_led(BLINK_FAST);
  }
}
