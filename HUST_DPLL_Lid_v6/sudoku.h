#ifndef __SUDOKU_H
#define __SUDOKU_H
#include "def.h"
void PrintProblem(int sudoku[][9]);
void CreateFirstLine(int sudoku[][9]);
int FillSudoku(int sudoku[][9], int x, int y);
void CreateSudoku(int sudoku[][9]);
void CreateHole(int sudoku[][9], int sudoku_temp[][9], int empty_number);
void Sudoku2CNF(int sudoku[][9], int empty_number);
void solve_sudoku(int sudoku[][9]);
#endif