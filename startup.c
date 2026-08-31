#include <stdint.h>

// Provided by the linker file
extern uint32_t _la_data;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _stack;

extern int main(void);

// Type for the IVT handlers (ISRs)
typedef void (*isr_handler_t)(void);

// Definitions of all the handlers
static void reset_handler(void);
static void data_init(void);
static void bss_init(void);

static void nmi_handler(void);
static void hard_fault_handler(void);
static void mem_manage_handler(void);
static void bus_fault_handler(void);
static void usage_fault_handler(void);
static void svc_handler(void);
static void pend_sv_handler(void);
static void systick_handler(void);

// IVT layout
__attribute__((section(".isr_vector"))) struct {
  uintptr_t initial_sp;       // 0x0000
  isr_handler_t reset;        // 0x0004
  isr_handler_t nmi;          // 0x0008
  isr_handler_t hard_fault;   // 0x000C
  isr_handler_t mem_manage;   // 0x0010
  isr_handler_t bus_fault;    // 0x0014
  isr_handler_t usage_fault;  // 0x0018

  isr_handler_t reserved_7;   // 0x001C
  isr_handler_t reserved_8;   // 0x0020
  isr_handler_t reserved_9;   // 0x0024
  isr_handler_t reserved_10;  // 0x0028

  isr_handler_t svc;             // 0x002C
  isr_handler_t reserved_debug;  // 0x0030
  isr_handler_t reserved_13;     // 0x0034
  isr_handler_t pend_sv;         // 0x0038
  isr_handler_t systick;         // 0x003C

  isr_handler_t irq[68];  // 0x0040 - 0x014C
} vector_table = {.initial_sp = (uintptr_t)&_stack,
                  .reset = reset_handler,
                  .nmi = nmi_handler,
                  .hard_fault = hard_fault_handler,
                  .mem_manage = mem_manage_handler,
                  .bus_fault = bus_fault_handler,
                  .usage_fault = usage_fault_handler,
                  .svc = svc_handler,
                  .pend_sv = pend_sv_handler,
                  .systick = systick_handler};

__attribute__((noreturn)) static void reset_handler(void) {
  data_init();
  bss_init();
  main();
  while (1);
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

__attribute__((noreturn)) static void nmi_handler(void) { while (1); }
__attribute__((noreturn)) static void hard_fault_handler(void) { while (1); }
__attribute__((noreturn)) static void mem_manage_handler(void) { while (1); }
__attribute__((noreturn)) static void bus_fault_handler(void) { while (1); }
__attribute__((noreturn)) static void usage_fault_handler(void) { while (1); }
__attribute__((noreturn)) static void svc_handler(void) { while (1); }
__attribute__((noreturn)) static void pend_sv_handler(void) { while (1); }
__attribute__((noreturn)) static void systick_handler(void) { while (1); }
