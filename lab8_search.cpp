#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const int MAXT = 10001;

struct Stats {
    long long comparisons = 0;
    int positions = 0;
    int matches = 0;
};

int textLength(const char s[]) {
    int n = 0;
    while (s[n] != '\0') n++;
    return n;
}

char toLowerChar(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
    return c;
}

bool sameChar(char a, char b, bool ignoreCase) {
    if (ignoreCase) return toLowerChar(a) == toLowerChar(b);
    return a == b;
}

bool matchAt(const char text[], int start, const char pat[], int m, bool ignoreCase, Stats& st) {
    for (int j = 0; j < m; j++) {
        st.comparisons++;
        if (!sameChar(text[start + j], pat[j], ignoreCase)) return false;
    }
    return true;
}

int findFirst(const char text[], const char pat[], bool ignoreCase, Stats& st) {
    st = Stats();
    int n = textLength(text);
    int m = textLength(pat);
    if (m == 0) {
        st.matches = 1;
        return 0;
    }
    if (m > n) return -1;
    for (int i = 0; i <= n - m; i++) {
        st.positions++;
        if (matchAt(text, i, pat, m, ignoreCase, st)) {
            st.matches = 1;
            return i;
        }
    }
    return -1;
}

int findAll(const char text[], const char pat[], bool ignoreCase, int result[], Stats& st) {
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
        st.positions++;
        if (matchAt(text, i, pat, m, ignoreCase, st)) {
            result[count] = i;
            count++;
        }
    }
    st.matches = count;
    return count;
}

bool matchAtNum(const int a[], int start, const int p[], int m, Stats& st) {
    for (int j = 0; j < m; j++) {
        st.comparisons++;
        if (a[start + j] != p[j]) return false;
    }
    return true;
}

int findFirstNum(const int a[], int n, const int p[], int m, Stats& st) {
    st = Stats();
    if (m == 0) {
        st.matches = 1;
        return 0;
    }
    if (m > n) return -1;
    for (int i = 0; i <= n - m; i++) {
        st.positions++;
        if (matchAtNum(a, i, p, m, st)) {
            st.matches = 1;
            return i;
        }
    }
    return -1;
}

int findAllNum(const int a[], int n, const int p[], int m, int result[], Stats& st) {
    st = Stats();
    if (m == 0) {
        result[0] = 0;
        st.matches = 1;
        return 1;
    }
    if (m > n) return 0;
    int count = 0;
    for (int i = 0; i <= n - m; i++) {
        st.positions++;
        if (matchAtNum(a, i, p, m, st)) {
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

int firstPassed = 0, allPassed = 0, totalTests = 0;

void showResult(int first, int expFirst, const int pos[], int count, const int expAll[], int expCount,
                const Stats& s1, const Stats& s2) {
    bool okFirst = (first == expFirst);
    bool okAll = (count == expCount) && samePositions(pos, expAll, count);
    totalTests++;
    if (okFirst) firstPassed++;
    if (okAll) allPassed++;
    cout << "   перше: " << first << " (пор. " << s1.comparisons << ", поз. " << s1.positions << ") "
         << (okFirst ? "успішно" : "ПОМИЛКА") << "\n";
    cout << "   усі: ";
    printPositions(pos, count);
    cout << " (пор. " << s2.comparisons << ", поз. " << s2.positions << ", збігів " << s2.matches << ") "
         << (okAll ? "успішно" : "ПОМИЛКА") << "\n";
}

struct TextTest { int num; const char* text; const char* pat; int first; int count; int all[5]; };
struct NumTest { int num; int n; int a[10]; int m; int p[5]; int first; int count; int all[5]; };

void runTests() {
    TextTest tt[] = {
        {1, "algorithm", "algo", 0, 1, {0}},
        {2, "algorithm", "rith", 4, 1, {4}},
        {3, "algorithm", "thm", 6, 1, {6}},
        {4, "algorithm", "sort", -1, 0, {}},
        {5, "aaaa", "aa", 0, 3, {0, 1, 2}},
        {6, "abababa", "aba", 0, 3, {0, 2, 4}},
        {7, "abc", "abcd", -1, 0, {}},
        {8, "Data data DATA", "data", 5, 1, {5}},
        {9, "one two two", "two", 4, 2, {4, 8}},
        {12, "", "abc", -1, 0, {}},
        {13, "Mississippi", "issi", 1, 2, {1, 4}},
        {14, "Hello, world!", ", w", 5, 1, {5}},
        {17, "abc", "", 0, 1, {0}},
    };
    NumTest nt[] = {
        {10, 5, {1, 2, 1, 2, 1}, 3, {1, 2, 1}, 0, 2, {0, 2}},
        {11, 3, {4, 5, 6}, 2, {5, 7}, -1, 0, {}},
        {15, 5, {7, 7, 7, 7, 7}, 3, {7, 7, 7}, 0, 3, {0, 1, 2}},
        {16, 6, {-3, 0, -3, 0, -3, 1}, 2, {-3, 1}, 4, 1, {4}},
        {18, 3, {4, 5, 6}, 0, {}, 0, 1, {0}},
    };
    int pos[MAXT];
    Stats s1, s2;
    cout << "=== Автоматизовані тести ===\n";
    for (int t = 0; t < 13; t++) {
        cout << tt[t].num << ". текст \"" << tt[t].text << "\", зразок \"" << tt[t].pat << "\"\n";
        int first = findFirst(tt[t].text, tt[t].pat, false, s1);
        int count = findAll(tt[t].text, tt[t].pat, false, pos, s2);
        showResult(first, tt[t].first, pos, count, tt[t].all, tt[t].count, s1, s2);
    }
    for (int t = 0; t < 5; t++) {
        cout << nt[t].num << ". числовий тест\n";
        int first = findFirstNum(nt[t].a, nt[t].n, nt[t].p, nt[t].m, s1);
        int count = findAllNum(nt[t].a, nt[t].n, nt[t].p, nt[t].m, pos, s2);
        showResult(first, nt[t].first, pos, count, nt[t].all, nt[t].count, s1, s2);
    }
    cout << "Успішних тестів першого входження: " << firstPassed << " з " << totalTests << "\n";
    cout << "Успішних тестів усіх входжень: " << allPassed << " з " << totalTests << "\n\n";
}

void compareModes(const char text[], const char pat[]) {
    int pos[MAXT];
    Stats st;
    for (int mode = 0; mode < 2; mode++) {
        bool ignoreCase = (mode == 1);
        int first = findFirst(text, pat, ignoreCase, st);
        cout << (ignoreCase ? "Без урахування регістру: " : "З урахуванням регістру: ")
             << "перше " << first << " (пор. " << st.comparisons << ", поз. " << st.positions << "), усі: ";
        int count = findAll(text, pat, ignoreCase, pos, st);
        printPositions(pos, count);
        cout << " (пор. " << st.comparisons << ", поз. " << st.positions << ", збігів " << st.matches << ")\n";
    }
}

char expText[MAXT];

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

void runExperiment() {
    const char pat[] = "aaaaaaab";
    int sizes[3] = {100, 1000, 10000};
    char scenarios[4] = {'A', 'B', 'C', 'D'};
    cout << "=== Експеримент (зразок \"" << pat << "\") ===\n";
    for (int s = 0; s < 3; s++) {
        for (int k = 0; k < 4; k++) {
            makeText(expText, sizes[s], scenarios[k], pat);
            Stats st;
            int idx = findFirst(expText, pat, false, st);
            cout << "n = " << sizes[s] << ", сценарій " << scenarios[k] << ": індекс " << idx
                 << ", порівнянь " << st.comparisons << ", позицій " << st.positions << "\n";
        }
    }
    cout << "\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    runTests();

    cout << "=== Режими регістру ===\n";
    cout << "Текст \"Data data DATA\", зразок \"data\"\n";
    compareModes("Data data DATA", "data");
    cout << "\n";

    runExperiment();

    char text[MAXT];
    char pat[MAXT];
    cout << "=== Індивідуальний варіант ===\n";
    cout << "Введіть основний текст: ";
    cin.getline(text, MAXT);
    cout << "Введіть зразок: ";
    cin.getline(pat, MAXT);
    cout << "Довжина тексту " << textLength(text) << ", довжина зразка " << textLength(pat) << "\n";
    if (textLength(pat) > textLength(text)) cout << "Зразок довший за текст\n";
    compareModes(text, pat);
    return 0;
}
