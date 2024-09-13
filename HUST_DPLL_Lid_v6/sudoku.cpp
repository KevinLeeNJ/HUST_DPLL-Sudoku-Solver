#include "def.h"
#include "dpll.h"
using namespace std;
void PrintProblem(int sudoku[][9])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (sudoku[i][j] != 0)
            {
                printf("%d ", sudoku[i][j]);
            }
            else
            {
                printf("_ ");
            }
        }
        printf("\n");
    }
}
void CreateFirstLine(int sudoku[][9])
{
    srand((unsigned)time(NULL)); // 随机数种子
    bool isused[10];             // 存储数字是否已经使用过
    int random;
    memset(isused, false, sizeof(bool));
    for (int i = 0; i < 9; i++)
    {
        random = rand() % 9 + 1;
        while (isused[random] == true)
            random = rand() % 9 + 1;
        sudoku[0][i] = random;
        isused[random] = true;
    }
}
int FillSudoku(int sudoku[][9], int x, int y)
{
    if (x < 9 && y < 9)
    {
        bool isused[10];
        memset(isused, 0, sizeof(isused));

        for (int i = 0; i < y; ++i)
        {
            isused[sudoku[x][i]] = 1; // 在第 x 行出现的数字 非法
        }
        for (int i = 0; i < x; ++i)
        {
            isused[sudoku[i][y]] = 1; // 在第 y 列出现的数字 非法
        }
        // 在 x,y处出现的值，不能在其3×3 的小方格内出现
        for (int i = x / 3 * 3; i < x / 3 * 3 + 3; ++i)
        {
            for (int j = y / 3 * 3; j < y / 3 * 3 + 3; ++j)
            {
                if (i == x or j == y)
                {
                    continue;
                }
                isused[sudoku[i][j]] = 1;
            }
        }
        if (x == y)
        {
            for (int i = 0; i < 9; i++)
            {
                if (sudoku[i][i])
                    isused[sudoku[i][i]] = 1;
            }
        }
        if (x + y == 8)
        {
            for (int i = 0; i < 9; i++)
            {
                if (sudoku[i][8 - i])
                    isused[sudoku[i][8 - i]] = 1;
            }
        }
        bool flag = false;
        for (int i = 1; i <= 9 and flag == 0; ++i)
        {
            if (isused[i] == 0)
            {
                flag = 1;
                sudoku[x][y] = i;
                if (y == 8 and x != 8)
                {
                    if (FillSudoku(sudoku, x + 1, 0) == 0) // 递归验证该取值能否生成完整数独，不能则回溯并取其他值
                    {
                        return 0;
                    }
                    else
                    {
                        flag = 0;
                    }
                }
                else if (y != 8)
                {
                    if (FillSudoku(sudoku, x, y + 1) == 0)
                    {
                        return 0;
                    }
                    else
                    {
                        flag = 0;
                    }
                }
            }
        }
        if (flag == 0)
        {
            sudoku[x][y] = 0;
            return 1;
        }
    }
    return 0;
}
void CreateSudoku(int sudoku[][9])
{
    CreateFirstLine(sudoku);
    FillSudoku(sudoku, 1, 0);
}
void CreateHole(int sudoku[][9], int sudoku_temp[][9], int empty_number) // 给数独挖洞
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            sudoku_temp[i][j] = sudoku[i][j];
        }
    }
    for (int i = 0; i < empty_number; ++i)
    {
        int x = rand() % 9;
        int y = rand() % 9;

        if (sudoku_temp[x][y] != 0)
        {
            sudoku_temp[x][y] = 0;
        }
        else
        {
            i--;
        }
    }
}
void Sudoku2CNF(int sudoku[][9], int empty_number)
{
    FILE *cnf_file = fopen("./sudoku.cnf", "w+");
    if (!cnf_file)
    {
        printf("can't open!\n");
    }
    fprintf(cnf_file, "p cnf 729 %d\n", 12717 - empty_number); // 11988
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (sudoku[i][j] != 0)
            {
                fprintf(cnf_file, "%d 0\n", 81 * i + 9 * j + sudoku[i][j]);
            }
        }
    }
    for (int i = 0; i < 9; i++) // 每格只能填入1-9的唯一一个数字
    {
        for (int j = 0; j < 9; j++)
        {
            for (int k = 0; k < 9; k++)
            {
                fprintf(cnf_file, "%d ", i * 81 + j * 9 + k + 1);
            }
            fprintf(cnf_file, "0\n");
            for (int k = 0; k < 8; k++)
            {
                for (int l = k + 1; l < 9; l++)
                    fprintf(cnf_file, "%d %d 0\n", -(i * 81 + j * 9 + k + 1), -(i * 81 + j * 9 + l + 1));
            }
        }
    }
    for (int i = 0; i < 9; i++) // 行约束-每一行都含有1-9
    {
        for (int k = 0; k < 9; k++)
        {
            for (int j = 0; j < 9; j++)
                fprintf(cnf_file, "%d ", i * 81 + j * 9 + k + 1);
            fprintf(cnf_file, "0\n");
        }
    }
    for (int i = 0; i < 9; i++) // 行约束-每行不相同
    {
        for (int k = 0; k < 9; k++)
        {
            for (int j = 0; j < 8; j++)
            {
                for (int l = j + 1; l < 9; l++)
                    fprintf(cnf_file, "%d %d 0\n", -(i * 81 + j * 9 + k + 1), -(i * 81 + l * 9 + k + 1));
            }
        }
    }
    for (int j = 0; j < 9; j++) // 列约束-每一列都含有1-9
    {
        for (int k = 0; k < 9; k++)
        {
            for (int i = 0; i < 9; i++)
                fprintf(cnf_file, "%d ", i * 81 + j * 9 + k + 1);
            fprintf(cnf_file, "0\n");
        }
    }
    for (int j = 0; j < 9; j++) // 列约束-每列不相同
    {
        for (int k = 0; k < 9; k++)
        {
            for (int i = 0; i < 8; i++)
            {
                for (int l = i + 1; l < 9; l++)
                    fprintf(cnf_file, "%d %d 0\n", -(i * 81 + j * 9 + k + 1), -(l * 81 + j * 9 + k + 1));
            }
        }
    }
    for (int i = 0; i < 3; i++) // 每个3x3格都有0-9
    {
        for (int j = 0; j < 3; j++)
        {
            for (int z = 0; z < 9; z++)
            {
                for (int x = 0; x < 3; x++)
                {
                    for (int y = 0; y < 3; y++)
                        fprintf(cnf_file, "%d ", 81 * (x + 3 * i) + 9 * (y + 3 * j) + z + 1);
                }
                fprintf(cnf_file, "0\n");
            }
        }
    }
    for (int i = 0; i < 3; i++) // 每个3x3小格不相同
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 9; k++)
            {
                for (int num = 0; num < 8; num++)
                {
                    for (int l = num + 1; l < 9; l++)
                        fprintf(cnf_file, "%d %d 0\n", -(81 * (i * 3 + num / 3) + 9 * (j * 3 + num % 3) + k + 1), -(81 * (i * 3 + l / 3) + 9 * (j * 3 + l % 3) + k + 1));
                }
            }
        }
    }
    for (int z = 0; z < 9; z++) // 对角线无重复
    {
        for (int xy = 0; xy < 8; xy++)
        {
            for (int l = xy + 1; l < 9; l++)
                fprintf(cnf_file, "%d %d 0\n", -(81 * xy + 9 * xy + z + 1), -(81 * l + 9 * l + z + 1));
        }
    }
    for (int z = 0; z < 9; z++) // 对角线无重复
    {
        for (int xy = 0; xy < 8; xy++)
        {
            for (int l = xy + 1; l < 9; l++)
                fprintf(cnf_file, "%d %d 0\n", -(81 * xy + 9 * (8 - xy) + z + 1), -(81 * l + 9 * (8 - l) + z + 1));
        }
    }
    fclose(cnf_file);
}
void sat_solver(void)
{
    char filename[100];
    FILE *pf = fopen("./sudoku.cnf", "r");
    if (pf == nullptr)
    {
        cout << "failed" << endl;
        return;
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
        // fprintf(pf, "v ");
        for (int i = 1; i <= problem.literal_num; i++)
            fprintf(pf, "%d ", i * book[i - 1]);
        fprintf(pf, "0");
    }
    else
        printf("unsat\n");
    fprintf(pf, "\nt%lfms", (double(end - start) / CLOCKS_PER_SEC) * 1000);
    cout
        << "time = " << (double(end - start) / CLOCKS_PER_SEC) * 1000 << "ms" << endl;
    freetmp(problem);
    fclose(pf);
}
void flash_sudoku(int sudoku[][9])
{
    FILE *sudoku_result = fopen("./result.res", "r");
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
            x = (tmp - 1) / 81;
            tmp -= 81 * x;
            y = (tmp - 1) / 9;
            tmp -= y * 9;
            z = tmp;
            sudoku[x][y] = z;
        }
        fscanf(sudoku_result, "%d", &tmp);
    }
    fclose(sudoku_result);
}
void solve_sudoku(int sudoku[][9])
{
    int sudoku_display[9][9] = {0};
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
            sudoku_display[i][j] = sudoku[i][j];
    }
    int hole_num = 0;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
            if (sudoku[i][j] == 0)
                hole_num++;
    }
    while (hole_num != 0)
    {
        Sudoku2CNF(sudoku, hole_num);
        sat_solver();
        flash_sudoku(sudoku);
        hole_num = 0;
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
                if (sudoku[i][j] == 0)
                    hole_num++;
        }
    }
    int mode;
    printf("1.show answer 2.play sudoku game 0.exit\n");
    scanf("%d", &mode);
    while (1)
    {
        system("clear");
        printf("1.show answer 2.play sudoku game 0.exit\n");
        switch (mode)
        {
        case 1:
            PrintProblem(sudoku);
            return;
        case 2:
            system("clear");
            printf("Please input data as x y z,x=row,y=column,z=elem number.\n");
            PrintProblem(sudoku_display);
            int x, y, z, m;
            while (1)
            {
                scanf("%d %d %d", &x, &y, &z);
                if (x == 0 || y == 0 || z == 0)
                    break;
                if (sudoku[x - 1][y - 1] == z)
                {
                    printf("right\n");
                    sudoku_display[x - 1][y - 1] = z;
                    system("clear");
                    printf("Please input data as x y z,x=row,y=column,z=elem number.\n");
                    PrintProblem(sudoku_display);
                }
                else
                {
                    printf("Wrong!\n");
                    printf("1.hints 2.continue\n");
                    scanf("%d", &m);
                    if (m == 1)
                    {
                        sudoku_display[x - 1][y - 1] = sudoku[x - 1][y - 1];
                        system("clear");
                        printf("Please input data as x y z,x=row,y=column,z=elem number.\n");
                        PrintProblem(sudoku_display);
                    }
                }
                hole_num = 0;
                for (int i = 0; i < 9; i++)
                {
                    for (int j = 0; j < 9; j++)
                        if (sudoku_display[i][j] == 0)
                            hole_num++;
                }
                if (hole_num == 0)
                {
                    printf("Congratulations!");
                    break;
                }
            }
        case 0:
            return;
        }
    }
}