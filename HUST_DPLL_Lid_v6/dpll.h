#ifndef __DPLL_H
#define __DPLL_H
#include "def.h"
int Issingleclause(cnf problem);
int selectword(cnf problem, int *weights);
cnf &spread(cnf &problem, int l, int *book);
void copycnf(cnf src, cnf &des);
void freetmp(cnf &temp);
bool DPLL(cnf problem, int *book, int *weights);
#endif