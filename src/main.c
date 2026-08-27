#define RCC_APB2ENR (*(volatile unsigned *)0x40021018)
#define GPIOC_CRH   (*(volatile unsigned *)0x40011004)
#define GPIOC_ODR   (*(volatile unsigned *)0x4001100c)

#define IOPCEN   (1u << 4)
#define LED_PIN  13
#define PC13_MODE_OUT_2MHZ (2u << 20)
#define PC13_CFG_MASK      (0xFu << 20)

int main(void) {
  RCC_APB2ENR |= IOPCEN;
  GPIOC_CRH = (GPIOC_CRH & ~PC13_CFG_MASK) | PC13_MODE_OUT_2MHZ;

  while (1) {
    GPIOC_ODR ^= 1u << LED_PIN;
    for (volatile unsigned i = 0; i < 500000u; i++)
      ;
  }
}
