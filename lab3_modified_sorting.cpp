#include <iostream>

using namespace std;

const int MAX_N = 1000;
long long comparisons, moves, passes, swaps;
int failed = 0;

void swapCounted(int& x, int& y) {
    int temp = x;
    x = y;
    y = temp;
    moves += 3;
    swaps++;
}

int findInsertPosition(const int a[], int count, int value) {
    int low = 0, high = count;
    while (low < high) {
        int mid = low + (high - low) / 2;
        comparisons++;
        if (a[mid] <= value) low = mid + 1;
        else high = mid;
    }
    return low;
}

void insertionSort(int a[], int n) {
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

void binaryInsertionSort(int a[], int n) {
    for (int i = 1; i < n; i++) {
        int key = a[i];
        moves++;
        int position = findInsertPosition(a, i, key);
        for (int j = i; j > position; j--) {
            a[j] = a[j - 1];
            moves++;
        }
        a[position] = key;
        moves++;
    }
}

void bubbleSort(int a[], int n) {
    for (int pass = 0; pass < n - 1; pass++) {
        bool swapped = false;
        passes++;
        for (int j = 0; j < n - pass - 1; j++) {
            comparisons++;
            if (a[j] > a[j + 1]) {
                swapCounted(a[j], a[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void shakerSort(int a[], int n) {
    int left = 0, right = n - 1;
    bool swapped = true;
    while (left < right && swapped) {
        swapped = false;
        passes++;
        for (int j = left; j < right; j++) {
            comparisons++;
            if (a[j] > a[j + 1]) {
                swapCounted(a[j], a[j + 1]);
                swapped = true;
            }
        }
        right--;
        if (!swapped) break;
        swapped = false;
        passes++;
        for (int j = right; j > left; j--) {
            comparisons++;
            if (a[j - 1] > a[j]) {
                swapCounted(a[j - 1], a[j]);
                swapped = true;
            }
        }
        left++;
    }
}

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

void copyArray(const int from[], int to[], int n) {
    for (int i = 0; i < n; i++) to[i] = from[i];
}

typedef void (*SortFunction)(int a[], int n);
const char* names[4] = {"Вставки", "Бінарні вставки", "Бульбашкове", "Шейкерне"};
SortFunction algorithms[4] = {insertionSort, binaryInsertionSort, bubbleSort, shakerSort};

void runSort(int k, const int base[], int work[], int n) {
    copyArray(base, work, n);
    comparisons = moves = passes = swaps = 0;
    algorithms[k](work, n);
}

void runTests() {
    int tests[12][6] = {{0}, {5}, {1, 2, 3, 4, 5}, {5, 4, 3, 2, 1}, {4, 2, 5, 1, 3}, {3, 1, 3, 2, 1},
                        {0, -4, 7, -1, 3}, {8, 8, 8, 8}, {2, 1}, {1, 2, 3, 5, 4, 6},
                        {9, 9, 1, 1, 5}, {-3, 10, -3, 0, 10, -7}};
    int expected[12][6] = {{0}, {5}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, {1, 1, 2, 3, 3},
                           {-4, -1, 0, 3, 7}, {8, 8, 8, 8}, {1, 2}, {1, 2, 3, 4, 5, 6},
                           {1, 1, 5, 9, 9}, {-7, -3, -3, 0, 10, 10}};
    int sizes[12] = {0, 1, 5, 5, 5, 5, 5, 4, 2, 6, 5, 6};
    for (int t = 0; t < 12; t++) {
        for (int k = 1; k < 4; k += 2) {
            int work[6];
            runSort(k, tests[t], work, sizes[t]);
            bool ok = isCorrect(tests[t], work, sizes[t]);
            for (int i = 0; i < sizes[t]; i++) {
                if (work[i] != expected[t][i]) ok = false;
            }
            cout << names[k] << "\tтест " << t + 1 << "\t" << (ok ? "успішно" : "помилка") << "\n";
            if (!ok) failed++;
        }
    }
    int prefix[7][5] = {{2, 4, 6, 8, 10}, {2, 4, 6, 8, 10}, {2, 4, 6, 8, 10}, {2, 4, 6, 8, 10},
                        {2, 4, 6, 8, 10}, {3, 3, 3}, {0}};
    int count[7] = {5, 5, 5, 5, 5, 3, 0};
    int value[7] = {1, 5, 11, 6, 10, 3, 5};
    int position[7] = {0, 2, 5, 3, 5, 3, 0};
    for (int t = 0; t < 7; t++) {
        bool ok = findInsertPosition(prefix[t], count[t], value[t]) == position[t];
        cout << "Позиція\tтест " << t + 1 << "\t" << (ok ? "успішно" : "помилка") << "\n";
        if (!ok) failed++;
    }
}

void runAlgorithms(const int base[], int n, const char* setName) {
    for (int k = 0; k < 4; k++) {
        int work[MAX_N];
        runSort(k, base, work, n);
        cout << n << "\t" << setName << "\t" << names[k] << "\t" << comparisons << "\t" << moves
             << "\t" << passes << "\t" << swaps << "\t"
             << (isCorrect(base, work, n) ? "успішно" : "помилка") << "\n";
        if (n <= 20) {
            for (int i = 0; i < n; i++) cout << work[i] << " ";
            cout << "\n";
        }
    }
}

void runExperiment() {
    const int sizes[3] = {10, 100, 1000};
    const int pattern[7] = {5, 3, 8, 1, 9, 2, 7};
    const char* setNames[4] = {"Зростання", "Спадання", "Майже впорядкований", "Змішаний"};
    cout << "\nn\tНабір\tАлгоритм\tПорівняння\tПереміщення\tПроходи\tОбміни\tПеревірка\n";
    for (int z = 0; z < 3; z++) {
        int n = sizes[z];
        for (int t = 0; t < 4; t++) {
            int base[MAX_N];
            for (int i = 0; i < n; i++) {
                if (t == 1) base[i] = n - i;
                else if (t == 3) base[i] = pattern[i % 7];
                else base[i] = i + 1;
            }
            if (t == 2) {
                for (int i = 0; i + 1 < n; i += 10) swapCounted(base[i], base[i + 1]);
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
    runAlgorithms(base, n, "введений");
    return 0;
}
