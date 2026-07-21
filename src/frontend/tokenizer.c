#include "tokenizer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *tokenize(const char *code, size_t size) {
    Token *tokens = malloc(sizeof(Token));
    size_t tokens_size = 0;
    char linec[_token_value_size] = {0};

    {
        size_t i = 0;
        while (i < size && code[i] != '\n') {
            linec[i] = code[i];
            ++i;
        }
    }

    for (size_t ip = 0, col = 0, line = 1; ip < size; ip++) {
        switch (code[ip]) {
        case 0x00:
            continue;

        case '\n': {
            ++line;
            col = 0;

            size_t i = ip + 1;
            while (i < size && code[i] != '\n') {
                linec[i - ip] = code[i];
                ++i;
            }
            linec[i - ip] = 0x00;

            break;
        }

        case ' ':
            ++col;
            break;

        case '\t':
            col += _tab_width;
            break;

        case '#':
            while (ip < size && code[ip] != '\n') {
                ++ip;
            }
            --ip;
            continue;

        case '+':
        case '-':
        case '>':
        case '<':
        case '.':
        case ',':
        case '[':
        case ']':
        case '=':
        case '$':
        case '?':
        case '!': {
            struct symbol_type_map {
                char symbol;
                TokenType type;
            };

            struct symbol_type_map map[] = {
                {'+', TokenType_ADD},      {'-', TokenType_SUB},
                {'>', TokenType_Right},    {'<', TokenType_Left},
                {'.', TokenType_Dot},      {',', TokenType_Comma},
                {'[', TokenType_SqOpen},   {']', TokenType_SqClose},
                {'=', TokenType_Equals},   {'$', TokenType_Dollar},
                {'?', TokenType_Question}, {'!', TokenType_Exclaim}};
            size_t items = sizeof(map);

            for (size_t i = 0; i < items; ++i) {
                if (map[i].symbol == code[ip]) {
                    tokens[tokens_size] = (Token){
                        .line = line,
                        .col = col,
                        .type = map[i].type,
                        .value.value_char = map[i].symbol,
                    };
                    strcpy(tokens[tokens_size].linec, linec);
                    ++tokens_size;

                    tokens = realloc(tokens, (tokens_size + 1) * sizeof(Token));
                    ++col;
                }
            }
            break;
        }

        default:
            if (isdigit(code[ip])) {
                char num_str[_num_max_digits] = {0};
                size_t i = 0;

                while (ip < size && isdigit(code[ip]) &&
                       i < _num_max_digits - 1) {
                    num_str[i++] = code[ip++];
                    ++col;
                }
                --ip;

                tokens[tokens_size] = (Token){
                    .line = line,
                    .col = col,
                    .type = TokenType_Number,
                    .value.value_long = atoi(num_str),
                };
                strcpy(tokens[tokens_size].linec, linec);
                ++tokens_size;

                tokens = realloc(tokens, (tokens_size + 1) * sizeof(Token));
            } else {
                free(tokens);
                fprintf(stderr, "error: invalid symbol '%c'\n", code[ip]);
                exit(1);
            }
            break;
        }
    }

    tokens[tokens_size] = (Token){
        .type = TokenType_EOF,
    };

    return tokens;
}
