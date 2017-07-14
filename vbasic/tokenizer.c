//
//  tokenizer.c
//  vbasic
//
//  Created by arvin on 2017/7/14.
//  Copyright © 2017年 com.fuwo. All rights reserved.
//

#define DEBUG0 0

#if DEBUG0
#define DEBUG_PRINTF(...)  printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#include "tokenizer.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static char const *ptr, *nextptr;

#define MAX_NUMLEN 5

static int current_token = TOKENIZER_ERROR;

static const struct {
    char* kw;
    int token;
} keywords[] = {
    {"let", TOKENIZER_LET},
    {"print", TOKENIZER_PRINT},
    {"if", TOKENIZER_IF},
    {"then", TOKENIZER_THEN},
    {"else", TOKENIZER_ELSE},
    {"for", TOKENIZER_FOR},
    {"to", TOKENIZER_TO},
    {"next", TOKENIZER_NEXT},
    {"goto", TOKENIZER_GOTO},
    {"gosub", TOKENIZER_GOSUB},
    
    {"return", TOKENIZER_RETURN},
    {"call", TOKENIZER_CALL},
    {"end", TOKENIZER_END},
    {NULL, TOKENIZER_ERROR}
}, *kt;

static int single_char(void)
{
    if(*ptr == '\n') {
        return TOKENIZER_CR;
    } else if(*ptr == ',') {
        return TOKENIZER_COMMA;
    } else if(*ptr == '+') {
        return TOKENIZER_PLUS;
    } else if(*ptr == '-') {
        return TOKENIZER_MINUS;
    } else if(*ptr == '&') {
        return TOKENIZER_AND;
    } else if(*ptr == '|') {
        return TOKENIZER_OR;
    } else if(*ptr == '*') {
        return TOKENIZER_ASTR;
    } else if(*ptr == '/') {
        return TOKENIZER_SLASH;
    } else if(*ptr == '%') {
        return TOKENIZER_MOD;
        
    } else if(*ptr == '(') {
        return TOKENIZER_LEFTPAREN;
    } else if(*ptr == ')') {
        return TOKENIZER_RIGHTPAREN;
    } else if(*ptr == '<') {
        return TOKENIZER_LT;
    } else if(*ptr == '>') {
        return TOKENIZER_GT;
    } else if(*ptr == '=') {
        return TOKENIZER_EQ;
    }
    return 0;
}

static int get_next_token(void)
{
    int i;
    
    DEBUG_PRINTF("get_next_token(): '%s'\n", ptr);

    if (*ptr == 0) {
        return TOKENIZER_ENDOFINPUT;
    }
    
    if (isdigit(*ptr)) {
        for (i = 0; i < MAX_NUMLEN; i++) {
            if (!isdigit(ptr[i])) {
                if (i > 0) {
                    nextptr = ptr + i;
                    return TOKENIZER_NUMBER;
                }else{
                    DEBUG_PRINTF("get_next_token: error due to too short number\n");
                    return TOKENIZER_ERROR;
                }
            }
            if (!isdigit(ptr[i])) {
                DEBUG_PRINTF("get_next_token: error due to malformed number\n");
                return TOKENIZER_ERROR;
            }
        }
        DEBUG_PRINTF("get_next_token: error due to too long number\n");
        return TOKENIZER_ERROR;
    }else if (single_char()){
        nextptr = ptr + 1;
        return single_char();
    }else if (*ptr == '"'){
        nextptr = ptr;
        do {
            ++nextptr;
        } while (*nextptr != '"');
        ++nextptr;
        return TOKENIZER_STRING;
    }else{
        for(kt = keywords; kt->kw != NULL; ++kt) {
            if(strncmp(ptr, kt->kw, strlen(kt->kw)) == 0) {
                nextptr = ptr + strlen(kt->kw);
                return kt->token;
            }
        }
    }
    
    if(*ptr >= 'a' && *ptr <= 'z') {
        nextptr = ptr + 1;
        return TOKENIZER_VARIABLE;
    }
    
    return TOKENIZER_ERROR;
}

//对外接口函数
void tokenizer_init(const char *program)
{
    ptr = program;
    current_token = get_next_token();
}
void tokenizer_next(void)
{
    if (tokenizer_finished()) {
        return;
    }
    DEBUG_PRINTF("tokenizer_next: %p\n", nextptr);
    ptr = nextptr;
    while(*ptr == ' ') {
        ++ptr;
    }
    current_token = get_next_token();
    DEBUG_PRINTF("tokenizer_next: '%s' %d\n", ptr, current_token);
}
int  tokenizer_token(void)
{
    return current_token;
}
int  tokenizer_num(void)
{
    return atoi(ptr);
}
int  tokenizer_variable_num(void)
{
    return *ptr - 'a';
}
void tokenizer_string(char *dest, int len)
{
    char *string_end;
    int string_len;
    
    if(tokenizer_token() != TOKENIZER_STRING) {
        return;
    }
    string_end = strchr(ptr + 1, '"');
    if(string_end == NULL) {
        return;
    }
    string_len = (int)(string_end - ptr) - 1;
    if(len < string_len) {
        string_len = len;
    }
    memcpy(dest, ptr + 1, string_len);
    dest[string_len] = 0;
}

int  tokenizer_finished(void)
{
    return *ptr == 0 || current_token == TOKENIZER_ENDOFINPUT;
}
void tokenizer_error_print(void)
{
    DEBUG_PRINTF("tokenizer_error_print: '%s'\n", ptr);
}
