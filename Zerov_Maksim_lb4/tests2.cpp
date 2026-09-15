#include <gtest/gtest.h>
#include <bits/stdc++.h>
using namespace std;

// Возвращает сдвиг или -1
int solveShift(const string& A, const string& B) {
    if (A.size() != B.size()) return -1;
    int n = (int)A.size();
    if (n == 0) return 0;

    vector<int> pi(n, 0);
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j > 0 && B[i] != B[j])
            j = pi[j - 1];
        if (B[i] == B[j]) ++j;
        pi[i] = j;
    }

    string S = A + A;
    int j = 0;
    for (int i = 0; i < (int)S.size(); ++i) {
        while (j > 0 && S[i] != B[j])
            j = pi[j - 1];
        if (S[i] == B[j]) ++j;
        if (j == n) {
            return i - n + 1;
        }
    }
    return -1;
}

class KMP2Test : public ::testing::Test {
protected:
    void TearDown() override { cout << "\n"; }
};

TEST_F(KMP2Test, SimpleShift) {
    cout << "[1] Простой сдвиг\n\n";

    string A = "abcab";
    string B = "cabab";

    cout << "входные данные:\n";
    cout << "  A = \"" << A << "\"\n";
    cout << "  B = \"" << B << "\"\n\n";

    int res = solveShift(A, B);
    cout << "результат: " << res << "\n";
    cout << "  OK (сдвиг = " << res << ")";

    EXPECT_EQ(res, 2);
}

TEST_F(KMP2Test, NoShift) {
    cout << "[2] Не циклический сдвиг\n\n";

    string A = "abcde";
    string B = "abced";

    cout << "входные данные:\n";
    cout << "  A = \"" << A << "\"\n";
    cout << "  B = \"" << B << "\"\n\n";

    int res = solveShift(A, B);
    cout << "результат: " << res << "\n";
    cout << "  OK (не является циклическим сдвигом)";

    EXPECT_EQ(res, -1);
}

TEST_F(KMP2Test, DifferentLengths) {
    cout << "[3] Разные длины\n\n";

    string A = "abc";
    string B = "ab";

    cout << "входные данные:\n";
    cout << "  A = \"" << A << "\"\n";
    cout << "  B = \"" << B << "\"\n\n";

    int res = solveShift(A, B);
    cout << "результат: " << res << "\n";
    cout << "  OK (длины разные, сдвига быть не может)";

    EXPECT_EQ(res, -1);
}

TEST_F(KMP2Test, SameStrings) {
    cout << "[4] Одинаковые строки\n\n";

    string A = "abcdef";
    string B = "abcdef";

    cout << "входные данные:\n";
    cout << "  A = \"" << A << "\"\n";
    cout << "  B = \"" << B << "\"\n\n";

    int res = solveShift(A, B);
    cout << "результат: " << res << "\n";
    cout << "  OK (сдвиг = 0)";

    EXPECT_EQ(res, 0);
}

TEST_F(KMP2Test, FullCycle) {
    cout << "[5] Полный цикл\n\n";

    string A = "abcde";
    string B = "eabcd";

    cout << "входные данные:\n";
    cout << "  A = \"" << A << "\"\n";
    cout << "  B = \"" << B << "\"\n\n";

    int res = solveShift(A, B);
    cout << "результат: " << res << "\n";
    cout << "  OK (сдвиг = " << res << ")";

    EXPECT_EQ(res, 4);
}

TEST_F(KMP2Test, RepeatingChars) {
    cout << "[6] Повторяющиеся символы\n\n";

    string A = "aaaa";
    string B = "aaaa";

    cout << "входные данные:\n";
    cout << "  A = \"" << A << "\"\n";
    cout << "  B = \"" << B << "\"\n\n";

    int res = solveShift(A, B);
    cout << "результат: " << res << "\n";
    cout << "  OK (сдвиг = 0)";

    EXPECT_EQ(res, 0);
}

TEST_F(KMP2Test, SingleChar) {
    cout << "[7] Один символ\n\n";

    string A = "a";
    string B = "a";

    cout << "входные данные:\n";
    cout << "  A = \"" << A << "\"\n";
    cout << "  B = \"" << B << "\"\n\n";

    int res = solveShift(A, B);
    cout << "результат: " << res << "\n";
    cout << "  OK (сдвиг = 0)";

    EXPECT_EQ(res, 0);
}

TEST_F(KMP2Test, RotationNotMatch) {
    cout << "[8] Похоже на сдвиг, но не он\n\n";

    string A = "abcabc";
    string B = "abcacb";

    cout << "входные данные:\n";
    cout << "  A = \"" << A << "\"\n";
    cout << "  B = \"" << B << "\"\n\n";

    int res = solveShift(A, B);
    cout << "результат: " << res << "\n";
    cout << "  OK (не является циклическим сдвигом)";

    EXPECT_EQ(res, -1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    cout << "тесты: КМП — циклический сдвиг\n";

    int result = RUN_ALL_TESTS();

    cout << "\nрезультаты\n";
    if (result == 0)
        cout << "  пройдено: 8\n  не пройдено: 0\n";
    else
        cout << "  обнаружены ошибки. См. лог выше.\n";

    return result;
}