#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// parse.c
// value of token type
enum{
    TK_NUM = 256, // integer token
    TK_IDENT,     // identifier
    TK_RETURN,
    TK_EQ,        // EQual operator
    TK_NE,        // Not Equal operator
    TK_LE,        // Less than or Equal operator
    TK_GE,        // Grater than or Equal operator
    TK_IF,
    TK_ELS,
    TK_FOR,
    TK_WHILE,
    TK_EOF,       // EOF token
};

// Token type
typedef struct{
    int ty;         // Token type
    int val;        // value of TK_NUM token
    char *name;     // name of TK_IDENT token
    char *input;    // Token strings for error message
}Token;

// value of node type
enum{
    ND_NUM = 256,
    ND_LVAR,        // Node type of local variables
    ND_RETURN,
    ND_EQ,
    ND_NE,
    ND_LE,
    ND_IF,
    ND_FOR,
    ND_WHILE,
};

// Node type
typedef struct Node{
    int ty;         // Token type
    struct Node *lhs;      // left-hand side node
    struct Node *rhs;      // right-hand side node
    int val;        // in case ty is ND_NUM only
    int offset;     // in case ty is ND_LVAR only
    struct Node *cond;
    struct Node *then;
    struct Node *els;
    struct Node *init;
    struct Node *inc;
    struct Node *stmts;
}Node;

void tokenize();
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_if(Node *cond, Node *then, Node *els);
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
int consume(int ty);
void expect_token(int ty);

// codegen.c
void gen(Node *node);
void gen_lval(Node *node);
void gen_if(Node *node);
void gen_for(Node *node);
void gen_while(Node *node);

// container.c
typedef struct{
    void **data;
    int capacity;
    int len;
}Vector;

typedef struct{
    Vector *keys;
    Vector *vals;
} Map;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);
void expect(int line, int expected, int actual);
void runtest();
Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);

// main.c
// program inputted
extern char *user_input;

// Token sequence
extern Vector *tokens;
extern Vector *code;
extern Map *local_var;

// position of token read
extern int pos;
extern int count_local_var;
extern int count_begin;
extern int count_else;
extern int count_end;

// declaration
void error_at(char *loc, char *msg);
void error(char *fmt, ...);
