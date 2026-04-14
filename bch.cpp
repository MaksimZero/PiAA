#include <vector>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <stack>

using namespace std;

struct PackedSquare {
    int data;
    
    PackedSquare() : data(0) {}
    PackedSquare(int x, int y, int size) {
        data = (x) | (y << 6) | (size << 12);
    }
    
    int getX() const { return data & 0x3F; }
    int getY() const { return (data >> 6) & 0x3F; }
    int getSize() const { return (data >> 12) & 0x3F; }
};

struct CompactBoard {
    static const int MAX_N = 41;
    unsigned long long bits[MAX_N];
    static unsigned long long masks[MAX_N][MAX_N];
    int n;
    
    static void initMasks(int N) {
        for (int size = 1; size <= N; ++size) {
            for (int y = 0; y <= N - size; ++y) {
                masks[size][y] = ((1ULL << size) - 1) << y;
            }
        }
    }
    
    CompactBoard(int N) : n(N) {
        memset(bits, 0, sizeof(bits));
    }
    
    bool canPlace(int x, int y, int size) const {
        if (x + size > n || y + size > n) return false;
        unsigned long long mask = masks[size][y];
        for (int r = x; r < x + size; ++r) {
            if (bits[r] & mask) return false;
        }
        return true;
    }
    
    void placeSquare(int x, int y, int size) {
        unsigned long long mask = masks[size][y];
        for (int r = x; r < x + size; ++r) {
            bits[r] |= mask;
        }
    }
    
    void removeSquare(int x, int y, int size) {
        unsigned long long mask = masks[size][y];
        for (int r = x; r < x + size; ++r) {
            bits[r] &= ~mask;
        }
    }
    
    bool findFirstFree(int &x, int &y) const {
        unsigned long long fullMask = (1ULL << n) - 1;
        for (int r = 0; r < n; ++r) {
            if (bits[r] != fullMask) {
                unsigned long long free = ~bits[r] & fullMask;
                y = __builtin_ctzll(free);
                x = r;
                return true;
            }
        }
        return false;
    }
    
    bool isComplete() const {
        unsigned long long fullMask = (1ULL << n) - 1;
        for (int i = 0; i < n; ++i) {
            if (bits[i] != fullMask) return false;
        }
        return true;
    }
    
    int getFilledArea() const {
        int area = 0;
        for (int i = 0; i < n; ++i) {
            area += __builtin_popcountll(bits[i]);
        }
        return area;
    }
    
    CompactBoard clone() const {
        CompactBoard result(n);
        memcpy(result.bits, bits, sizeof(bits));
        return result;
    }
};

unsigned long long CompactBoard::masks[MAX_N][MAX_N] = {{0}};

int sizes[41];

void initSizes(int N) {
    for (int i = 0; i < N; ++i) {
        sizes[i] = N - i;
    }
}

struct State {
    CompactBoard board;
    vector<PackedSquare> squares;
    int filledArea;
    int remainingArea;
    
    State(int N) : board(N), filledArea(0), remainingArea(N*N) {}
    
    void addSquare(int x, int y, int size) {
        board.placeSquare(x, y, size);
        squares.push_back(PackedSquare(x, y, size));
        filledArea += size * size;
        remainingArea -= size * size;
    }
    
    void removeLastSquare() {
        if (squares.empty()) return;
        PackedSquare last = squares.back();
        board.removeSquare(last.getX(), last.getY(), last.getSize());
        filledArea -= last.getSize() * last.getSize();
        remainingArea += last.getSize() * last.getSize();
        squares.pop_back();
    }
    
    bool isComplete() const {
        return board.isComplete();
    }
    
    bool findFirstFree(int &x, int &y) const {
        return board.findFirstFree(x, y);
    }
    
    bool canPlace(int x, int y, int size) const {
        return board.canPlace(x, y, size);
    }
};

int pruningCount = 0;
int improvementCount = 0;

void fillRemaining(State& state, int& bestCount, vector<PackedSquare>& bestPlaced) {
    if (state.squares.size() >= bestCount) {
        pruningCount++;
        return;
    }
    
    if (state.isComplete()) {
        if (state.squares.size() < bestCount) {
            bestCount = state.squares.size();
            bestPlaced = state.squares;
            improvementCount++;
            cout << ">>> НОВОЕ ЛУЧШЕЕ РЕШЕНИЕ: " << bestCount << " квадратов <<<" << endl;
        }
        return;
    }
    
    int x, y;
    if (!state.findFirstFree(x, y)) return;
    
    int maxSize = min(state.board.n - x, state.board.n - y);
    
    for (int size = maxSize; size >= 1; --size) {
        if (state.canPlace(x, y, size)) {
            state.addSquare(x, y, size);
            fillRemaining(state, bestCount, bestPlaced);
            state.removeLastSquare();
        }
    }
}

void generateMandatoryPlacements(vector<int>& mandatory, int idx, State& current, 
                                  vector<State>& allConfigurations) {
    if (idx == mandatory.size()) {
        allConfigurations.push_back(current);
        return;
    }
    
    int size = mandatory[idx];
    int N = current.board.n;
    
    for (int px = 0; px <= N - size; ++px) {
        for (int py = 0; py <= N - size; ++py) {
            if (current.canPlace(px, py, size)) {
                current.addSquare(px, py, size);
                generateMandatoryPlacements(mandatory, idx + 1, current, allConfigurations);
                current.removeLastSquare();
            }
        }
    }
}

int solveWithMandatory(int N, vector<int>& mandatory, 
                       vector<PackedSquare>& result, int& resultCount) {
    cout << "\n========================================" << endl;
    cout << "ВХОДНЫЕ ДАННЫЕ:" << endl;
    cout << "  Размер доски: " << N << "x" << N << endl;
    if (!mandatory.empty()) {
        cout << "  Обязательные квадраты: ";
        for (int sz : mandatory) cout << sz << " ";
        cout << endl;
    }
    
    initSizes(N);
    CompactBoard::initMasks(N);
    
    cout << "\nГЕНЕРАЦИЯ КОНФИГУРАЦИЙ:" << endl;
    cout << "  Генерируем все возможные размещения обязательных квадратов..." << endl;
    
    vector<State> configurations;
    State start(N);
    generateMandatoryPlacements(mandatory, 0, start, configurations);
    
    cout << "  Сгенерировано " << configurations.size() << " начальных конфигураций" << endl;
    
    if (configurations.empty()) {
        cout << "\nРЕЗУЛЬТАТ: Невозможно разместить обязательные квадраты" << endl;
        return 0;
    }
    
    cout << "\nПОИСК РЕШЕНИЙ:" << endl;
    cout << "  Начальная верхняя граница: " << N * N + 1 << " квадратов" << endl;
    
    int bestCount = N * N + 1;
    vector<PackedSquare> bestPlaced;
    pruningCount = 0;
    improvementCount = 0;
    
    stack<State> configStack;
    for (int i = configurations.size() - 1; i >= 0; --i) {
        configStack.push(configurations[i]);
    }
    
    int processed = 0;
    while (!configStack.empty()) {
        State currentState = configStack.top();
        configStack.pop();
        processed++;
        
        if (processed % 1000 == 0) {
            cout << "  Обработано конфигураций: " << processed << "/" << configurations.size() << endl;
        }
        
        if (currentState.squares.size() >= bestCount) continue;
        
        if (currentState.isComplete()) {
            if (currentState.squares.size() < bestCount) {
                bestCount = currentState.squares.size();
                bestPlaced = currentState.squares;
                improvementCount++;
                cout << ">>> НОВОЕ ЛУЧШЕЕ РЕШЕНИЕ: " << bestCount << " квадратов <<<" << endl;
            }
            continue;
        }
        
        fillRemaining(currentState, bestCount, bestPlaced);
    }
    
    cout << "\nСТАТИСТИКА:" << endl;
    cout << "  Обработано конфигураций: " << processed << endl;
    cout << "  Отсечений в fillRemaining: " << pruningCount << endl;
    cout << "  Найдено улучшений: " << improvementCount << endl;
    
    if (!bestPlaced.empty() && bestCount <= N * N) {
        resultCount = bestCount;
        result = bestPlaced;
        cout << "\nРЕЗУЛЬТАТ: Решение найдено!" << endl;
        return 1;
    }
    
    cout << "\nРЕЗУЛЬТАТ: Решение не найдено" << endl;
    return 0;
}

int main() {
    int N;
    cout << "Введите N: ";
    cin >> N;


    int m;
    cout << "Введите количество обязательных квадратов: ";
    cin >> m;
    
    vector<int> mandatorySizes(m);
    if (m > 0) {
        cout << "Введите их размеры: ";
        for (int i = 0; i < m; ++i) {
            cin >> mandatorySizes[i];
        }
    }
    
    vector<PackedSquare> result;
    int resultCount;
    
    int success = solveWithMandatory(N, mandatorySizes, result, resultCount);
    
    if (success && !result.empty()) {
        cout << "\n========================================" << endl;
        cout << "ИТОГОВЫЙ ОТВЕТ:" << endl;
        cout << resultCount << endl;
        for (const auto& sq : result) {
            cout << sq.getX() + 1 << " " << sq.getY() + 1 << " " << sq.getSize() << endl;
        }
    } else if (!success) {
        cout << "\nНе найдено решения с заданными обязательными квадратами." << endl;
    }

    return 0;
}