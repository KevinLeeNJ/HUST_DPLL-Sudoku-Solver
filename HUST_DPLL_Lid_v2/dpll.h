#ifndef __DPLL_H
#define __DPLL_H
#include"def.h"
cnf_clause *Issingleclause(cnf problem);
cnf &spread(cnf &problem, int l, int *book);
int selectword(cnf problem);
void copycnf(cnf src, cnf &des);
void freetmp(cnf &temp);
bool DPLL(cnf problem, int *book);
#endif