// parse.c
#include "hcc.h"

// break string up into tokens
void tokenize(){
    char *p = user_input;

    int i=0;
    while(*p){
        // skip space
        if(isspace(*p)){
            p++;
            continue;
        }

        if(!strncmp(p, "==", 2)){
            Token *t = (Token *)malloc(sizeof(Token));
            t->ty = TK_EQ;
            t->input = p;
            vec_push(tokens, t);
            p+=2;
            continue;
        }

        if(!strncmp(p, "!=", 2)){
            Token *t = (Token *)malloc(sizeof(Token));
            t->ty = TK_NE;
            t->input = p;
            vec_push(tokens, t);
            p+=2;
            continue;
        }

        if(!strncmp(p, "<=", 2)){
            Token *t = (Token *)malloc(sizeof(Token));
            t->ty = TK_LE;
            t->input = p;
            vec_push(tokens, t);
            p+=2;
            continue;
        }
        
        if(!strncmp(p, ">=", 2)){
            Token *t = (Token *)malloc(sizeof(Token));
            t->ty = TK_GE;
            t->input = p;
            vec_push(tokens, t);
            p+=2;
            continue;
        }

        if('a' <= *p && *p <= 'z'){
            Token *t = (Token *)malloc(sizeof(Token));
            t->ty = TK_IDENT;
            t->input = p;
            vec_push(tokens, t);
            p++;
            continue;
        }
        
        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == ')' || *p == '(' || *p == '<' || *p == '>' || *p == '=' ||*p == ';'){
            Token *t = (Token *)malloc(sizeof(Token));
            t->ty = *p;
            t->input = p;
            vec_push(tokens, t);
            p++;
            continue;
        }

        if(isdigit(*p)){
            Token *t = (Token *)malloc(sizeof(Token));
            t->ty = TK_NUM;
            t->input = p;
            t->val = strtol(p, &p, 10);
            vec_push(tokens, t);
            i++;
            continue;
        }

        error_at(p, "failed to tokenize");
    }

    Token *t = (Token *)malloc(sizeof(Token));
    t->ty = TK_EOF;
    t->input = p;
    vec_push(tokens, t);
}

// tree
Node *new_node(int ty, Node *lhs, Node *rhs)
{
    Node *node = (Node *)(malloc(sizeof(Node)));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val)
{
    Node *node = (Node *)(malloc(sizeof(Node)));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

int consume(int ty)
{
    if(((Token *)(tokens->data[pos]))->ty != ty){
        return 0;
    }
    pos++;
    return 1;
}

void program()
{
    while(((Token *)(tokens->data[pos]))->ty!=TK_EOF){
        vec_push(code, stmt());
    }
    vec_push(code, NULL);
}

Node *stmt()
{
    Node *node = expr();
    if(!consume(';')){
        error_at(((Token *)(tokens->data[pos]))->input, "expected ';' token");
    }
    return node;
}

Node *expr()
{
    return assign();
}

Node *assign()
{
    Node *node = equality();
    if(consume('=')){
        node = new_node('=', node, assign());
    }
    return node;
}

Node *equality()
{
    Node *node = relational();

    while(1){
        if(consume(TK_EQ)){
            node = new_node(TK_EQ, node, relational());
        }else if(consume(TK_NE)){
            node = new_node(TK_NE, node, relational());
        }else{
            return node;
        }
    }
}

Node *relational()
{
    Node *node = add();

    while(1){
        if(consume('<')){
            node = new_node('<', node, add());
        }else if(consume('>')){
            node = new_node('<', add(), node);
        }else if(consume(TK_LE)){
            node = new_node(TK_LE, node, add());
        }else if(consume(TK_GE)){
            node = new_node(TK_LE, add(), node);
        }else{
            return node;
        }
    }
}

Node *add()
{
    Node *node = mul();

    for(;;){
        if(consume('+')){
            node = new_node('+', node, mul());
        }else if(consume('-')){
            node = new_node('-', node, mul());
        }else{
            return node;
        }
    }
}

Node *mul()
{
    Node *node = unary();

    for(;;){
        if(consume('*')){
            node = new_node('*', node, unary());
        }else if(consume('/')){
            node = new_node('/', node, unary());
        }else{
            return node;
        }
    }
}

Node *unary()
{
    if(consume('+')){
        return term();
    }else if(consume('-')){
        return new_node('-', new_node_num(0), term());
    }
    return term();
}

Node *term()
{
    Node *node;

    if(consume('(')){
        node = expr();
        if(!consume(')')){
            error_at(((Token *)(tokens->data[pos]))->input, 
                    ") expected");
        }
        return node;
    }

    if(((Token *)(tokens->data[pos]))->ty == TK_NUM){
        node = new_node_num(((Token *)(tokens->data[pos++]))->val);
    }else if(((Token *)(tokens->data[pos]))->ty == TK_IDENT){
        char varname = ((Token *)(tokens->data[pos++]))->input[0];
        node = malloc(sizeof(Node));
        node->ty = ND_LVAR;
        node->offset = (varname - 'a' + 1) * 8;
    }else{
        error_at(((Token *)(tokens->data[pos]))->input, 
                "unexpected token: expected a number");
    }
    return node;
}

