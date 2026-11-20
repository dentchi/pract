#include <iostream>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const int MAX_SIZE = 10000;
const int NAME_LEN = 60;
const int CUISINE_LEN = 40;

struct Restaurant {
    char name[NAME_LEN];
    char cuisine[CUISINE_LEN];
    double rating;
};

struct RestaurantList {
    Restaurant* data = nullptr;
    int size = 0;
    int capacity = 0;
};

long long allocCount = 0;
long long freeCount = 0;
long long movedCount = 0;
long long reallocCount = 0;

long long activeBlocks() {
    return allocCount - freeCount;
}

Restaurant* allocBlock(int n) {
    allocCount++;
    return new Restaurant[n];
}

void freeBlock(Restaurant*& p) {
    if (p != nullptr) {
        delete[] p;
        freeCount++;
        p = nullptr;
    }
}

Restaurant* allocOne() {
    allocCount++;
    return new Restaurant;
}

void freeOne(Restaurant*& p) {
    if (p != nullptr) {
        delete p;
        freeCount++;
        p = nullptr;
    }
}

void copyText(char dst[], const char src[], int maxLen) {
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

bool sameText(const char a[], const char b[]) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == b[i];
}

Restaurant makeRestaurant(const char name[], const char cuisine[], double rating) {
    Restaurant r;
    copyText(r.name, name, NAME_LEN);
    copyText(r.cuisine, cuisine, CUISINE_LEN);
    r.rating = rating;
    return r;
}

bool sameRestaurant(const Restaurant& a, const Restaurant& b) {
    return sameText(a.name, b.name) && sameText(a.cuisine, b.cuisine) && a.rating == b.rating;
}

void printRestaurant(const Restaurant& r) {
    cout << r.name << " | " << r.cuisine << " | " << r.rating << "\n";
}

int printList(const RestaurantList& l) {
    if (l.size == 0) {
        cout << "  Структура порожня\n";
        return 0;
    }
    cout << "  № | Назва | Кухня | Рейтинг\n";
    for (int i = 0; i < l.size; i++) {
        cout << "  " << i + 1 << " | ";
        printRestaurant(l.data[i]);
    }
    return l.size;
}

bool createList(RestaurantList& l, int n) {
    if (n <= 0 || n > MAX_SIZE) return false;
    l.data = allocBlock(n);
    l.size = n;
    l.capacity = n;
    return true;
}

bool pushBack(RestaurantList& l, const Restaurant& r, bool doubling) {
    if (l.size == l.capacity) {
        int newCap;
        if (doubling) newCap = (l.capacity == 0) ? 1 : l.capacity * 2;
        else newCap = l.capacity + 1;
        if (newCap > MAX_SIZE) return false;
        Restaurant* block = allocBlock(newCap);
        for (int i = 0; i < l.size; i++) {
            block[i] = l.data[i];
            movedCount++;
        }
        freeBlock(l.data);
        l.data = block;
        l.capacity = newCap;
        reallocCount++;
    }
    l.data[l.size] = r;
    l.size++;
    return true;
}

bool removeAt(RestaurantList& l, int index) {
    if (index < 0 || index >= l.size) return false;
    if (l.size == 1) {
        freeBlock(l.data);
        l.size = 0;
        l.capacity = 0;
        return true;
    }
    Restaurant* block = allocBlock(l.size - 1);
    int k = 0;
    for (int i = 0; i < l.size; i++) {
        if (i != index) {
            block[k] = l.data[i];
            k++;
            movedCount++;
        }
    }
    freeBlock(l.data);
    l.data = block;
    l.size--;
    l.capacity = l.size;
    reallocCount++;
    return true;
}

void clearList(RestaurantList& l) {
    freeBlock(l.data);
    l.size = 0;
    l.capacity = 0;
}

int findByCuisine(const RestaurantList& l, const char cuisine[], int& count) {
    int first = -1;
    count = 0;
    for (int i = 0; i < l.size; i++) {
        if (sameText(l.data[i].cuisine, cuisine)) {
            if (first == -1) first = i;
            count++;
        }
    }
    return first;
}

double sumRating(const RestaurantList& l) {
    double sum = 0;
    for (int i = 0; i < l.size; i++) sum += l.data[i].rating;
    return sum;
}

bool averageRating(const RestaurantList& l, double& avg) {
    if (l.size == 0) return false;
    avg = sumRating(l) / l.size;
    return true;
}

int changeFirstRating(RestaurantList& l, const char cuisine[], double newRating) {
    int count;
    int index = findByCuisine(l, cuisine, count);
    if (index != -1) l.data[index].rating = newRating;
    return index;
}

const Restaurant samples[5] = {
    {"Пузата хата", "українська", 4.2},
    {"Мафія", "італійська", 4.0},
    {"Сушія", "японська", 4.4},
    {"Козачок", "українська", 4.7},
    {"Челентано", "італійська", 3.9},
};

void makeTestList(RestaurantList& l, int n) {
    createList(l, n);
    for (int i = 0; i < n; i++) l.data[i] = samples[i % 5];
}

bool checkOrder(const RestaurantList& l, const int idx[], int n) {
    if (l.size != n) return false;
    for (int i = 0; i < n; i++)
        if (!sameRestaurant(l.data[i], samples[idx[i]])) return false;
    return true;
}

int testsPassed = 0, testsTotal = 0;

void report(int num, const char* name, bool ok, int size, int capacity, long long activeBefore) {
    bool balanced = (activeBlocks() == activeBefore);
    bool result = ok && balanced;
    testsTotal++;
    if (result) testsPassed++;
    cout << num << ". " << name << ": розмір " << size << ", місткість " << capacity
         << ", активних блоків після тесту " << activeBlocks() - activeBefore
         << " | " << (result ? "успішно" : "ПОМИЛКА") << "\n";
}

void runTests() {
    cout << "=== Автоматизовані тести ===\n";
    {
        RestaurantList l;
        long long b = activeBlocks();
        long long a0 = allocCount;
        bool ok = (printList(l) == 0) && allocCount == a0;
        report(1, "порожня структура, перегляд", ok, l.size, l.capacity, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        long long a0 = allocCount;
        bool ok = !removeAt(l, 0) && l.size == 0 && l.data == nullptr && allocCount == a0;
        report(2, "порожня структура, видалення", ok, l.size, l.capacity, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 1);
        int count;
        bool ok = findByCuisine(l, "українська", count) == 0 && count == 1;
        int s = l.size, c = l.capacity;
        clearList(l);
        report(3, "один елемент, пошук наявного", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 1);
        bool ok = removeAt(l, 0) && l.size == 0 && l.capacity == 0 && l.data == nullptr;
        report(4, "один елемент, видалення", ok, l.size, l.capacity, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 3);
        int idx[4] = {0, 1, 2, 3};
        bool ok = pushBack(l, samples[3], true) && checkOrder(l, idx, 4) && l.capacity == 6;
        int s = l.size, c = l.capacity;
        clearList(l);
        report(5, "три елементи, додавання", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 3);
        int idx[2] = {1, 2};
        bool ok = removeAt(l, 0) && checkOrder(l, idx, 2);
        int s = l.size, c = l.capacity;
        clearList(l);
        report(6, "три елементи, видалення першого", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 3);
        int idx[2] = {0, 2};
        bool ok = removeAt(l, 1) && checkOrder(l, idx, 2);
        int s = l.size, c = l.capacity;
        clearList(l);
        report(7, "три елементи, видалення середнього", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 3);
        int idx[2] = {0, 1};
        bool ok = removeAt(l, 2) && checkOrder(l, idx, 2);
        int s = l.size, c = l.capacity;
        clearList(l);
        report(8, "три елементи, видалення останнього", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 3);
        long long a0 = allocCount;
        Restaurant* oldData = l.data;
        int idx[3] = {0, 1, 2};
        bool ok = !removeAt(l, -1) && checkOrder(l, idx, 3) && l.data == oldData && allocCount == a0;
        int s = l.size, c = l.capacity;
        clearList(l);
        report(9, "три елементи, індекс -1", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 3);
        long long a0 = allocCount;
        Restaurant* oldData = l.data;
        int idx[3] = {0, 1, 2};
        bool ok = !removeAt(l, 3) && checkOrder(l, idx, 3) && l.data == oldData && allocCount == a0;
        int s = l.size, c = l.capacity;
        clearList(l);
        report(10, "три елементи, індекс, рівний розміру", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 4);
        int oldCap = l.capacity;
        int idx[5] = {0, 1, 2, 3, 4};
        bool ok = l.size == l.capacity && pushBack(l, samples[4], true) && l.capacity > oldCap && checkOrder(l, idx, 5);
        int s = l.size, c = l.capacity;
        clearList(l);
        report(11, "заповнена місткість, додавання", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        bool ok = true;
        for (int i = 0; i < 100; i++) ok = ok && pushBack(l, samples[i % 5], true);
        ok = ok && l.size == 100;
        while (l.size > 0) ok = ok && removeAt(l, 0);
        ok = ok && l.data == nullptr && l.capacity == 0;
        report(12, "100 елементів, послідовне очищення", ok, l.size, l.capacity, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 3);
        clearList(l);
        bool ok = l.data == nullptr && l.size == 0;
        for (int i = 0; i < 3; i++) ok = ok && pushBack(l, samples[i], true);
        int idx[3] = {0, 1, 2};
        ok = ok && checkOrder(l, idx, 3);
        int s = l.size, c = l.capacity;
        clearList(l);
        report(13, "повторне заповнення після очищення", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 2);
        bool ok = true;
        for (int i = 0; i < 50; i++) {
            ok = ok && pushBack(l, samples[4], true);
            ok = ok && removeAt(l, l.size - 1);
        }
        int idx[2] = {0, 1};
        ok = ok && checkOrder(l, idx, 2);
        int s = l.size, c = l.capacity;
        clearList(l);
        report(14, "50 чергувань додавання і видалення", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        makeTestList(l, 3);
        int count;
        int idx[3] = {0, 1, 2};
        bool ok = findByCuisine(l, "французька", count) == -1 && count == 0
                  && changeFirstRating(l, "французька", 5.0) == -1 && checkOrder(l, idx, 3);
        int s = l.size, c = l.capacity;
        clearList(l);
        report(15, "невдалий пошук", ok, s, c, b);
    }
    {
        RestaurantList l;
        long long b = activeBlocks();
        long long a0 = allocCount;
        bool ok = !createList(l, 0) && !createList(l, -5) && !createList(l, MAX_SIZE + 1)
                  && allocCount == a0 && l.data == nullptr;
        report(16, "розмір 0, -5 і більший за максимум", ok, l.size, l.capacity, b);
    }
    cout << "Успішних тестів: " << testsPassed << " з " << testsTotal << "\n\n";
}

void printState(const char* operation, const RestaurantList& l) {
    cout << operation << " | розмір " << l.size << " | місткість " << l.capacity
         << " | виділень " << allocCount << " | звільнень " << freeCount
         << " | активних " << activeBlocks() << " | перенесено " << movedCount << "\n";
}

void runStateTable() {
    cout << "=== Таблиця станів ===\n";
    allocCount = freeCount = movedCount = reallocCount = 0;
    RestaurantList l;
    printState("початок", l);
    makeTestList(l, 3);
    printState("створення 3 елементів", l);
    pushBack(l, samples[3], true);
    printState("додавання 4-го", l);
    pushBack(l, samples[4], true);
    printState("додавання 5-го", l);
    pushBack(l, samples[0], true);
    printState("додавання 6-го", l);
    pushBack(l, samples[1], true);
    printState("додавання 7-го", l);
    removeAt(l, 1);
    printState("видалення індексу 1", l);
    removeAt(l, 10);
    printState("видалення індексу 10 (відмова)", l);
    clearList(l);
    printState("очищення", l);
    cout << "\n";
}

void runStrategy(int count, bool doubling, long long& reallocs, long long& moved) {
    long long r0 = reallocCount, m0 = movedCount;
    RestaurantList l;
    for (int i = 0; i < count; i++) pushBack(l, samples[i % 5], doubling);
    reallocs = reallocCount - r0;
    moved = movedCount - m0;
    clearList(l);
}

void runCapacityExperiment() {
    cout << "=== Порівняння стратегій збільшення місткості ===\n";
    int counts[3] = {10, 100, 1000};
    for (int k = 0; k < 3; k++) {
        long long r1, m1, r2, m2;
        runStrategy(counts[k], false, r1, m1);
        runStrategy(counts[k], true, r2, m2);
        cout << counts[k] << " додавань: +1 -> перевиділень " << r1 << ", перенесено " << m1
             << "; подвоєння -> перевиділень " << r2 << ", перенесено " << m2
             << "; зекономлено перевиділень " << r1 - r2 << ", перенесень " << m1 - m2 << "\n";
    }
    cout << "Активних блоків після експерименту: " << activeBlocks() << "\n\n";
}

void readLine(const char* prompt, char buf[], int maxLen) {
    cout << prompt;
    cin.getline(buf, maxLen);
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

double readRating(const char* prompt) {
    double r;
    while (true) {
        cout << prompt;
        if (cin >> r && r >= 0 && r <= 5) break;
        cout << "Рейтинг має бути числом від 0 до 5\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    cin.ignore(10000, '\n');
    return r;
}

void readRestaurant(Restaurant& r) {
    readLine("  Назва: ", r.name, NAME_LEN);
    readLine("  Кухня: ", r.cuisine, CUISINE_LEN);
    r.rating = readRating("  Рейтинг (0-5): ");
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    runTests();
    runStateTable();
    runCapacityExperiment();

    cout << "=== Динамічний об'єкт ===\n";
    Restaurant* one = allocOne();
    readRestaurant(*one);
    cout << "Об'єкт: ";
    printRestaurant(*one);
    one->rating = readRating("Новий рейтинг: ");
    cout << "Після зміни: ";
    printRestaurant(*one);
    freeOne(one);
    cout << "Пам'ять звільнено, покажчик " << (one == nullptr ? "порожній" : "НЕ порожній") << "\n\n";

    cout << "=== Динамічний масив ресторанів ===\n";
    RestaurantList list;
    int n;
    while (true) {
        cout << "Кількість ресторанів (1-" << MAX_SIZE << "): ";
        if (cin >> n && createList(list, n)) break;
        cout << "Недопустима кількість, пам'ять не виділено\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    cin.ignore(10000, '\n');
    for (int i = 0; i < n; i++) {
        cout << "Ресторан " << i + 1 << ":\n";
        readRestaurant(list.data[i]);
    }
    cout << "Створено елементів: " << list.size << "\n";
    printList(list);

    char cuisine[CUISINE_LEN];
    readLine("Кухня для пошуку: ", cuisine, CUISINE_LEN);
    int count;
    int first = findByCuisine(list, cuisine, count);
    if (first == -1) {
        cout << "Ресторанів з такою кухнею немає, масив не змінено\n";
    } else {
        cout << "Знайдено " << count << ":\n";
        for (int i = 0; i < list.size; i++)
            if (sameText(list.data[i].cuisine, cuisine)) {
                cout << "  " << i + 1 << " | ";
                printRestaurant(list.data[i]);
            }
        double newRating = readRating("Новий рейтинг для першого знайденого: ");
        changeFirstRating(list, cuisine, newRating);
    }
    double avg;
    cout << fixed << setprecision(2);
    cout << "Сума рейтингів: " << sumRating(list) << "\n";
    if (averageRating(list, avg)) cout << "Середній рейтинг: " << avg << "\n";
    cout.unsetf(ios::fixed);
    cout << setprecision(6);

    cout << "Новий ресторан для додавання в кінець:\n";
    Restaurant extra;
    readRestaurant(extra);
    pushBack(list, extra, true);
    printList(list);
    cout << "Розмір " << list.size << ", місткість " << list.capacity << "\n";

    int index;
    cout << "Номер ресторану для видалення: ";
    cin >> index;
    if (removeAt(list, index - 1)) cout << "Видалено\n";
    else cout << "Недопустимий номер, масив не змінено\n";
    printList(list);
    if (averageRating(list, avg)) cout << "Середній рейтинг: " << fixed << setprecision(2) << avg << "\n";

    clearList(list);
    cout << "\nВиділень: " << allocCount << ", звільнень: " << freeCount
         << ", активних блоків пам'яті: " << activeBlocks() << "\n";
    return 0;
}
