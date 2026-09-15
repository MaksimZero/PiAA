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

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string T;
    cin >> T;
    int n;
    cin >> n;
    vector<string> patterns(n);
    for (int i = 0; i < n; ++i) cin >> patterns[i];

    const int MAXN = 250000;
    vector<vector<int>> nxt(MAXN, vector<int>(5, -1));
    vector<int> fail(MAXN, 0);
    vector<int> outLink(MAXN, -1);
    vector<vector<int>> out(MAXN);
    vector<int> len(n + 1);
    int nodes = 1;

    for (int i = 0; i < n; ++i) {
        int v = 0;
        for (char c : patterns[i]) {
            int idx = mapChar(c);
            if (idx == -1) continue;
            if (nxt[v][idx] == -1) nxt[v][idx] = nodes++;
            v = nxt[v][idx];
        }
        out[v].push_back(i + 1);
        len[i + 1] = (int)patterns[i].size();
    }

    vector<array<int, 5>> raw(nodes);
    for (int v = 0; v < nodes; ++v)
        for (int c = 0; c < 5; ++c)
            raw[v][c] = nxt[v][c];

    queue<int> q;
    for (int i = 0; i < 5; ++i) {
        if (nxt[0][i] != -1) {
            fail[nxt[0][i]] = 0;
            q.push(nxt[0][i]);
        } else {
            nxt[0][i] = 0;
        }
    }

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        int f = fail[v];
        if (!out[f].empty())
            outLink[v] = f;
        else
            outLink[v] = outLink[f];

        for (int i = 0; i < 5; ++i) {
            if (nxt[v][i] != -1) {
                int u = nxt[v][i];
                fail[u] = nxt[fail[v]][i];
                q.push(u);
            } else {
                nxt[v][i] = nxt[fail[v]][i];
            }
        }
    }

    const string dotFile = "automaton.dot";
    {
        ofstream fout(dotFile);
        if (!fout) {
            cerr << "Не удалось открыть " << dotFile << " для записи\n";
            return 1;
        }

        fout << "digraph Automaton {\n";
        fout << "  rankdir=LR;\n";
        fout << "  node [shape=circle];\n";
        fout << "  start [shape=point];\n";
        fout << "  start -> 0;\n";

        // Узлы
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

    vector<pair<int, int>> res;
    int state = 0;
    for (int i = 0; i < (int)T.size(); ++i) {
        int idx = mapChar(T[i]);
        if (idx == -1) {
            state = 0;
            continue;
        }
        state = nxt[state][idx];

        int u = state;
        while (u != -1) {
            for (int p : out[u]) {
                int pos = i - len[p] + 1;
                res.push_back({pos + 1, p});
            }
            u = outLink[u];
        }
    }
    sort(res.begin(), res.end());

    const string resFile = "occurrences.txt";
    {
        ofstream fout(resFile);
        if (!fout) {
            cerr << "Не удалось открыть " << resFile << " для записи\n";
            return 1;
        }
        for (auto &pr : res) {
            fout << pr.first << ' ' << pr.second << '\n';
        }
    }

    cout << "Файлы сохранены:\n";

    cout << "--- Вхождения ---\n";
    for (auto &pr : res) {
        cout << pr.first << ' ' << pr.second << '\n';
    }

    return 0;
}
