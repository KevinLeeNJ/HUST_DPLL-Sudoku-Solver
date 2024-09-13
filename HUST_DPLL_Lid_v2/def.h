#ifndef _DEF_H
#define _DEF_H
#include <iostream>
#include <ctime>
#include <malloc.h>
typedef struct lit_node
{
    int elem;
    struct lit_node *clause_next;
    struct lit_node *clause_pre;
    struct lit_node *ava_next;
    struct lit_node *ava_pre;
    struct cnf_clause *head;
} lit_node;

typedef struct cnf_ava
{
    int num;
    lit_node *first;
} cnf_ava;
typedef struct cnf_clause
{
    struct cnf_clause *pre;
    struct cnf_clause *next;
    lit_node *first;
} cnf_clause;
typedef struct cnf
{
    int literal_num;
    int clause_num;
    cnf_clause *clause;
    cnf_ava *ava;

} cnf;
#endif