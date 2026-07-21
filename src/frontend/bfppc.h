#ifndef _cbrainfuck_h
#define _cbrainfuck_h

#define _tmp_files_path_max_size 4096
#define _filename_max_size 100
#define _loop_stack_size 61440 // 60kb

#define CC "gcc"

#include "tokenizer.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    const char input_name[_filename_max_size];
    const char output_name[_filename_max_size];

    bool show_temp;
    size_t registers_size;
    size_t tape_size;
} Options;

typedef enum {
    instruct_NONE,

    instruct_add,
    instruct_sub,
    instruct_right,
    instruct_left,

    instruct_putc,
    instruct_getc,

    instruct_loop,
    instruct_loopend,

    instruct_setreg,
    instruct_getreg,

    instruct_random,
    instruct_exec,
} instruct_type;

bool bfppc_compile(Token *tokens, Options options);

#endif
