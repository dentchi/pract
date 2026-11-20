#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const int MAXT = 10001;

struct Stats {
    long long comparisons = 0;
    long long prefixComparisons = 0;
    long long shifts = 0;
    int matches = 0;
    int textBack = 0;
};

int lpsArr[MAXT];
char expText[MAXT];

int textLength(const char s[]) {
    int n = 0;
    while (s[n] != '\0') n++;
    return n;
}

void buildPrefix(const char pat[], int m, int lps[], long long& comps) {
    if (m == 0) return;
    lps[0] = 0;
    int k = 0;
    for (int i = 1; i < m; i++) {
        while (true) {
            comps++;
            if (pat[i] == pat[k]) {
                k++;
                break;
            }
            if (k == 0) break;
            k = lps[k - 1];
        }
        lps[i] = k;
    }
}

void printPrefix(const char pat[]) {
    int m = textLength(pat);
    long long comps = 0;
    buildPrefix(pat, m, lpsArr, comps);
    cout << "Індекс: ";
    for (int i = 0; i < m; i++) cout << i << " ";
    cout << "\nСимвол: ";
    for (int i = 0; i < m; i++) cout << pat[i] << " ";
    cout << "\nLPS:    ";
    for (int i = 0; i < m; i++) cout << lpsArr[i] << " ";
    cout << "\nПорівнянь при побудові: " << comps << "\n";
}

void traceStep(int i, int j, char a, char b, bool eq) {
    cout << "  i=" << i << ", j=" << j << ": '" << a << "' і '" << b << "' " << (eq ? "збіг" : "різні");
}

int kmpFirst(const char text[], const char pat[], Stats& st, bool trace) {
    st = Stats();
    int n = textLength(text);
    int m = textLength(pat);
    if (m == 0) {
        st.matches = 1;
        return 0;
    }
    if (m > n) return -1;
    buildPrefix(pat, m, lpsArr, st.prefixComparisons);
    int i = 0, j = 0, lastI = 0;
    while (i < n) {
        st.comparisons++;
        bool eq = (text[i] == pat[j]);
        if (trace) traceStep(i, j, text[i], pat[j], eq);
        if (eq) {
            i++;
            j++;
            if (j == m) {
                st.matches = 1;
                if (trace) cout << " -> повний збіг на позиції " << i - m << "\n";
                return i - m;
            }
            if (trace) cout << " -> довжина збігу " << j << "\n";
        } else if (j > 0) {
            j = lpsArr[j - 1];
            st.shifts++;
            if (trace) {
                cout << " -> довжина збігу " << j;
                if (j > 0) cout << " [префікс-функція: не з початку зразка]";
                cout << "\n";
            }
        } else {
            i++;
            st.shifts++;
            if (trace) cout << " -> довжина збігу 0, далі i=" << i << "\n";
        }
        if (i < lastI) st.textBack++;
        lastI = i;
    }
    return -1;
}

int kmpAll(const char text[], const char pat[], int result[], Stats& st, bool trace) {
    st = Stats();
    int n = textLength(text);
    int m = textLength(pat);
    if (m == 0) {
        result[0] = 0;
        st.matches = 1;
        return 1;
    }
    if (m > n) return 0;
    buildPrefix(pat, m, lpsArr, st.prefixComparisons);
    int i = 0, j = 0, count = 0, lastI = 0;
    while (i < n) {
        st.comparisons++;
        bool eq = (text[i] == pat[j]);
        if (trace) traceStep(i, j, text[i], pat[j], eq);
        if (eq) {
            i++;
            j++;
            if (j == m) {
                result[count] = i - m;
                count++;
                j = lpsArr[m - 1];
                st.shifts++;
                if (trace) {
                    cout << " -> повний збіг на позиції " << i - m << ", довжина збігу " << j;
                    if (j > 0) cout << " [префікс-функція: не з початку зразка]";
                    cout << "\n";
                }
            } else if (trace) {
                cout << " -> довжина збігу " << j << "\n";
            }
        } else if (j > 0) {
            j = lpsArr[j - 1];
            st.shifts++;
            if (trace) {
                cout << " -> довжина збігу " << j;
                if (j > 0) cout << " [префікс-функція: не з початку зразка]";
                cout << "\n";
            }
        } else {
            i++;
            st.shifts++;
            if (trace) cout << " -> довжина збігу 0, далі i=" << i << "\n";
        }
        if (i < lastI) st.textBack++;
        lastI = i;
    }
    st.matches = count;
    return count;
}

int directFirst(const char text[], const char pat[], Stats& st) {
    st = Stats();
    int n = textLength(text);
    int m = textLength(pat);
    if (m == 0) {
        st.matches = 1;
        return 0;
    }
    if (m > n) return -1;
    for (int i = 0; i <= n - m; i++) {
        if (i > 0) st.shifts++;
        int j = 0;
        while (j < m) {
            st.comparisons++;
            if (text[i + j] != pat[j]) break;
            j++;
        }
        if (j == m) {
            st.matches = 1;
            return i;
        }
    }
    return -1;
}

int directAll(const char text[], const char pat[], int result[], Stats& st) {
    st = Stats();
    int n = textLength(text);
    int m = textLength(pat);
    if (m == 0) {
        result[0] = 0;
        st.matches = 1;
        return 1;
    }
    if (m > n) return 0;
    int count = 0;
    for (int i = 0; i <= n - m; i++) {
        if (i > 0) st.shifts++;
        int j = 0;
        while (j < m) {
            st.comparisons++;
            if (text[i + j] != pat[j]) break;
            j++;
        }
        if (j == m) {
            result[count] = i;
            count++;
        }
    }
    st.matches = count;
    return count;
}

void printPositions(const int pos[], int count) {
    if (count == 0) {
        cout << "немає";
        return;
    }
    for (int i = 0; i < count; i++) {
        cout << pos[i];
        if (i + 1 < count) cout << ", ";
    }
}

bool samePositions(const int a[], const int b[], int count) {
    for (int i = 0; i < count; i++)
        if (a[i] != b[i]) return false;
    return true;
}

struct PrefixTest { const char* pat; int expected[10]; };

bool runPrefixTests() {
    PrefixTest tests[] = {
        {"a", {0}},
        {"aaaa", {0, 1, 2, 3}},
        {"abcd", {0, 0, 0, 0}},
        {"ababaca", {0, 0, 1, 2, 3, 0, 1}},
        {"aabaaab", {0, 1, 0, 1, 2, 2, 3}},
        {"abcababcab", {0, 0, 0, 1, 2, 1, 2, 3, 4, 5}},
        {"zzzzy", {0, 1, 2, 3, 0}},
    };
    int total = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;
    cout << "=== Тести префікс-функції ===\n";
    for (int t = 0; t < total; t++) {
        int m = textLength(tests[t].pat);
        long long comps = 0;
        buildPrefix(tests[t].pat, m, lpsArr, comps);
        int bad = -1;
        for (int i = 0; i < m; i++) {
            if (lpsArr[i] != tests[t].expected[i]) {
                bad = i;
                break;
            }
        }
        cout << t + 1 << ". " << tests[t].pat << ": ";
        for (int i = 0; i < m; i++) cout << lpsArr[i] << (i + 1 < m ? ", " : "");
        cout << " (порівнянь " << comps << ")";
        if (bad == -1) {
            passed++;
            cout << " | успішно\n";
        } else {
            cout << " | ПОМИЛКА: індекс " << bad << ", символ '" << tests[t].pat[bad]
                 << "', очікувано " << tests[t].expected[bad] << ", отримано " << lpsArr[bad] << "\n";
        }
    }
    cout << "Успішних перевірок префікс-функції: " << passed << " з " << total << "\n\n";
    return passed == total;
}

struct SearchTest { const char* text; const char* pat; int first; int count; int all[5]; };

void runSearchTests() {
    SearchTest tests[] = {
        {"algorithm", "rith", 4, 1, {4}},
        {"aaaa", "aa", 0, 3, {0, 1, 2}},
        {"abababa", "aba", 0, 3, {0, 2, 4}},
        {"abc", "abcd", -1, 0, {}},
        {"mississippi", "issi", 1, 2, {1, 4}},
        {"abcabcabd", "abcabd", 3, 1, {3}},
        {"no matches here", "xyz", -1, 0, {}},
        {"", "a", -1, 0, {}},
        {"abababcababc", "ababc", 2, 2, {2, 7}},
        {"abcabcabcabc", "abcabc", 0, 3, {0, 3, 6}},
        {"abc", "", 0, 1, {0}},
    };
    int total = sizeof(tests) / sizeof(tests[0]);
    int firstPassed = 0, allPassed = 0;
    int pos[MAXT], pos2[MAXT];
    cout << "=== Тести пошуку КМП ===\n";
    for (int t = 0; t < total; t++) {
        Stats k1, k2, d1, d2;
        int first = kmpFirst(tests[t].text, tests[t].pat, k1, false);
        int count = kmpAll(tests[t].text, tests[t].pat, pos, k2, false);
        directFirst(tests[t].text, tests[t].pat, d1);
        int dCount = directAll(tests[t].text, tests[t].pat, pos2, d2);
        bool okFirst = (first == tests[t].first) && k1.textBack == 0;
        bool okAll = (count == tests[t].count) && samePositions(pos, tests[t].all, count) && k2.textBack == 0
                     && dCount == count && samePositions(pos, pos2, count);
        if (okFirst) firstPassed++;
        if (okAll) allPassed++;
        cout << t + 1 << ". \"" << tests[t].text << "\", \"" << tests[t].pat << "\": перше " << first << ", усі: ";
        printPositions(pos, count);
        cout << " | " << (okFirst ? "успішно" : "ПОМИЛКА") << " / " << (okAll ? "успішно" : "ПОМИЛКА") << "\n";
        cout << "   КМП: перше пор. " << k1.comparisons << ", зміщ. " << k1.shifts
             << "; усі пор. " << k2.comparisons << ", зміщ. " << k2.shifts
             << "; префікс-функція " << k2.prefixComparisons << "\n";
        cout << "   Прямий: перше пор. " << d1.comparisons << ", зміщ. " << d1.shifts
             << "; усі пор. " << d2.comparisons << ", зміщ. " << d2.shifts << "\n";
    }
    cout << "Успішних перевірок першого входження: " << firstPassed << " з " << total << "\n";
    cout << "Успішних перевірок усіх входжень: " << allPassed << " з " << total << "\n\n";
}

void makeText(char t[], int n, char scenario, const char pat[]) {
    int m = textLength(pat);
    for (int i = 0; i < n; i++) {
        if (scenario == 'D') t[i] = 'a';
        else t[i] = 'c' + i % 24;
    }
    t[n] = '\0';
    int start = -1;
    if (scenario == 'A') start = 0;
    if (scenario == 'B') start = (n - m) / 2;
    if (scenario == 'C') start = n - m;
    if (start >= 0)
        for (int j = 0; j < m; j++) t[start + j] = pat[j];
}

void runComparison() {
    const char pat[] = "aaaaaaab";
    int sizes[3] = {100, 1000, 10000};
    char scenarios[4] = {'A', 'B', 'C', 'D'};
    cout << "=== Порівняння КМП і прямого пошуку (зразок \"" << pat << "\") ===\n";
    for (int s = 0; s < 3; s++) {
        for (int k = 0; k < 4; k++) {
            makeText(expText, sizes[s], scenarios[k], pat);
            Stats ks, ds;
            int a = kmpFirst(expText, pat, ks, false);
            int b = directFirst(expText, pat, ds);
            cout << "n = " << sizes[s] << ", " << scenarios[k] << ": індекс " << a
                 << " | КМП пор. " << ks.comparisons << " + " << ks.prefixComparisons << ", зміщ. " << ks.shifts
                 << " | прямий пор. " << ds.comparisons << ", зміщ. " << ds.shifts
                 << " | " << (a == b ? "збігаються" : "ПОМИЛКА") << "\n";
        }
    }
    cout << "\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    if (!runPrefixTests()) {
        cout << "Префікс-функція працює неправильно, пошук не виконується\n";
        return 0;
    }
    runSearchTests();

    cout << "=== Трасування КМП: \"abababa\", \"aba\" ===\n";
    int pos[MAXT];
    Stats st;
    kmpAll("abababa", "aba", pos, st, true);
    cout << "Порівнянь " << st.comparisons << ", зміщень " << st.shifts << "\n\n";

    runComparison();

    char text[MAXT];
    char pat[MAXT];
    cout << "=== Індивідуальний варіант ===\n";
    cout << "Введіть основний текст: ";
    cin.getline(text, MAXT);
    cout << "Введіть зразок: ";
    cin.getline(pat, MAXT);
    printPrefix(pat);
    bool trace = textLength(text) <= 30;
    if (trace) cout << "Трасування:\n";
    Stats k2;
    int count = kmpAll(text, pat, pos, k2, trace);
    Stats k1;
    int first = kmpFirst(text, pat, k1, false);
    cout << "КМП: перше " << first << " (пор. " << k1.comparisons << ", зміщ. " << k1.shifts << "), усі: ";
    printPositions(pos, count);
    cout << " (пор. " << k2.comparisons << ", зміщ. " << k2.shifts << ", префікс-функція " << k2.prefixComparisons << ")\n";
    Stats d1, d2;
    int pos2[MAXT];
    int dFirst = directFirst(text, pat, d1);
    int dCount = directAll(text, pat, pos2, d2);
    cout << "Прямий: перше " << dFirst << " (пор. " << d1.comparisons << ", зміщ. " << d1.shifts << "), усі: ";
    printPositions(pos2, dCount);
    cout << " (пор. " << d2.comparisons << ", зміщ. " << d2.shifts << ")\n";
    return 0;
}
