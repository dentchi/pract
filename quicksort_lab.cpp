// Лабораторна робота: QuickSort зі схемами розбиття Ломуто та Гоара
// Стандарт: C++17. Без готових функцій сортування та контейнерів.
//
// Запуск:
//   ./quicksort_lab          - тести, перевірка розбиття, трасування, експеримент
//   ./quicksort_lab input    - ввести власний масив (n, потім n чисел) і відсортувати обома схемами
//
// Домовленості підрахунку:
//   * порівняння значень - кожне порівняння елемента з опорним значенням у розбитті;
//   * обмін - кожен виклик swapEl (в т.ч. обмін елемента із самим собою) = 1 обмін;
//   * виклик - кожен виклик функції QuickSort (включно з порожніми/одноелементними ділянками);
//   * глибина - кількість вкладених викликів QuickSort (перший виклик має глибину 1).

#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>

using namespace std;

struct Stats {
    long long comparisons = 0;
    long long swaps = 0;
    long long calls = 0;
    int maxDepth = 0;
    void reset() { comparisons = swaps = calls = 0; maxDepth = 0; }
};

static bool g_trace = false;
static int g_callId = 0;

// ---------------------------------------------------------------- допоміжне

static void swapEl(int& a, int& b, Stats& s) {
    int t = a;
    a = b;
    b = t;
    s.swaps++;
}

static void printRange(const int* a, int lo, int hi) {
    cout << "[";
    for (int i = lo; i <= hi; ++i) {
        cout << a[i];
        if (i < hi) cout << ", ";
    }
    cout << "]";
}

static void printArray(const int* a, int n) {
    if (n == 0) { cout << "[]"; return; }
    printRange(a, 0, n - 1);
}

static void copyArray(const int* src, int* dst, int n) {
    for (int i = 0; i < n; ++i) dst[i] = src[i];
}

// ------------------------------------------------------- розбиття Ломуто

// Опорний елемент - правий край. Після виконання він стоїть на остаточній позиції.
static int lomutoPartition(int* a, int lo, int hi, Stats& s) {
    int pivot = a[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; ++j) {
        s.comparisons++;
        if (a[j] <= pivot) {
            ++i;
            swapEl(a[i], a[j], s);
        }
    }
    swapEl(a[i + 1], a[hi], s);
    return i + 1;
}

// ------------------------------------------------------- розбиття Гоара

// Опорне значення - середній елемент. Два індекси рухаються назустріч.
// Повернена межа j: ліва ділянка [lo..j], права [j+1..hi].
// Опорне значення НЕ обов'язково стоїть на позиції j.
static int hoarePartition(int* a, int lo, int hi, Stats& s) {
    int pivot = a[lo + (hi - lo) / 2];
    int i = lo - 1;
    int j = hi + 1;
    while (true) {
        do { ++i; s.comparisons++; } while (a[i] < pivot);
        do { --j; s.comparisons++; } while (a[j] > pivot);
        if (i >= j) {
            if (g_trace) cout << "    індекси зустрілися: i=" << i << ", j=" << j << "\n";
            return j;
        }
        swapEl(a[i], a[j], s);
    }
}

// ------------------------------------------------------------- QuickSort

static void quickSortLomuto(int* a, int lo, int hi, int depth, Stats& s) {
    s.calls++;
    if (depth > s.maxDepth) s.maxDepth = depth;
    int id = g_trace ? ++g_callId : 0;

    if (lo >= hi) {
        if (g_trace)
            cout << "виклик #" << id << ", глибина " << depth << ", lo=" << lo << ", hi=" << hi
                 << " -> базовий випадок (ділянка з <2 елементів)\n";
        return;
    }
    if (g_trace) {
        cout << "виклик #" << id << ", глибина " << depth << ", lo=" << lo << ", hi=" << hi
             << ", опорне=" << a[hi] << ", ділянка ";
        printRange(a, lo, hi);
        cout << "\n";
    }
    int p = lomutoPartition(a, lo, hi, s);
    if (g_trace) {
        cout << "    p=" << p << ", ділянка після розбиття ";
        printRange(a, lo, hi);
        cout << "\n    наступні: ліва [" << lo << ".." << p - 1 << "], права [" << p + 1 << ".." << hi << "]\n";
    }
    quickSortLomuto(a, lo, p - 1, depth + 1, s);
    quickSortLomuto(a, p + 1, hi, depth + 1, s);
}

static void quickSortHoare(int* a, int lo, int hi, int depth, Stats& s) {
    s.calls++;
    if (depth > s.maxDepth) s.maxDepth = depth;
    int id = g_trace ? ++g_callId : 0;

    if (lo >= hi) {
        if (g_trace)
            cout << "виклик #" << id << ", глибина " << depth << ", lo=" << lo << ", hi=" << hi
                 << " -> базовий випадок (ділянка з <2 елементів)\n";
        return;
    }
    if (g_trace) {
        cout << "виклик #" << id << ", глибина " << depth << ", lo=" << lo << ", hi=" << hi
             << ", опорне=" << a[lo + (hi - lo) / 2] << ", ділянка ";
        printRange(a, lo, hi);
        cout << "\n";
    }
    int p = hoarePartition(a, lo, hi, s);
    if (g_trace) {
        cout << "    p=" << p << ", ділянка після розбиття ";
        printRange(a, lo, hi);
        cout << "\n    наступні: ліва [" << lo << ".." << p << "], права [" << p + 1 << ".." << hi << "]\n";
    }
    quickSortHoare(a, lo, p, depth + 1, s);
    quickSortHoare(a, p + 1, hi, depth + 1, s);
}

// ------------------------------------------------------ автоматична перевірка

struct Fingerprint {
    int n = 0;
    long long sum = 0;
    long long sumSq = 0;
};

static Fingerprint fingerprint(const int* a, int n) {
    Fingerprint f;
    f.n = n;
    for (int i = 0; i < n; ++i) {
        f.sum += a[i];
        f.sumSq += static_cast<long long>(a[i]) * a[i];
    }
    return f;
}

static bool sameFingerprint(const Fingerprint& x, const Fingerprint& y) {
    return x.n == y.n && x.sum == y.sum && x.sumSq == y.sumSq;
}

static bool isSorted(const int* a, int n) {
    for (int i = 1; i < n; ++i)
        if (a[i - 1] > a[i]) return false;
    return true;
}

enum Scheme { LOMUTO = 0, HOARE = 1 };

static const char* schemeName(Scheme sc) { return sc == LOMUTO ? "Ломуто" : "Гоар"; }

// Сортує копію масиву обраною схемою та перевіряє результат.
// Повертає true, якщо масив впорядкований і склад не змінився.
static bool runSort(Scheme sc, const int* src, int n, int* out, Stats& s) {
    s.reset();
    copyArray(src, out, n);
    Fingerprint before = fingerprint(out, n);
    if (sc == LOMUTO) quickSortLomuto(out, 0, n - 1, 1, s);
    else quickSortHoare(out, 0, n - 1, 1, s);
    Fingerprint after = fingerprint(out, n);
    return isSorted(out, n) && sameFingerprint(before, after);
}

// ------------------------------------------------------------ автоматичні тести

struct TestCase {
    const char* purpose;
    const int* data;
    int n;
    const int* expected;
};

static const int T1_in[] = {0};
static const int T2_in[] = {7}, T2_ex[] = {7};
static const int T3_in[] = {1, 2}, T3_ex[] = {1, 2};
static const int T4_in[] = {2, 1}, T4_ex[] = {1, 2};
static const int T5_in[] = {1, 2, 3, 4, 5}, T5_ex[] = {1, 2, 3, 4, 5};
static const int T6_in[] = {5, 4, 3, 2, 1}, T6_ex[] = {1, 2, 3, 4, 5};
static const int T7_in[] = {4, 1, 5, 2, 3}, T7_ex[] = {1, 2, 3, 4, 5};
static const int T8_in[] = {3, 1, 3, 2, 1}, T8_ex[] = {1, 1, 2, 3, 3};
static const int T9_in[] = {0, -5, 8, -2, 4}, T9_ex[] = {-5, -2, 0, 4, 8};
static const int T10_in[] = {6, 6, 6, 6}, T10_ex[] = {6, 6, 6, 6};
static const int T11_in[] = {9, -1, 9, 0, -1, 5}, T11_ex[] = {-1, -1, 0, 5, 9, 9};
static const int T12_in[] = {3, 2, 1, 4, 5}, T12_ex[] = {1, 2, 3, 4, 5};
// власні тести
static const int T13_in[] = {100, -100, 0, 100, -100}, T13_ex[] = {-100, -100, 0, 100, 100};
static const int T14_in[] = {2, 2, 1, 1, 3, 3}, T14_ex[] = {1, 1, 2, 2, 3, 3};

static const TestCase TESTS[] = {
    {"порожній масив", T1_in, 0, T1_in},
    {"один елемент", T2_in, 1, T2_ex},
    {"два впорядковані", T3_in, 2, T3_ex},
    {"два переставлені", T4_in, 2, T4_ex},
    {"зростання", T5_in, 5, T5_ex},
    {"спадання", T6_in, 5, T6_ex},
    {"змішаний порядок", T7_in, 5, T7_ex},
    {"повторення", T8_in, 5, T8_ex},
    {"від'ємні числа", T9_in, 5, T9_ex},
    {"усі рівні", T10_in, 4, T10_ex},
    {"комбінований випадок", T11_in, 6, T11_ex},
    {"частково впорядкований", T12_in, 5, T12_ex},
    {"власний: крайні значення з повторами", T13_in, 5, T13_ex},
    {"власний: пари повторів", T14_in, 6, T14_ex},
};
static const int TEST_COUNT = sizeof(TESTS) / sizeof(TESTS[0]);

static int runAutomatedTests() {
    cout << "=== Автоматизовані тести сортування ===\n";
    int passed = 0, total = 0;
    int buf[16];
    for (int t = 0; t < TEST_COUNT; ++t) {
        for (int sc = 0; sc < 2; ++sc) {
            Stats s;
            bool ok = runSort(static_cast<Scheme>(sc), TESTS[t].data, TESTS[t].n, buf, s);
            // порівняння з очікуваним результатом
            for (int i = 0; i < TESTS[t].n && ok; ++i)
                if (buf[i] != TESTS[t].expected[i]) ok = false;
            ++total;
            if (ok) ++passed;
            cout << "тест " << setw(2) << (t + 1) << " | " << setw(6) << left
                 << schemeName(static_cast<Scheme>(sc)) << right << " | " << (ok ? "OK  " : "FAIL")
                 << " | " << TESTS[t].purpose << "\n";
        }
    }
    cout << "Успішних сортувань: " << passed << " з " << total << "\n\n";
    return passed == total ? passed : -1;
}

// ------------------------------------------------- автоматичні перевірки розбиття

static bool checkLomutoPartition(const char* label, const int* src, int n) {
    int a[16];
    Stats s;
    copyArray(src, a, n);
    Fingerprint before = fingerprint(a, n);
    int pivot = a[n - 1];
    int p = lomutoPartition(a, 0, n - 1, s);
    bool ok = (p >= 0 && p < n) && a[p] == pivot && sameFingerprint(before, fingerprint(a, n));
    for (int i = 0; ok && i < p; ++i)
        if (a[i] > pivot) ok = false;
    for (int i = p + 1; ok && i < n; ++i)
        if (a[i] <= pivot) ok = false;
    cout << "Ломуто | " << (ok ? "OK  " : "FAIL") << " | " << label << " | p=" << p << " | ";
    printArray(a, n);
    cout << "\n";
    return ok;
}

static bool checkHoarePartition(const char* label, const int* src, int n) {
    int a[16];
    Stats s;
    copyArray(src, a, n);
    Fingerprint before = fingerprint(a, n);
    int pivot = a[(n - 1) / 2];
    int p = hoarePartition(a, 0, n - 1, s);
    bool ok = (p >= 0 && p < n - 1) && sameFingerprint(before, fingerprint(a, n));
    for (int i = 0; ok && i <= p; ++i)
        if (a[i] > pivot) ok = false;
    for (int i = p + 1; ok && i < n; ++i)
        if (a[i] < pivot) ok = false;
    cout << "Гоар   | " << (ok ? "OK  " : "FAIL") << " | " << label << " | p=" << p << " | ";
    printArray(a, n);
    cout << "\n";
    return ok;
}

static int runPartitionChecks() {
    cout << "=== Автоматичні перевірки функцій розбиття ===\n";
    static const int L_min[] = {6, 2, 8, 4, 1};   // опорне (правий край) - найменше
    static const int L_max[] = {3, 7, 1, 5, 9};   // опорне - найбільше
    static const int L_mid[] = {6, 2, 8, 1, 4};   // опорне - середнє за значенням
    static const int L_dup[] = {4, 2, 4, 7, 4};   // опорне повторюється
    static const int H_min[] = {6, 8, 1, 9, 4};   // середній елемент (індекс 2) - найменше
    static const int H_max[] = {3, 1, 9, 2, 5};   // середній елемент - найбільше
    static const int H_mid[] = {6, 2, 4, 1, 8};   // середній елемент - середнє за значенням
    static const int H_dup[] = {4, 4, 4, 2, 4};   // опорне значення повторюється
    int ok = 0;
    ok += checkLomutoPartition("опорне найменше", L_min, 5);
    ok += checkLomutoPartition("опорне найбільше", L_max, 5);
    ok += checkLomutoPartition("опорне середнє", L_mid, 5);
    ok += checkLomutoPartition("опорне повторюється", L_dup, 5);
    ok += checkHoarePartition("опорне найменше", H_min, 5);
    ok += checkHoarePartition("опорне найбільше", H_max, 5);
    ok += checkHoarePartition("опорне середнє", H_mid, 5);
    ok += checkHoarePartition("опорне повторюється", H_dup, 5);
    cout << "Успішних перевірок розбиття: " << ok << " з 8\n\n";
    return ok == 8 ? ok : -1;
}

// ---------------------------------------------------------------- трасування

static void traceArray(const char* title, const int* src, int n) {
    int* a = new int[n];
    Stats s;
    for (int sc = 0; sc < 2; ++sc) {
        cout << "--- Трасування: " << title << ", схема " << schemeName(static_cast<Scheme>(sc)) << " ---\n";
        cout << "вихідний масив: ";
        printArray(src, n);
        cout << "\n";
        g_trace = (n <= 30);   // для великих масивів трасування вимкнено
        g_callId = 0;
        bool ok = runSort(static_cast<Scheme>(sc), src, n, a, s);
        g_trace = false;
        cout << "результат: ";
        printArray(a, n);
        cout << " | статус: " << (ok ? "OK" : "FAIL") << "\n";
        cout << "порівнянь=" << s.comparisons << ", обмінів=" << s.swaps << ", викликів=" << s.calls
             << ", макс. глибина=" << s.maxDepth << "\n\n";
    }
    delete[] a;
}

// ---------------------------------------------------------------- експеримент

static unsigned long long g_rng = 12345ULL;
static unsigned int nextRand() {   // власний лінійний конгруентний генератор (повторюваний)
    g_rng = g_rng * 6364136223846793005ULL + 1442695040888963407ULL;
    return static_cast<unsigned int>(g_rng >> 33);
}

static void generate(int type, int* a, int n) {
    switch (type) {
        case 0: for (int i = 0; i < n; ++i) a[i] = i + 1; break;          // зростання
        case 1: for (int i = 0; i < n; ++i) a[i] = n - i; break;          // спадання
        case 2:                                                           // змішаний
            for (int i = 0; i < n; ++i) a[i] = i + 1;
            for (int i = n - 1; i > 0; --i) {
                int j = static_cast<int>(nextRand() % static_cast<unsigned>(i + 1));
                int t = a[i]; a[i] = a[j]; a[j] = t;
            }
            break;
        default:                                                          // багато однакових (10 різних значень)
            for (int i = 0; i < n; ++i) a[i] = static_cast<int>(nextRand() % 10u);
    }
}

static const char* typeName(int t) {
    static const char* names[] = {"Зростання", "Спадання", "Змішаний", "Багато однакових"};
    return names[t];
}

static bool runExperiment() {
    cout << "=== Порівняльний експеримент (порівняння / обміни / глибина) ===\n";
    static const int sizes[] = {100, 1000, 10000};
    bool allOk = true;
    cout << "n\tНабір\tЛомуто: cmp / swap / depth\tГоар: cmp / swap / depth\tстатус\n";
    for (int si = 0; si < 3; ++si) {
        int n = sizes[si];
        int* base = new int[n];
        int* out = new int[n];
        for (int type = 0; type < 4; ++type) {
            g_rng = 12345ULL + static_cast<unsigned long long>(n) * 10 + type;   // фіксоване зерно
            generate(type, base, n);
            Stats sl, sh;
            bool okL = runSort(LOMUTO, base, n, out, sl);
            bool okH = runSort(HOARE, base, n, out, sh);
            allOk = allOk && okL && okH;
            cout << n << "\t" << typeName(type) << "\t";
            cout << sl.comparisons << " / " << sl.swaps << " / " << sl.maxDepth << "\t";
            cout << sh.comparisons << " / " << sh.swaps << " / " << sh.maxDepth << "\t";
            cout << ((okL && okH) ? "OK" : "FAIL") << "\n";
        }
        delete[] base;
        delete[] out;
    }
    cout << "Усього пар запусків: 12\n\n";
    return allOk;
}

// ------------------------------------------------------------------- головне

int main(int argc, char** argv) {
    if (argc > 1 && strcmp(argv[1], "input") == 0) {
        int n;
        cout << "Введіть n: ";
        if (!(cin >> n) || n < 0 || n > 100000) { cout << "Некоректне n\n"; return 1; }
        int* a = new int[n > 0 ? n : 1];
        cout << "Введіть " << n << " цілих чисел: ";
        for (int i = 0; i < n; ++i) cin >> a[i];
        traceArray("введений масив", a, n);
        delete[] a;
        return 0;
    }

    int tests = runAutomatedTests();
    int parts = runPartitionChecks();
    if (tests < 0 || parts < 0) {
        cout << "Тести не пройдені - експеримент не виконується.\n";
        return 1;
    }
    cout << "Умова переходу до експерименту виконана: усі тести пройдено.\n\n";

    static const int example[] = {6, 2, 8, 1, 4};
    traceArray("приклад із лекції (5 елементів)", example, 5);

    // Індивідуальний варіант 26
    static const int variant26[] = {48, 24, 32, 19, 43, 27, 16, 39, 22, 35, 15, 46, 29, 20, 38};
    traceArray("індивідуальний варіант 26", variant26, 15);

    bool ok = runExperiment();
    cout << "Мінімальний результат: " << tests << " успішних сортувань і " << parts
         << " успішних перевірок розбиття.\n";
    return ok ? 0 : 1;
}
