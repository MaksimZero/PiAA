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
    
    cout << "A=" << A << " B=" << B << "\n";
    cout << "стоимости: замена=" << cost_replace << " вставка=" << cost_insert 
         << " удаление=" << cost_delete << " замена2=" << cost_replace2 << "\n\n";
    
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            if (dp[i][j] == INF) {
                cout << "(" << i << "," << j << ") недостижимо\n";
                continue;
            }
            cout << "(" << i << "," << j << ") стоимость=" << dp[i][j] << "\n";
            
            if (i < n && j < m && A[i] == B[j]) {
                int ni = i+1, nj = j+1;
                int nc = dp[i][j];
                if (nc < dp[ni][nj]) {
                    dp[ni][nj] = nc;
                    cout << "  совпадение -> (" << ni << "," << nj << ")=" << nc << "\n";
                }
            }
            
            if (i < n && j < m && A[i] != B[j]) {
                int ni = i+1, nj = j+1;
                int nc = dp[i][j] + cost_replace;
                if (nc < dp[ni][nj]) {
                    dp[ni][nj] = nc;
                    cout << "  замена -> (" << ni << "," << nj << ")=" << nc << "\n";
                }
            }
            
            if (i < n) {
                int ni = i+1, nj = j;
                int nc = dp[i][j] + cost_delete;
                if (nc < dp[ni][nj]) {
                    dp[ni][nj] = nc;
                    cout << "  удаление -> (" << ni << "," << nj << ")=" << nc << "\n";
                }
            }
            
            if (j < m) {
                int ni = i, nj = j+1;
                int nc = dp[i][j] + cost_insert;
                if (nc < dp[ni][nj]) {
                    dp[ni][nj] = nc;
                    cout << "  вставка -> (" << ni << "," << nj << ")=" << nc << "\n";
                }
            }
            
            if (i + 2 <= n && j + 1 <= m) {
                int ni = i+2, nj = j+1;
                int nc = dp[i][j] + cost_replace2;
                if (nc < dp[ni][nj]) {
                    dp[ni][nj] = nc;
                    cout << "  замена двух на один -> (" << ni << "," << nj << ")=" << nc << "\n";
                }
            }
            cout << "\n";
        }
    }
    
    cout << "\nТаблица DP:\n";
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            if (dp[i][j] >= INF/2) cout << "INF ";
            else cout << dp[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\nОтвет: " << dp[n][m] << "\n";
    
    return 0;
}

// редакционное предписание
// как оптимизировать алгоритм по памяти.