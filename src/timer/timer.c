#include "timer.h"

irq_handler_t tim2_irq_handler_ptr = NULL;

__attribute__((used)) void tim2_irq_handler(void) {
  if (tim2_irq_handler_ptr != NULL) {
    tim2_irq_handler_ptr();
    TIM2_SR &= ~TIM2_SR_UIF;
  }
}

void set_timer(timer_t timer, uint32_t wait_ms) {
  // Turn APB1 on
  RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

  // Enable TIM2 update interrupt
  TIM2_DIER |= TIM2_DIER_UIE;

  // Enable IRQ 28 in NVIC
  NVIC_ISER0 |= (1 << TIM2_IRQn);

  // Set prescaler (72MHz / (7199 + 1) = 10 kHz)
  TIM2_PSC = 7199;

  // Set current counter value
  TIM2_CNT = 0;

  // Set counter overflow value (max_count = 10 kHz * wait_ms / 1000)
  TIM2_ARR = 10 * wait_ms - 1;

  // Enable clock
  TIM2_CR1 = 1;
}

void set_irq_handler(timer_t timer, irq_handler_t irq_handler) {
  tim2_irq_handler_ptr = irq_handler;
}
