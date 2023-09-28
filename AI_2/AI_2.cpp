/*
Разработать программу решения транспортной задачи.

Составить оптимальный план перевозок между N складами и
K магазинами, при котором стоимость перевозок будет
минимальна. Известна потребность в товаре каждым
магазином, наличие товара на складах и стоимость
перевозки единицы продукции с каждого склада до каждого
магазина.

Использовать алгоритм отжига.
*/

#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>

using namespace std;

const int N = 10;             // число складов (y)
const int K = 10;             // число магазинов (x)
vector<vector<int>> cost;     // матрица стоимости перевозок
const int cost_min = 1;       // минимальная цена перевозки
const int cost_max = 10;      // максимальная цена перевозки
const int plan_min = 2;       // минимальное значение плана перевозки из какого-то склада в какой-то магазин
const int plan_max = 10;      // максимальное значение плана перевозки из какого-то склада в какой-то магазин
const int transfer_size = 1;  // размер изменения величины перевоза товара в функции Swap()

const double Tn = 1000.0;      // начальная температура
const double Tk = 0.01;       // конечная температура
const double Alfa = 0.98;     // скорость охлаждения
const int ST = 1000;          // число итераций при смене T

struct TSolution {            // решение
    vector<vector<int>> plan; // матрица плана перевозок - кодировка решения
    int Energy;               // энергия решения    
};

TSolution Current;            // текущее решение
TSolution Working;            // рабочее решение
TSolution Best;               // лучшее решение

double Random_double(double min, double max)  // случайное число от min до max
{
    return (double)(rand()) * (max - min) / RAND_MAX + min;
}

int random_int(int min, int max) { // случайное целочисленное число от min до max 
    return (rand() % (max - min + 1)) + min;
}


vector<vector<int>> generate_cost_matrix(int N, int K) {
    vector<std::vector<int>> matrix;
    for (int y = 0; y < N; y++) {
        vector<int> v;
        for (int x = 0; x < K; x++) {
            v.push_back(random_int(cost_min, cost_max));
        }
        matrix.push_back(v);
    }
    return matrix;
}

void show_cost_matrix(vector<vector<int>> matrix) {
    cout << "Матрица стоимостей перевозок:" << endl;
    for (auto row : matrix) {
        for (auto val : row) {
            std::cout << setw(std::to_string(cost_max).length()) << val << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << endl;

}

void Swap(TSolution* M) { // модификация решения
    int timeoutSeconds = 0.00001;
    std::chrono::steady_clock::time_point startTime;
    int timeoutSecondsOuterLoop1 = 0.001;
    std::chrono::steady_clock::time_point startTime_for_outer_loop1;
    int timeoutSecondsOuterLoop2 = 0.001;
    std::chrono::steady_clock::time_point startTime_for_outer_loop2;

    if (random_int(0, 1)) {
        // Нахождение двух случайных элементов в плане, чтобы они образовали прямоугольник
        //
        // - ----→ +
        // |       ↑
        // |       |
        // ↓       |
        // + ←---- -
        //
        
        int x1 = -1, y1 = -1;
        int x2 = -1, y2 = -1;
        while (true) {
            startTime_for_outer_loop1 = std::chrono::high_resolution_clock::now();

            startTime = std::chrono::high_resolution_clock::now();
            do {
                x1 = random_int(0, K - 2);
                y1 = random_int(0, N - 2);

                auto currentTime = std::chrono::high_resolution_clock::now();
                auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
                if (elapsedTime >= timeoutSeconds) break;
            } while (M->plan[y1][x1] == 0);

            startTime = std::chrono::high_resolution_clock::now();
            do {
                x2 = random_int(x1 + 1, K - 1);
                y2 = random_int(y1 + 1, N - 1);

                auto currentTime = std::chrono::high_resolution_clock::now();
                auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
                if (elapsedTime >= timeoutSeconds) break;
            } while (M->plan[y1][x1] == 0);

            if ((M->plan[y1][x1] == 0) || (M->plan[y2][x2] == 0) || (x1 == -1) || (y1 == -1) || (x2 == -1) || (y2 == -1)) {
                continue;
            } else {
                break;
            }

            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            if (elapsedTime >= timeoutSecondsOuterLoop1) break;
        }
        // Изменение четырех элементов в плане
        M->plan[y1][x1] -= transfer_size;
        M->plan[y1][x2] += transfer_size;
        M->plan[y2][x1] += transfer_size;
        M->plan[y2][x2] -= transfer_size;
    } else {
        // Нахождение двух случайных элементов в плане, чтобы они образовали прямоугольник
        //
        // + ←---- -
        // ↑       |
        // |       |
        // |       ↓
        // - ----→ +
        //
        int x1 = -1, y1 = -1;
        int x2 = -1, y2 = -1;
        while (true) {
            startTime_for_outer_loop2 = std::chrono::high_resolution_clock::now();

            startTime = std::chrono::high_resolution_clock::now();
            do {
                x1 = random_int(1, K - 1);
                y1 = random_int(0, N - 2);

                auto currentTime = std::chrono::high_resolution_clock::now();
                auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
                if (elapsedTime >= timeoutSeconds) break;
            } while (M->plan[y1][x1] == 0);

            startTime = std::chrono::high_resolution_clock::now();
            do {
                x2 = random_int(0, x1 - 1);
                y2 = random_int(y1 + 1, N - 1);

                auto currentTime = std::chrono::high_resolution_clock::now();
                auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
                if (elapsedTime >= timeoutSeconds) break;
            } while (M->plan[y1][x1] == 0);

            if ((M->plan[y1][x1] == 0) || (M->plan[y2][x2] == 0) || (x1 == -1) || (y1 == -1) || (x2 == -1) || (y2 == -1)) {
                continue;
            } else {
                break;
            }

            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            if (elapsedTime >= timeoutSecondsOuterLoop2) break;

        }
        // Изменение четырех элементов в плане
        M->plan[y1][x1] -= transfer_size;
        M->plan[y1][x2] += transfer_size;
        M->plan[y2][x1] += transfer_size;
        M->plan[y2][x2] -= transfer_size;
    }

}


void New(TSolution* M) { // инициализация решения (плана перевозок)
    // Заполнение плана случайными числами
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < K; x++) {
            M->plan[y][x] = random_int(plan_min, plan_max);
        }
    }
}


void CalcEnergy(TSolution* M) { // расчет энергии
    // Сложение всех произведений соответствующих элементов матриц cost и plan
    int sum = 0;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < K; x++) {
            sum += cost[y][x] * M->plan[y][x];
        }
    }
    M->Energy = sum;
}


void Copy(TSolution* MD, TSolution* MS) { // копирование решения
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < K; x++) {
            MD->plan[y][x] = MS->plan[y][x];
        }
    }
    MD->Energy = MS->Energy;
}


void Show(TSolution M) { // отображение на экране матрицы
    cout << "Решение:" << endl;
    for (auto row : M.plan) {
        for (auto val : row) {
            std::cout << setw(std::to_string(plan_max).length() + 1) << val << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << endl;
}


int main() {
    //setlocale(LC_ALL, "Russian");
    std::srand(std::time(nullptr));
    
    // Инициализация матрицы стоимостей перевозок
    cost = generate_cost_matrix(N, K);
    show_cost_matrix(cost);



    double  T = Tn;           // температура
    bool    fBest = false;    // флаг лучшего решения
    bool    fNew;             // флаг нового решения
    int     Time = 0;         // этап поиска
    // Инициализация векторов (нулями)
    Current.plan.resize(N, vector<int>(K));
    Working.plan.resize(N, vector<int>(K));
    Best.plan.resize(N, vector<int>(K));

    New(&Current);
    Show(Current);

    CalcEnergy(&Current);

    Copy(&Best, &Current);  
    Copy(&Working, &Current);

    while ((T > Tk) && (Best.Energy > 0)) {
        for (int Step = 0; Step < ST; Step++) {
            fNew = false;   
            Swap(&Working);   
            CalcEnergy(&Working);
            if (Working.Energy <= Current.Energy) { 
                fNew = true; 
            }
            else {
                double Delta = Working.Energy - Current.Energy;  // разница энергий
                double P = exp(-Delta / T);                      // вероятность допуска
                if (P > Random_double(0, 1)) fNew = true;
            }
            if (fNew) {
                fNew = false;  
                Copy(&Current, &Working);
                if (Current.Energy < Best.Energy) {
                    Copy(&Best, &Current);  
                    fBest = true;
                }
            } else Copy(&Working, &Current);
        }
        cout << "Temp=" << T << " Energy=" << Best.Energy << endl;
        T *= Alfa;
        Time++;
    }
    if ((fBest) && (N < 80)) Show(Best);
    system("pause");
    return 0;
}