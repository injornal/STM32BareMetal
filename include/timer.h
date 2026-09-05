#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

#include "types.h"

#define TIM2_BASE 0x40000000
#define TIM2_CR1 *((volatile uint32_t*)(TIM2_BASE))
#define TIM2_CNT *((volatile uint32_t*)(TIM2_BASE + 0x24))
#define TIM2_PSC *((volatile uint32_t*)(TIM2_BASE + 0x28))
#define TIM2_ARR *((volatile uint32_t*)(TIM2_BASE + 0x2C))
#define TIM2_DIER (*(volatile uint32_t*)(TIM2_BASE + 0x0C))
#define TIM2_DIER_UIE 1
#define TIM2_SR (*(volatile uint32_t*)(TIM2_BASE + 0x10))
#define TIM2_SR_UIF 1

#define TIM2_FREQ 8000000

#define NVIC_ISER0 (*(volatile uint32_t*)0xE000E100)

#define RCC_BASE 0x40021000
#define RCC_APB1ENR (*(volatile uint32_t*)(RCC_BASE + 0x1C))
#define RCC_APB1ENR_TIM2EN 1

#define TIM2_IRQn 28

typedef void (*irq_handler_t)(void);
typedef enum { TIM2 } timer_t;

__attribute((used)) void tim2_irq_handler(void);

void set_timer(timer_t timer, uint32_t wait_ms);
void set_irq_handler(timer_t timer, irq_handler_t irq_handler);

#endif  // TIMER_H_
