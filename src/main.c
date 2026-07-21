#include "frontend/bfppc.h"
#include "frontend/tokenizer.h"
#include "frontend/version.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void throw_ifnot_number(const char *num_str) {
    for (size_t i = 0; i < strlen(num_str); ++i) {
        if (!isdigit(num_str[i])) {
            fprintf(stderr,
                    "error: invalid value %s; "
                    "expected an unsigned number.\n",
                    num_str);
            exit(1);
        }
    }
}

Options manage_args(int argc, char *argv[]) {
    Options options = {
        .input_name = "main.bfpp",
        .output_name = "bfppc_output",

        .show_temp = false,
        .registers_size = 30,
        .tape_size = 4096,
    };

    for (size_t ip = 1; ip < argc; ++ip) {
        if (argv[ip][0] == '-') {
            // -h, --help
            if (strcmp(argv[ip], "--help") == 0 ||
                strcmp(argv[ip], "-h") == 0) {
                puts("bfppc version " bfppc_version_string "\n"
                     "usage: bfpcc input_file [OPTIONS]...\n\n"

                     "-h, --help                 Show this text\n\n"

                     "-v, --version              Show the bfppc installed\n"
                     "                           version\n\n"

                     "-o                         The output file name should\n"
                     "                           be after this arg (-o\n"
                     "                           output)\n\n"

                     "-show-tmp                  Show the temporary\n"
                     "                           files made during\n"
                     "                           compilation\n\n"

                     "--max-reg                  The next arg should be the\n"
                     "                           maximum number of registers.\n"
                     "                           If this flag is not used,\n"
                     "                           the compiler will initilize\n"
                     "                           this to 30\n\n"

                     "--tape-size                The next arg should be the\n"
                     "                           tape size. If this flag is\n"
                     "                           not used, the compiler will\n"
                     "                           initilize this to 4096\n\n"

                );

                if (argc == 2) {
                    exit(0);
                }
            }

            // -v, --version
            else if (strcmp(argv[ip], "-v") == 0 ||
                     strcmp(argv[ip], "--version") == 0) {
                printf(bfppc_version_string "\n");

                if (argc == 2) {
                    exit(0);
                }
            }

            // -o
            else if (strcmp(argv[ip], "-o") == 0) {
                if (ip + 1 > argc) {
                    fprintf(stderr, "error: no output name specified.\n");
                    exit(1);
                }

                strcpy((char *)options.output_name, argv[++ip]);
            }

            // -show-tmp
            else if (strcmp(argv[ip], "-show-tmp") == 0) {
                options.show_temp = true;
            }

            // --max-reg
            else if (strcmp(argv[ip], "--max-reg") == 0) {
                if (ip + 1 > argc) {
                    fprintf(stderr,
                            "error: no value for `--max-reg` specified.\n");
                    exit(1);
                }

                const char *num_str = argv[++ip];
                throw_ifnot_number(num_str);

                options.registers_size = atoi(num_str);
            }

            // --tape-size
            else if (strcmp(argv[ip], "--tape-size") == 0) {
                if (ip + 1 > argc) {
                    fprintf(stderr,
                            "error: no value for `--tape-size` specified.\n");
                    exit(1);
                }

                const char *num_str = argv[++ip];
                throw_ifnot_number(num_str);

                options.tape_size = atoi(num_str);
            }

            // invalid flag
            else {
                fprintf(stderr, "error: invalid flag \"%s\"\n", argv[ip]);
                exit(1);
            }
        }

        // if not a flag, seeing as an input file
        else {
            strcpy((char *)options.input_name, argv[ip]);
        }
    }

    return options;
}

int main(int argc, char *argv[]) {
    Options options = manage_args(argc, argv);

    FILE *input_file = fopen(options.input_name, "r");
    if (!input_file) {
        fprintf(stderr, "error: unable to open input file.\n");
        return 1;
    }

    fseek(input_file, 0, SEEK_END);
    long input_file_size = ftell(input_file);
    rewind(input_file);

    if (input_file_size < 0) {
        fclose(input_file);
        fprintf(stderr, "error: the input file size somewhat make no sense.\n");
        return 1;
    }

    char *code = (char *)malloc(input_file_size * sizeof(char) + 1);
    if (!code) {
        fclose(input_file);
        fprintf(stderr,
                "error: unable to allocate buffer size for input code.\n");
        return 1;
    }

    size_t bytes_read = fread(code, sizeof(char), input_file_size, input_file);
    code[bytes_read] = 0x00;

    fclose(input_file);

    Token *tokens = tokenize(code, input_file_size);

    bool success = bfppc_compile(tokens, options);

    free(code);
    free(tokens);

    return !success;
}
