#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

int main() {
    int cost_replace, cost_insert, cost_delete, cost_replace2;
    cin >> cost_replace >> cost_insert >> cost_delete >> cost_replace2;
    
    string A, B;
    cin >> A >> B;
    
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
    
    cout << dp[n][m] << endl;
    
    return 0;
}