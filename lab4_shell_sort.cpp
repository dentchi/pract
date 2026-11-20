#include <iostream>

using namespace std;

const int MAX_N = 10000;
long long comparisons, moves;
int usedSteps[40];
int stepsCount;
bool demoMode = false;
int failed = 0;

void printArray(const int a[], int n) {
    for (int i = 0; i < n; i++) cout << a[i] << (i + 1 < n ? ", " : "");
}

void printSteps() {
    for (int i = 0; i < stepsCount; i++) cout << usedSteps[i] << (i + 1 < stepsCount ? " -> " : "");
}

void buildSteps(int n, int type) {
    stepsCount = 0;
    if (type == 0) {
        for (int gap = n / 2; gap >= 1; gap /= 2) usedSteps[stepsCount++] = gap;
    } else {
        int increasing[40], count = 0;
        for (int h = 1; h < n; h = 3 * h + 1) increasing[count++] = h;
        for (int i = count - 1; i >= 0; i--) usedSteps[stepsCount++] = increasing[i];
    }
}

void shellSort(int a[], int n, int type) {
    buildSteps(n, type);
    for (int s = 0; s < stepsCount; s++) {
        int gap = usedSteps[s];
        for (int i = gap; i < n; i++) {
            int key = a[i], j = i;
            moves++;
            while (j >= gap) {
                comparisons++;
                if (a[j - gap] <= key) break;
                a[j] = a[j - gap];
                moves++;
                j -= gap;
            }
            a[j] = key;
            moves++;
        }
        if (demoMode) {
            cout << "  після кроку " << gap << ": ";
            printArray(a, n);
            cout << "\n";
        }
    }
}

void insertionSort(int a[], int n) {
    stepsCount = 1;
    usedSteps[0] = 1;
    for (int i = 1; i < n; i++) {
        int key = a[i], j = i - 1;
        moves++;
        while (j >= 0) {
            comparisons++;
            if (a[j] <= key) break;
            a[j + 1] = a[j];
            moves++;
            j--;
        }
        a[j + 1] = key;
        moves++;
    }
}

void shellHalving(int a[], int n) { shellSort(a, n, 0); }
void shellKnuth(int a[], int n) { shellSort(a, n, 1); }

bool isCorrect(const int before[], const int after[], int n) {
    long long sum1 = 0, sq1 = 0, sum2 = 0, sq2 = 0;
    for (int i = 0; i < n; i++) {
        sum1 += before[i];
        sq1 += (long long)before[i] * before[i];
        sum2 += after[i];
        sq2 += (long long)after[i] * after[i];
        if (i > 0 && after[i] < after[i - 1]) return false;
    }
    return sum1 == sum2 && sq1 == sq2;
}

bool stepsAreValid(int n) {
    if (n < 2) return stepsCount == 0;
    return usedSteps[stepsCount - 1] == 1 && usedSteps[0] < n;
}

void copyArray(const int from[], int to[], int n) {
    for (int i = 0; i < n; i++) to[i] = from[i];
}

typedef void (*SortFunction)(int a[], int n);
const char* names[3] = {"Вставки", "Шелл: ділення навпіл", "Шелл: 3h+1"};
SortFunction algorithms[3] = {insertionSort, shellHalving, shellKnuth};

void runSort(int k, const int base[], int work[], int n) {
    copyArray(base, work, n);
    comparisons = moves = 0;
    algorithms[k](work, n);
}

void runTests() {
    int tests[12][6] = {{0}, {4}, {1, 2, 3, 4, 5}, {5, 4, 3, 2, 1}, {7, 2, 9, 1, 5}, {3, 1, 3, 2, 1},
                        {0, -6, 8, -2, 4}, {6, 6, 6, 6}, {2, 1}, {1, 2, 4, 3, 5, 6},
                        {9, 9, 1, 1, 5}, {-3, 10, -3, 0, 10, -7}};
    int expected[12][6] = {{0}, {4}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, {1, 2, 5, 7, 9}, {1, 1, 2, 3, 3},
                           {-6, -2, 0, 4, 8}, {6, 6, 6, 6}, {1, 2}, {1, 2, 3, 4, 5, 6},
                           {1, 1, 5, 9, 9}, {-7, -3, -3, 0, 10, 10}};
    int sizes[12] = {0, 1, 5, 5, 5, 5, 5, 4, 2, 6, 5, 6};
    for (int t = 0; t < 12; t++) {
        for (int k = 1; k < 3; k++) {
            int work[6];
            runSort(k, tests[t], work, sizes[t]);
            bool ok = isCorrect(tests[t], work, sizes[t]) && stepsAreValid(sizes[t]);
            for (int i = 0; i < sizes[t]; i++) {
                if (work[i] != expected[t][i]) ok = false;
            }
            cout << names[k] << "\tтест " << t + 1 << "\t" << (ok ? "успішно" : "помилка");
            if (!ok) {
                cout << ", отримано: ";
                printArray(work, sizes[t]);
                failed++;
            }
            cout << "\n";
        }
    }
}

void runAlgorithms(const int base[], int n, const char* setName) {
    for (int k = 0; k < 3; k++) {
        int work[MAX_N];
        runSort(k, base, work, n);
        cout << n << "\t" << setName << "\t" << names[k] << "\t" << comparisons << "\t" << moves
             << "\t" << stepsCount << "\t" << (isCorrect(base, work, n) ? "успішно" : "помилка") << "\n";
    }
}

void runExperiment() {
    const int sizes[3] = {100, 1000, 10000};
    const char* setNames[4] = {"Впорядкований", "Зворотний", "Майже впорядкований", "Змішаний"};
    cout << "\nn\tНабір\tАлгоритм\tПорівняння\tПереміщення\tКроки\tПеревірка\n";
    for (int z = 0; z < 3; z++) {
        int n = sizes[z];
        for (int type = 0; type < 2; type++) {
            buildSteps(n, type);
            cout << "Кроки (" << names[type + 1] << "), n = " << n << ": ";
            printSteps();
            cout << "\n";
        }
        unsigned long long x = 12345;
        for (int t = 0; t < 4; t++) {
            int base[MAX_N];
            for (int i = 0; i < n; i++) {
                if (t == 1) {
                    base[i] = n - i;
                } else if (t == 3) {
                    x = (x * 1103515245ULL + 12345ULL) % 2147483648ULL;
                    base[i] = (int)((x / 65536) % 10000);
                } else {
                    base[i] = i + 1;
                }
            }
            if (t == 2) {
                for (int i = 0; i + 1 < n; i += 10) {
                    int temp = base[i];
                    base[i] = base[i + 1];
                    base[i + 1] = temp;
                }
            }
            runAlgorithms(base, n, setNames[t]);
        }
    }
}

int main() {
    runTests();
    if (failed > 0) {
        cout << "Знайдено помилок: " << failed << "\n";
        return 1;
    }
    cout << "Усі перевірки пройдено\n";
    runExperiment();
    int n, base[MAX_N];
    cout << "\nВведіть кількість елементів (від 1 до " << MAX_N << "): ";
    if (!(cin >> n) || n < 1 || n > MAX_N) {
        cout << "Некоректна кількість елементів\n";
        return 1;
    }
    cout << "Введіть " << n << " цілих чисел: ";
    for (int i = 0; i < n; i++) cin >> base[i];
    cout << "\nВихідний масив: ";
    printArray(base, n);
    cout << "\n";
    demoMode = true;
    for (int k = 1; k < 3; k++) {
        int work[MAX_N];
        cout << names[k] << "\n";
        runSort(k, base, work, n);
        cout << "  кроки: ";
        printSteps();
        cout << "\n";
    }
    demoMode = false;
    cout << "\nАлгоритм\tПорівняння\tПереміщення\tКроки\tПеревірка\n";
    for (int k = 0; k < 3; k++) {
        int work[MAX_N];
        runSort(k, base, work, n);
        cout << names[k] << "\t" << comparisons << "\t" << moves << "\t" << stepsCount << "\t"
             << (isCorrect(base, work, n) ? "успішно" : "помилка") << "\n";
    }
    return 0;
}
