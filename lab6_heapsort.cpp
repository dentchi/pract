#include <iostream>
#include <iomanip>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

struct Stats {
    long long comparisons = 0;
    long long swaps = 0;
    long long restores = 0;
};

struct Signature {
    int count;
    long long sum;
    long long sumSq;
};

Signature makeSignature(const int a[], int n) {
    Signature s{n, 0, 0};
    for (int i = 0; i < n; i++) {
        s.sum += a[i];
        s.sumSq += 1LL * a[i] * a[i];
    }
    return s;
}

bool sameSignature(const Signature& x, const Signature& y) {
    return x.count == y.count && x.sum == y.sum && x.sumSq == y.sumSq;
}

void printArray(const int a[], int n, int active = -1) {
    for (int i = 0; i < n; i++) {
        if (i == active) cout << "| ";
        cout << a[i] << (i + 1 < n ? " " : "");
    }
    cout << "\n";
}

bool isSorted(const int a[], int from, int to) {
    for (int i = from + 1; i < to; i++)
        if (a[i - 1] > a[i]) return false;
    return true;
}

bool isMaxHeap(const int a[], int size, int& badIndex) {
    for (int i = 0; i < size; i++) {
        int l = 2 * i + 1, r = 2 * i + 2;
        if ((l < size && a[i] < a[l]) || (r < size && a[i] < a[r])) {
            badIndex = i;
            return false;
        }
    }
    badIndex = -1;
    return true;
}

void restoreHeap(int a[], int size, int i, Stats& st) {
    st.restores++;
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
        st.swaps++;
        i = largest;
    }
}

void buildMaxHeap(int a[], int n, Stats& st, bool trace) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        restoreHeap(a, n, i, st);
        if (trace) {
            cout << "  вузол i=" << i << ", активний розмір=" << n << ", масив: ";
            printArray(a, n);
        }
    }
}

bool checkStep(const int a[], int n, int active) {
    int bad;
    if (!isMaxHeap(a, active, bad)) return false;
    if (!isSorted(a, active, n)) return false;
    if (active > 0 && active < n && a[0] > a[active]) return false;
    return true;
}

void heapSort(int a[], int n, Stats& build, Stats& sortSt, bool trace, int& stepFails) {
    build = Stats();
    sortSt = Stats();
    stepFails = 0;
    buildMaxHeap(a, n, build, trace);
    int extracted = 0;
    for (int end = n - 1; end > 0; end--) {
        swap(a[0], a[end]);
        sortSt.swaps++;
        restoreHeap(a, end, 0, sortSt);
        extracted++;
        if (!checkStep(a, n, end)) stepFails++;
        if (trace && extracted <= 3) {
            cout << "  вилучення " << extracted << ", активний розмір=" << end << ": ";
            printArray(a, n, end);
        }
    }
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
        else if (type == 2) a[i] = nextRandom() % 10000;
        else a[i] = nextRandom() % 10;
    }
}

struct RunResult {
    Stats build, sortSt;
    bool sortedOk, compositionOk, stepsOk, heapAfterBuildOk;
    bool ok() const { return sortedOk && compositionOk && stepsOk && heapAfterBuildOk; }
};

RunResult runAndCheck(int a[], int n, bool trace) {
    RunResult r;
    Signature before = makeSignature(a, n);
    int stepFails;

    Stats b, s;
    int* tmp = new int[n > 0 ? n : 1];
    for (int i = 0; i < n; i++) tmp[i] = a[i];
    Stats dummy;
    buildMaxHeap(tmp, n, dummy, false);
    int bad;
    r.heapAfterBuildOk = isMaxHeap(tmp, n, bad) && sameSignature(before, makeSignature(tmp, n));
    delete[] tmp;
    heapSort(a, n, b, s, trace, stepFails);
    r.build = b;
    r.sortSt = s;
    r.sortedOk = isSorted(a, 0, n);
    r.compositionOk = sameSignature(before, makeSignature(a, n));
    r.stepsOk = (stepFails == 0);
    return r;
}

bool equalArrays(const int a[], const int b[], int n) {
    for (int i = 0; i < n; i++) if (a[i] != b[i]) return false;
    return true;
}

struct TestCase { int n; int in[10]; int out[10]; const char* note; };

void runTests() {
    TestCase tests[] = {
        {0, {}, {}, "порожній масив"},
        {1, {7}, {7}, "один елемент"},
        {2, {1, 2}, {1, 2}, "два впорядковані"},
        {2, {2, 1}, {1, 2}, "два переставлені"},
        {5, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, "зростання"},
        {5, {5, 4, 3, 2, 1}, {1, 2, 3, 4, 5}, "спадання"},
        {5, {4, 1, 5, 2, 3}, {1, 2, 3, 4, 5}, "змішаний порядок"},
        {5, {3, 1, 3, 2, 1}, {1, 1, 2, 3, 3}, "повторення"},
        {5, {0, -5, 8, -2, 4}, {-5, -2, 0, 4, 8}, "від'ємні числа"},
        {4, {6, 6, 6, 6}, {6, 6, 6, 6}, "усі рівні"},
        {6, {9, -1, 9, 0, -1, 5}, {-1, -1, 0, 5, 9, 9}, "комбінований випадок"},
        {7, {10, 7, 8, 4, 2, 1, 3}, {1, 2, 3, 4, 7, 8, 10}, "неповний останній рівень"},
        {6, {20, -3, 20, 0, -3, 15}, {-3, -3, 0, 15, 20, 20}, "власний 1"},
        {8, {11, 2, 9, 4, 7, 5, 3, 8}, {2, 3, 4, 5, 7, 8, 9, 11}, "власний 2"},
    };
    int total = sizeof(tests) / sizeof(tests[0]);
    int sortPass = 0, buildChecks = 0, buildPass = 0;
    cout << "=== Автоматизовані тести ===\n";
    for (int t = 0; t < total; t++) {
        int a[10];
        for (int i = 0; i < tests[t].n; i++) a[i] = tests[t].in[i];
        RunResult r = runAndCheck(a, tests[t].n, false);
        bool match = equalArrays(a, tests[t].out, tests[t].n);
        bool pass = r.ok() && match;
        if (pass) sortPass++;
        bool checkBuild = tests[t].n >= 5;
        if (checkBuild) { buildChecks++; if (r.heapAfterBuildOk) buildPass++; }
        cout << setw(2) << t + 1 << " | " << setw(26) << left << tests[t].note << right
             << " | сортування: " << (pass ? "успішно" : "ПОМИЛКА")
             << " | побудова: " << (checkBuild ? (r.heapAfterBuildOk ? "успішно" : "ПОМИЛКА") : "-")
             << "\n";
    }
    cout << "Успішних сортувань: " << sortPass << " з " << total << "\n";
    cout << "Успішних перевірок побудови купи: " << buildPass << " з " << buildChecks << "\n\n";
}

void runHeapPropertyTests() {
    cout << "=== Перевірка властивості купи ===\n";
    struct HC { int n; int v[8]; bool expected; const char* name; };
    HC cases[] = {
        {7, {9, 7, 8, 2, 3, 4, 1}, true, "правильна 1"},
        {4, {5, 5, 5, 5}, true, "правильна 2"},
        {8, {10, 9, 8, 7, 6, 5, 4, 3}, true, "правильна 3"},
        {3, {1, 2, 3}, false, "неправильна 1"},
        {7, {9, 7, 8, 2, 10, 4, 1}, false, "неправильна 2"},
        {7, {9, 7, 8, 2, 3, 4, 10}, false, "неправильна 3"},
    };
    int passed = 0;
    for (int c = 0; c < 6; c++) {
        int bad;
        bool res = isMaxHeap(cases[c].v, cases[c].n, bad);
        bool ok = (res == cases[c].expected);
        if (ok) passed++;
        cout << cases[c].name << ": ";
        printArray(cases[c].v, cases[c].n);
        cout << "   -> " << (res ? "купа коректна" : "порушення в вузлі з індексом " + to_string(bad))
             << " | перевірка: " << (ok ? "успішно" : "ПОМИЛКА") << "\n";
    }
    cout << "Успішних окремих перевірок властивості: " << passed << " з 6\n";

    Stats st;
    int x[] = {3, 9};
    restoreHeap(x, 2, 0, st);
    int y[] = {1, 5, 4, 9, 10};
    restoreHeap(y, 3, 0, st);
    int bad;
    bool ok1 = x[0] == 9 && x[1] == 3;
    bool ok2 = y[0] == 5 && y[1] == 1 && y[2] == 4 && y[3] == 9 && y[4] == 10 && isMaxHeap(y, 3, bad);
    cout << "Вузол з одним нащадком: " << (ok1 ? "успішно" : "ПОМИЛКА")
         << "; межа активної купи: " << (ok2 ? "успішно" : "ПОМИЛКА") << "\n\n";
}

void runExperiment() {
    const char* names[] = {"зростаючий", "спадний", "змішаний", "з повторами"};
    int sizes[] = {100, 1000, 10000};
    cout << "=== Порівняльний експеримент ===\n";
    cout << "n | набір | побудова пор./обміни | вилучення пор./обміни | усього пор./обміни | відновлень | перевірка\n";
    int okRuns = 0;
    for (int s = 0; s < 3; s++) {
        for (int t = 0; t < 4; t++) {
            int n = sizes[s];
            int* a = new int[n];
            fillData(a, n, t);
            RunResult r = runAndCheck(a, n, false);
            if (r.ok()) okRuns++;
            long long totC = r.build.comparisons + r.sortSt.comparisons;
            long long totS = r.build.swaps + r.sortSt.swaps;
            long long rest = r.build.restores + r.sortSt.restores;
            cout << n << " | " << names[t] << " | " << r.build.comparisons << " / " << r.build.swaps
                 << " | " << r.sortSt.comparisons << " / " << r.sortSt.swaps
                 << " | " << totC << " / " << totS << " | " << rest
                 << " | " << (r.ok() ? "успішно" : "ПОМИЛКА") << "\n";
            delete[] a;
        }
    }
    cout << "Успішних запусків: " << okRuns << " з 12\n\n";
}

void runVariant() {
    int n = 15;
    int def[] = {47, 24, 32, 19, 43, 27, 16, 39, 22, 35, 15, 45, 29, 20, 38};
    cout << "Варіант 26. Введіть кількість елементів і сам масив (або Ctrl+Z/Ctrl+D для значень за замовчуванням): ";
    int a[100];
    if (!(cin >> n) || n < 1 || n > 100) {
        n = 15;
        for (int i = 0; i < n; i++) a[i] = def[i];
        cout << "\n(використано масив варіанта 26)\n";
    } else {
        for (int i = 0; i < n; i++) cin >> a[i];
    }
    cout << "\n=== Індивідуальний варіант (n=" << n << ") ===\nПочатковий масив: ";
    printArray(a, n);
    Signature before = makeSignature(a, n);
    cout << "\nТрасування побудови купи і перших трьох вилучень:\n";
    Stats b, s;
    int stepFails;
    heapSort(a, n, b, s, true, stepFails);
    cout << "\nРезультат: ";
    printArray(a, n);
    bool ok = isSorted(a, 0, n) && sameSignature(before, makeSignature(a, n)) && stepFails == 0;
    cout << "Перевірка: " << (ok ? "успішно" : "ПОМИЛКА") << "\n";
    cout << "Побудова:   порівнянь=" << b.comparisons << ", обмінів=" << b.swaps << ", відновлень=" << b.restores << "\n";
    cout << "Вилучення:  порівнянь=" << s.comparisons << ", обмінів=" << s.swaps << ", відновлень=" << s.restores << "\n";
    cout << "Усього:     порівнянь=" << b.comparisons + s.comparisons << ", обмінів=" << b.swaps + s.swaps
         << ", відновлень=" << b.restores + s.restores << "\n\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    runTests();
    runHeapPropertyTests();
    runExperiment();
    runVariant();
    return 0;
}
