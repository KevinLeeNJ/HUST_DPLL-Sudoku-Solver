#ifndef __DEF_H
#define __DEF_H
#include <ctime>
#include <stdlib.h>
#include <iostream>
#include <cstring>
typedef struct clause
{
    bool issatisfied;
    int *literals;
    int literal_num_in_clause;
} clause;
typedef struct cnf
{
    int literal_num;
    int clause_num;
    int *ava_num;
    int capacity;
    bool empty_clause;
    struct clause *clauses;
} cnf;
#endif