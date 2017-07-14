//
//  tokenizer.h
//  vbasic
//
//  Created by arvin on 2017/7/14.
//  Copyright © 2017年 com.fuwo. All rights reserved.
//

#ifndef tokenizer_h
#define tokenizer_h

enum {
    TOKENIZER_ERROR,
    TOKENIZER_ENDOFINPUT,
    TOKENIZER_NUMBER,
    
    TOKENIZER_STRING,
    TOKENIZER_VARIABLE,
    TOKENIZER_LET,
    TOKENIZER_PRINT,
    TOKENIZER_IF,
    TOKENIZER_THEN,
    TOKENIZER_ELSE,
    TOKENIZER_FOR,
    TOKENIZER_TO,
    TOKENIZER_NEXT,
    
    TOKENIZER_GOTO,
    TOKENIZER_GOSUB,
    TOKENIZER_RETURN,
    TOKENIZER_CALL,
    TOKENIZER_END,
    TOKENIZER_COMMA,
    TOKENIZER_PLUS,
    TOKENIZER_MINUS,
    TOKENIZER_AND,
    
    TOKENIZER_OR,
    TOKENIZER_ASTR,
    TOKENIZER_SLASH,
    TOKENIZER_MOD,
    TOKENIZER_LEFTPAREN,
    TOKENIZER_RIGHTPAREN,
    TOKENIZER_LT,
    TOKENIZER_GT,
    TOKENIZER_EQ,
    TOKENIZER_CR,
};

void tokenizer_init(const char *program);
void tokenizer_next(void);
int  tokenizer_token(void);
int  tokenizer_num(void);
int  tokenizer_variable_num(void);
void tokenizer_string(char *dest, int len);

int  tokenizer_finished(void);
void tokenizer_error_print(void);

#endif /* tokenizer_h */
