#include <iostream>

using namespace std;

const int MAX_N = 100;

typedef void (*SortFunction)(int a[], int n, long long& comparisons, long long& moves);

void insertionSort(int a[], int n, long long& comparisons, long long& moves) {
    for (int i = 1; i < n; i++) {
        int key = a[i];
        moves++;
        int j = i - 1;
        while (j >= 0) {
            comparisons++;
            if (a[j] > key) {
                a[j + 1] = a[j];
                moves++;
                j--;
            } else {
                break;
            }
        }
        a[j + 1] = key;
        moves++;
    }
}

void selectionSort(int a[], int n, long long& comparisons, long long& moves) {
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            comparisons++;
            if (a[j] < a[minIndex]) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            int temp = a[i];
            moves++;
            a[i] = a[minIndex];
            moves++;
            a[minIndex] = temp;
            moves++;
        }
    }
}

void bubbleSort(int a[], int n, long long& comparisons, long long& moves) {
    for (int pass = 0; pass < n - 1; pass++) {
        bool swapped = false;
        for (int j = 0; j < n - pass - 1; j++) {
            comparisons++;
            if (a[j] > a[j + 1]) {
                int temp = a[j];
                moves++;
                a[j] = a[j + 1];
                moves++;
                a[j + 1] = temp;
                moves++;
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

bool isSorted(const int a[], int n) {
    for (int i = 1; i < n; i++) {
        if (a[i] < a[i - 1]) {
            return false;
        }
    }
    return true;
}

void getStats(const int a[], int n, long long& sum, long long& sumSquares) {
    sum = 0;
    sumSquares = 0;
    for (int i = 0; i < n; i++) {
        sum += a[i];
        sumSquares += (long long)a[i] * a[i];
    }
}

bool sameElements(const int before[], const int after[], int n) {
    long long s1, q1, s2, q2;
    getStats(before, n, s1, q1);
    getStats(after, n, s2, q2);
    return s1 == s2 && q1 == q2;
}

void copyArray(const int from[], int to[], int n) {
    for (int i = 0; i < n; i++) {
        to[i] = from[i];
    }
}

void printArray(const int a[], int n) {
    if (n == 0) {
        cout << "(порожній)";
        return;
    }
    for (int i = 0; i < n; i++) {
        cout << a[i] << (i + 1 < n ? ", " : "");
    }
}

const char* algorithmNames[3] = {"Вставки", "Вибір", "Бульбашковий"};
SortFunction algorithms[3] = {insertionSort, selectionSort, bubbleSort};

int failedChecks = 0;

void runSortTest(int number, const int initial[], int n, const int expected[]) {
    for (int k = 0; k < 3; k++) {
        int work[MAX_N];
        copyArray(initial, work, n);
        long long comparisons = 0, moves = 0;
        algorithms[k](work, n, comparisons, moves);
        bool ok = isSorted(work, n) && sameElements(initial, work, n);
        for (int i = 0; i < n; i++) {
            if (work[i] != expected[i]) {
                ok = false;
            }
        }
        cout << algorithmNames[k] << "\tтест " << number << "\t";
        if (ok) {
            cout << "успішно\n";
        } else {
            failedChecks++;
            cout << "помилка, фактичний масив: ";
            printArray(work, n);
            cout << "\n";
        }
    }
}

void runAllTests() {
    const int e0[1] = {0};
    const int t2[1] = {7};
    const int t3[5] = {1, 2, 3, 4, 5};
    const int t4[5] = {5, 4, 3, 2, 1};
    const int t5[5] = {4, 1, 5, 2, 3};
    const int t6[5] = {3, 1, 3, 2, 1};
    const int r6[5] = {1, 1, 2, 3, 3};
    const int t7[5] = {0, -5, 8, -2, 4};
    const int r7[5] = {-5, -2, 0, 4, 8};
    const int t8[4] = {6, 6, 6, 6};
    const int t9[2] = {2, 1};
    const int r9[2] = {1, 2};
    const int t10[6] = {9, -1, 9, 0, -1, 5};
    const int r10[6] = {-1, -1, 0, 5, 9, 9};
    const int t11[5] = {3, 3, 2, 2, 1};
    const int r11[5] = {1, 2, 2, 3, 3};
    const int t12[4] = {100, -100, 50, -50};
    const int r12[4] = {-100, -50, 50, 100};

    runSortTest(1, e0, 0, e0);
    runSortTest(2, t2, 1, t2);
    runSortTest(3, t3, 5, t3);
    runSortTest(4, t4, 5, t3);
    runSortTest(5, t5, 5, t3);
    runSortTest(6, t6, 5, r6);
    runSortTest(7, t7, 5, r7);
    runSortTest(8, t8, 4, t8);
    runSortTest(9, t9, 2, r9);
    runSortTest(10, t10, 6, r10);
    runSortTest(11, t11, 5, r11);
    runSortTest(12, t12, 4, r12);
}

void runExperiment() {
    const char* setNames[3] = {"Зростання", "Спадання", "Змішаний"};
    cout << "\nТаблиця. Порівняльний експеримент (n = 100)\n";
    cout << "Алгоритм\tТип набору\tПорівняння\tПереміщення\tПеревірка\n";
    for (int k = 0; k < 3; k++) {
        for (int s = 0; s < 3; s++) {
            int base[MAX_N];
            for (int i = 0; i < MAX_N; i++) {
                if (s == 0) base[i] = i;
                else if (s == 1) base[i] = MAX_N - 1 - i;
                else base[i] = (i * 37) % MAX_N;
            }
            int work[MAX_N];
            copyArray(base, work, MAX_N);
            long long comparisons = 0, moves = 0;
            algorithms[k](work, MAX_N, comparisons, moves);
            bool ok = isSorted(work, MAX_N) && sameElements(base, work, MAX_N);
            cout << algorithmNames[k] << "\t" << setNames[s] << "\t" << comparisons
                 << "\t" << moves << "\t" << (ok ? "успішно" : "помилка") << "\n";
        }
    }
}

void runUserArray() {
    int n;
    cout << "\nВведіть кількість елементів (від 1 до " << MAX_N << "): ";
    if (!(cin >> n) || n < 1 || n > MAX_N) {
        cout << "Некоректна кількість елементів\n";
        return;
    }
    int original[MAX_N];
    cout << "Введіть " << n << " цілих чисел: ";
    for (int i = 0; i < n; i++) {
        if (!(cin >> original[i])) {
            cout << "Некоректне значення\n";
            return;
        }
    }
    cout << "\nВихідний масив: ";
    printArray(original, n);
    cout << "\n";
    for (int k = 0; k < 3; k++) {
        int work[MAX_N];
        copyArray(original, work, n);
        long long comparisons = 0, moves = 0;
        algorithms[k](work, n, comparisons, moves);
        bool ok = isSorted(work, n) && sameElements(original, work, n);
        cout << algorithmNames[k] << ": ";
        printArray(work, n);
        cout << "\tпорівнянь " << comparisons << ", переміщень " << moves
             << "\t" << (ok ? "масив впорядковано" : "виявлено помилку") << "\n";
    }
}

int main() {
    cout << "=== Автоматизовані перевірки ===\n";
    runAllTests();
    if (failedChecks > 0) {
        cout << "\nЗнайдено помилок: " << failedChecks << ". До експерименту переходити не можна.\n";
        return 1;
    }
    cout << "\nУсі перевірки пройдено.\n";
    runExperiment();
    runUserArray();
    return 0;
}
