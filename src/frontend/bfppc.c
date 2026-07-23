#define _DEFAULT_SOURCE

#include "bfppc.h"
#include "tokenizer.h"
#include "version.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


static instruct_type prv_instruct = instruct_NONE;
static size_t prv_instruct_count = 0;

static size_t loop_id = 0;
static size_t loop_breakpoints[_loop_stack_size];
static size_t highest_loop_brakepoint = 0;

// the template for the assembly code (just hard coding cuz it just maps symbols
// with function names, and some other tasts for loops and all)
constexpr char _init_templete[] =
    "# bfpcc generated code (version " bfppc_version_string ")\n\n"
    "# backend functions\n"
    ".extern bfpp_backend_init\n"
    ".extern bfpp_backend_destroy\n"
    ".extern bfpp_backend_get_curret_cell_value\n"
    ".extern bfpp_backend_ptr_inc\n"
    ".extern bfpp_backend_ptr_dec\n"
    ".extern bfpp_backend_ptr_value_add\n"
    ".extern bfpp_backend_ptr_value_sub\n"
    ".extern bfpp_backend_putchar\n"
    ".extern bfpp_backend_getchar\n"
    ".extern bfpp_backend_setreg\n"
    ".extern bfpp_backend_getreg\n"
    ".extern bfpp_backend_random\n"
    ".extern bfpp_backend_exec\n"
    "\n.section .text\n"
    "\n.global main\n"
    ".type main, @function\n"
    "main:\n\n"
    "    pushq       %%rbp\n"
    "    movq        %%rsp, %%rbp\n\n"
    "    # initilizing the backend\n"
    "    movq        $%ld, %%rdi # max registers\n"
    "    movq        $%ld, %%rsi # tape size\n"
    "    call        bfpp_backend_init\n\n";

// private

bool error(Token token, const char *input_filename, const char *details,
           char *code_to_free) {
    fprintf(stderr,
            "Error in "
            "%s:%ld:%ld:\n\n\033[35m    %s\33[0m\n    "
            "%*c\n\n\033[91m%s\033[0m\nExiting with "
            "code 1.\n\n",
            input_filename, token.line, token.col, token.linec, (int)token.col,
            '^', details);
    free(code_to_free);

    return false;
}

bool output(const char *output_name, const char *code, bool show_temp) {
    const char *backend_lib = "libbfpp_backend.a";

    char tmp_asm[_tmp_files_path_max_size];
    char tmp_obj[_tmp_files_path_max_size];

    strcpy(tmp_asm, show_temp ? "bfppc_XXXXXX.s" : "/tmp/bfppc_XXXXXX.s");
    strcpy(tmp_obj, show_temp ? "bfppc_XXXXXX.o" : "/tmp/bfppc_XXXXXX.o");

    int asm_fd = mkstemps(tmp_asm, 2);
    if (asm_fd == -1) {
        remove(tmp_asm);
        fprintf(stderr, "error: cannot make assembly file.\n");
        return false;
    }
    close(asm_fd);

    int obj_fd = mkstemps(tmp_obj, 2);
    if (obj_fd == -1) {
        remove(tmp_asm);
        fprintf(stderr, "error: cannot make object file.\n");
        return false;
    }
    close(obj_fd);

    FILE *asm = fopen(tmp_asm, "w");
    if (!asm) {
        fprintf(stderr, "error: unable to create assembly file.\n");
        return false;
    }

    fputs(code, asm);
    fclose(asm);

    {
        char command[_tmp_files_path_max_size * 3];
        snprintf(command, sizeof(command), CC " \"%s\" -o \"%s\"", tmp_asm,
                 tmp_obj);

        if (system(command) != 0) {
            fprintf(stderr, "error: unexpected behaivour during assembling.\n");
            return false;
        }
    }

    {
        char command[_tmp_files_path_max_size * 3];
        snprintf(command, sizeof(command),
                 CC " \"%s\" -L/usr/local/lib -l:\"%s\" -o \"%s\"", tmp_obj,
                 backend_lib, output_name);

        if (system(command) != 0) {
            fprintf(stderr, "error: unexpected behaivour during linking.\n");
            return false;
        }
    }

    return true;
}

void add_instruct(char **code, size_t *code_size, instruct_type instruct_,
                  size_t count) {
    if (instruct_ == instruct_NONE) {
        return;
    }

    char setarg_buf[100];
    snprintf(setarg_buf, 100, "    movq        $%ld, %%rdi\n", count);
    size_t setarg_size = strlen(setarg_buf);

    char instruct[1024] = {0};

    switch (instruct_) {
    case instruct_NONE:
        return;

    case instruct_right:
        strcpy(instruct, "    call        bfpp_backend_ptr_inc\n\n");
        break;
    case instruct_left:
        strcpy(instruct, "    call        bfpp_backend_ptr_dec\n\n");
        break;
    case instruct_add:
        strcpy(instruct, "    call        bfpp_backend_ptr_value_add\n\n");
        break;
    case instruct_sub:
        strcpy(instruct, "    call        bfpp_backend_ptr_value_sub\n\n");
        break;

    case instruct_putc:
        strcpy(instruct, "    call        bfpp_backend_putchar\n\n");
        setarg_size = 0;
        break;
    case instruct_getc:
        strcpy(instruct, "    call        bfpp_backend_getchar\n\n");
        setarg_size = 0;
        break;

    case instruct_loop: {
        ++loop_id;

        char buf[100];
        snprintf(buf, 100, "\n#loop %ld\n__loop_%ld_:\n", loop_id, loop_id);
        loop_breakpoints[++highest_loop_brakepoint] = loop_id;

        strcpy(instruct, buf);
        setarg_size = 0;
        break;
    }
    case instruct_loopend: {
        char buf[1024];
        snprintf(buf, 1024,
                 "\n# loop %ld end\n"
                 "call        bfpp_backend_get_curret_cell_value\n"
                 "cmpq        $0, %%rax\n"
                 "jne         __loop_%ld_\n\n",
                 loop_breakpoints[highest_loop_brakepoint],
                 loop_breakpoints[highest_loop_brakepoint]);

        --highest_loop_brakepoint;
        strcpy(instruct, buf);
        setarg_size = 0;
        break;
    }

    case instruct_setreg:
        strcpy(instruct, "    call        bfpp_backend_setreg\n\n");
        break;

    case instruct_getreg:
        strcpy(instruct, "    call        bfpp_backend_getreg\n\n");
        break;

    case instruct_random:
        strcpy(instruct, "    call        bfpp_backend_random\n\n");
        setarg_size = 0;
        break;

    case instruct_exec:
        strcpy(instruct, "    call        bfpp_backend_exec\n\n");
        setarg_size = 0;
        break;
    }

    size_t instruct_len = strlen(instruct);
    size_t full_size = instruct_len + setarg_size;

    *code = realloc(*code, *code_size + full_size + 1);
    if (!*code) {
        fprintf(stderr, "error: memory allocation failed.\n");
        exit(1);
    }

    if (setarg_size > 0) {
        memcpy(*code + *code_size, setarg_buf, setarg_size);
        *code_size += setarg_size;
    }

    memcpy(*code + *code_size, instruct, instruct_len);
    *code_size += instruct_len;

    (*code)[*code_size] = '\0';
}

// public

bool bfppc_compile(Token *tokens, Options options) {
    prv_instruct = instruct_NONE;
    prv_instruct_count = 0;
    loop_id = 0;
    highest_loop_brakepoint = 0;

    size_t tokens_size = 0;
    while (tokens[tokens_size++].type != TokenType_EOF)
        ;

    char init_instructs[sizeof(_init_templete) +
                        _num_max_digits]; // padding with _num_max_digits (for
                                          // numbers with more than 3
                                          // ('%','l','d') digits)
    sprintf(init_instructs, _init_templete, options.registers_size,
            options.tape_size);

    char *code = (char *)malloc(sizeof(init_instructs));
    strcpy(code, init_instructs);
    size_t code_size = strlen(code);

    for (size_t ip = 0; ip < tokens_size; ++ip) {
        instruct_type current_instruct = instruct_NONE;

        switch (tokens[ip].type) {
        case TokenType_EOF:
            continue;

        case TokenType_Right:
            current_instruct = instruct_right;
            break;
        case TokenType_Left:
            current_instruct = instruct_left;
            break;
        case TokenType_ADD:
            current_instruct = instruct_add;
            break;
        case TokenType_SUB:
            current_instruct = instruct_sub;
            break;

        case TokenType_Dot:
            current_instruct = instruct_putc;
            break;
        case TokenType_Comma:
            current_instruct = instruct_getc;
            break;

        case TokenType_SqOpen:
            current_instruct = instruct_loop;
            break;
        case TokenType_SqClose:
            current_instruct = instruct_loopend;
            break;

        case TokenType_Equals:
            current_instruct = instruct_setreg;
            break;
        case TokenType_Dollar:
            current_instruct = instruct_getreg;
            break;

        case TokenType_Question:
            current_instruct = instruct_random;
            break;

        case TokenType_Exclaim:
            current_instruct = instruct_exec;
            break;

        default:
            continue;
        }

        bool is_optimizable = (current_instruct == instruct_add ||
                               current_instruct == instruct_sub ||
                               current_instruct == instruct_right ||
                               current_instruct == instruct_left);
        bool is_arg_needed = (current_instruct == instruct_setreg ||
                              current_instruct == instruct_getreg);

        if (is_optimizable) {
            if (current_instruct == prv_instruct) {
                ++prv_instruct_count;
            } else {
                add_instruct(&code, &code_size, prv_instruct,
                             prv_instruct_count);
                prv_instruct = current_instruct;
                prv_instruct_count = 1;
            }
        }

        else if (is_arg_needed) {
            if (++ip > tokens_size) {
                return error(tokens[ip - 1], options.input_name,
                             "Expected another token after setreg or getarg",
                             code);
            }

            if (prv_instruct != instruct_NONE) {
                add_instruct(&code, &code_size, prv_instruct,
                             prv_instruct_count);
                prv_instruct = instruct_NONE;
                prv_instruct_count = 0;
            }

            if (tokens[ip].type == TokenType_Number) {
                size_t value = tokens[ip].value.value_long;

                if (value > options.registers_size) {
                    return error(tokens[ip], options.input_name,
                                 "Value more than maximum registers", code);
                }

                add_instruct(&code, &code_size, current_instruct, value);
            } else {
                return error(tokens[ip], options.input_name,
                             "Expected the next token after setreg "
                             "or getreg be a number (register)",
                             code);
            }
        }

        else {
            if (prv_instruct != instruct_NONE) {
                add_instruct(&code, &code_size, prv_instruct,
                             prv_instruct_count);

                prv_instruct = instruct_NONE;
                prv_instruct_count = 0;
            }

            add_instruct(&code, &code_size, current_instruct, 0);
        }
    }

    add_instruct(&code, &code_size, prv_instruct, prv_instruct_count);

    constexpr char exit_code[] = "\n"
                                 "    # destroying backend\n"
                                 "    call        bfpp_backend_destroy\n\n"
                                 "    # Exiting with code 0\n"
                                 "    xorq        %rax, %rax\n"
                                 "    popq        %rbp\n"
                                 "    ret\n";
    code = realloc(code, code_size + sizeof(exit_code));
    strcpy(code + code_size, exit_code);
    code_size += sizeof(exit_code) - 1;

    bool success = output(options.output_name, code, options.show_temp);

    free(code);
    return success;
}
