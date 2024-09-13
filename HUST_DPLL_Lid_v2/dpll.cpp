#include "def.h"
cnf_clause *Issingleclause(cnf problem)
{
    cnf_clause *tail = problem.clause->next;
    while (tail)
    {
        if (tail->first == nullptr)
            return nullptr;
        else if (tail->first->clause_next == nullptr)
            return tail;
        tail = tail->next;
    }
    return nullptr;
}
cnf &spread(cnf &problem, int l, int *book)
{
    book[abs(l)] = l / abs(l);
    lit_node *tail = problem.ava[abs(l)].first;
    while (tail)
    {
        if (tail->elem == l) // 删除整句
        {
            problem.clause_num--;
            cnf_clause *del_head = tail->head;
            lit_node *del_lit = del_head->first;
            lit_node *del_lit_next;
            tail = tail->ava_next;
            while (del_lit)
            {
                problem.ava[abs(del_lit->elem)].num--;
                if (del_lit->ava_pre && del_lit->ava_next)
                {
                    del_lit->ava_next->ava_pre = del_lit->ava_pre;
                    del_lit->ava_pre->ava_next = del_lit->ava_next;
                }
                else if (del_lit->ava_pre == nullptr)
                {
                    problem.ava[abs(del_lit->elem)].first = del_lit->ava_next;
                    if (del_lit->ava_next)
                        del_lit->ava_next->ava_pre = nullptr;
                }
                else
                    del_lit->ava_pre->ava_next = del_lit->ava_next;
                del_lit_next = del_lit->clause_next;
                if (del_lit == tail)
                    tail = tail->ava_next;
                delete del_lit;
                del_lit = del_lit_next;
            }
            if (del_head->next)
            {
                del_head->pre->next = del_head->next;
                del_head->next->pre = del_head->pre;
            }
            else
                del_head->pre->next = nullptr;
        }
        else // 删除单字
        {
            problem.ava[abs(l)].num--;
            lit_node *del_lit = tail;
            tail = tail->ava_next;
            if (del_lit->clause_pre && del_lit->clause_next)
            {
                del_lit->clause_pre->clause_next = del_lit->clause_next;
                del_lit->clause_next->clause_pre = del_lit->clause_pre;
            }
            else if (del_lit->clause_pre == nullptr)
            {
                del_lit->head->first = del_lit->clause_next;
                if (del_lit->clause_next)
                    del_lit->clause_next->clause_pre = nullptr;
            }
            else
                del_lit->clause_pre->clause_next = nullptr;
            delete (del_lit);
        }
    }
    problem.ava[abs(l)].first = nullptr;
    return problem;
}
int selectword(cnf problem)
{
    int count, max = 0, loc;
    lit_node *tail;
    for (int i = 1; i <= problem.literal_num; i++)
    {
        count = problem.ava[i].num;
        if (count > max)
        {
            max = count;
            loc = i;
        }
    }
    return loc;
}
void copycnf(cnf src, cnf &des)
{
    des.clause_num = src.clause_num;
    des.literal_num = src.literal_num;
    des.ava = new cnf_ava[des.literal_num + 1];
    for (int i = 1; i <= des.literal_num; i++)
    {
        des.ava[i].num = src.ava[i].num;
        des.ava[i].first = nullptr;
    }
    des.clause = new cnf_clause();
    cnf_clause *tail_src, *tail_clause = des.clause;
    for (int i = 0; i < src.clause_num; i++)
    {
        tail_clause->next = new cnf_clause();
        tail_clause->next->pre = tail_clause;
        tail_clause = tail_clause->next;
    }
    lit_node *lit_tail, *lit_src, *lit_pre;
    tail_clause = des.clause->next;
    tail_src = src.clause->next;
    while (tail_src)
    {
        if (tail_src != nullptr)
        {
            lit_tail = new lit_node();
            tail_clause->first = lit_tail;
            lit_src = tail_src->first;
            while (lit_src)
            {
                lit_tail->elem = lit_src->elem; // 节点赋值
                if (des.ava[abs(lit_tail->elem)].first == nullptr)
                    des.ava[abs(lit_tail->elem)].first = lit_tail;
                else
                {
                    lit_pre = des.ava[abs(lit_tail->elem)].first;
                    while (lit_pre->ava_next)
                        lit_pre = lit_pre->ava_next;
                    lit_pre->ava_next = lit_tail;
                    lit_tail->ava_pre = lit_pre;
                }
                lit_tail->head = tail_clause;
                if (lit_src->clause_next)
                {
                    lit_tail->clause_next = new lit_node();
                    lit_tail->clause_next->clause_pre = lit_tail;
                    lit_tail = lit_tail->clause_next;
                }
                lit_src = lit_src->clause_next;
            }
        }
        tail_clause = tail_clause->next;
        tail_src = tail_src->next;
    }
}
void freetmp(cnf &temp)
{
    temp.clause_num = 0;
    temp.literal_num = 0;
    cnf_clause *del_clause, *del_clause_next;
    lit_node *del_node, *del_node_next;
    del_clause_next = del_clause = temp.clause;
    while (del_clause)
    {
        del_clause_next = del_clause->next;
        del_node = del_clause->first;
        del_node_next = del_node;
        while (del_node)
        {
            del_node_next = del_node->clause_next;
            delete del_node;
            del_node = del_node_next;
        }
        delete del_clause;
        del_clause = del_clause_next;
    }
    delete temp.ava;
}
bool DPLL(cnf problem, int *book)
{
    if (problem.clause_num == 0)
        return true; // 没有子句则为真
    cnf_clause *tail = Issingleclause(problem);
    while (tail) // 单子句则传播
    {
        problem = spread(problem, tail->first->elem, book);
        tail = Issingleclause(problem);
    }
    if (problem.clause_num == 0)
        return true;
    tail = problem.clause->next;
    while (tail)
    {
        if (tail->first == nullptr)
            return false; // 空子句不可满足
        tail = tail->next;
    }
    int v = selectword(problem); // 基于某种规则选择变元
    cnf temp;                    // 用于回溯
    copycnf(problem, temp);
    if (DPLL(spread(problem, v, book), book))
    {
        freetmp(temp);
        return true;
    }
    bool result = DPLL(spread(temp, -v, book), book);
    freetmp(temp);
    return result;
}