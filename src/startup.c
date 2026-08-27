extern unsigned int _stack; // Provided by the linker file
extern int main(void);

// Let compiler know what section this code belongs to
__attribute__((section(".vectors"))) struct {
  unsigned int *initial_stack_pointer;
  int (*reset)(void);
} vector_table = {
    &_stack,
    main,
};
