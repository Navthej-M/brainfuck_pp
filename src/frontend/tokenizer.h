#ifndef _TOKENIZER_H
#define _TOKENIZER_H

#include <stddef.h>

#define _token_value_size 1020
#define _tab_width 4
#define _num_max_digits 100

typedef enum {
    TokenType_ADD,
    TokenType_SUB,
    TokenType_Right,
    TokenType_Left,

    TokenType_Dot,
    TokenType_Comma,

    TokenType_SqOpen,
    TokenType_SqClose,

    TokenType_Equals,
    TokenType_Dollar,

    TokenType_Identifier,
    TokenType_Number,

    TokenType_Question,
    TokenType_Exclaim,

    TokenType_EOF,
} TokenType;

typedef struct {
    size_t line;
    size_t col;

    TokenType type;
    char linec[_token_value_size];

    union {
        long value_long;
        char value_char;
        char value_str[_token_value_size];
    } value;
} Token;

/// @return null terminated array of tokens (should free btw)
Token *tokenize(const char *code, size_t size);

#endif
