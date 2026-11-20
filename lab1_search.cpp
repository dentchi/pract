#include <iostream>
#include <iomanip>

using namespace std;

int linearSearch(const int a[], int n, int key, long long& comparisons) {
    for (int i = 0; i < n; i++) {
        comparisons++;
        if (a[i] == key) {
            return i;
        }
    }
    return -1;
}

int binarySearch(const int a[], int n, int key, long long& comparisons) {
    int left = 0;
    int right = n - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        comparisons++;
        if (a[mid] == key) {
            return mid;
        } else if (a[mid] < key) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

void insertionSort(int a[], int n) {
    for (int i = 1; i < n; i++) {
        int x = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > x) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = x;
    }
}

int failedTests = 0;

void runTest(int num, const int a[], int n, int key, int expected) {
    long long c1 = 0, c2 = 0;
    int r1 = linearSearch(a, n, key, c1);
    int r2 = binarySearch(a, n, key, c2);
    bool ok = (r1 == expected) && (r2 == expected);
    cout << "Тест " << setw(2) << num << ": ключ " << setw(4) << key
         << ", очікується " << setw(2) << expected << " -> ";
    if (ok) {
        cout << "успішно\n";
    } else {
        failedTests++;
        cout << "помилка (прямий: " << r1 << ", бінарний: " << r2 << ")\n";
    }
}

void runAllTests() {
    const int dummy[1] = {0};
    const int one[1] = {8};
    const int seven[7] = {2, 5, 8, 11, 14, 17, 20};
    const int own1[7] = {1, 3, 5, 7, 9, 11, 13};
    const int own2[4] = {10, 20, 30, 40};
    const int own3[2] = {-5, 5};

    runTest(1, one, 1, 8, 0);
    runTest(2, one, 1, 3, -1);
    runTest(3, seven, 7, 2, 0);
    runTest(4, seven, 7, 11, 3);
    runTest(5, seven, 7, 20, 6);
    runTest(6, seven, 7, 9, -1);
    runTest(7, seven, 7, -4, -1);
    runTest(8, seven, 7, 100, -1);
    runTest(9, dummy, 0, 5, -1);
    runTest(10, own1, 7, 13, 6);
    runTest(11, own2, 4, 25, -1);
    runTest(12, own3, 2, 5, 1);
    runTest(13, own3, 2, -5, 0);
}

static int bigArray[100000];

void experiment() {
    const int sizes[4] = {10, 100, 1000, 100000};
    const char names[3] = {'A', 'B', 'C'};
    cout << "\nТаблиця 1. Кількість порівнянь\n";
    cout << "n\tСценарій\tКлюч\tПрямий\tБінарний\n";
    for (int s = 0; s < 4; s++) {
        int n = sizes[s];
        for (int i = 0; i < n; i++) bigArray[i] = i;
        int keys[3] = {0, n - 1, n};
        for (int sc = 0; sc < 3; sc++) {
            long long cl = 0, cb = 0;
            linearSearch(bigArray, n, keys[sc], cl);
            binarySearch(bigArray, n, keys[sc], cb);
            cout << n << "\t" << names[sc] << "\t" << keys[sc] << "\t" << cl << "\t" << cb << "\n";
        }
    }
}

void averages() {
    const int sizes[4] = {10, 100, 1000, 100000};
    const int searches = 1000;
    cout << "\nТаблиця 2. Середня кількість порівнянь за " << searches << " пошуків\n";
    cout << "n\tПрямий\tБінарний\n";
    cout << fixed << setprecision(3);
    for (int s = 0; s < 4; s++) {
        int n = sizes[s];
        for (int i = 0; i < n; i++) bigArray[i] = i;
        long long sumLinear = 0, sumBinary = 0;
        for (int i = 0; i < searches; i++) {
            int key = (i * 37) % n;
            linearSearch(bigArray, n, key, sumLinear);
            binarySearch(bigArray, n, key, sumBinary);
        }
        cout << n << "\t" << (double)sumLinear / searches << "\t" << (double)sumBinary / searches << "\n";
    }
    cout << defaultfloat;
}

void unsortedCheck() {
    const int bad[6] = {12, 3, 19, 5, 8, 1};
    const int good[6] = {1, 3, 5, 8, 12, 19};
    cout << "\nТаблиця 3. Бінарний пошук кожного елемента\n";
    cout << "Ключ\t{12,3,19,5,8,1}\t\t{1,3,5,8,12,19}\n";
    int wrongCount = 0;
    for (int i = 0; i < 6; i++) {
        long long c = 0;
        int r1 = binarySearch(bad, 6, bad[i], c);
        int r2 = binarySearch(good, 6, bad[i], c);
        cout << bad[i] << "\t" << r1 << (r1 == i ? " (вірно)" : " (ПОМИЛКА)") << "\t\t" << r2 << "\n";
        if (r1 != i) wrongCount++;
    }
    cout << "Неправильних результатів на невпорядкованому масиві: " << wrongCount << " з 6\n";
}

void variant26() {
    int a[7] = {39, 22, 46, 17, 29, 56, 25};
    const int keys[3] = {17, 56, 44};
    int sorted[7];
    for (int i = 0; i < 7; i++) sorted[i] = a[i];
    insertionSort(sorted, 7);

    cout << "\nТаблиця 4. Варіант 26\n";
    cout << "Відсортований масив: ";
    for (int i = 0; i < 7; i++) cout << sorted[i] << (i < 6 ? ", " : "\n");
    cout << "Ключ\tПрямий (вихідний)\tПрямий (відсортований)\tБінарний (відсортований)\n";
    for (int k = 0; k < 3; k++) {
        long long c1 = 0, c2 = 0, c3 = 0;
        int r1 = linearSearch(a, 7, keys[k], c1);
        int r2 = linearSearch(sorted, 7, keys[k], c2);
        int r3 = binarySearch(sorted, 7, keys[k], c3);
        cout << keys[k] << "\t" << r1 << " (" << c1 << ")\t\t" << r2 << " (" << c2 << ")\t\t" << r3 << " (" << c3 << ")\n";
    }
    cout << "(у дужках - кількість порівнянь)\n";
    long long c = 0;
    cout << "Контроль: бінарний пошук 22 у вихідному масиві -> " << binarySearch(a, 7, 22, c)
         << " (фактичний індекс 1)\n";
}

int main() {
    cout << "=== Автоматизовані перевірки ===\n";
    runAllTests();
    if (failedTests > 0) {
        cout << "\nЗнайдено помилок: " << failedTests << ". До експерименту переходити не можна.\n";
        return 1;
    }
    cout << "\nУсі перевірки пройдено.\n";
    experiment();
    averages();
    unsortedCheck();
    variant26();
    return 0;
}
