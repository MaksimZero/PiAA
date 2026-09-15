#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string A, B;
    cin >> A >> B;

    cerr << "\nввод\n";
    cerr << "A = \"" << A << "\" (длина " << A.size() << ")\n";
    cerr << "B = \"" << B << "\" (длина " << B.size() << ")\n";

    // Проверка длин
    if (A.size() != B.size()) {
        cerr << "\nдлины не совпадают — B не может быть циклическим сдвигом A\n";
        cout << -1 << '\n';
        return 0;
    }

    int n = (int)A.size();
    if (n == 0) {
        cerr << "\nобе строки пустые — тривиальный случай, сдвиг = 0\n";
        cout << 0 << '\n';
        return 0;
    }

    // B является циклическим сдвигом A тогда и только тогда
    // когда B встречается как подстрока в A + A
    cerr << "ищем B как подстроку в A+A\n";
    cerr << "A+A = \"" << A + A << "\"\n";

    // Построение префикс-функции для B
    cerr << "\nпостроение префикс-функции B\n";

    vector<int> pi(n, 0);
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        cerr << "i = " << i << ", начинаем с j = pi[" << (i - 1) << "] = " << j << "\n";

        while (j > 0 && B[i] != B[j]) {
            cerr << "  '" << B[i] << "' != '" << B[j]
                 << "', откат j: " << j << " -> " << pi[j - 1] << "\n";
            j = pi[j - 1];
        }

        if (B[i] == B[j]) {
            ++j;
            cerr << "  '" << B[i] << "' == '" << B[j - 1] << "', j = " << j << "\n";
        } else {
            cerr << "  j = 0\n";
        }

        pi[i] = j;
        cerr << "  pi[" << i << "] = " << pi[i] << "\n";
    }

    cerr << "\nпрефикс-функция B:\n  i    :";
    for (int i = 0; i < n; ++i) cerr << setw(4) << i;
    cerr << "\n  B[i] :";
    for (int i = 0; i < n; ++i) cerr << setw(4) << B[i];
    cerr << "\n  pi[i]:";
    for (int i = 0; i < n; ++i) cerr << setw(4) << pi[i];
    cerr << "\n";

    // Поиск B в A+A.
    cerr << "\nпоиск B в A+A\n";

    string S = A + A;
    int j = 0;
    for (int i = 0; i < (int)S.size(); ++i) {
        cerr << "\nсимвол #" << i << " = '" << S[i] << "' (j = " << j << ")\n";

        while (j > 0 && S[i] != B[j]) {
            cerr << "  '" << S[i] << "' != '" << B[j]
                 << "', откат j: " << j << " -> " << pi[j - 1] << "\n";
            j = pi[j - 1];
        }

        if (S[i] == B[j]) {
            ++j;
            cerr << "  '" << S[i] << "' == '" << B[j - 1] << "' -> j = " << j << "\n";
        } else {
            cerr << "  j = 0, совпадений нет\n";
        }

        if (j == n) {
            int shift = i - n + 1;
            cerr << "\nнайдено совпадение. B начинается с позиции " << shift
                 << " в A+A\n";
            cerr << "это означает: B = циклический сдвиг A на " << shift
                 << " позиций\n";
            cout << shift << '\n';
            return 0;
        }
    }

    cerr << "\nB не найден в A+A — B не является циклическим сдвигом A\n";
    cout << -1 << '\n';
    return 0;
}