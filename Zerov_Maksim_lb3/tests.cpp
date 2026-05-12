#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>
#include <iostream>

using namespace std;

int levenshtein_with_replace2(int cost_replace, int cost_insert, int cost_delete, int cost_replace2, 
                               const string& A, const string& B) {
    int n = A.length();
    int m = B.length();
    
    const int INF = 1e9;
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, INF));
    dp[0][0] = 0;
    
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            if (dp[i][j] == INF) continue;
            
            if (i < n && j < m && A[i] == B[j]) {
                dp[i + 1][j + 1] = min(dp[i + 1][j + 1], dp[i][j]);
            }
            
            if (i < n && j < m && A[i] != B[j]) {
                dp[i + 1][j + 1] = min(dp[i + 1][j + 1], dp[i][j] + cost_replace);
            }
            
            if (i < n) {
                dp[i + 1][j] = min(dp[i + 1][j], dp[i][j] + cost_delete);
            }
            
            if (j < m) {
                dp[i][j + 1] = min(dp[i][j + 1], dp[i][j] + cost_insert);
            }
            
            if (i + 2 <= n && j + 1 <= m) {
                dp[i + 2][j + 1] = min(dp[i + 2][j + 1], dp[i][j] + cost_replace2);
            }
        }
    }
    
    return dp[n][m];
}

class LevenshteinReplace2Test : public ::testing::Test {
protected:
    void SetUp() override {
        cout << "\n>>> Запуск теста: ";
    }
    
    void TearDown() override {
        cout << " <<<" << endl;
    }
};

TEST_F(LevenshteinReplace2Test, IdenticalStrings) {
    cout << "IdenticalStrings (совпадающие строки)";
    EXPECT_EQ(levenshtein_with_replace2(1, 1, 1, 1, "abc", "abc"), 0);
    EXPECT_EQ(levenshtein_with_replace2(5, 5, 5, 5, "hello", "hello"), 0);
    EXPECT_EQ(levenshtein_with_replace2(10, 1, 1, 100, "x", "x"), 0);
    cout << " ✓ OK";
}

TEST_F(LevenshteinReplace2Test, SingleReplacement) {
    cout << "SingleReplacement (одиночная замена)";
    EXPECT_EQ(levenshtein_with_replace2(3, 1, 1, 10, "cat", "bat"), 2);
    EXPECT_EQ(levenshtein_with_replace2(1, 10, 10, 10, "cat", "bat"), 1);
    cout << " ✓ OK";
}

TEST_F(LevenshteinReplace2Test, ReplaceTwoCharsWithOne) {
    cout << "ReplaceTwoCharsWithOne (замена 2 символов на 1)";
    EXPECT_EQ(levenshtein_with_replace2(3, 3, 3, 5, "abc", "c"), 6);
    EXPECT_EQ(levenshtein_with_replace2(1, 1, 1, 2, "ab", "c"), 2);
    EXPECT_EQ(levenshtein_with_replace2(3, 3, 3, 1, "ab", "c"), 1);
    cout << " ✓ OK";
}

TEST_F(LevenshteinReplace2Test, DeleteAndInsert) {
    cout << "DeleteAndInsert (удаление и вставка)";
    EXPECT_EQ(levenshtein_with_replace2(10, 1, 1, 100, "ab", "a"), 1);
    EXPECT_EQ(levenshtein_with_replace2(10, 1, 1, 100, "a", "ab"), 1);
    cout << " ✓ OK";
}

TEST_F(LevenshteinReplace2Test, Replace2Cheaper) {
    cout << "Replace2Cheaper (замена 2 на 1 выгоднее)";
    EXPECT_EQ(levenshtein_with_replace2(10, 10, 10, 1, "ab", "c"), 1);
    cout << " ✓ OK";
}

TEST_F(LevenshteinReplace2Test, ComplexExample) {
    cout << "ComplexExample (сложный пример)";
    EXPECT_EQ(levenshtein_with_replace2(1, 1, 1, 2, "abcd", "ad"), 2);
    EXPECT_EQ(levenshtein_with_replace2(1, 1, 1, 1, "abcd", "ad"), 2);
    cout << " ✓ OK";
}

TEST_F(LevenshteinReplace2Test, EmptyStrings) {
    cout << "EmptyStrings (пустые строки)";
    EXPECT_EQ(levenshtein_with_replace2(1, 1, 1, 1, "", ""), 0);
    EXPECT_EQ(levenshtein_with_replace2(1, 2, 3, 4, "", "abc"), 6);
    EXPECT_EQ(levenshtein_with_replace2(1, 2, 3, 4, "abc", ""), 9);
    cout << " ✓ OK";
}

TEST_F(LevenshteinReplace2Test, ReplaceTwoAtBeginning) {
    cout << "ReplaceTwoAtBeginning (замена 2 символов в начале)";
    EXPECT_EQ(levenshtein_with_replace2(10, 10, 10, 1, "abxyz", "cxyz"), 1);
    cout << " ✓ OK";
}

TEST_F(LevenshteinReplace2Test, ReplaceTwoAtEnd) {
    cout << "ReplaceTwoAtEnd (замена 2 символов в конце)";
    EXPECT_EQ(levenshtein_with_replace2(10, 10, 10, 1, "xyzab", "xyzc"), 1);
    cout << " ✓ OK";
}

TEST_F(LevenshteinReplace2Test, ReplaceTwoMultipleTimes) {
    cout << "ReplaceTwoMultipleTimes (многократная замена 2 на 1)";
    EXPECT_EQ(levenshtein_with_replace2(10, 10, 10, 1, "abcd", "xy"), 2);
    cout << " ✓ OK";
}

TEST_F(LevenshteinReplace2Test, Replace2VsDeleteInsert) {
    cout << "Replace2VsDeleteInsert (сравнение Replace2 с Delete+Insert)";
    EXPECT_EQ(levenshtein_with_replace2(100, 100, 1, 50, "ab", "c"), 50);
    cout << " ✓ OK";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    cout << "\n========================================" << endl;
    cout << "Запуск тестов для алгоритма Левенштейна" << endl;
    cout << "с операцией замены двух символов на один" << endl;
    cout << "========================================\n" << endl;
    
    int result = RUN_ALL_TESTS();
    
    cout << "\n========================================" << endl;
    if (result == 0) {
        cout << "✓ ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО" << endl;
    } else {
        cout << "✗ ОБНАРУЖЕНЫ ОШИБКИ" << endl;
    }
    cout << "========================================" << endl;
    
    return result;
}