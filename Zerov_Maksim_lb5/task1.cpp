#include <bits/stdc++.h>
using namespace std;

int mapChar(char c) {
    if (c == 'A') return 0;
    if (c == 'C') return 1;
    if (c == 'G') return 2;
    if (c == 'T') return 3;
    if (c == 'N') return 4;
    return -1;
}

char idChar(int x) {
    if (x == 0) return 'A';
    if (x == 1) return 'C';
    if (x == 2) return 'G';
    if (x == 3) return 'T';
    return 'N';
}

string qToString(queue<int> q) {
    string s = "[";
    bool first = true;
    while (!q.empty()) {
        if (!first) s += ", ";
        s += to_string(q.front());
        q.pop();
        first = false;
    }
    return s + "]";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string T;
    cin >> T;
    int n;
    cin >> n;
    vector<string> patterns(n);
    for (int i = 0; i < n; ++i) cin >> patterns[i];

    cerr << "\nВВОД\n";
    cerr << "Текст T = \"" << T << "\"\n";
    cerr << "Количество образцов n = " << n << "\n";
    for (int i = 0; i < n; ++i)
        cerr << "  Образец #" << (i + 1) << ": \"" << patterns[i] << "\"\n";

    const int MAXN = 250000;
    vector<vector<int>> nxt(MAXN, vector<int>(5, -1));
    vector<int> fail(MAXN, 0);
    vector<int> outLink(MAXN, -1);
    vector<vector<int>> out(MAXN);
    vector<int> len(n + 1);
    int nodes = 1;

    cerr << "\nЭТАП 1: ПОСТРОЕНИЕ БОРА\n";
    cerr << "Начальное число вершин: " << nodes << " (корень 0)\n";

    for (int i = 0; i < n; ++i) {
        int v = 0;
        cerr << "\nДобавляем образец #" << (i + 1) << " = \"" << patterns[i] << "\":\n";
        cerr << "  старт в корне (вершина 0)\n";
        for (char c : patterns[i]) {
            int idx = mapChar(c);
            if (idx == -1) continue;
            if (nxt[v][idx] == -1) {
                nxt[v][idx] = nodes;
                cerr << "  '" << c << "': из " << v
                     << " создаём новую вершину " << nodes << "\n";
                nodes++;
            } else {
                cerr << "  '" << c << "': из " << v
                     << " идём по существующему переходу в " << nxt[v][idx] << "\n";
            }
            v = nxt[v][idx];
        }
        out[v].push_back(i + 1);
        len[i + 1] = (int)patterns[i].size();
        cerr << "  конец образца: вершина " << v
             << " помечена как терминальная для #" << (i + 1) << "\n";
    }

    cerr << "\n--- Таблица переходов бора ---\n";
    cerr << "вершина |  A   C   G   T   N  | терминальные\n";
    for (int v = 0; v < nodes; ++v) {
        cerr << "  " << v << "     |";
        for (int c = 0; c < 5; ++c) {
            if (nxt[v][c] == -1) cerr << "  . ";
            else cerr << " " << nxt[v][c] << "  ";
        }
        cerr << " |";
        for (int p : out[v]) cerr << " #" << p;
        cerr << "\n";
    }

    vector<vector<int>> raw(nodes, vector<int>(5, -1));
    for (int v = 0; v < nodes; ++v)
        for (int c = 0; c < 5; ++c)
            raw[v][c] = nxt[v][c];

    cerr << "\nЭТАП 2: ПОСТРОЕНИЕ АВТОМАТА\n";
    cerr << "Инициализация корня: у отсутствующих переходов ставим ссылку в 0,\n";
    cerr << "вершины 1-го уровня получают fail = 0 и попадают в очередь.\n";

    queue<int> q;
    for (int i = 0; i < 5; ++i) {
        if (nxt[0][i] != -1) {
            fail[nxt[0][i]] = 0;
            q.push(nxt[0][i]);
            cerr << "  fail[" << nxt[0][i] << "] = 0\n";
        } else {
            nxt[0][i] = 0;
        }
    }
    cerr << "Очередь: " << qToString(q) << "\n";

    int step = 1;
    while (!q.empty()) {
        int v = q.front(); q.pop();
        cerr << "\nШаг BFS " << step++ << ": вершина " << v
             << " (fail = " << fail[v] << ")\n";

        int f = fail[v];
        if (!out[f].empty())
            outLink[v] = f;
        else
            outLink[v] = outLink[f];

        if (outLink[v] != -1)
            cerr << "  outLink[" << v << "] = " << outLink[v] << "\n";

        for (int i = 0; i < 5; ++i) {
            if (nxt[v][i] != -1) {
                int u = nxt[v][i];
                fail[u] = nxt[fail[v]][i];
                q.push(u);
                cerr << "  '" << idChar(i) << "': " << v << " -> " << u
                     << ", fail[" << u << "] = " << fail[u] << "\n";
            } else {
                nxt[v][i] = nxt[fail[v]][i];
                cerr << "  '" << idChar(i) << "': нет перехода, ссылка в "
                     << nxt[v][i] << "\n";
            }
        }
        cerr << "  очередь: " << qToString(q) << "\n";
    }

    cerr << "\nЭТАП 3: ОПИСАНИЕ ВЕРШИН\n";
    for (int v = 0; v < nodes; ++v) {
        cerr << "Вершина " << v << ": fail = " << fail[v]
             << ", outLink = " << outLink[v] << ", переходы: ";
        for (int c = 0; c < 5; ++c)
            cerr << idChar(c) << "->" << nxt[v][c] << "  ";
        cerr << ", терминальные:";
        if (out[v].empty()) cerr << " нет";
        else for (int p : out[v]) cerr << " #" << p;
        cerr << "\n";
    }

    const string dotFile = "automaton.dot";
    {
        ofstream fout(dotFile);
        if (!fout) {
            cerr << "Не удалось открыть " << dotFile << "\n";
            return 1;
        }

        fout << "digraph Automaton {\n";
        fout << "  rankdir=LR;\n";
        fout << "  node [shape=circle];\n";
        fout << "  start [shape=point];\n";
        fout << "  start -> 0;\n";

        for (int v = 0; v < nodes; ++v) {
            string label = to_string(v);
            if (!out[v].empty()) {
                label += "\\n";
                for (int i = 0; i < (int)out[v].size(); ++i) {
                    if (i) label += ",";
                    label += to_string(out[v][i]);
                }
                fout << "  " << v
                     << " [shape=doublecircle, label=\"" << label << "\"];\n";
            } else {
                fout << "  " << v
                     << " [label=\"" << label << "\"];\n";
            }
        }

        for (int v = 0; v < nodes; ++v) {
            for (int c = 0; c < 5; ++c) {
                int u = raw[v][c];
                if (u == -1) continue;
                fout << "  " << v << " -> " << u
                     << " [label=\"" << idChar(c) << "\"];\n";
            }
        }

        for (int v = 1; v < nodes; ++v) {
            if (fail[v] != v) {
                fout << "  " << v << " -> " << fail[v]
                     << " [style=dashed, color=blue, constraint=false];\n";
            }
        }

        for (int v = 1; v < nodes; ++v) {
            if (outLink[v] != -1) {
                fout << "  " << v << " -> " << outLink[v]
                     << " [style=dotted, color=red, constraint=false];\n";
            }
        }

        fout << "}\n";
    }

    cerr << "\nDOT-файл сохранён: " << dotFile << "\n"
         << "Для картинки: dot -Tpng " << dotFile << " -o automaton.png\n";

    cerr << "\nЭТАП 4: ПОИСК В ТЕКСТЕ\n";

    vector<pair<int,int>> res;
    int state = 0;
    for (int i = 0; i < (int)T.size(); ++i) {
        int idx = mapChar(T[i]);
        cerr << "Символ #" << (i + 1) << " = '" << T[i] << "': ";
        if (idx == -1) {
            state = 0;
            cerr << "неизвестный, сброс в 0\n";
            continue;
        }
        int old = state;
        state = nxt[state][idx];
        cerr << old << " -> " << state << "\n";

        int u = state;
        while (u != -1) {
            for (int p : out[u]) {
                int pos = i - len[p] + 1;
                cerr << "  образец #" << p
                     << " (старт " << (pos + 1) << ")\n";
                res.push_back({pos + 1, p});
            }
            if (outLink[u] != -1)
                cerr << "  переход по outLink: " << u << " -> " << outLink[u] << "\n";
            u = outLink[u];
        }
    }

    sort(res.begin(), res.end());

    cerr << "\nРЕЗУЛЬТАТ\n";
    cerr << "Найдено вхождений: " << res.size() << "\n";

    cout << "--- Вхождения (позиция, номер образца) ---\n";
    for (auto &pr : res) {
        cout << pr.first << ' ' << pr.second << '\n';
    }

    return 0;
}