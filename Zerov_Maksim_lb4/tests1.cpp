#include <gtest/gtest.h>
#include <bits/stdc++.h>
using namespace std;

vector<int> solveKMP(const string& P, const string& T) {
    int m = (int)P.size();
    int n = (int)T.size();

    vector<int> pi(m, 0);
    for (int i = 1; i < m; ++i) {
        int j = pi[i - 1];
        while (j > 0 && P[i] != P[j])
            j = pi[j - 1];
        if (P[i] == P[j]) ++j;
        pi[i] = j;
    }

    vector<int> res;
    int j = 0;
    for (int i = 0; i < n; ++i) {
        while (j > 0 && T[i] != P[j])
            j = pi[j - 1];
        if (T[i] == P[j]) ++j;
        if (j == m) {
            res.push_back(i - m + 1);
            j = pi[j - 1];
        }
    }
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

class KMP1Test : public ::testing::Test {
protected:
    void TearDown() override { cout << "\n"; }
};

TEST_F(KMP1Test, SingleMatch) {
    cout << "[1] Одно вхождение\n\n";

    string P = "abc";
    string T = "xxabcyy";

    cout << "входные данные:\n";
    cout << "  образец: \"" << P << "\"\n";
    cout << "  текст:   \"" << T << "\"\n\n";

    auto res = solveKMP(P, T);
    cout << "результат: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ((int)res.size(), 1);
    EXPECT_EQ(res[0], 2);
}

TEST_F(KMP1Test, MultipleMatches) {
    cout << "[2] Несколько вхождений\n\n";

    string P = "aba";
    string T = "abacaba";

    cout << "входные данные:\n";
    cout << "  образец: \"" << P << "\"\n";
    cout << "  текст:   \"" << T << "\"\n\n";

    auto res = solveKMP(P, T);
    cout << "результат: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ((int)res.size(), 2);
    EXPECT_EQ(res[0], 0);
    EXPECT_EQ(res[1], 4);
}

TEST_F(KMP1Test, NoMatches) {
    cout << "[3] Вхождений нет\n\n";

    string P = "xyz";
    string T = "abcabcabc";

    cout << "входные данные:\n";
    cout << "  образец: \"" << P << "\"\n";
    cout << "  текст:   \"" << T << "\"\n\n";

    auto res = solveKMP(P, T);
    cout << "результат: " << vecToStr(res) << "\n";
    cout << "  OK (вхождений не найдено)";

    EXPECT_TRUE(res.empty());
}

TEST_F(KMP1Test, Overlapping) {
    cout << "[4] Перекрывающиеся вхождения\n\n";

    string P = "aa";
    string T = "aaaaa";

    cout << "входные данные:\n";
    cout << "  образец: \"" << P << "\"\n";
    cout << "  текст:   \"" << T << "\"\n\n";

    auto res = solveKMP(P, T);
    cout << "результат: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ((int)res.size(), 4);
    EXPECT_EQ(res[0], 0);
    EXPECT_EQ(res[1], 1);
    EXPECT_EQ(res[2], 2);
    EXPECT_EQ(res[3], 3);
}

TEST_F(KMP1Test, PatternEqualsText) {
    cout << "[5] Образец совпадает с текстом\n\n";

    string P = "abcabc";
    string T = "abcabc";

    cout << "входные данные:\n";
    cout << "  образец: \"" << P << "\"\n";
    cout << "  текст:   \"" << T << "\"\n\n";

    auto res = solveKMP(P, T);
    cout << "результат: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ((int)res.size(), 1);
    EXPECT_EQ(res[0], 0);
}

TEST_F(KMP1Test, PatternLongerThanText) {
    cout << "[6] Образец длиннее текста\n\n";

    string P = "abcdef";
    string T = "abc";

    cout << "входные данные:\n";
    cout << "  образец: \"" << P << "\"\n";
    cout << "  текст:   \"" << T << "\"\n\n";

    auto res = solveKMP(P, T);
    cout << "результат: " << vecToStr(res) << "\n";
    cout << "  OK (вхождений нет)";

    EXPECT_TRUE(res.empty());
}

TEST_F(KMP1Test, RepeatingPattern) {
    cout << "[7] Повторяющийся образец\n\n";

    string P = "abab";
    string T = "abababab";

    cout << "входные данные:\n";
    cout << "  образец: \"" << P << "\"\n";
    cout << "  текст:   \"" << T << "\"\n\n";

    auto res = solveKMP(P, T);
    cout << "результат: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ((int)res.size(), 3);
    EXPECT_EQ(res[0], 0);
    EXPECT_EQ(res[1], 2);
    EXPECT_EQ(res[2], 4);
}

TEST_F(KMP1Test, PrefixSuffix) {
    cout << "[8] Префикс-суффикс в образце\n\n";

    string P = "aabaa";
    string T = "aabaabaabaa";

    cout << "входные данные:\n";
    cout << "  образец: \"" << P << "\"\n";
    cout << "  текст:   \"" << T << "\"\n\n";

    auto res = solveKMP(P, T);
    cout << "результат: " << vecToStr(res) << "\n";
    cout << "  OK (найдено " << res.size() << " вхождений)";

    ASSERT_EQ((int)res.size(), 3);
    EXPECT_EQ(res[0], 0);
    EXPECT_EQ(res[1], 3);
    EXPECT_EQ(res[2], 6);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    cout << "тесты: КМП — поиск всех вхождений\n";

    int result = RUN_ALL_TESTS();

    cout << "\nрезультаты\n";
    if (result == 0)
        cout << "  пройдено: 8\n  не пройдено: 0\n";
    else
        cout << "  обнаружены ошибки. См. лог выше.\n";

    return result;
}