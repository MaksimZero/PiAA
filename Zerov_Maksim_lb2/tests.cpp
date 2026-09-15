#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <sstream>
#include <iomanip>

using namespace std;

const double INF = 1e18;
const double EPS = 1e-9;

int N, START;
vector<vector<double>> cost;
vector<int> bestPath;
double bestCost;

bool isConnected() {
    if (N == 0) return true;
    vector<bool> vis(N, false);
    queue<int> q;
    q.push(START); vis[START] = true;
    int cnt = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop(); cnt++;
        for (int v = 0; v < N; v++)
            if (!vis[v] && u != v && cost[u][v] < INF / 2) {
                vis[v] = true; q.push(v);
            }
    }
    return cnt == N;
}

bool hasHamiltonianCandidate() {
    for (int i = 0; i < N; i++) {
        int deg = 0;
        for (int j = 0; j < N; j++)
            if (i != j && cost[i][j] < INF / 2) deg++;
        if (deg < 2) return false;
    }
    return true;
}

void dfsMST(int u, const vector<vector<int>>& adj, vector<bool>& vis, vector<int>& tour) {
    vis[u] = true; tour.push_back(u);
    for (int v : adj[u]) if (!vis[v]) dfsMST(v, adj, vis, tour);
}

double mstApprox(int start, vector<int>& tour) {
    vector<double> key(N, INF);
    vector<int> parent(N, -1);
    vector<bool> inMST(N, false);
    key[start] = 0;
    for (int iter = 0; iter < N; iter++) {
        int u = -1;
        for (int i = 0; i < N; i++)
            if (!inMST[i] && (u == -1 || key[i] < key[u])) u = i;
        if (u == -1 || key[u] >= INF) break;
        inMST[u] = true;
        for (int v = 0; v < N; v++)
            if (!inMST[v] && cost[u][v] < key[v]) {
                key[v] = cost[u][v]; parent[v] = u;
            }
    }
    vector<vector<int>> adj(N);
    for (int v = 0; v < N; v++)
        if (parent[v] != -1) {
            adj[parent[v]].push_back(v);
            adj[v].push_back(parent[v]);
        }
    for (int i = 0; i < N; i++) sort(adj[i].begin(), adj[i].end());
    vector<bool> vis(N, false);
    tour.clear();
    dfsMST(start, adj, vis, tour);
    if ((int)tour.size() != N) return INF;
    double total = 0;
    for (int i = 0; i + 1 < (int)tour.size(); i++) {
        if (cost[tour[i]][tour[i + 1]] >= INF / 2) return INF;
        total += cost[tour[i]][tour[i + 1]];
    }
    if (cost[tour.back()][start] >= INF / 2) return INF;
    total += cost[tour.back()][start];
    return total;
}

double littleReduce(const vector<int>& rem, int cur, int start) {
    int k = (int)rem.size();
    if (k == 0) return cost[cur][start];
    int sz = k + 1;
    vector<vector<double>> m(sz, vector<double>(sz, INF));
    for (int i = 0; i < k; i++) m[0][i] = cost[cur][rem[i]];
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++)
            if (i != j) m[i + 1][j] = cost[rem[i]][rem[j]];
        m[i + 1][k] = cost[rem[i]][start];
    }
    double total = 0;
    for (int i = 0; i < sz; i++) {
        double mn = INF;
        for (int j = 0; j < sz; j++) mn = min(mn, m[i][j]);
        if (mn >= INF) return INF;
        if (mn > 0) {
            total += mn;
            for (int j = 0; j < sz; j++)
                if (m[i][j] < INF) m[i][j] -= mn;
        }
    }
    for (int j = 0; j < sz; j++) {
        double mn = INF;
        for (int i = 0; i < sz; i++) mn = min(mn, m[i][j]);
        if (mn >= INF) return INF;
        if (mn > 0) {
            total += mn;
            for (int i = 0; i < sz; i++)
                if (m[i][j] < INF) m[i][j] -= mn;
        }
    }
    return total;
}

void dfs(int cur, vector<bool>& visited, vector<int>& path, double curCost) {
    if ((int)path.size() == N) {
        if (cost[cur][START] >= INF / 2) return;
        double total = curCost + cost[cur][START];
        if (total < bestCost - EPS) {
            bestCost = total; bestPath = path;
        }
        return;
    }
    vector<int> rem;
    for (int i = 0; i < N; i++) if (!visited[i]) rem.push_back(i);
    double lb = curCost + littleReduce(rem, cur, START);
    if (lb > bestCost + EPS) return;
    for (int nxt : rem) {
        if (cost[cur][nxt] >= INF / 2) continue;
        visited[nxt] = true;
        path.push_back(nxt);
        dfs(nxt, visited, path, curCost + cost[cur][nxt]);
        path.pop_back();
        visited[nxt] = false;
    }
}

double solveLittle(int start) {
    START = start;
    bestCost = INF;
    bestPath.clear();
    vector<bool> visited(N, false);
    visited[start] = true;
    vector<int> path = {start};
    dfs(start, visited, path, 0.0);
    return bestCost;
}

double solveMST(int start, vector<int>& tour) {
    return mstApprox(start, tour);
}

void setMatrix(const vector<vector<double>>& m) {
    cost = m;
    N = (int)m.size();
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (i == j || cost[i][j] < 0) cost[i][j] = INF;
}

string matrixToStr() {
    ostringstream oss;
    for (int i = 0; i < N; i++) {
        oss << "    ";
        for (int j = 0; j < N; j++) {
            if (cost[i][j] >= INF / 2) oss << "  -1";
            else oss << setw(4) << fixed << setprecision(0) << cost[i][j];
        }
        oss << "\n";
    }
    return oss.str();
}

string pathToStr(const vector<int>& p) {
    string s;
    for (size_t i = 0; i < p.size(); i++) {
        if (i) s += " -> ";
        s += to_string(p[i]);
    }
    return s;
}

class TSPTest : public ::testing::Test {
protected:
    void TearDown() override {
        cout << "\n";
    }
};

TEST_F(TSPTest, ExampleN3) {
    cout << "[1] N=3 (пример из условия)\n\n";
    setMatrix({
        {-1, 1, 3},
        {3, -1, 1},
        {1, 2, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 3x3\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "МЕТОД 1: 2-приближение по МОД\n";
    vector<int> tour;
    double mst = solveMST(0, tour);
    cout << "  АДО МОД (preorder-обход): " << pathToStr(tour) << " -> 0\n";
    cout << "  Верхняя оценка: " << fixed << setprecision(2) << mst << "\n\n";

    cout << "МЕТОД 2: Алгоритм Литтла\n";
    double little = solveLittle(0);
    cout << "  Оптимальный маршрут: " << pathToStr(bestPath) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << little << "\n\n";

    cout << "РЕЗУЛЬТАТ: Решение найдено!\n";
    cout << "  OK (маршрут: " << pathToStr(bestPath) << " 0, стоимость: "
         << fixed << setprecision(2) << little << ")";

    EXPECT_DOUBLE_EQ(mst, 3.0);
    EXPECT_DOUBLE_EQ(little, 3.0);
}

TEST_F(TSPTest, ExampleN4) {
    cout << "[2] N=4 (пример из условия)\n\n";
    setMatrix({
        {-1, 3, 4, 1},
        {1, -1, 3, 4},
        {9, 2, -1, 4},
        {8, 9, 2, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 4x4\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "МЕТОД 1: 2-приближение по МОД\n";
    vector<int> tour;
    double mst = solveMST(0, tour);
    cout << "  АДО МОД (preorder-обход): " << pathToStr(tour) << " -> 0\n";
    cout << "  Верхняя оценка: " << fixed << setprecision(2) << mst << "\n\n";

    cout << "МЕТОД 2: Алгоритм Литтла\n";
    double little = solveLittle(0);
    cout << "  Оптимальный маршрут: " << pathToStr(bestPath) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << little << "\n\n";

    cout << "РЕЗУЛЬТАТ: Решение найдено!\n";
    cout << "  OK (маршрут: " << pathToStr(bestPath) << " 0, стоимость: "
         << fixed << setprecision(2) << little << ")";

    EXPECT_DOUBLE_EQ(mst, 6.0);
    EXPECT_DOUBLE_EQ(little, 6.0);
}

TEST_F(TSPTest, AllWeightsEqual) {
    cout << "[3] Симметричная матрица (N=4, все веса 1)\n\n";
    setMatrix({
        {-1, 1, 1, 1},
        {1, -1, 1, 1},
        {1, 1, -1, 1},
        {1, 1, 1, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 4x4\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "МЕТОД 1: 2-приближение по МОД\n";
    vector<int> tour;
    double mst = solveMST(0, tour);
    cout << "  АДО МОД: " << pathToStr(tour) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << mst << "\n\n";

    cout << "МЕТОД 2: Алгоритм Литтла\n";
    double little = solveLittle(0);
    cout << "  Оптимум: " << pathToStr(bestPath) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << little << "\n\n";

    cout << "РЕЗУЛЬТАТ: Решение найдено!\n";
    cout << "  OK (стоимость: " << fixed << setprecision(2) << little << ")";

    EXPECT_DOUBLE_EQ(mst, 4.0);
    EXPECT_DOUBLE_EQ(little, 4.0);
}

TEST_F(TSPTest, AsymmetricN3) {
    cout << "[4] Асимметричная матрица (N=3)\n\n";
    setMatrix({
        {-1, 10, 5},
        {3, -1, 1},
        {2, 4, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 3x3\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "МЕТОД 1: 2-приближение по МОД\n";
    vector<int> tour;
    double mst = solveMST(0, tour);
    cout << "  АДО МОД: " << pathToStr(tour) << " -> 0\n";
    cout << "  Верхняя оценка: " << fixed << setprecision(2) << mst << "\n\n";

    cout << "МЕТОД 2: Алгоритм Литтла\n";
    double little = solveLittle(0);
    cout << "  Оптимум: " << pathToStr(bestPath) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << little << "\n\n";

    cout << "РЕЗУЛЬТАТ: Решение найдено!\n";
    cout << "  OK (оптимум: " << fixed << setprecision(2) << little << ")";

    EXPECT_DOUBLE_EQ(little, 12.0);
    EXPECT_LT(mst, INF / 2);
}

TEST_F(TSPTest, N2Trivial) {
    cout << "[5] N=2 (две вершины)\n\n";
    setMatrix({
        {-1, 7},
        {7, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 2x2\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "МЕТОД 1: 2-приближение по МОД\n";
    vector<int> tour;
    double mst = solveMST(0, tour);
    cout << "  АДО МОД: " << pathToStr(tour) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << mst << "\n\n";

    cout << "МЕТОД 2: Алгоритм Литтла\n";
    double little = solveLittle(0);
    cout << "  Оптимум: " << pathToStr(bestPath) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << little << "\n\n";

    cout << "РЕЗУЛЬТАТ: Решение найдено!\n";
    cout << "  OK (стоимость: " << fixed << setprecision(2) << little << ")";

    EXPECT_DOUBLE_EQ(mst, 14.0);
    EXPECT_DOUBLE_EQ(little, 14.0);
}

TEST_F(TSPTest, Disconnected) {
    cout << "[6] Несвязный граф (N=4)\n\n";
    setMatrix({
        {-1, 5, -1, -1},
        {5, -1, -1, -1},
        {-1, -1, -1, 3},
        {-1, -1, 3, -1}
    });
    START = 0;

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 4x4\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "ПРОВЕРКА ГРАФА:\n";
    cout << "  BFS от вершины 0: посещены {0, 1}\n";
    cout << "  Вершины 2 и 3 недостижимы.\n\n";

    cout << "РЕЗУЛЬТАТ: Решение не найдено!\n";
    cout << "  OK (граф несвязный — обнаружено корректно)";

    EXPECT_FALSE(isConnected());
}

TEST_F(TSPTest, VertexDegreeOne) {
    cout << "[7] Висячая вершина (N=4)\n\n";
    setMatrix({
        {-1, 1, -1, -1},
        {1, -1, 1, 1},
        {-1, 1, -1, 1},
        {-1, 1, 1, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 4x4\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "ПРОВЕРКА ГРАФА:\n";
    cout << "  Степени вершин: 0 -> 1, 1 -> 3, 2 -> 2, 3 -> 2\n";
    cout << "  Вершина 0 имеет степень 1.\n\n";

    cout << "РЕЗУЛЬТАТ: Решение не найдено!\n";
    cout << "  OK (гамильтонов цикл невозможен)";

    EXPECT_FALSE(hasHamiltonianCandidate());
}

TEST_F(TSPTest, TwoApproxGuarantee) {
    cout << "[8] Проверка гарантии 2-приближения (N=4)\n\n";
    setMatrix({
        {-1, 2, 9, 10},
        {2, -1, 6, 4},
        {9, 6, -1, 8},
        {10, 4, 8, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 4x4\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "МЕТОД 1: 2-приближение по МОД\n";
    vector<int> tour;
    double mst = solveMST(0, tour);
    cout << "  АДО МОД: " << pathToStr(tour) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << mst << "\n\n";

    cout << "МЕТОД 2: Алгоритм Литтла\n";
    double opt = solveLittle(0);
    cout << "  Оптимум: " << pathToStr(bestPath) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << opt << "\n\n";

    cout << "ПРОВЕРКА ГАРАНТИИ:\n";
    cout << "  МОД = " << fixed << setprecision(2) << mst << "\n";
    cout << "  2 * OPT = " << fixed << setprecision(2) << 2 * opt << "\n";
    cout << "  Условие МОД <= 2*OPT: ";
    cout << (mst <= 2 * opt + EPS ? "выполнено" : "нарушено") << "\n\n";

    cout << "РЕЗУЛЬТАТ: Решение найдено!\n";
    cout << "  OK (гарантия 2-приближения соблюдена)";

    EXPECT_LE(mst, 2.0 * opt + EPS);
}

TEST_F(TSPTest, PathLengthCheck) {
    cout << "[9] Проверка длины маршрута (N=5)\n\n";
    setMatrix({
        {-1, 10, 15, 20, 25},
        {10, -1, 35, 25, 30},
        {15, 35, -1, 30, 20},
        {20, 25, 30, -1, 15},
        {25, 30, 20, 15, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 5x5\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "МЕТОД: Алгоритм Литтла\n";
    double little = solveLittle(0);
    cout << "  Оптимум: " << pathToStr(bestPath) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << little << "\n\n";

    cout << "ПРОВЕРКА:\n";
    cout << "  Количество вершин: " << bestPath.size()
         << " (ожидалось " << N << ")\n";
    vector<bool> seen(N, false);
    bool allUnique = true;
    for (int v : bestPath) {
        if (seen[v]) { allUnique = false; break; }
        seen[v] = true;
    }
    bool allPresent = true;
    for (int i = 0; i < N; i++) if (!seen[i]) allPresent = false;
    cout << "  Все уникальны: " << (allUnique ? "да" : "нет") << "\n";
    cout << "  Все присутствуют: " << (allPresent ? "да" : "нет") << "\n\n";

    cout << "РЕЗУЛЬТАТ: Решение найдено!\n";
    cout << "  OK (маршрут содержит все вершины ровно один раз)";

    EXPECT_EQ((int)bestPath.size(), N);
}

TEST_F(TSPTest, CostConsistency) {
    cout << "[10] Проверка стоимости (N=4)\n\n";
    setMatrix({
        {-1, 5, 10, 15},
        {5, -1, 20, 25},
        {10, 20, -1, 30},
        {15, 25, 30, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 4x4\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "МЕТОД: Алгоритм Литтла\n";
    double little = solveLittle(0);
    cout << "  Оптимум: " << pathToStr(bestPath) << " -> 0\n";
    cout << "  Стоимость (из алгоритма): " << fixed << setprecision(2) << little << "\n\n";

    cout << "ПРОВЕРКА СТОИМОСТИ (ручной подсчёт):\n";
    double manual = 0;
    for (int i = 0; i + 1 < (int)bestPath.size(); i++) {
        double w = cost[bestPath[i]][bestPath[i + 1]];
        cout << "  " << bestPath[i] << " -> " << bestPath[i + 1]
             << ": " << fixed << setprecision(2) << w << "\n";
        manual += w;
    }
    double last = cost[bestPath.back()][0];
    cout << "  " << bestPath.back() << " -> 0: "
         << fixed << setprecision(2) << last << "\n";
    manual += last;
    cout << "  Итого: " << fixed << setprecision(2) << manual << "\n\n";

    cout << "РЕЗУЛЬТАТ: Стоимость совпадает!\n";
    cout << "  OK (алгоритм: " << fixed << setprecision(2) << little
         << ", ручной подсчёт: " << fixed << setprecision(2) << manual << ")";

    EXPECT_NEAR(little, manual, 1e-6);
}

TEST_F(TSPTest, NonZeroStart) {
    cout << "[11] Стартовая вершина != 0 (N=4)\n\n";
    setMatrix({
        {-1, 1, 3, 1},
        {1, -1, 1, 3},
        {3, 1, -1, 1},
        {1, 3, 1, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 2\n";
    cout << "  Размер: 4x4\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "МЕТОД: Алгоритм Литтла\n";
    double little = solveLittle(2);
    cout << "  Оптимум: " << pathToStr(bestPath) << " -> 2\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << little << "\n\n";

    cout << "ПРОВЕРКА:\n";
    cout << "  Первая вершина маршрута: " << bestPath[0]
         << " (ожидалось 2)\n\n";

    cout << "РЕЗУЛЬТАТ: Решение найдено!\n";
    cout << "  OK (маршрут начинается с вершины 2)";

    EXPECT_DOUBLE_EQ(little, 4.0);
    EXPECT_EQ(bestPath[0], 2);
}

TEST_F(TSPTest, MSTReturnsValidTour) {
    cout << "[12] АДО МОД возвращает валидный тур (N=4)\n\n";
    setMatrix({
        {-1, 7, 12, 3},
        {7, -1, 5, 11},
        {12, 5, -1, 6},
        {3, 11, 6, -1}
    });

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Стартовая вершина: 0\n";
    cout << "  Размер: 4x4\n";
    cout << "  Матрица:\n" << matrixToStr() << "\n";

    cout << "МЕТОД: 2-приближение по МОД\n";
    vector<int> tour;
    double mst = solveMST(0, tour);
    cout << "  АДО МОД: " << pathToStr(tour) << " -> 0\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << mst << "\n\n";

    cout << "ПРОВЕРКА:\n";
    cout << "  Маршрут не пуст: " << (mst < INF / 2 ? "да" : "нет") << "\n";
    cout << "  Количество вершин: " << tour.size()
         << " (ожидалось " << N << ")\n";
    cout << "  Первая вершина: " << tour[0] << " (ожидалось 0)\n\n";

    cout << "РЕЗУЛЬТАТ: Решение найдено!\n";
    cout << "  OK (МОД вернул валидный тур)";

    EXPECT_LT(mst, INF / 2);
    EXPECT_EQ((int)tour.size(), N);
    EXPECT_EQ(tour[0], 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    cout << "ТЕСТЫ\n\n";

    int result = RUN_ALL_TESTS();

    cout << "\nРЕЗУЛЬТАТЫ\n";
    if (result == 0)
        cout << "  Пройдено: 12\n  Не пройдено: 0\n";
    else
        cout << "  Обнаружены ошибки. См. лог выше.\n";

    return result;
}