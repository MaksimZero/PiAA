#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string P, T;
    cin >> P >> T;

    int m = (int)P.size();
    int n = (int)T.size();

    cerr << "\nввод\n";
    cerr << "образец P = \"" << P << "\" (длина " << m << ")\n";
    cerr << "текст   T = \"" << T << "\" (длина " << n << ")\n";

    // Построение префикс-функции образца
    // pi[i] — длина наибольшего собственного суффикса P[0..i]
    // совпадающего с префиксом P
    cerr << "\nпостроение префикс-функции образца\n";
    cerr << "pi[i] — длина наибольшего префикса P, совпадающего с суффиксом P[0..i]\n\n";

    vector<int> pi(m, 0);
    for (int i = 1; i < m; ++i) {
        int j = pi[i - 1];
        cerr << "i = " << i << ", начинаем с j = pi[" << (i - 1) << "] = " << j << "\n";

        while (j > 0 && P[i] != P[j]) {
            cerr << "  P[" << i << "]='" << P[i] << "' != P[" << j << "]='" << P[j]
                 << "', откат j: " << j << " -> " << pi[j - 1] << "\n";
            j = pi[j - 1];
        }

        if (P[i] == P[j]) {
            ++j;
            cerr << "  P[" << i << "]='" << P[i] << "' == P[" << j - 1 << "]='"
                 << P[j - 1] << "', j = " << j << "\n";
        } else {
            cerr << "  j = 0, символы не совпали\n";
        }

        pi[i] = j;
        cerr << "  pi[" << i << "] = " << pi[i] << "\n";
    }

    cerr << "\nитоговая префикс-функция:\n  i    :";
    for (int i = 0; i < m; ++i) cerr << setw(4) << i;
    cerr << "\n  P[i] :";
    for (int i = 0; i < m; ++i) cerr << setw(4) << P[i];
    cerr << "\n  pi[i]:";
    for (int i = 0; i < m; ++i) cerr << setw(4) << pi[i];
    cerr << "\n";

    // Поиск в тексте
    // j — текущая длина совпавшего префикса образца
    cerr << "\nпоиск в тексте\n";

    string out;
    bool found = false;
    int j = 0;

    for (int i = 0; i < n; ++i) {
        cerr << "\nсимвол #" << i << " = '" << T[i] << "' (j = " << j << ")\n";

        while (j > 0 && T[i] != P[j]) {
            cerr << "  '" << T[i] << "' != P[" << j << "]='" << P[j]
                 << "', откат j: " << j << " -> " << pi[j - 1] << "\n";
            j = pi[j - 1];
        }

        if (T[i] == P[j]) {
            ++j;
            cerr << "  '" << T[i] << "' == P[" << j - 1 << "] -> j = " << j << "\n";
        } else {
            cerr << "  j = 0, совпадений нет\n";
        }

        if (j == m) {
            int pos = i - m + 1;
            cerr << "  найдено вхождение на позиции " << pos << "\n";

            if (found) out += ',';
            out += to_string(pos);
            found = true;

            j = pi[j - 1];
            cerr << "  откат j -> " << j << " (по pi[" << (m - 1) << "])\n";
        }
    }

    cerr << "\nрезультат\n";
    if (!found) {
        cerr << "вхождений нет\n";
        cout << -1 << '\n';
    } else {
        cerr << "позиции вхождений: " << out << "\n";
        cout << out << '\n';
    }

    return 0;
}