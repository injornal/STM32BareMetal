#include <stdint.h>

// System timer registers
#define SYST_CSR (*(volatile uint32_t*)0xE000E010)
#define SYST_CSR_ENABLE (1u << 0)
#define SYST_CSR_TICKINT (1u << 1)
#define SYST_CSR_CLKSOURCE (1u << 2)
#define SYST_RVR (*(volatile uint32_t*)0xE000E014)
#define SYST_CVR (*(volatile uint32_t*)0xE000E018)
#define SYST_CALIB (*(volatile uint32_t*)0xE000E01C)
#define SCB_SHPR2 (*(volatile uint32_t*)0xE000ED1C)
#define CLOCK_FREQ 8000000

#define ASM_WFI __asm__ __volatile__("wfi")

// User defines
#define TIM2_IRQn 28

// Provided by the linker file
extern uint32_t _la_data;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _stack;

int main(void);
__attribute__((noreturn)) extern void error(void);

// Type for the IVT handlers (ISRs)
typedef void (*isr_handler_t)(void);
typedef void (*irq_handler_t)(void);

// Definitions of all the handlers
static void reset_handler(void);
static void clock_init(void);
static void data_init(void);
static void bss_init(void);
static void systick_init(void);

static void nmi_handler(void);
static void hard_fault_handler(void);
static void mem_manage_handler(void);
static void bus_fault_handler(void);
static void usage_fault_handler(void);
static void svc_handler(void);
static void pend_sv_handler(void);
static void systick_handler(void);

void tim2_irq_handler(void);

__attribute__((section(".isr_vector"))) struct {
  uintptr_t initial_sp;       // 0x0000
  isr_handler_t reset;        // 0x0004
  isr_handler_t nmi;          // 0x0008
  isr_handler_t hard_fault;   // 0x000c
  isr_handler_t mem_manage;   // 0x0010
  isr_handler_t bus_fault;    // 0x0014
  isr_handler_t usage_fault;  // 0x0018

  isr_handler_t reserved_7;   // 0x001c
  isr_handler_t reserved_8;   // 0x0020
  isr_handler_t reserved_9;   // 0x0024
  isr_handler_t reserved_10;  // 0x0028

  isr_handler_t svc;             // 0x002c
  isr_handler_t reserved_debug;  // 0x0030
  isr_handler_t reserved_13;     // 0x0034
  isr_handler_t pend_sv;         // 0x0038
  isr_handler_t systick;         // 0x003c

  isr_handler_t irq[68];  // 0x0040 - 0x014c
} vector_table = {.initial_sp = (uintptr_t)&_stack,
                  .reset = reset_handler,
                  .nmi = nmi_handler,
                  .hard_fault = hard_fault_handler,
                  .mem_manage = mem_manage_handler,
                  .bus_fault = bus_fault_handler,
                  .usage_fault = usage_fault_handler,
                  .svc = svc_handler,
                  .pend_sv = pend_sv_handler,
                  .systick = systick_handler,
                  .irq[TIM2_IRQn] = tim2_irq_handler};

__attribute__((noreturn)) static void reset_handler(void) {
  data_init();
  bss_init();
  clock_init();
  systick_init();

  main();

  while (1);
}

#define RCC_BASE 0x40021000
#define RCC_CR (*(volatile uint32_t*)(RCC_BASE + 0x00))
#define RCC_CFGR (*(volatile uint32_t*)(RCC_BASE + 0x04))
#define RCC_CR_HSEON (1u << 16)
#define RCC_CR_HSERDY (1u << 17)
#define RCC_CR_PLLON (1u << 24)
#define RCC_CR_PLLRDY (1u << 25)
#define RCC_CFGR_SW (0b11)
#define RCC_CFGR_SW_PLL (0b10)
#define RCC_CFGR_SWS (0b11 << 2)
#define RCC_CFGR_SWS_PLL (0b10 << 2)
#define RCC_CFGR_PLLSRC (1u << 16)
#define RCC_CFGR_PLLSRC_HSE (1u << 16)
#define RCC_CFGR_PLLXTPRE (1u << 17)
#define RCC_CFGR_PLLMULL (0b1111 << 18)
#define RCC_CFGR_PLLMULL9 (0b0111 << 18)
#define RCC_CFGR_HPRE (0b1111 << 4)
#define RCC_CFGR_HPRE_DIV1 (0b0000 << 4)
#define RCC_CFGR_PPRE1_DIV2 (0b100 << 8)
#define RCC_CFGR_PPRE2_DIV1 (0b000 << 11)
#define RCC_CFGR_PPRE2 (0b111 << 11)
#define FLASH_BASE 0x40022000
#define FLASH_ACR (*(volatile uint32_t*)(FLASH_BASE + 0x00))
#define FLASH_ACR_PRFTBE (1u << 4)
#define FLASH_ACR_LATENCY (0b111)
#define FLASH_ACR_LATENCY_2 (0b010)

static void clock_init(void) {
  // 1. Enable HSE
  RCC_CR |= RCC_CR_HSEON;
  while (!(RCC_CR & RCC_CR_HSERDY));

  // 2. Configure Flash 2 wait states
  FLASH_ACR |= FLASH_ACR_PRFTBE;
  FLASH_ACR &= ~FLASH_ACR_LATENCY;
  FLASH_ACR |= FLASH_ACR_LATENCY_2;

  // 3. Setup bus prescalers
  // AHB = SYSCLK / 1 = 72 MHz
  RCC_CFGR |= RCC_CFGR_HPRE_DIV1;

  // APB1 = HCLK / 2 = 36 MHz (max allowed for APB1)
  RCC_CFGR |= RCC_CFGR_PPRE1_DIV2;

  // APB2 = HCLK / 1 = 72 MHz
  RCC_CFGR |= RCC_CFGR_PPRE2_DIV1;

  // 4. Configure PLL: HSE as input (bit 16), PLLMUL = 9 (bits 18:21 -> 0b0111)
  RCC_CFGR &=
      (uint32_t)(~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
  RCC_CFGR |= (RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

  // 5. Enable PLL and wait for lock
  RCC_CR |= RCC_CR_PLLON;
  while (!(RCC_CR & RCC_CR_PLLRDY));

  // 6. Select PLL as system clock source
  RCC_CFGR &= ~RCC_CFGR_SW;
  RCC_CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC_CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

static void data_init(void) {
  uint32_t* src = &_la_data;
  uint32_t* dst = &_sdata;
  while (dst < &_edata) {
    *dst++ = *src++;
  }
}

static void bss_init(void) {
  uint32_t* dst = &_sbss;
  while (dst < &_ebss) {
    *dst++ = 0;
  }
}

static void systick_init(void) {
  SYST_RVR = CLOCK_FREQ / 1000 - 1;  // Set reload value for 1ms tick
  SYST_CVR = 0;                      // Clear current value
  SYST_CSR = SYST_CSR_ENABLE | SYST_CSR_TICKINT | SYST_CSR_CLKSOURCE;
}

__attribute__((noreturn)) static void nmi_handler(void) { error(); }
__attribute__((noreturn)) static void hard_fault_handler(void) { error(); }
__attribute__((noreturn)) static void mem_manage_handler(void) { error(); }
__attribute__((noreturn)) static void bus_fault_handler(void) { error(); }
__attribute__((noreturn)) static void usage_fault_handler(void) { error(); }
__attribute__((noreturn)) static void svc_handler(void) { error(); }

static void pend_sv_handler(void) { error(); }

volatile uint32_t systick_counter = 0;
static void systick_handler(void) { systick_counter++; }
