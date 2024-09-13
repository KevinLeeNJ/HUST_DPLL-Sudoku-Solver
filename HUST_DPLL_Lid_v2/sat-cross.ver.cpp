#include "def.h"
#include "dpll.h"
// awa subaru desu
using namespace std;

int main()
{
    FILE *pf = fopen("./problem.cnf", "r");
    if (pf == nullptr)
    {
        cout << "failed" << endl;
        return 1;
    }
    char temp[1000];
    cnf problem;
    while (fgets(temp, sizeof(temp), pf)) // 跳过注释
    {
        if (temp[0] == 'c')
            continue;
        else if (temp[0] == 'p')
        {
            sscanf(temp, "p cnf %d %d", &problem.literal_num, &problem.clause_num);
            break;
        }
    }
    problem.ava = new cnf_ava[problem.literal_num + 1];
    for (int i = 1; i < problem.literal_num + 1; i++) // 初始化文字节点
    {
        problem.ava[i].num = 0;
        problem.ava[i].first = nullptr;
    }
    problem.clause = new cnf_clause();
    cnf_clause *tail = problem.clause;
    for (int i = 0; i < problem.clause_num; i++) // 初始化语句节点
    {
        tail->next = new cnf_clause();
        tail->next->pre = tail;
        tail = tail->next;
    }
    int literal_count, literal_tmp, clause_count;
    int tmp[1000];
    lit_node *lit_tail, *lit_pre;
    cnf_clause *clause_tail = problem.clause->next;
    clause_count = 0;
    while (fscanf(pf, "%d", &literal_tmp) != EOF && clause_count < problem.clause_num)
    {
        literal_count = 0;
        while (literal_tmp)
        {
            tmp[literal_count] = literal_tmp;
            literal_count++;
            fscanf(pf, "%d", &literal_tmp);
        }
        lit_tail = new lit_node();
        clause_tail->first = lit_tail;
        for (int i = 0; i < literal_count; i++)
        {
            lit_tail->elem = tmp[i];
            lit_tail->head = clause_tail; // 节点赋值
            tmp[i] = 0;
            problem.ava[abs(lit_tail->elem)].num++;
            if (problem.ava[abs(lit_tail->elem)].first == nullptr)
                problem.ava[abs(lit_tail->elem)].first = lit_tail;
            else
            {
                lit_pre = problem.ava[abs(lit_tail->elem)].first;
                while (lit_pre->ava_next)
                    lit_pre = lit_pre->ava_next;
                lit_pre->ava_next = lit_tail;
                lit_tail->ava_pre = lit_pre;
            }
            if (i < literal_count - 1)
            {
                lit_tail->clause_next = new lit_node();
                lit_tail->clause_next->clause_pre = lit_tail;
                lit_tail = lit_tail->clause_next;
            }
        }
        clause_tail = clause_tail->next;
        clause_count++;
    }
    fclose(pf);
    int *book = new int[problem.literal_num + 1];
    for (int i = 0; i < problem.literal_num + 1; i++)
    {
        book[i] = 0;
    }
    clock_t start, end;
    start = clock();
    bool result = DPLL(problem, book);
    end = clock();
    pf = fopen("./result.res", "w+");
    fprintf(pf, "s %d\n", result);
    if (result)
    {
        printf("sat\n");
        fprintf(pf, "v ");
        for (int i = 1; i <= problem.literal_num; i++)
            fprintf(pf, "%d ", i * book[i]);
    }
    else
        printf("unsat\n");
    fprintf(pf, "\nt%lfms", (double(end - start) / CLOCKS_PER_SEC) * 1000);
    cout
        << "time = " << (double(end - start) / CLOCKS_PER_SEC) * 1000 << "ms" << endl;
    return 0;
}
