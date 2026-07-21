#ifndef _CBF_RUNTIME_H
#define _CBF_RUNTIME_H

#include <stddef.h>

// will throw runtime error and exit
void bfpp_backend_runtime_error(const char *error);

// will initialize backend (use at very start)
void bfpp_backend_init(size_t registers, size_t tape_size);

// will free allocated items (use at very end)
void bfpp_backend_destroy(void);

// will return the value of current cell
long bfpp_backend_get_curret_cell_value(void);

// '>'
void bfpp_backend_ptr_inc(size_t n);

// '<'
void bfpp_backend_ptr_dec(size_t n);

// '+'
void bfpp_backend_ptr_value_add(size_t n);

// '-'
void bfpp_backend_ptr_value_sub(size_t n);

// '.'
void bfpp_backend_putchar(void);

// ','
void bfpp_backend_getchar(void);

// =n (register n = current cell value)
void bfpp_backend_setreg(size_t reg);

// $n (current cell value = register n)
void bfpp_backend_getreg(size_t reg);

// ? (current cell value = random number between 0 and current value IF current
// cell value > 0, else random number between 0 and 255)
void bfpp_backend_random(void);

// ! (will execute the value of current cell as an instruction) (will NOT
// execute '!' itself for not having crash)
void bfpp_backend_exec(void);

#endif
