#include <iostream>
#include <string>
#include <climits>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const int MAXN = 10000;

struct Stats {
    long long comparisons = 0;
    long long writes = 0;
    long long merges = 0;
    int maxDepth = 0;
};

struct Signature {
    int count;
    long long sum;
    unsigned long long sumSq;
};

int tmpArr[MAXN];
bool traceMode = false;
int splitErrors = 0;

Signature makeSignature(const int a[], int n) {
    Signature s{n, 0, 0};
    for (int i = 0; i < n; i++) {
        s.sum += a[i];
        s.sumSq += (unsigned long long)(1LL * a[i] * a[i]);
    }
    return s;
}

bool sameSignature(const Signature& x, const Signature& y) {
    return x.count == y.count && x.sum == y.sum && x.sumSq == y.sumSq;
}

bool isSorted(const int a[], int n) {
    for (int i = 1; i < n; i++)
        if (a[i - 1] > a[i]) return false;
    return true;
}

bool equalArrays(const int a[], const int b[], int n) {
    for (int i = 0; i < n; i++)
        if (a[i] != b[i]) return false;
    return true;
}

void printArray(const int a[], int from, int to) {
    cout << "[";
    for (int i = from; i < to; i++) {
        cout << a[i];
        if (i + 1 < to) cout << ", ";
    }
    cout << "]";
}

void indent(int depth) {
    for (int i = 1; i < depth; i++) cout << "  ";
}

void merge(int a[], int left, int mid, int right, Stats& st) {
    int i = left, j = mid, k = left;
    while (i < mid && j < right) {
        st.comparisons++;
        if (a[i] <= a[j]) tmpArr[k++] = a[i++];
        else tmpArr[k++] = a[j++];
        st.writes++;
    }
    while (i < mid) {
        tmpArr[k++] = a[i++];
        st.writes++;
    }
    while (j < right) {
        tmpArr[k++] = a[j++];
        st.writes++;
    }
    for (int t = left; t < right; t++) {
        a[t] = tmpArr[t];
        st.writes++;
    }
    st.merges++;
}

void mergeSortRec(int a[], int left, int right, int depth, Stats& st) {
    if (depth > st.maxDepth) st.maxDepth = depth;
    if (traceMode) {
        indent(depth);
        cout << "поділ: глибина " << depth << ", межі [" << left << ", " << right << "): ";
        printArray(a, left, right);
        cout << "\n";
    }
    if (right - left <= 1) return;
    int mid = left + (right - left) / 2;
    if (mid - left >= right - left || right - mid >= right - left) splitErrors++;
    mergeSortRec(a, left, mid, depth + 1, st);
    mergeSortRec(a, mid, right, depth + 1, st);
    if (traceMode) {
        indent(depth);
        cout << "злиття: ";
        printArray(a, left, mid);
        cout << " + ";
        printArray(a, mid, right);
    }
    merge(a, left, mid, right, st);
    if (traceMode) {
        cout << " -> ";
        printArray(a, left, right);
        cout << "\n";
    }
}

void mergeSort(int a[], int n, Stats& st, bool trace) {
    st = Stats();
    splitErrors = 0;
    traceMode = trace && n <= 16;
    if (n > 0) mergeSortRec(a, 0, n, 1, st);
    traceMode = false;
}

void insertionSort(int a[], int n, Stats& st) {
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0) {
            st.comparisons++;
            if (a[j] <= key) break;
            a[j + 1] = a[j];
            st.writes++;
            j--;
        }
        a[j + 1] = key;
        st.writes++;
    }
}

void selectionSort(int a[], int n, Stats& st) {
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            st.comparisons++;
            if (a[j] < a[minIndex]) minIndex = j;
        }
        if (minIndex != i) {
            swap(a[i], a[minIndex]);
            st.writes++;
        }
    }
}

void bubbleSort(int a[], int n, Stats& st) {
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; j++) {
            st.comparisons++;
            if (a[j] > a[j + 1]) {
                swap(a[j], a[j + 1]);
                st.writes++;
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void shellSort(int a[], int n, Stats& st) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int key = a[i];
            int j = i;
            while (j >= gap) {
                st.comparisons++;
                if (a[j - gap] <= key) break;
                a[j] = a[j - gap];
                st.writes++;
                j -= gap;
            }
            a[j] = key;
            st.writes++;
        }
    }
}

void quickSortRec(int a[], int left, int right, int depth, Stats& st) {
    if (depth > st.maxDepth) st.maxDepth = depth;
    if (left >= right) return;
    int pivot = a[left + (right - left) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (true) {
            st.comparisons++;
            if (a[i] < pivot) i++;
            else break;
        }
        while (true) {
            st.comparisons++;
            if (a[j] > pivot) j--;
            else break;
        }
        if (i <= j) {
            swap(a[i], a[j]);
            st.writes++;
            i++;
            j--;
        }
    }
    if (left < j) quickSortRec(a, left, j, depth + 1, st);
    if (i < right) quickSortRec(a, i, right, depth + 1, st);
}

void restoreHeap(int a[], int size, int i, Stats& st) {
    while (true) {
        int l = 2 * i + 1, r = 2 * i + 2, largest = i;
        if (l < size) {
            st.comparisons++;
            if (a[l] > a[largest]) largest = l;
        }
        if (r < size) {
            st.comparisons++;
            if (a[r] > a[largest]) largest = r;
        }
        if (largest == i) break;
        swap(a[i], a[largest]);
        st.writes++;
        i = largest;
    }
}

void heapSort(int a[], int n, Stats& st) {
    for (int i = n / 2 - 1; i >= 0; i--) restoreHeap(a, n, i, st);
    for (int end = n - 1; end > 0; end--) {
        swap(a[0], a[end]);
        st.writes++;
        restoreHeap(a, end, 0, st);
    }
}

const int ALG_COUNT = 7;
const char* algNames[ALG_COUNT] = {"Вставками", "Вибором", "Бульбашкове", "Шелла", "QuickSort", "HeapSort", "MergeSort"};
const char* algUnits[ALG_COUNT] = {"записи", "обміни", "обміни", "записи", "обміни", "обміни", "записи"};

void runAlgorithm(int id, int a[], int n, Stats& st) {
    st = Stats();
    if (id == 0) insertionSort(a, n, st);
    else if (id == 1) selectionSort(a, n, st);
    else if (id == 2) bubbleSort(a, n, st);
    else if (id == 3) shellSort(a, n, st);
    else if (id == 4) { if (n > 0) quickSortRec(a, 0, n - 1, 1, st); }
    else if (id == 5) heapSort(a, n, st);
    else mergeSort(a, n, st, false);
}

string extraMemory(int id, int n, const Stats& st) {
    if (id == 4) return "стек глибини " + to_string(st.maxDepth);
    if (id == 6) return "масив " + to_string(n) + " + стек глибини " + to_string(st.maxDepth);
    return "1 змінна";
}

bool checkResult(const int original[], const int a[], int n) {
    bool ok = isSorted(a, n) && sameSignature(makeSignature(original, n), makeSignature(a, n));
    if (!ok) {
        cout << "ПОМИЛКА! Вхідний масив: ";
        printArray(original, 0, n);
        cout << "\nФактичний масив: ";
        printArray(a, 0, n);
        cout << "\n";
    }
    return ok;
}

struct MergeCase { int nl; int nr; int in[8]; int out[8]; const char* name; };

void runMergeTests() {
    MergeCase cases[] = {
        {3, 3, {1, 4, 7, 2, 5, 8}, {1, 2, 4, 5, 7, 8}, "однакова довжина"},
        {1, 4, {3, 1, 2, 6, 9}, {1, 2, 3, 6, 9}, "різна довжина"},
        {0, 3, {2, 4, 6}, {2, 4, 6}, "порожня ліва частина"},
        {2, 0, {1, 3}, {1, 3}, "порожня права частина"},
        {3, 3, {2, 2, 5, 2, 5, 5}, {2, 2, 2, 5, 5, 5}, "повторення"},
        {3, 3, {-7, -3, 0, -5, -1, 4}, {-7, -5, -3, -1, 0, 4}, "від'ємні числа"},
        {3, 3, {6, 8, 9, 1, 2, 3}, {1, 2, 3, 6, 8, 9}, "ліва частина більша"},
    };
    int total = sizeof(cases) / sizeof(cases[0]);
    int passed = 0;
    cout << "=== Тести злиття ===\n";
    for (int c = 0; c < total; c++) {
        int n = cases[c].nl + cases[c].nr;
        int a[8];
        for (int i = 0; i < n; i++) a[i] = cases[c].in[i];
        Stats st;
        merge(a, 0, cases[c].nl, n, st);
        bool ok = equalArrays(a, cases[c].out, n) && checkResult(cases[c].in, a, n);
        if (ok) passed++;
        cout << c + 1 << ". " << cases[c].name << ": ";
        printArray(cases[c].in, 0, cases[c].nl);
        cout << " + ";
        printArray(cases[c].in, cases[c].nl, n);
        cout << " -> ";
        printArray(a, 0, n);
        cout << " | " << (ok ? "успішно" : "ПОМИЛКА") << "\n";
    }
    cout << "Успішних перевірок злиття: " << passed << " з " << total << "\n\n";
}

struct TestCase { int n; int in[15]; int out[15]; const char* name; };

void runSortTests() {
    TestCase tests[] = {
        {0, {}, {}, "порожній масив"},
        {1, {7}, {7}, "один елемент"},
        {2, {1, 2}, {1, 2}, "два впорядковані"},
        {2, {2, 1}, {1, 2}, "два переставлені"},
        {5, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, "зростання"},
        {5, {5, 4, 3, 2, 1}, {1, 2, 3, 4, 5}, "спадання"},
        {5, {4, 1, 5, 2, 3}, {1, 2, 3, 4, 5}, "непарна довжина"},
        {5, {3, 1, 3, 2, 1}, {1, 1, 2, 3, 3}, "повторення"},
        {5, {0, -5, 8, -2, 4}, {-5, -2, 0, 4, 8}, "від'ємні числа"},
        {4, {6, 6, 6, 6}, {6, 6, 6, 6}, "усі рівні"},
        {6, {9, -1, 9, 0, -1, 5}, {-1, -1, 0, 5, 9, 9}, "комбінований випадок"},
        {7, {8, 3, 7, 2, 6, 1, 5}, {1, 2, 3, 5, 6, 7, 8}, "кілька рівнів поділу"},
        {15, {14, -2, 33, 7, 0, 21, -9, 7, 50, 3, -15, 28, 11, 1, 40},
             {-15, -9, -2, 0, 1, 3, 7, 7, 11, 14, 21, 28, 33, 40, 50}, "власний: 15 елементів"},
        {6, {INT_MAX, INT_MIN, 0, INT_MAX, -1, INT_MIN},
            {INT_MIN, INT_MIN, -1, 0, INT_MAX, INT_MAX}, "власний: крайні значення"},
    };
    int total = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;
    cout << "=== Тести MergeSort ===\n";
    for (int t = 0; t < total; t++) {
        int a[15];
        int n = tests[t].n;
        for (int i = 0; i < n; i++) a[i] = tests[t].in[i];
        Stats st;
        mergeSort(a, n, st, false);
        bool ok = equalArrays(a, tests[t].out, n) && checkResult(tests[t].in, a, n) && splitErrors == 0;
        if (ok) passed++;
        cout << t + 1 << ". " << tests[t].name << ": ";
        printArray(a, 0, n);
        cout << " | порівнянь " << st.comparisons << ", записів " << st.writes
             << ", злиттів " << st.merges << ", глибина " << st.maxDepth
             << " | " << (ok ? "успішно" : "ПОМИЛКА") << "\n";
    }
    cout << "Успішних перевірок MergeSort: " << passed << " з " << total << "\n\n";
}

unsigned long long lcg = 12345;

int nextRandom() {
    lcg = (lcg * 1103515245ULL + 12345ULL) % 2147483648ULL;
    return (int)(lcg / 65536);
}

void fillData(int a[], int n, int type) {
    lcg = 12345;
    for (int i = 0; i < n; i++) {
        if (type == 0) a[i] = i + 1;
        else if (type == 1) a[i] = n - i;
        else if (type == 2) a[i] = i + 1;
        else a[i] = nextRandom() % 10000;
    }
    if (type == 2) {
        for (int k = 0; k < n / 20; k++) {
            int x = nextRandom() % n;
            int y = nextRandom() % n;
            swap(a[x], a[y]);
        }
    }
}

int baseArr[MAXN];
int workArr[MAXN];

void runExperiment() {
    const char* typeNames[4] = {"зростаючі", "спадні", "майже впорядковані", "змішані"};
    int sizes[3] = {100, 1000, 10000};
    int runs = 0, okRuns = 0;
    cout << "=== Порівняння алгоритмів ===\n";
    for (int s = 0; s < 3; s++) {
        int n = sizes[s];
        for (int type = 0; type < 4; type++) {
            fillData(baseArr, n, type);
            cout << "n = " << n << ", " << typeNames[type] << "\n";
            for (int id = 0; id < ALG_COUNT; id++) {
                for (int i = 0; i < n; i++) workArr[i] = baseArr[i];
                Stats st;
                runAlgorithm(id, workArr, n, st);
                bool ok = checkResult(baseArr, workArr, n);
                runs++;
                if (ok) okRuns++;
                cout << "  " << algNames[id] << ": порівнянь " << st.comparisons
                     << ", " << algUnits[id] << " " << st.writes
                     << ", пам'ять: " << extraMemory(id, n, st);
                if (id == 6) cout << ", злиттів " << st.merges;
                cout << " | " << (ok ? "успішно" : "ПОМИЛКА") << "\n";
            }
        }
    }
    cout << "Успішних запусків: " << okRuns << " з " << runs << "\n\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    runMergeTests();
    runSortTests();

    cout << "=== Трасування ===\n";
    int traceArr[7] = {8, 3, 7, 2, 6, 1, 5};
    Stats traceSt;
    mergeSort(traceArr, 7, traceSt, true);
    cout << "Порівнянь " << traceSt.comparisons << ", записів " << traceSt.writes
         << ", злиттів " << traceSt.merges << ", глибина " << traceSt.maxDepth << "\n\n";

    runExperiment();

    cout << "=== Індивідуальний варіант ===\n";
    int n;
    cout << "Введіть кількість елементів: ";
    cin >> n;
    if (n < 0 || n > MAXN) {
        cout << "Неправильна кількість\n";
        return 0;
    }
    cout << "Введіть елементи: ";
    for (int i = 0; i < n; i++) cin >> baseArr[i];

    for (int i = 0; i < n; i++) workArr[i] = baseArr[i];
    Stats st;
    mergeSort(workArr, n, st, true);
    cout << "Результат: ";
    printArray(workArr, 0, n);
    cout << "\nПорівнянь " << st.comparisons << ", записів " << st.writes
         << ", злиттів " << st.merges << ", глибина " << st.maxDepth
         << " | перевірка: " << (checkResult(baseArr, workArr, n) ? "успішно" : "ПОМИЛКА") << "\n\n";

    cout << "Усі алгоритми на масиві варіанта:\n";
    for (int id = 0; id < ALG_COUNT; id++) {
        for (int i = 0; i < n; i++) workArr[i] = baseArr[i];
        Stats s2;
        runAlgorithm(id, workArr, n, s2);
        bool ok = checkResult(baseArr, workArr, n);
        cout << "  " << algNames[id] << ": порівнянь " << s2.comparisons
             << ", " << algUnits[id] << " " << s2.writes
             << " | " << (ok ? "успішно" : "ПОМИЛКА") << "\n";
    }
    return 0;
}
