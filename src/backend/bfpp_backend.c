#include "bfpp_backend.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static size_t *tape = nullptr;
static size_t ptr = 0;
static size_t tape_size = 0;

static long *registers = nullptr;
static size_t max_registers = 0;

void bfpp_backend_runtime_error(const char *error) {
    fprintf(stderr, "%s%s\n", "\033[31mbf++ runtime error: \033[0m", error);
    bfpp_backend_destroy();
    exit(1);
}

void bfpp_backend_init(size_t max_reg, size_t _tape_size) {
    // random
    srand(time(nullptr));

    // registers
    max_registers = max_reg;
    registers = malloc(max_reg * sizeof(long));

    // tape
    tape_size = _tape_size;
    tape = malloc(sizeof(size_t) * _tape_size);
    if (tape) {
        memset(tape, 0, sizeof(size_t) * _tape_size);
    }
}

void bfpp_backend_destroy(void) {
    free(registers); // registers
    free(tape);      // tape
}

long bfpp_backend_get_curret_cell_value(void) { return tape[ptr]; }

void bfpp_backend_ptr_inc(size_t n) { ptr = (ptr + n) % tape_size; }

void bfpp_backend_ptr_dec(size_t n) { ptr = (ptr - n + tape_size) % tape_size; }

void bfpp_backend_ptr_value_add(size_t n) { tape[ptr] += n; }

void bfpp_backend_ptr_value_sub(size_t n) { tape[ptr] -= n; }

void bfpp_backend_putchar(void) { putchar(tape[ptr]); }

void bfpp_backend_getchar(void) { tape[ptr] = getchar(); }

void bfpp_backend_setreg(size_t reg) {
    if (reg >= max_registers) {
        bfpp_backend_runtime_error("register out of bounds to set.");
    }
    registers[reg] = bfpp_backend_get_curret_cell_value();
};

void bfpp_backend_getreg(size_t reg) {
    if (reg >= max_registers) {
        bfpp_backend_runtime_error("register out of bounds to get.");
    }
    tape[ptr] = registers[reg];
}

void bfpp_backend_random(void) {
    if (tape[ptr] > 0) {
        tape[ptr] = rand() % (tape[ptr] + 1);
    } else {
        tape[ptr] = rand() % 256;
    }
}

void bfpp_backend_exec(void) {
    switch (tape[ptr]) {
    case '+':
        bfpp_backend_ptr_value_add(1);
        break;
    case '-':
        bfpp_backend_ptr_value_sub(1);
        break;
    case '>':
        bfpp_backend_ptr_inc(1);
        break;
    case '<':
        bfpp_backend_ptr_dec(1);
        break;
    case '.':
        bfpp_backend_putchar();
        break;
    case ',':
        bfpp_backend_getchar();
        break;
    case '?':
        bfpp_backend_random();
        break;
    default:
        bfpp_backend_runtime_error("invalid instruction; cannot execute it");
    }
}
