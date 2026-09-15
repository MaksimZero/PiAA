#include <bits/stdc++.h>
using namespace std;

const double INF = 1e18;
const double EPS = 1e-9;

int N, START;
vector<vector<double>> cost;
vector<int> bestPath;
double bestCost;

// Логирование
ofstream g_log;

void logLine(const string& s) {
    cout << s;
    cout.flush();
    if (g_log) g_log << s;
}

// Шаги визуализации
struct Step {
    string phase;
    double curCost, lb, best;
    vector<int> curPath, bestP;
    vector<pair<int,int>> mstEdges;
    string message;
};
vector<Step> steps;
vector<pair<int,int>> globalMstEdges;

void addStep(const string& phase, double curCost, double lb, double best,
             const vector<int>& curPath, const vector<int>& bestP, const string& msg) {
    steps.push_back({phase, curCost, lb, best, curPath, bestP, globalMstEdges, msg});
}

string pathToStr(const vector<int>& p) {
    string s;
    for (size_t i = 0; i < p.size(); i++) {
        if (i) s += ',';
        s += to_string(p[i]);
    }
    return s;
}

string edgesToStr(const vector<pair<int,int>>& e) {
    string s;
    for (size_t i = 0; i < e.size(); i++) {
        if (i) s += ',';
        s += to_string(e[i].first) + "-" + to_string(e[i].second);
    }
    return s;
}

string cleanMsg(string s) {
    for (char& c : s) if (c == '\n' || c == '\r' || c == '\t') c = ' ';
    return s;
}

// Проверки графа
bool isConnected() {
    if (N == 0) return true;
    vector<bool> vis(N, false);
    queue<int> q;
    q.push(START);
    vis[START] = true;
    int cnt = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        cnt++;
        for (int v = 0; v < N; v++)
            if (!vis[v] && u != v && cost[u][v] < INF / 2) {
                vis[v] = true;
                q.push(v);
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

// Генерация матрицы
vector<vector<double>> generateMatrix(int n, int mode) {
    vector<vector<double>> m(n, vector<double>(n, 0.0));
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(1, 99);

    if (mode == 0) {
        logLine("\n[Генерация] Симметричная матрица, N = " + to_string(n) + "\n");
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++) {
                int w = dist(rng);
                m[i][j] = m[j][i] = w;
            }
    } else {
        logLine("\n[Генерация] Асимметричная матрица, N = " + to_string(n) + "\n");
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (i != j) m[i][j] = dist(rng);
    }
    return m;
}

// Сохранение / загрузка
void saveMatrixToFile(const string& path) {
    ofstream fout(path);
    if (!fout) { logLine("[Ошибка] Не удалось открыть " + path + "\n"); return; }
    fout << N << "\n";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (j) fout << ' ';
            if (i == j) fout << -1;
            else fout << fixed << setprecision(2) << cost[i][j];
        }
        fout << "\n";
    }
    logLine("[Сохранение] Матрица записана в " + path + "\n");
}

bool loadMatrixFromFile(const string& path) {
    ifstream fin(path);
    if (!fin) { logLine("[Ошибка] Не удалось открыть " + path + "\n"); return false; }
    fin >> N;
    cost.assign(N, vector<double>(N));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            fin >> cost[i][j];
            if (i == j || cost[i][j] < 0) cost[i][j] = INF;
        }
    logLine("[Загрузка] Матрица прочитана из " + path +
            " (N = " + to_string(N) + ")\n");
    return true;
}

void printMatrixToLog() {
    logLine("\nМатрица " + to_string(N) + "x" + to_string(N) + ":\n");
    for (int i = 0; i < N; i++) {
        ostringstream oss;
        for (int j = 0; j < N; j++) {
            if (cost[i][j] >= INF / 2) oss << "     -1";
            else oss << setw(7) << fixed << setprecision(2) << cost[i][j];
        }
        oss << "\n";
        logLine(oss.str());
    }
}

// МЕТОД 1: 2-приближение по МОД (АДО МОД)
void dfsMST(int u, const vector<vector<int>>& adj, vector<bool>& vis, vector<int>& tour) {
    vis[u] = true;
    tour.push_back(u);
    for (int v : adj[u]) if (!vis[v]) dfsMST(v, adj, vis, tour);
}

double mstApprox(int start, vector<int>& tour) {
    vector<double> key(N, INF);
    vector<int> parent(N, -1);
    vector<bool> inMST(N, false);
    key[start] = 0;

    vector<int> mstSoFar;
    logLine("\n===== Построение МОД (алгоритм Прима) =====\n");

    for (int iter = 0; iter < N; iter++) {
        int u = -1;
        for (int i = 0; i < N; i++)
            if (!inMST[i] && (u == -1 || key[i] < key[u])) u = i;
        if (u == -1 || key[u] >= INF) break;
        inMST[u] = true;
        mstSoFar.push_back(u);

        if (parent[u] != -1)
            globalMstEdges.push_back({min(parent[u], u), max(parent[u], u)});

        ostringstream oss;
        oss << "MST (Прим): добавлена вершина " << u;
        if (parent[u] != -1)
            oss << ", ребро " << parent[u] << "-" << u
                << " (вес " << fixed << setprecision(2) << cost[parent[u]][u] << ")";
        else oss << " (корень)";
        oss << "\n";
        logLine(oss.str());
        addStep("MST_BUILD", key[u], 0, INF, mstSoFar, {}, oss.str());

        for (int v = 0; v < N; v++)
            if (!inMST[v] && cost[u][v] < key[v]) {
                key[v] = cost[u][v];
                parent[v] = u;
            }
    }

    if ((int)mstSoFar.size() != N) {
        logLine("[МОД] Ошибка: не все вершины достижимы, граф несвязный\n");
        tour.clear();
        return INF;
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

    double total = 0;
    bool valid = true;
    for (int i = 0; i + 1 < (int)tour.size(); i++) {
        if (cost[tour[i]][tour[i + 1]] >= INF / 2) { valid = false; break; }
        total += cost[tour[i]][tour[i + 1]];
    }
    if (valid && !tour.empty()) {
        if (cost[tour.back()][start] >= INF / 2) valid = false;
        else total += cost[tour.back()][start];
    }

    if (!valid) {
        logLine("[МОД] Обход дал разрыв (неполный граф) — АДО МОД не применимо\n");
        tour.clear();
        return INF;
    }

    ostringstream oss;
    oss << "\nАДО МОД (preorder-обход): ";
    for (size_t i = 0; i < tour.size(); i++) {
        if (i) oss << " -> ";
        oss << tour[i];
    }
    oss << " -> " << start << "\n";
    oss << "Верхняя оценка (стоимость маршрута): "
        << fixed << setprecision(2) << total << "\n";
    logLine(oss.str());

    addStep("MST_WALK", total, 0, total, tour, tour, oss.str());
    return total;
}

// МЕТОД 2: ВЕТВИ И ГРАНИЦЫ (ЛИТТЛ)
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
            bestCost = total;
            bestPath = path;
            ostringstream oss;
            oss << "[Улучшение] Найден лучший маршрут, стоимость = "
                << fixed << setprecision(2) << total << "\n";
            logLine(oss.str());
            addStep("IMPROVE", curCost, total, total, path, path, oss.str());
        }
        return;
    }
    vector<int> rem;
    for (int i = 0; i < N; i++) if (!visited[i]) rem.push_back(i);

    double lb = curCost + littleReduce(rem, cur, START);
    if (lb > bestCost + EPS) {
        ostringstream oss;
        oss << "[Отсечение] путь ";
        for (size_t i = 0; i < path.size(); i++) {
            if (i) oss << " -> ";
            oss << path[i];
        }
        oss << ", нижняя оценка " << fixed << setprecision(2) << lb
            << " > best " << bestCost << "\n";
        logLine(oss.str());
        addStep("PRUNE", curCost, lb, bestCost, path, bestPath, oss.str());
        return;
    }
    {
        ostringstream oss;
        oss << "[Ветвление] путь ";
        for (size_t i = 0; i < path.size(); i++) {
            if (i) oss << " -> ";
            oss << path[i];
        }
        oss << ", стоимость " << fixed << setprecision(2) << curCost
            << ", оценка снизу " << lb << ", best = " << bestCost << "\n";
        logLine(oss.str());
        addStep("BRANCH", curCost, lb, bestCost, path, bestPath, oss.str());
    }
    for (int i = 0; i < (int)rem.size(); i++) {
        int nxt = rem[i];
        if (cost[cur][nxt] >= INF / 2) continue;
        visited[nxt] = true;
        path.push_back(nxt);
        dfs(nxt, visited, path, curCost + cost[cur][nxt]);
        path.pop_back();
        visited[nxt] = false;
    }
}

// out.txt
void writeOut() {
    ofstream fout("out.txt");
    fout << N << "\n";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (j) fout << ' ';
            if (cost[i][j] >= INF / 2) fout << "-1";
            else fout << fixed << setprecision(6) << cost[i][j];
        }
        fout << "\n";
    }
    fout << steps.size() << "\n";
    for (auto& s : steps) {
        fout << s.phase << '\t'
             << fixed << setprecision(6) << s.curCost << '\t'
             << (s.lb >= INF / 2 ? -1.0 : s.lb) << '\t'
             << (s.best >= INF / 2 ? -1.0 : s.best) << '\t'
             << pathToStr(s.curPath) << '\t'
             << pathToStr(s.bestP) << '\t'
             << edgesToStr(s.mstEdges) << '\t'
             << cleanMsg(s.message) << '\n';
    }
    logLine("[Сохранение] Файл визуализации out.txt (" +
            to_string(steps.size()) + " шагов)\n");
}

// Меню работы с матрицей
void printMenu() {
    logLine("\n===== Меню =====\n");
    logLine("1. Сгенерировать симметричную матрицу\n");
    logLine("2. Сгенерировать асимметричную матрицу\n");
    logLine("3. Сохранить матрицу в файл\n");
    logLine("4. Загрузить матрицу из файла\n");
    logLine("5. Запустить алгоритм\n");
    logLine("6. Показать матрицу\n");
    logLine("0. Выход\n");
    logLine("Выбор: ");
}

// Выбор метода
int askMethod() {
    logLine("\n===== Выбор метода =====\n");
    logLine("1. Метод ветвей и границ (алгоритм Литтла)\n");
    logLine("2. 2-приближение по МОД (АДО МОД)\n");
    logLine("3. Оба метода (для сравнения)\n");
    logLine("0. Отмена\n");
    logLine("Выбор: ");
    int m;
    if (!(cin >> m)) return 0;
    logLine(to_string(m) + "\n");
    return m;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    g_log.open("log.txt");
    if (!g_log) { cerr << "Не удалось открыть log.txt\n"; return 1; }

    logLine("==========================================================\n");
    logLine("        ЗАДАЧА КОММИВОЯЖЁРА (TSP)\n");
    logLine("==========================================================\n");
    logLine("\n");
    logLine("  Программа решает задачу коммивояжёра двумя методами:\n");
    logLine("\n");
    logLine("  1) Метод ветвей и границ (алгоритм Литтла)\n");
    logLine("     - точное решение, находит гарантированно оптимальный\n");
    logLine("       гамильтонов цикл минимальной стоимости;\n");
    logLine("     - использует нижнюю оценку Литтла для отсечения\n");
    logLine("       бесперспективных веток перебора.\n");
    logLine("\n");
    logLine("  2) 2-приближение по МОД (АДО МОД)\n");
    logLine("     - приближённый метод: строит минимальное остовное\n");
    logLine("       дерево (алгоритм Прима), затем обходит его в preorder\n");
    logLine("       и замыкает маршрут;\n");
    logLine("     - может дать верхнюю оценку для метода Литтла,\n");
    logLine("       ускоряя отсечения.\n");
    logLine("\n");
    logLine("  На вход подаётся матрица весов графа. Значение -1\n");
    logLine("  означает отсутствие ребра или петлю (i == j).\n");
    logLine("  Все остальные веса неотрицательны.\n");
    logLine("\n");
    logLine("  Результат: маршрут коммивояжёра и его стоимость.\n");
    logLine("==========================================================\n");

    logLine("\nВведите стартовую вершину: ");
    cin >> START;
    logLine(to_string(START) + "\n");

    int choice = -1;
    while (choice != 0) {
        printMenu();
        if (!(cin >> choice)) break;

        if (choice == 1 || choice == 2) {
            int n;
            logLine("Введите N (число городов): ");
            cin >> n;
            logLine(to_string(n) + "\n");
            if (n < 2 || n > 15) {
                logLine("[Ошибка] N должно быть в диапазоне 2..15\n");
                continue;
            }
            N = n;
            if (START < 0 || START >= N) {
                logLine("[Ошибка] Стартовая вершина вне диапазона 0.." +
                        to_string(N-1) + "\n");
                continue;
            }
            int mode = choice - 1;
            cost = generateMatrix(N, mode);
            logLine("Матрица " + to_string(N) + "x" + to_string(N) + " сгенерирована.\n");
            printMatrixToLog();
            steps.clear();
            globalMstEdges.clear();
            bestPath.clear();
            bestCost = INF;
        }
        else if (choice == 3) {
            if (N == 0) { logLine("Сначала сгенерируйте или загрузите матрицу\n"); continue; }
            string path;
            logLine("Имя файла для сохранения: ");
            cin >> path;
            logLine(path + "\n");
            saveMatrixToFile(path);
        }
        else if (choice == 4) {
            string path;
            logLine("Имя файла для загрузки: ");
            cin >> path;
            logLine(path + "\n");
            if (loadMatrixFromFile(path)) {
                if (START < 0 || START >= N) {
                    logLine("[Ошибка] Стартовая вершина вне диапазона 0.." +
                            to_string(N-1) + "\n");
                    cost.clear();
                    N = 0;
                    continue;
                }
                printMatrixToLog();
                steps.clear();
                globalMstEdges.clear();
                bestPath.clear();
                bestCost = INF;
            }
        }        
        else if (choice == 5) {
            if (N == 0) { logLine("Сначала сгенерируйте или загрузите матрицу\n"); continue; }

            steps.clear();
            globalMstEdges.clear();
            bestPath.clear();
            bestCost = INF;

            if (N == 1) {
                logLine("Тривиальный случай: N = 1\n");
                continue;
            }

            logLine("\n########## ПРОВЕРКА ГРАФА ##########\n");

            if (!isConnected()) {
                logLine("[Ошибка] Граф несвязный!\n");
                logLine("Из стартовой вершины нельзя добраться до всех остальных.\n");
                logLine("Задача коммивояжёра не имеет решения.\n");
                logLine("\n########## ОШИБКА ##########\n");
                continue;
            }
            if (!hasHamiltonianCandidate()) {
                logLine("[Ошибка] В графе есть вершина со степенью < 2.\n");
                logLine("Гамильтонов цикл в таком графе невозможен.\n");
                logLine("\n########## ОШИБКА ##########\n");
                continue;
            }

            logLine("[OK] Граф связный, у всех вершин степень >= 2\n");

            int method = askMethod();
            if (method == 0) continue;

            vector<int> approxTour;
            double approxCost = INF;

            if (method == 2 || method == 3) {
                logLine("\n########## МЕТОД: 2-приближение по МОД ##########\n");
                approxCost = mstApprox(START, approxTour);

                if (approxCost >= INF / 2) {
                    logLine("[Инфо] АДО МОД не дало корректной верхней оценки\n");
                } else {
                    logLine("\n===== Результат АДО МОД =====\n");
                    logLine("Маршрут: ");
                    for (int i = 0; i < (int)approxTour.size(); i++)
                        logLine(to_string(approxTour[i]) + " ");
                    logLine(" " + to_string(START) + "\n");
                    logLine("Стоимость: " + to_string(approxCost) + "\n");
                }
            }

            if (method == 1 || method == 3) {
                logLine("\n########## МЕТОД: ВЕТВИ И ГРАНИЦЫ (ЛИТТЛ) ##########\n");

                if (method == 3 && approxCost < INF) {
                    bestCost = approxCost;
                    bestPath = approxTour;
                } else {
                    bestCost = INF;
                    bestPath.clear();
                }

                vector<bool> visited(N, false);
                visited[START] = true;
                vector<int> path = {START};
                dfs(START, visited, path, 0.0);

                if (bestPath.empty() || bestCost >= INF / 2) {
                    logLine("[Ошибка] Гамильтонов цикл не найден!\n");
                    logLine("\n########## ОШИБКА ##########\n");
                    continue;
                }

                addStep("DONE", bestCost, bestCost, bestCost, bestPath, bestPath,
                        "Алгоритм завершён");

                logLine("\n===== Результат Литтла =====\n");
                logLine("Оптимальный маршрут: ");
                for (int i = 0; i < N; i++) logLine(to_string(bestPath[i]) + " ");
                logLine(" " + to_string(START) + "\n");
                logLine("Стоимость: " + to_string(bestCost) + "\n");
            }

            writeOut();
            logLine("\n########## ЗАВЕРШЕНО ##########\n");
        }
        else if (choice == 6) {
            if (N == 0) { logLine("Матрица пуста\n"); continue; }
            printMatrixToLog();
        }
    }

    logLine("\n[Завершение] Программа остановлена пользователем\n");
    g_log.close();
    return 0;
}