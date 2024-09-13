#include "def.h"
#include "dpll.h"
#include "sudoku.h"
using namespace std;
int main()
{
    int mode = 0;
    int sudoku_problem[9][9] = {0};

    while (1)
    {
        system("clear");
        printf("HUST_DPLL&Sudoku Solver by Lid\n");
        printf("This is version 6.0.0\n");
        printf("Enter a number to select function\n");
        printf("1.SAT solver 2.Sudoku auto generator 3.Solve sudoku from cnf file 0.exit\n");
        scanf("%d", &mode);
        switch (mode)
        {
        case 0:
        {
            system("clear");
            printf("Bye!");
            return 0;
        }
        case 1:
        {
            system("clear");
            char filename[100];
            printf("Enter a filename\n");
            scanf("%s", filename);
            FILE *pf = fopen(filename, "r");
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
            problem.clauses = new clause[problem.clause_num];
            problem.ava_num = new int[problem.literal_num];
            problem.capacity = problem.clause_num;
            problem.empty_clause = false;
            for (int i = 0; i < problem.clause_num; i++) // 初始化每个语句
            {
                problem.clauses[i].issatisfied = 0;
            }
            for (int i = 0; i < problem.literal_num; i++)
            {
                problem.ava_num[i] = 0;
            }
            int literal_count, literal_tmp, clause_count;
            int tmp[1000];
            clause_count = 0;
            while (fscanf(pf, "%d", &literal_tmp) != EOF && clause_count <= problem.clause_num)
            {
                literal_count = 0;
                while (literal_tmp)
                {
                    tmp[literal_count] = literal_tmp;
                    literal_count++;
                    fscanf(pf, "%d", &literal_tmp);
                }
                problem.clauses[clause_count].literals = new int[literal_count];
                for (int i = 0; i < literal_count; i++)
                {
                    problem.clauses[clause_count].literals[i] = tmp[i];
                    problem.ava_num[abs(tmp[i]) - 1]++;
                }
                problem.clauses[clause_count].literal_num_in_clause = literal_count;
                clause_count++;
            }
            fclose(pf);
            int *book = new int[problem.literal_num];
            for (int i = 0; i < problem.literal_num; i++)
            {
                book[i] = 0;
            }
            clock_t start, end;
            int *weights;
            weights = new int[problem.literal_num];
            for (int i = 0; i < problem.literal_num; i++)
            {
                weights[i] = 0;
            }
            start = clock();
            bool result = DPLL(problem, book, weights);
            end = clock();
            pf = fopen("./result.res", "w+");
            fprintf(pf, "s %d\n", result);
            if (result)
            {
                printf("sat\n");
                fprintf(pf, "v ");
                for (int i = 1; i <= problem.literal_num; i++)
                {
                    fprintf(pf, "%d ", i * book[i - 1]);
                    printf("%d\n", i * book[i - 1]);
                }
                fprintf(pf, "0");
            }
            else
                printf("unsat\n");
            fprintf(pf, "\nt%lfms", (double(end - start) / CLOCKS_PER_SEC) * 1000);
            cout
                << "time = " << (double(end - start) / CLOCKS_PER_SEC) * 1000 << "ms" << endl;
            freetmp(problem);
            fclose(pf);
            getchar();
            getchar();
            break;
        }
        case 2:
        {
            system("clear");
            int hole_num;
            int sudoku[9][9] = {0};
            printf("How much holes?\n");
            scanf("%d", &hole_num);
            if (hole_num > 64)
            {
                printf("wrong number!");
                continue;
            }
            CreateSudoku(sudoku);
            CreateHole(sudoku, sudoku_problem, hole_num);
            PrintProblem(sudoku_problem);
            solve_sudoku(sudoku_problem);
            getchar();
            getchar();
            break;
        }
        case 3:
        {
            FILE *sudoku_result = fopen("./result.res", "r");
            int sudoku[9][9] = {0};
            int x, y, z, tmp, hole_num;
            char temp[1000];
            fgetc(sudoku_result);
            fgetc(sudoku_result);
            fgets(temp, sizeof(temp), sudoku_result);
            fscanf(sudoku_result, "%d", &tmp);
            while (tmp)
            {
                if (tmp > 0)
                {
                    x = tmp / 81;
                    tmp -= 81 * x;
                    y = tmp / 9;
                    tmp -= y * 9;
                    z = tmp + 1;
                    sudoku[x][y] = z;
                }
                fscanf(sudoku_result, "%d", &tmp);
            }
            fclose(sudoku_result);
            PrintProblem(sudoku);
            getchar();
            getchar();
        }
        default:
            break;
        }
    }
}