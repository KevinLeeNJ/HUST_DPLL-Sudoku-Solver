#include "def.h"
int Issingleclause(cnf problem)
{
    int count = 0;
    int loc;
    while (++count <= problem.clause_num)
    {
        if (problem.clauses[count - 1].literal_num_in_clause == 1 && problem.clauses[count - 1].issatisfied == 0)
        {
            return problem.clauses[count - 1].literals[0];
        }
    }
    return 0;
}
void flashweights(cnf problem, int *weights, int *x, int *y)
{
    int count, min = 99999;
    for (int i = 0; i < problem.capacity; i++)
    {
        if (problem.clauses[i].issatisfied == false)
        {
            count = problem.clauses[i].literal_num_in_clause;
            if (count < min)
            {
                min = count;
            }
        }
    }
    for (int i = 0; i < problem.capacity; i++)
    {
        if (problem.clauses[i].issatisfied == false)
        {
            if (problem.clauses[i].literal_num_in_clause == min)
            {
                for (int j = 0; j < min; j++)
                {
                    if (problem.clauses[i].literals[j] > 0)
                        x[problem.clauses[i].literals[j] - 1]++;
                    else
                        y[-problem.clauses[i].literals[j] - 1]++;
                }
            }
        }
    }
    for (int i = 0; i < problem.literal_num; i++)
    {
        weights[i] = (x[i] + 1) * (y[i] + 1);
    }
}
int selectword(cnf problem, int *weights)
{

    int *x, *y;
    x = new int[problem.literal_num];
    y = new int[problem.literal_num];
    for (int i = 0; i < problem.literal_num; i++)
    {
        x[i] = y[i] = 0;
    }
    flashweights(problem, weights, x, y);
    int max = 0, loc = 0;
    for (int i = 0; i < problem.literal_num; i++)
    {
        if (weights[i] > max)
        {
            loc = i;
            max = weights[i];
        }
    }
    if (x[loc] > y[loc])
    {
        delete x;
        delete y;
        return loc + 1;
    }
    delete x;
    delete y;
    return -(loc + 1);
}
cnf &spread(cnf &problem, int l, int *book)
{
    book[abs(l) - 1] = l / abs(l);
    int count;
    for (int i = 0; i < problem.capacity; i++)
    {
        if (problem.clauses[i].issatisfied == 0) // 忽略已满足的子句
        {
            for (int k = 0; k < problem.clauses[i].literal_num_in_clause; k++)
            {
                if (problem.clauses[i].literals[k] == l) // 有相同文字，则该子句已经满足
                {
                    problem.clauses[i].issatisfied = true;
                    problem.clause_num--;
                    for (int j = 0; j < problem.clauses[i].literal_num_in_clause; j++)
                        problem.ava_num[abs(problem.clauses[i].literals[j]) - 1]--;
                }
                else if (problem.clauses[i].literals[k] == -l) // 有相反文字，则删除该文字
                {
                    count = 0;
                    problem.ava_num[abs(l) - 1]--;
                    for (int j = 0; j < problem.clauses[i].literal_num_in_clause; j++)
                    {
                        if (j != k)
                        {
                            problem.clauses[i].literals[count++] = problem.clauses[i].literals[j];
                        }
                    }
                    problem.clauses[i].literal_num_in_clause--;
                    if (problem.clauses[i].literal_num_in_clause == 0)
                    {
                        problem.empty_clause = true;
                        break;
                    }
                }
            }
        }
    }
    return problem;
}
void copycnf(cnf src, cnf &des)
{
    des.clause_num = des.capacity = src.clause_num;
    des.literal_num = src.literal_num;
    des.empty_clause = src.empty_clause;
    des.ava_num = new int[des.literal_num];
    for (int i = 0; i < des.literal_num; i++)
        des.ava_num[i] = src.ava_num[i];
    des.clauses = new clause[des.capacity];
    int count = 0;
    for (int i = 0; i < src.capacity; i++)
    {
        if (src.clauses[i].issatisfied == false) // 不满足则复制到des
        {
            des.clauses[count].literals = new int[src.clauses[i].literal_num_in_clause];
            des.clauses[count].issatisfied = false;
            des.clauses[count].literal_num_in_clause = src.clauses[i].literal_num_in_clause;
#pragma openmp parallel for
            for (int j = 0; j < src.clauses[i].literal_num_in_clause; j++)
                des.clauses[count].literals[j] = src.clauses[i].literals[j];
            count++;
        }
    }
}
void freetmp(cnf &temp)
{
    temp.clause_num = 0;
    temp.literal_num = 0;
    temp.empty_clause = true;
    delete temp.ava_num;
    for (int i = 0; i < temp.capacity; i++)
    {
        delete temp.clauses[i].literals;
    }
    temp.capacity = 0;
    delete temp.clauses;
}
bool DPLL(cnf problem, int *book, int *weights)
{
    if (problem.clause_num == 0)
        return true; // 没有子句则为真
    int spread_num = Issingleclause(problem);
    while (spread_num) // 单子句则传播
    {
        problem = spread(problem, spread_num, book);
        spread_num = Issingleclause(problem);
    }
    if (problem.clause_num == 0)
        return true;
    if (problem.empty_clause == true)
        return false; // 空子句不可满足
    spread_num = selectword(problem, weights);
    cnf temp; // 用于回溯
    copycnf(problem, temp);
    if (DPLL(spread(problem, spread_num, book), book, weights))
    {
        freetmp(temp);
        return true;
    }
    bool result = DPLL(spread(temp, -spread_num, book), book, weights);
    freetmp(temp);
    return result;
}