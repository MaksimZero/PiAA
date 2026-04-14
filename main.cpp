#include <vector>
#include <iostream>
#include <queue>
#include <climits>
#include <algorithm>
#include <cstring>
#include <cmath>

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
        for (int size = 1; size < N; ++size) {
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
    
    bool findFirstFreeFromBottomRight(int &x, int &y) const {
        unsigned long long fullMask = (1ULL << n) - 1;
        for (int r = n - 1; r >= 0; --r) {
            if (bits[r] != fullMask) {
                unsigned long long free = ~bits[r] & fullMask;
                y = 63 - __builtin_clzll(free);
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
int firstSquareSizes[41];
int firstSquareCount;




int smallestPrimeFactor(int N) {
    if (N <= 1) return N;
    if (N % 2 == 0) return 2;
    
    int limit = sqrt(N);
    for (int i = 3; i <= limit; i += 2) {
        if (N % i == 0) {
            return i;
        }
    }

    return N;
}

bool isPrime(int N){
    if (smallestPrimeFactor(N) == N){
        return true;
    }
    return false;
}
void initSizes(int N) {
    for (int i = 0; i < N-1; ++i) {
        sizes[i] = (N-1) - i;
    }
    
    firstSquareCount = 0;
    for (int size = N-1; size >= max(1, N/2); --size) {
        firstSquareSizes[firstSquareCount++] = size;
    }
}

struct State {
    CompactBoard board;
    PackedSquare squares[100];
    int count;
    int filledArea;
    int remainingArea;
    
    State(int N) : board(N), count(0), filledArea(0), remainingArea(N*N) {}
    
    void addSquare(int x, int y, int size) {
        board.placeSquare(x, y, size);
        squares[count++] = PackedSquare(x, y, size);
        filledArea += size * size;
        remainingArea -= size * size;
    }
    
    State clone() const {
        State result(board.n);
        result.board = board.clone();
        result.count = count;
        result.filledArea = filledArea;
        result.remainingArea = remainingArea;
        for (int i = 0; i < count; ++i) {
            result.squares[i] = squares[i];
        }
        return result;
    }
    
    bool isComplete() const {
        return board.isComplete();
    }
    
    bool findFirstFreeFromBottomRight(int &x, int &y) const {
        return board.findFirstFreeFromBottomRight(x, y);
    }
    
    bool canPlace(int x, int y, int size) const {
        return board.canPlace(x, y, size);
    }
    
    int getFilledArea() const {
        return filledArea;
    }
    
    int getRemainingArea() const {
        return remainingArea;
    }
    
    int getLowerBound() const {
        int remaining = remainingArea;
        if (remaining <= 0) return 0;
        
        int maxSquareArea = (board.n - 1) * (board.n - 1);
        return (remaining + maxSquareArea - 1) / maxSquareArea;
    }
};

struct CompareState {
    bool operator()(const State& a, const State& b) const {
        if (a.filledArea != b.filledArea) 
            return a.filledArea < b.filledArea;
        return a.count > b.count;
    }
};

int backtracking(int N, vector<PackedSquare>& bestPlaced, int& bestCount) {
    bool flag = false;
    int k = 1;
    if ( smallestPrimeFactor(N) != N) {
        int b = N;
        N = smallestPrimeFactor(N);
        k = b / N;
        flag = true;
        
    }   
    initSizes(N);
    CompactBoard::initMasks(N);
    
    priority_queue<State, vector<State>, CompareState> pq;
    if (flag == false) {
        State primeState(N);
        int half1 = N/2 + 1;
        int half2 = N/2;
        primeState.addSquare(0, 0, half1);
        primeState.addSquare(0, half1, half2);
        primeState.addSquare(half1, 0, half2);
        pq.push(primeState);
    } else {
        for (int i = 0; i < firstSquareCount; ++i) {
            int size = firstSquareSizes[i];
            State state(N);
    
            state.addSquare(N - size, N - size, size);
            pq.push(state);
        }
    }
    
    while (!pq.empty()) {
        State cur = pq.top();
        pq.pop();
        
        if (cur.count >= bestCount) continue;
        
        if (cur.count + cur.getLowerBound() >= bestCount) continue;
        
        if (cur.isComplete()) {
            bestCount = cur.count;
            bestPlaced.clear();
            for (int i = 0; i < cur.count; ++i) {
                bestPlaced.push_back(cur.squares[i]);
            }
            continue;
        }
        
        int x, y;
        if (!cur.findFirstFreeFromBottomRight(x, y)) {
            continue;
        }
        
        int maxPossible = min(x + 1, y + 1);
        
        for (int idx = 0; idx < N-1; ++idx) {
            int size = sizes[idx];
            if (size > maxPossible) continue;
            
            int placeX = x - size + 1;
            int placeY = y - size + 1;
            
            if (placeX < 0 || placeY < 0) continue;
            if (!cur.canPlace(placeX, placeY, size)) continue;
            
            State next = cur.clone();
            next.addSquare(placeX, placeY, size);
            
            if (next.count + next.getLowerBound() < bestCount) {
                pq.push(next);
            }
        }
    }
    return k;
}

int main() {
    int N;
    cin >> N;

    if (N == 2){
        cout << "4" << endl;
        cout << "1 1 1" << endl;
        cout << "2 1 1" << endl;
        cout << "1 2 1" << endl;
        cout << "2 2 1" << endl;
        return 0;
    }    
    
    vector<PackedSquare> bestPlaced;
    int bestCount = 13;

    if (N >= 13) {
        bestCount = N;
    }
    
    int k = backtracking(N, bestPlaced, bestCount);

    cout << bestCount << endl;
    for (const auto& sq : bestPlaced) {
        cout << k * sq.getX() + 1 << " " << k * sq.getY() + 1 << " " << k * sq.getSize() << endl;
    }

    return 0;
}