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

int main(void) {
  RCC_APB2ENR |= RCC_IOPCEN;
  GPIOC_CRH &= 0xFF0FFFFF;
  GPIOC_CRH |= 0x00200000;

  // if led is ON turn it off, if led is OFF turn it ON
  GPIOC_ODR &= ~GPIOC13;
  for (volatile uint32_t i = 0; i < 500000; ++i);
  GPIOC_ODR |= GPIOC13;
  return EXIT_SUCCESS;
}
