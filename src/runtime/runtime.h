#ifndef _CBF_RUNTIME_H
#define _CBF_RUNTIME_H

#include <stddef.h>

// will throw runtime error and exit
void runtime_runtime_error(const char *error);

// will initialize backend (use at very start)
void runtime_init(size_t registers, size_t tape_size);

// will free allocated items (use at very end)
void runtime_destroy(void);

// will return the value of current cell
long runtime_get_curret_cell_value(void);

// '>'
void runtime_ptr_inc(size_t n);

// '<'
void runtime_ptr_dec(size_t n);

// '+'
void runtime_ptr_value_add(size_t n);

// '-'
void runtime_ptr_value_sub(size_t n);

// '.'
void runtime_putchar(void);

// ','
void runtime_getchar(void);

// =n (register n = current cell value)
void runtime_setreg(size_t reg);

// $n (current cell value = register n)
void runtime_getreg(size_t reg);

// ? (current cell value = random number between 0 and current value IF current
// cell value > 0, else random number between 0 and 255)
void runtime_random(void);

// ! (will execute the value of current cell as an instruction) (will NOT
// execute '!' itself for not having crash)
void runtime_exec(void);

#endif
