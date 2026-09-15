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

    string T, P;
    char wild;
    cin >> T >> P >> wild;

    int n = (int)T.size();
    int m = (int)P.size();

    cerr << "\nВВОД\n";
    cerr << "Текст   T = \"" << T << "\" (длина " << n << ")\n";
    cerr << "Образец P = \"" << P << "\" (длина " << m << ")\n";
    cerr << "Джокер  wild = '" << wild << "'\n";

    if (m > n) {
        cerr << "\nДлина образца больше текста — совпадений нет.\n";
        return 0;
    }

    cerr << "\nЭТАП 1: РАЗБИЕНИЕ ОБРАЗЦА\n";

    vector<pair<int,string>> segs;
    int s = -1;
    for (int i = 0; i < m; ++i) {
        if (P[i] == wild) {
            if (s != -1) {
                segs.push_back({s, P.substr(s, i - s)});
                s = -1;
            }
        } else if (s == -1) {
            s = i;
        }
    }
    if (s != -1) segs.push_back({s, P.substr(s, m - s)});

    cerr << "Найдено сегментов: " << segs.size() << "\n";
    for (int k = 0; k < (int)segs.size(); ++k)
        cerr << "  Сегмент #" << k << ": offset = " << segs[k].first
             << ", строка = \"" << segs[k].second << "\"\n";

    cerr << "\nЭТАП 2: ПОСТРОЕНИЕ БОРА\n";

    vector<vector<int>> nxt(1, vector<int>(5, -1));
    vector<int> fail(1, 0);
    vector<int> outLink(1, -1);
    vector<vector<int>> out(1);
    int nodes = 1;

    for (int k = 0; k < (int)segs.size(); ++k) {
        int v = 0;
        cerr << "\nДобавляем сегмент #" << k
             << " = \"" << segs[k].second << "\":\n";
        for (char c : segs[k].second) {
            int idx = mapChar(c);
            if (idx == -1) continue;
            if (nxt[v][idx] == -1) {
                nxt[v][idx] = nodes;
                nxt.push_back(vector<int>(5, -1));
                fail.push_back(0);
                outLink.push_back(-1);
                out.push_back({});
                cerr << "  '" << c << "': из " << v
                     << " создаём вершину " << nodes << "\n";
                nodes++;
            } else {
                cerr << "  '" << c << "': из " << v
                     << " идём в существующую " << nxt[v][idx] << "\n";
            }
            v = nxt[v][idx];
        }
        out[v].push_back(k);
        cerr << "  конец сегмента: вершина " << v
             << " помечена терминальной для #" << k << "\n";
    }

    vector<vector<int>> raw(nodes, vector<int>(5, -1));
    for (int v = 0; v < nodes; ++v)
        for (int c = 0; c < 5; ++c)
            raw[v][c] = nxt[v][c];

    cerr << "\nЭТАП 3: ПОСТРОЕНИЕ АВТОМАТА\n";

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
                cerr << "  '" << idChar(i) << "': ссылка в " << nxt[v][i] << "\n";
            }
        }
        cerr << "  очередь: " << qToString(q) << "\n";
    }

    cerr << "\nЭТАП 4: ОПИСАНИЕ ВЕРШИН\n";
    for (int v = 0; v < nodes; ++v) {
        cerr << "Вершина " << v << ": fail = " << fail[v]
             << ", outLink = " << outLink[v] << ", терминальные:";
        if (out[v].empty()) cerr << " нет";
        else for (int k : out[v]) cerr << " #" << k;
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

    cerr << "\nЭТАП 5: ПОИСК В ТЕКСТЕ\n";

    int total = (int)segs.size();
    vector<int> cnt(n - m + 1, 0);

    int state = 0;
    for (int i = 0; i < n; ++i) {
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
            for (int k : out[u]) {
                int segLen = (int)segs[k].second.size();
                int start = i - segLen + 1 - segs[k].first;
                if (start >= 0 && start < (int)cnt.size()) {
                    cnt[start]++;
                    cerr << "  найден сегмент #" << k
                         << " (offset " << segs[k].first
                         << ") -> возможный старт " << (start + 1)
                         << ", cnt[" << start << "] = " << cnt[start] << "\n";
                }
            }
            u = outLink[u];
        }
    }

    cerr << "\nРЕЗУЛЬТАТ\n";
    cerr << "Всего сегментов: " << total << "\n";
    cerr << "Позиции, где совпали все сегменты:\n";

    cout << "--- Позиции вхождений ---\n";
    for (int i = 0; i < (int)cnt.size(); ++i) {
        if (cnt[i] == total) {
            cerr << "  " << (i + 1) << " (cnt = " << cnt[i] << ")\n";
            cout << (i + 1) << '\n';
        }
    }

    return 0;
}