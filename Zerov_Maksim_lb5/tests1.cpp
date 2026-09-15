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

vector<pair<int,int>> solveAC(const string& T,
                              const vector<string>& patterns) {
    int n = (int)T.size();
    int k = (int)patterns.size();

    vector<vector<int>> nxt(1, vector<int>(5, -1));
    vector<int> fail(1, 0);
    vector<int> outLink(1, -1);
    vector<vector<int>> out(1);
    vector<int> len(k + 1);
    int nodes = 1;

    for (int i = 0; i < k; ++i) {
        int v = 0;
        for (char c : patterns[i]) {
            int idx = mapChar(c);
            if (idx == -1) continue;
            if (nxt[v][idx] == -1) {
                nxt[v][idx] = nodes++;
                nxt.push_back(vector<int>(5, -1));
                fail.push_back(0);
                outLink.push_back(-1);
                out.push_back({});
            }
            v = nxt[v][idx];
        }
        out[v].push_back(i + 1);
        len[i + 1] = (int)patterns[i].size();
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

    vector<pair<int,int>> res;
    int state = 0;
    for (int i = 0; i < n; ++i) {
        int idx = mapChar(T[i]);
        if (idx == -1) { state = 0; continue; }
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
    return res;
}

string pairsToStr(const vector<pair<int,int>>& v) {
    if (v.empty()) return "(пусто)";
    string s;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) s += ", ";
        s += "(" + to_string(v[i].first) + "," + to_string(v[i].second) + ")";
    }
    return s;
}

class ACTest : public ::testing::Test {
protected:
    void TearDown() override { cout << "\n"; }
};

TEST_F(ACTest, SinglePattern) {
    cout << "[1] Один образец в тексте\n\n";

    string T = "ACACACA";
    vector<string> P = {"ACA"};

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст:    \"" << T << "\"\n";
    cout << "  Образцов: 1\n";
    cout << "  Образец #1: \"" << P[0] << "\"\n\n";

    auto res = solveAC(T, P);
    cout << "РЕЗУЛЬТАТ: " << pairsToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ((int)res.size(), 3);
    EXPECT_EQ(res[0].first, 1);
    EXPECT_EQ(res[1].first, 3);
    EXPECT_EQ(res[2].first, 5);
}

TEST_F(ACTest, MultiplePatterns) {
    cout << "[2] Несколько образцов\n\n";

    string T = "TAGCTN";
    vector<string> P = {"TA", "TAG", "GC", "CT"};

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст: \"" << T << "\"\n";
    cout << "  Образцы:\n";
    for (size_t i = 0; i < P.size(); ++i)
        cout << "    #" << (i + 1) << ": \"" << P[i] << "\"\n";
    cout << "\n";

    auto res = solveAC(T, P);
    cout << "РЕЗУЛЬТАТ: " << pairsToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    EXPECT_EQ((int)res.size(), 4);
}

TEST_F(ACTest, NoMatches) {
    cout << "[3] Образец не найден\n\n";

    string T = "AAAA";
    vector<string> P = {"CCC"};

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст:    \"" << T << "\"\n";
    cout << "  Образцов: 1\n";
    cout << "  Образец #1: \"" << P[0] << "\"\n\n";

    auto res = solveAC(T, P);
    cout << "РЕЗУЛЬТАТ: " << pairsToStr(res) << "\n";
    cout << "  OK (вхождений не найдено)";

    EXPECT_TRUE(res.empty());
}

TEST_F(ACTest, OverlappingPatterns) {
    cout << "[4] Перекрывающиеся вхождения\n\n";

    string T = "AAAAA";
    vector<string> P = {"AA", "AAA"};

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст: \"" << T << "\"\n";
    cout << "  Образцы: \"" << P[0] << "\", \"" << P[1] << "\"\n\n";

    auto res = solveAC(T, P);
    cout << "РЕЗУЛЬТАТ: " << pairsToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    EXPECT_EQ((int)res.size(), 7);
}

TEST_F(ACTest, SuffixPattern) {
    cout << "[5] Один образец — суффикс другого (проверка outLink)\n\n";

    string T = "TAGT";
    vector<string> P = {"TAGT", "AGT", "GT", "T"};

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст: \"" << T << "\"\n";
    cout << "  Образцы:\n";
    for (size_t i = 0; i < P.size(); ++i)
        cout << "    #" << (i + 1) << ": \"" << P[i] << "\"\n";
    cout << "\n";

    auto res = solveAC(T, P);
    cout << "РЕЗУЛЬТАТ: " << pairsToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    EXPECT_EQ((int)res.size(), 5);
}

TEST_F(ACTest, EmptyText) {
    cout << "[6] Пустой текст\n\n";

    string T = "";
    vector<string> P = {"ACG"};

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст: пустой\n";
    cout << "  Образец #1: \"" << P[0] << "\"\n\n";

    auto res = solveAC(T, P);
    cout << "РЕЗУЛЬТАТ: " << pairsToStr(res) << "\n";
    cout << "  OK (вхождений нет)";

    EXPECT_TRUE(res.empty());
}

TEST_F(ACTest, PatternEqualsText) {
    cout << "[7] Образец совпадает с текстом\n\n";

    string T = "ACG";
    vector<string> P = {"ACG"};

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст: \"" << T << "\"\n";
    cout << "  Образец #1: \"" << P[0] << "\"\n\n";

    auto res = solveAC(T, P);
    cout << "РЕЗУЛЬТАТ: " << pairsToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ((int)res.size(), 1);
    EXPECT_EQ(res[0].first, 1);
    EXPECT_EQ(res[0].second, 1);
}

TEST_F(ACTest, MultipleSuffixes) {
    cout << "[8] Цепочка суффиксов A, AA, AAA, AAAA в AAAA\n\n";

    string T = "AAAA";
    vector<string> P = {"A", "AA", "AAA", "AAAA"};

    cout << "ВХОДНЫЕ ДАННЫЕ:\n";
    cout << "  Текст: \"" << T << "\"\n";
    cout << "  Образцы:\n";
    for (size_t i = 0; i < P.size(); ++i)
        cout << "    #" << (i + 1) << ": \"" << P[i] << "\"\n";
    cout << "\n";

    auto res = solveAC(T, P);
    cout << "РЕЗУЛЬТАТ: " << pairsToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    EXPECT_EQ((int)res.size(), 10);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    cout << "ТЕСТЫ: Алгоритм Ахо-Корасик (множественный поиск)\n";

    int result = RUN_ALL_TESTS();

    cout << "\nРЕЗУЛЬТАТЫ\n";
    if (result == 0)
        cout << "  Пройдено: 8\n  Не пройдено: 0\n";
    else
        cout << "  Обнаружены ошибки. См. лог выше.\n";

    return result;
}