#include <vector>
#include <iostream>
#include <cassert>

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

int solveWithMandatory(int N, vector<int>& mandatory, 
                       vector<PackedSquare>& result, int& resultCount);

void testSquarePacking() {
    cout << "\n========== ТЕСТЫ ==========" << endl;
    int testsPassed = 0;
    int testsFailed = 0;
    
    {
        cout << "\n[1] N=2, обязательный 1 1 1 1" << endl;
        int N = 2;
        vector<int> mandatory = {1,1,1,1};
        vector<PackedSquare> result;
        int resultCount;
        
        int success = solveWithMandatory(N, mandatory, result, resultCount);
        if (success == 1 && resultCount == 4) {
            cout << "  OK (квадратов: " << resultCount << ")" << endl;
            testsPassed++;
        } else {
            cout << "  FAIL (expected 4, got " << resultCount << ")" << endl;
            testsFailed++;
        }
    }
    {
        cout << "\n[2] N=3, обязательный 2" << endl;
        int N = 3;
        vector<int> mandatory = {2};
        vector<PackedSquare> result;
        int resultCount;
        
        int success = solveWithMandatory(N, mandatory, result, resultCount);
        if (success == 1 && resultCount == 6) {
            cout << "  OK (квадратов: " << resultCount << ")" << endl;
            testsPassed++;
        } else {
            cout << "  FAIL (expected 6, got " << resultCount << ")" << endl;
            testsFailed++;
        }
    }
    
    {
        cout << "\n[3] N=4, обязательный 2" << endl;
        int N = 4;
        vector<int> mandatory = {2};
        vector<PackedSquare> result;
        int resultCount;
        
        int success = solveWithMandatory(N, mandatory, result, resultCount);
        if (success == 1 && resultCount == 4) {
            cout << "  OK (квадратов: " << resultCount << ")" << endl;
            testsPassed++;
        } else {
            cout << "  FAIL (expected 4, got " << resultCount << ")" << endl;
            testsFailed++;
        }
    }
    
    {
        cout << "\n[4] N=4, обязательный 3" << endl;
        int N = 4;
        vector<int> mandatory = {3};
        vector<PackedSquare> result;
        int resultCount;
        
        int success = solveWithMandatory(N, mandatory, result, resultCount);
        if (success == 1 && resultCount == 8) {
            cout << "  OK (квадратов: " << resultCount << ")" << endl;
            testsPassed++;
        } else {
            cout << "  FAIL (expected 8, got " << resultCount << ")" << endl;
            testsFailed++;
        }
    }
    
    {
        cout << "\n[5] N=5, обязательный 4" << endl;
        int N = 5;
        vector<int> mandatory = {4};
        vector<PackedSquare> result;
        int resultCount;
        
        int success = solveWithMandatory(N, mandatory, result, resultCount);
        if (success == 1 && resultCount == 10) {
            cout << "  OK (квадратов: " << resultCount << ")" << endl;
            testsPassed++;
        } else {
            cout << "  FAIL (expected 10, got " << resultCount << ")" << endl;
            testsFailed++;
        }
    }
    
    {
        cout << "\n[6] N=5, обязательный 3" << endl;
        int N = 5;
        vector<int> mandatory = {3};
        vector<PackedSquare> result;
        int resultCount;
        
        int success = solveWithMandatory(N, mandatory, result, resultCount);
        if (success == 1 && resultCount > 0) {
            cout << "  OK (квадратов: " << resultCount << ")" << endl;
            testsPassed++;
        } else {
            cout << "  FAIL" << endl;
            testsFailed++;
        }
    }
    
    {
        cout << "\n[7] N=3, обязательные 1 и 2" << endl;
        int N = 3;
        vector<int> mandatory = {1, 2};
        vector<PackedSquare> result;
        int resultCount;
        
        int success = solveWithMandatory(N, mandatory, result, resultCount);
        if (success == 1 && resultCount == 6) {
            cout << "  OK (квадратов: " << resultCount << ")" << endl;
            testsPassed++;
        } else {
            cout << "  FAIL (expected 6, got " << resultCount << ")" << endl;
            testsFailed++;
        }
    }
    
    {
        cout << "\n[8] N=4, обязательные 2 и 2" << endl;
        int N = 4;
        vector<int> mandatory = {2, 2};
        vector<PackedSquare> result;
        int resultCount;
        
        int success = solveWithMandatory(N, mandatory, result, resultCount);
        if (success == 1 && resultCount == 4) {
            cout << "  OK (квадратов: " << resultCount << ")" << endl;
            testsPassed++;
        } else {
            cout << "  FAIL (expected 4, got " << resultCount << ")" << endl;
            testsFailed++;
        }
    }
    
    {
        cout << "\n[9] Проверка площади (N=4)" << endl;
        int N = 4;
        vector<int> mandatory = {2};
        vector<PackedSquare> result;
        int resultCount;
        
        solveWithMandatory(N, mandatory, result, resultCount);
        
        int totalArea = 0;
        for (const auto& sq : result) {
            totalArea += sq.getSize() * sq.getSize();
        }
        if (totalArea == N * N) {
            cout << "  OK (площадь: " << totalArea << "/" << N*N << ")" << endl;
            testsPassed++;
        } else {
            cout << "  FAIL (площадь: " << totalArea << "/" << N*N << ")" << endl;
            testsFailed++;
        }
    }
    
    {
        cout << "\n[10] Проверка обязательных квадратов (N=5)" << endl;
        int N = 5;
        vector<int> mandatory = {3, 2};
        vector<PackedSquare> result;
        int resultCount;
        
        solveWithMandatory(N, mandatory, result, resultCount);
        
        vector<int> foundSizes;
        for (const auto& sq : result) {
            foundSizes.push_back(sq.getSize());
        }
        
        bool allFound = true;
        for (int req : mandatory) {
            bool found = false;
            for (int sz : foundSizes) {
                if (sz == req) {
                    found = true;
                    break;
                }
            }
            if (!found) allFound = false;
        }
        if (allFound) {
            cout << "  OK (все обязательные квадраты найдены)" << endl;
            testsPassed++;
        } else {
            cout << "  FAIL (не все обязательные квадраты найдены)" << endl;
            testsFailed++;
        }
    }
    
    cout << "\n========== РЕЗУЛЬТАТЫ ==========" << endl;
    cout << "  Пройдено: " << testsPassed << endl;
    cout << "  Не пройдено: " << testsFailed << endl;
    cout << "================================\n" << endl;
}

int main() {
    testSquarePacking();
    return 0;
}