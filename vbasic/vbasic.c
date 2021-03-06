//
//  vbasic.c
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


#include "vbasic.h"
#include "tokenizer.h"

#include <stdio.h> /* printf() */
#include <stdlib.h> /* exit() */

static char const *program_ptr;
#define MAX_STRINGLEN 40
static char string[MAX_STRINGLEN];

#define MAX_GOSUB_STACK_DEPTH 10
static int gosub_stack[MAX_GOSUB_STACK_DEPTH];
static int gosub_stack_ptr;

typedef struct {
    int line_after_for;
    int for_variable;
    int to;
} for_state;
#define MAX_FOR_STACK_DEPTH 4
static for_state for_stack[MAX_FOR_STACK_DEPTH];
static int for_stack_ptr;

#define MAX_VARNUM 26
static char variables[MAX_VARNUM];

static int ended;

static int expr(void);
static void line_statement(void);
static void statement(void);

static void accept(int token)
{
    if (token != tokenizer_token()) {
        DEBUG_PRINTF("Token not what was expected (expected %d, got %d)\n",
                     token, tokenizer_token());
        tokenizer_error_print();
        exit(1);
    }
    DEBUG_PRINTF("Expected %d, got it\n", token);
    tokenizer_next();
}

//下面是表达式解析和运算
static int factor(void)
{
    int r;
    
    DEBUG_PRINTF("factor: token %d\n", tokenizer_token());
    switch(tokenizer_token()) {
        case TOKENIZER_NUMBER:
            r = tokenizer_num();
            DEBUG_PRINTF("factor: number %d\n", r);
            accept(TOKENIZER_NUMBER);
            break;
        case TOKENIZER_LEFTPAREN:
            accept(TOKENIZER_LEFTPAREN);
            r = expr();
            accept(TOKENIZER_RIGHTPAREN);
            break;
        default:
            DEBUG_PRINTF("varfactor: get %d from variable %d\n", variables[tokenizer_variable_num()], tokenizer_variable_num());
            r = vbasic_get_variable(tokenizer_variable_num());
            accept(TOKENIZER_VARIABLE);
            break;
    }
    return r;
}

static int term(void)
{
    int f1, f2;
    int op;
    
    f1 = factor();
    op = tokenizer_token();
    DEBUG_PRINTF("term: token %d\n", op);
    while(op == TOKENIZER_ASTR ||
          op == TOKENIZER_SLASH ||
          op == TOKENIZER_MOD) {
        tokenizer_next();
        f2 = factor();
        DEBUG_PRINTF("term: %d %d %d\n", f1, op, f2);
        switch(op) {
            case TOKENIZER_ASTR:
                f1 = f1 * f2;
                break;
            case TOKENIZER_SLASH:
                f1 = f1 / f2;
                break;
            case TOKENIZER_MOD:
                f1 = f1 % f2;
                break;
        }
        op = tokenizer_token();
    }
    DEBUG_PRINTF("term: %d\n", f1);
    return f1;
}

static int expr(void)
{
    int t1, t2;
    int op;
    
    t1 = term();
    op = tokenizer_token();
    DEBUG_PRINTF("expr: token %d\n", op);
    while(op == TOKENIZER_PLUS ||
          op == TOKENIZER_MINUS ||
          op == TOKENIZER_AND ||
          op == TOKENIZER_OR) {
        tokenizer_next();
        t2 = term();
        DEBUG_PRINTF("expr: %d %d %d\n", t1, op, t2);
        switch(op) {
            case TOKENIZER_PLUS:
                t1 = t1 + t2;
                break;
            case TOKENIZER_MINUS:
                t1 = t1 - t2;
                break;
            case TOKENIZER_AND:
                t1 = t1 & t2;
                break;
            case TOKENIZER_OR:
                t1 = t1 | t2;
                break;
        }
        op = tokenizer_token();
    }
    DEBUG_PRINTF("expr: %d\n", t1);
    return t1;
}

static int relation(void)
{
    int r1, r2;
    int op;
    
    r1 = expr();
    op = tokenizer_token();
    DEBUG_PRINTF("relation: token %d\n", op);
    while(op == TOKENIZER_LT ||
          op == TOKENIZER_GT ||
          op == TOKENIZER_EQ) {
        tokenizer_next();
        r2 = expr();
        DEBUG_PRINTF("relation: %d %d %d\n", r1, op, r2);
        switch(op) {
            case TOKENIZER_LT:
                r1 = r1 < r2;
                break;
            case TOKENIZER_GT:
                r1 = r1 > r2;
                break;
            case TOKENIZER_EQ:
                r1 = r1 == r2;
                break;
        }
        op = tokenizer_token();
    }
    return r1;
}

//下面是语句的解析和运算
static void jump_linenum(int linenum)
{
    tokenizer_init(program_ptr);//从头扫描定位
    while(tokenizer_num() != linenum) {
        do {
            do {
                tokenizer_next();
            } while(tokenizer_token() != TOKENIZER_CR &&
                    tokenizer_token() != TOKENIZER_ENDOFINPUT);
            if(tokenizer_token() == TOKENIZER_CR) {
                tokenizer_next();
            }
        } while(tokenizer_token() != TOKENIZER_NUMBER);
        DEBUG_PRINTF("jump_linenum: Found line %d\n", tokenizer_num());
    }
}

static void goto_statement(void)
{
    accept(TOKENIZER_GOTO);
    jump_linenum(tokenizer_num());
}

static void print_statement(void)
{
    accept(TOKENIZER_PRINT);
    do {
        DEBUG_PRINTF("Print loop\n");
        if(tokenizer_token() == TOKENIZER_STRING) {
            tokenizer_string(string, sizeof(string));
            printf("%s", string);
            tokenizer_next();
        } else if(tokenizer_token() == TOKENIZER_COMMA) {
            printf(" ");
            tokenizer_next();
        } else if(tokenizer_token() == TOKENIZER_VARIABLE ||
                  tokenizer_token() == TOKENIZER_NUMBER) {
            printf("%d", expr());
        } else {
            break;
        }
    } while(tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ENDOFINPUT);
    printf("\n");
    DEBUG_PRINTF("End of print\n");
    tokenizer_next();
}

static void if_statement(void)
{
    int r;
    
    accept(TOKENIZER_IF);
    
    r = relation();
    DEBUG_PRINTF("if_statement: relation %d\n", r);
    accept(TOKENIZER_THEN);
    if(r) {
        statement();
    } else {
        do {
            tokenizer_next();
        } while(tokenizer_token() != TOKENIZER_ELSE &&
                tokenizer_token() != TOKENIZER_CR &&
                tokenizer_token() != TOKENIZER_ENDOFINPUT);
        if(tokenizer_token() == TOKENIZER_ELSE) {
            tokenizer_next();
            statement();
        } else if(tokenizer_token() == TOKENIZER_CR) {
            tokenizer_next();
        }
    }
}

static void let_statement(void)
{
    int var;
    
    var = tokenizer_variable_num();
    
    accept(TOKENIZER_VARIABLE);
    accept(TOKENIZER_EQ);
    vbasic_set_variable(var, expr());
    DEBUG_PRINTF("let_statement: assign %d to %d\n", variables[var], var);
    accept(TOKENIZER_CR);
}

static void gosub_statement(void)
{
    int linenum;
    accept(TOKENIZER_GOSUB);
    linenum = tokenizer_num();
    accept(TOKENIZER_NUMBER);
    accept(TOKENIZER_CR);
    if(gosub_stack_ptr < MAX_GOSUB_STACK_DEPTH) {
        gosub_stack[gosub_stack_ptr] = tokenizer_num();
        gosub_stack_ptr++;
        jump_linenum(linenum);
    } else {
        DEBUG_PRINTF("gosub_statement: gosub stack exhausted\n");
    }
}

static void return_statement(void)
{
    accept(TOKENIZER_RETURN);
    if(gosub_stack_ptr > 0) {
        gosub_stack_ptr--;
        jump_linenum(gosub_stack[gosub_stack_ptr]);
    } else {
        DEBUG_PRINTF("return_statement: non-matching return\n");
    }
}

static void next_statement(void)
{
    int var;
    
    accept(TOKENIZER_NEXT);
    var = tokenizer_variable_num();
    accept(TOKENIZER_VARIABLE);
    if(for_stack_ptr > 0 && var == for_stack[for_stack_ptr - 1].for_variable) {
        vbasic_set_variable(var, vbasic_get_variable(var) + 1);
        if(vbasic_get_variable(var) <= for_stack[for_stack_ptr - 1].to) {
            jump_linenum(for_stack[for_stack_ptr - 1].line_after_for);
        } else {
            for_stack_ptr--;
            accept(TOKENIZER_CR);
        }
    } else {
        DEBUG_PRINTF("next_statement: non-matching next (expected %d, found %d)\n", for_stack[for_stack_ptr - 1].for_variable, var);
        accept(TOKENIZER_CR);
    }
}

static void for_statement(void)
{
    int for_variable, to;
    
    accept(TOKENIZER_FOR);
    for_variable = tokenizer_variable_num();
    accept(TOKENIZER_VARIABLE);
    accept(TOKENIZER_EQ);
    vbasic_set_variable(for_variable, expr());
    accept(TOKENIZER_TO);
    to = expr();
    accept(TOKENIZER_CR);
    
    if(for_stack_ptr < MAX_FOR_STACK_DEPTH) {
        for_stack[for_stack_ptr].line_after_for = tokenizer_num();
        for_stack[for_stack_ptr].for_variable = for_variable;
        for_stack[for_stack_ptr].to = to;
        DEBUG_PRINTF("for_statement: new for, var %d to %d\n",
                     for_stack[for_stack_ptr].for_variable,
                     for_stack[for_stack_ptr].to);
        
        for_stack_ptr++;
    } else {
        DEBUG_PRINTF("for_statement: for stack depth exceeded\n");
    }
}

static void end_statement(void)
{
    accept(TOKENIZER_END);
    ended = 1;
}

static void statement(void)
{
    int token;
    
    token = tokenizer_token();
    
    switch(token) {
        case TOKENIZER_PRINT:
            print_statement();
            break;
        case TOKENIZER_IF:
            if_statement();
            break;
        case TOKENIZER_GOTO:
            goto_statement();
            break;
        case TOKENIZER_GOSUB:
            gosub_statement();
            break;
        case TOKENIZER_RETURN:
            return_statement();
            break;
        case TOKENIZER_FOR:
            for_statement();
            break;
        case TOKENIZER_NEXT:
            next_statement();
            break;
        case TOKENIZER_END:
            end_statement();
            break;
        case TOKENIZER_LET:
            accept(TOKENIZER_LET);
            /* Fall through. */
        case TOKENIZER_VARIABLE:
            let_statement();
            break;
        default:
            DEBUG_PRINTF("vbasic.c: statement(): not implemented %d\n", token);
            exit(1);
    }
}

static void line_statement(void)
{
    DEBUG_PRINTF("----------- Line number %d ---------\n", tokenizer_num());
    /*    current_linenum = tokenizer_num();*/
    accept(TOKENIZER_NUMBER);
    statement();
    return;
}


//对外接口函数
void vbasic_init(const char* prgm)
{
    program_ptr = prgm;
    for_stack_ptr = gosub_stack_ptr = 0;
    tokenizer_init(prgm);
    ended = 0;
}
void vbasic_run()
{
    if(tokenizer_finished()) {
        DEBUG_PRINTF("vBASIC program finished\n");
        return;
    }
    line_statement();
}
int  vbasic_finished()
{
    return ended || tokenizer_finished();
}

//
int vbasic_get_variable(int varnum)
{
    if(varnum > 0 && varnum <= MAX_VARNUM) {
        return variables[varnum];
    }
    return 0;
}
void vbasic_set_variable(int varnum, int value)
{
    if(varnum > 0 && varnum <= MAX_VARNUM) {
        variables[varnum] = value;
    }
}
