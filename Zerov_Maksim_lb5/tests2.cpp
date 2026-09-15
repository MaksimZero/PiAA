#include <gtest/gtest.h>
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

vector<int> solveWild(const string& T, const string& P, char wild) {
    int n = (int)T.size();
    int m = (int)P.size();
    if (m > n) return {};

    vector<pair<int,string>> segs;
    int s = -1;
    for (int i = 0; i < m; ++i) {
        if (P[i] == wild) {
            if (s != -1) { segs.push_back({s, P.substr(s, i - s)}); s = -1; }
        } else if (s == -1) {
            s = i;
        }
    }
    if (s != -1) segs.push_back({s, P.substr(s, m - s)});

    vector<vector<int>> nxt(1, vector<int>(5, -1));
    vector<int> fail(1, 0);
    vector<int> outLink(1, -1);
    vector<vector<int>> out(1);

    for (int k = 0; k < (int)segs.size(); ++k) {
        int v = 0;
        for (char c : segs[k].second) {
            int idx = mapChar(c);
            if (idx == -1) continue;
            if (nxt[v][idx] == -1) {
                nxt[v][idx] = (int)nxt.size();
                nxt.push_back(vector<int>(5, -1));
                fail.push_back(0);
                outLink.push_back(-1);
                out.push_back({});
            }
            v = nxt[v][idx];
        }
        out[v].push_back(k);
    }

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
        int v = q.front(); q.pop();
        int f = fail[v];
        if (!out[f].empty()) outLink[v] = f;
        else outLink[v] = outLink[f];

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

    int total = (int)segs.size();
    vector<int> cnt(n - m + 1, 0);
    int state = 0;
    for (int i = 0; i < n; ++i) {
        int idx = mapChar(T[i]);
        if (idx == -1) { state = 0; continue; }
        state = nxt[state][idx];

        int u = state;
        while (u != -1) {
            for (int k : out[u]) {
                int segLen = (int)segs[k].second.size();
                int start = i - segLen + 1 - segs[k].first;
                if (start >= 0 && start < (int)cnt.size()) cnt[start]++;
            }
            u = outLink[u];
        }
    }

    vector<int> res;
    for (int i = 0; i < (int)cnt.size(); ++i)
        if (cnt[i] == total) res.push_back(i + 1);
    return res;
}

string vecToStr(const vector<int>& v) {
    if (v.empty()) return "(пусто)";
    string s;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) s += ", ";
        s += to_string(v[i]);
    }
    return s;
}

class WildTest : public ::testing::Test {
protected:
    void TearDown() override { cout << "\n"; }
};

TEST_F(WildTest, ExampleFromTask) {
    cout << "[1] Пример из условия\n\n";

    string T = "ACTANCA";
    string P = "A$$A$";
    char wild = '$';

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст:    \"" << T << "\"\n";
    cout << "  Образец:  \"" << P << "\"\n";
    cout << "  Джокер:   '" << wild << "'\n\n";

    auto res = solveWild(T, P, wild);
    cout << "РЕЗУЛЬТАТ: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ(res.size(), 1u);
    EXPECT_EQ(res[0], 1);
}

TEST_F(WildTest, NoWildcard) {
    cout << "[2] Образец без джокеров (обычный поиск)\n\n";

    string T = "ACACACA";
    string P = "ACA";
    char wild = '?';

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст:    \"" << T << "\"\n";
    cout << "  Образец:  \"" << P << "\"\n";
    cout << "  Джокер:   '" << wild << "'\n\n";

    auto res = solveWild(T, P, wild);
    cout << "РЕЗУЛЬТАТ: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ(res.size(), 3u);
    EXPECT_EQ(res[0], 1);
    EXPECT_EQ(res[1], 3);
    EXPECT_EQ(res[2], 5);
}

TEST_F(WildTest, WildcardAtStart) {
    cout << "[3] Джокер в начале образца\n\n";

    string T = "NACG";
    string P = "?ACG";
    char wild = '?';

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст:    \"" << T << "\"\n";
    cout << "  Образец:  \"" << P << "\"\n";
    cout << "  Джокер:   '" << wild << "'\n\n";

    auto res = solveWild(T, P, wild);
    cout << "РЕЗУЛЬТАТ: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ(res.size(), 1u);
    EXPECT_EQ(res[0], 1);
}

TEST_F(WildTest, WildcardAtEnd) {
    cout << "[4] Джокер в конце образца\n\n";

    string T = "ACGN";
    string P = "ACG?";
    char wild = '?';

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст:    \"" << T << "\"\n";
    cout << "  Образец:  \"" << P << "\"\n";
    cout << "  Джокер:   '" << wild << "'\n\n";

    auto res = solveWild(T, P, wild);
    cout << "РЕЗУЛЬТАТ: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ(res.size(), 1u);
    EXPECT_EQ(res[0], 1);
}

TEST_F(WildTest, MultipleWildcards) {
    cout << "[5] Несколько джокеров\n\n";

    string T = "ACGACGACG";
    string P = "A?G";
    char wild = '?';

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст:    \"" << T << "\"\n";
    cout << "  Образец:  \"" << P << "\"\n";
    cout << "  Джокер:   '" << wild << "'\n\n";

    auto res = solveWild(T, P, wild);
    cout << "РЕЗУЛЬТАТ: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ(res.size(), 3u);
}

TEST_F(WildTest, NoMatches) {
    cout << "[6] Совпадений нет\n\n";

    string T = "AAAA";
    string P = "C?C";
    char wild = '?';

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст:    \"" << T << "\"\n";
    cout << "  Образец:  \"" << P << "\"\n";
    cout << "  Джокер:   '" << wild << "'\n\n";

    auto res = solveWild(T, P, wild);
    cout << "РЕЗУЛЬТАТ: " << vecToStr(res) << "\n";
    cout << "  OK (вхождений не найдено)";

    EXPECT_TRUE(res.empty());
}

TEST_F(WildTest, PatternLongerThanText) {
    cout << "[7] Образец длиннее текста\n\n";

    string T = "ACG";
    string P = "ACGT";
    char wild = '?';

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст:    \"" << T << "\"\n";
    cout << "  Образец:  \"" << P << "\"\n";
    cout << "  Джокер:   '" << wild << "'\n\n";

    auto res = solveWild(T, P, wild);
    cout << "РЕЗУЛЬТАТ: " << vecToStr(res) << "\n";
    cout << "  OK (вхождений нет)";

    EXPECT_TRUE(res.empty());
}

TEST_F(WildTest, OnlyOneSegment) {
    cout << "[8] Один сегмент (джокеры только в конце)\n\n";

    string T = "ACGACG";
    string P = "ACG??";
    char wild = '?';

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст:    \"" << T << "\"\n";
    cout << "  Образец:  \"" << P << "\"\n";
    cout << "  Джокер:   '" << wild << "'\n\n";

    auto res = solveWild(T, P, wild);
    cout << "РЕЗУЛЬТАТ: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ(res.size(), 1u);
    EXPECT_EQ(res[0], 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    cout << "ТЕСТЫ: Поиск образца с джокером (Ахо-Корасик)\n";

    int result = RUN_ALL_TESTS();

    cout << "\nРЕЗУЛЬТАТЫ\n";
    if (result == 0)
        cout << "  Пройдено: 8\n  Не пройдено: 0\n";
    else
        cout << "  Обнаружены ошибки. См. лог выше.\n";

    return result;
}