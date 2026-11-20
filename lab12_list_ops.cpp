#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const int NAME_LEN = 60;
const int CUISINE_LEN = 40;
const int MAX_STEPS = 100000;
const int MAX_SNAP = 1000;

struct Node {
    int code = 0;
    char name[NAME_LEN] = "";
    char cuisine[CUISINE_LEN] = "";
    double rating = 0;
    Node* next = nullptr;
};

struct List {
    Node* head = nullptr;
    Node* tail = nullptr;
    int count = 0;
};

long long createdNodes = 0;
long long freedNodes = 0;

long long activeNodes() {
    return createdNodes - freedNodes;
}

void copyText(char dst[], const char src[], int maxLen) {
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

Node* createNode(int code, const char name[], const char cuisine[], double rating) {
    Node* node = new Node;
    createdNodes++;
    node->code = code;
    copyText(node->name, name, NAME_LEN);
    copyText(node->cuisine, cuisine, CUISINE_LEN);
    node->rating = rating;
    node->next = nullptr;
    return node;
}

Node* createTestNode(int code) {
    return createNode(code, "Ресторан", "українська", 4.0);
}

void freeNode(Node*& node) {
    delete node;
    freedNodes++;
    node = nullptr;
}

void addToEnd(List& l, Node* node) {
    if (l.head == nullptr) {
        l.head = node;
        l.tail = node;
    } else {
        l.tail->next = node;
        l.tail = node;
    }
    l.count++;
}

void clearList(List& l) {
    Node* current = l.head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        freeNode(current);
        current = nextNode;
    }
    l.head = nullptr;
    l.tail = nullptr;
    l.count = 0;
}

void printList(const List& l) {
    if (l.head == nullptr) {
        cout << "  Список порожній\n";
        return;
    }
    Node* current = l.head;
    int pos = 0;
    while (current != nullptr) {
        cout << "  " << pos << " | код " << current->code << " | " << current->name << " | "
             << current->cuisine << " | " << current->rating << "\n";
        pos++;
        current = current->next;
    }
}

void printKeys(const List& l) {
    cout << "[";
    Node* current = l.head;
    int k = 0;
    while (current != nullptr && k < MAX_STEPS) {
        cout << current->code;
        if (current->next != nullptr) cout << ", ";
        current = current->next;
        k++;
    }
    cout << "]";
}

const int CHECK_OK = 0;
const int CHECK_EMPTY = 1;
const int CHECK_COUNT = 2;
const int CHECK_TAIL = 3;
const int CHECK_BREAK = 4;
const int CHECK_CYCLE = 5;

int checkList(const List& l) {
    if (l.count == 0 || l.head == nullptr || l.tail == nullptr) {
        if (l.count == 0 && l.head == nullptr && l.tail == nullptr) return CHECK_OK;
        return CHECK_EMPTY;
    }
    Node* current = l.head;
    Node* last = nullptr;
    int reachable = 0;
    bool tailReached = false;
    while (current != nullptr) {
        if (reachable >= MAX_STEPS) return CHECK_CYCLE;
        reachable++;
        if (current == l.tail) tailReached = true;
        last = current;
        current = current->next;
    }
    if (!tailReached && reachable < l.count) return CHECK_BREAK;
    if (last != l.tail) return CHECK_TAIL;
    if (reachable != l.count) return CHECK_COUNT;
    return CHECK_OK;
}

const char* checkText(int code) {
    if (code == CHECK_OK) return "структура правильна";
    if (code == CHECK_EMPTY) return "неузгоджений порожній стан";
    if (code == CHECK_COUNT) return "неправильна кількість";
    if (code == CHECK_TAIL) return "хибний кінець";
    if (code == CHECK_BREAK) return "розрив списку";
    return "можливий цикл";
}

Node* findFirst(const List& l, int key, int& pos, int& visited) {
    pos = -1;
    visited = 0;
    Node* current = l.head;
    int i = 0;
    while (current != nullptr) {
        visited++;
        if (current->code == key) {
            pos = i;
            return current;
        }
        current = current->next;
        i++;
    }
    return nullptr;
}

int findAll(const List& l, int key, int positions[], int maxPositions, int& visited) {
    int found = 0;
    visited = 0;
    Node* current = l.head;
    int i = 0;
    while (current != nullptr) {
        visited++;
        if (current->code == key) {
            if (found < maxPositions) positions[found] = i;
            found++;
        }
        current = current->next;
        i++;
    }
    return found;
}

Node* nodeAt(const List& l, int pos) {
    Node* current = l.head;
    for (int i = 0; i < pos && current != nullptr; i++) current = current->next;
    return current;
}

bool insertAt(List& l, int pos, Node* node) {
    if (pos < 0 || pos > l.count) return false;
    if (pos == 0) {
        node->next = l.head;
        l.head = node;
        if (l.tail == nullptr) l.tail = node;
    } else if (pos == l.count) {
        l.tail->next = node;
        l.tail = node;
    } else {
        Node* previous = nodeAt(l, pos - 1);
        node->next = previous->next;
        previous->next = node;
    }
    l.count++;
    return true;
}

bool insertValueAt(List& l, int pos, int newCode) {
    if (pos < 0 || pos > l.count) return false;
    return insertAt(l, pos, createTestNode(newCode));
}

bool insertBefore(List& l, int key, int newCode) {
    int pos, visited;
    if (findFirst(l, key, pos, visited) == nullptr) return false;
    return insertAt(l, pos, createTestNode(newCode));
}

bool insertAfter(List& l, int key, int newCode) {
    int pos, visited;
    if (findFirst(l, key, pos, visited) == nullptr) return false;
    return insertAt(l, pos + 1, createTestNode(newCode));
}

bool removeAt(List& l, int pos) {
    if (pos < 0 || pos >= l.count) return false;
    Node* target;
    if (pos == 0) {
        target = l.head;
        l.head = target->next;
        if (l.head == nullptr) l.tail = nullptr;
    } else {
        Node* previous = nodeAt(l, pos - 1);
        target = previous->next;
        previous->next = target->next;
        if (target == l.tail) l.tail = previous;
    }
    freeNode(target);
    l.count--;
    return true;
}

bool removeFirstKey(List& l, int key) {
    int pos, visited;
    if (findFirst(l, key, pos, visited) == nullptr) return false;
    return removeAt(l, pos);
}

int removeAllKeys(List& l, int key) {
    int removed = 0;
    while (l.head != nullptr && l.head->code == key) {
        Node* target = l.head;
        l.head = target->next;
        freeNode(target);
        l.count--;
        removed++;
    }
    if (l.head == nullptr) {
        l.tail = nullptr;
        return removed;
    }
    Node* previous = l.head;
    while (previous->next != nullptr) {
        if (previous->next->code == key) {
            Node* target = previous->next;
            previous->next = target->next;
            if (target == l.tail) l.tail = previous;
            freeNode(target);
            l.count--;
            removed++;
        } else {
            previous = previous->next;
        }
    }
    return removed;
}

void buildList(List& l, const int keys[], int n) {
    for (int i = 0; i < n; i++) addToEnd(l, createTestNode(keys[i]));
}

int snapshot(const List& l, int keys[]) {
    Node* current = l.head;
    int n = 0;
    while (current != nullptr && n < MAX_SNAP) {
        keys[n] = current->code;
        n++;
        current = current->next;
    }
    return n;
}

bool keysAre(const List& l, const int expected[], int n) {
    if (l.count != n) return false;
    Node* current = l.head;
    for (int i = 0; i < n; i++) {
        if (current == nullptr || current->code != expected[i]) return false;
        current = current->next;
    }
    if (current != nullptr) return false;
    if (n > 0) return l.head->code == expected[0] && l.tail->code == expected[n - 1] && l.tail->next == nullptr;
    return l.head == nullptr && l.tail == nullptr;
}

bool unchanged(const List& l, const int before[], int n, Node* head, Node* tail) {
    return keysAre(l, before, n) && l.head == head && l.tail == tail;
}

bool memoryOk(const List& l, long long base) {
    return activeNodes() - base == l.count;
}

int passed = 0, total = 0;

void finishTest(int num, const char* name, bool dataOk, List& l, long long base) {
    bool structOk = checkList(l) == CHECK_OK;
    bool memOk = memoryOk(l, base);
    cout << num << ". " << name << ": ";
    printKeys(l);
    clearList(l);
    bool finalOk = checkList(l) == CHECK_OK && activeNodes() == base;
    bool ok = dataOk && structOk && memOk && finalOk;
    total++;
    if (ok) passed++;
    cout << " | дані " << (dataOk ? "так" : "НІ") << ", структура " << (structOk ? "так" : "НІ")
         << ", пам'ять " << (memOk ? "так" : "НІ") << " | " << (ok ? "успішно" : "ПОМИЛКА") << "\n";
}

void runTests() {
    cout << "=== Автоматизовані сценарії ===\n";
    int before[MAX_SNAP];
    {
        List l;
        long long b = activeNodes();
        int pos, visited;
        bool ok = findFirst(l, 5, pos, visited) == nullptr && pos == -1 && visited == 0;
        finishTest(1, "порожньо, пошук 5 -> не знайдено", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int e[1] = {5};
        bool ok = insertAt(l, 0, createTestNode(5)) && keysAre(l, e, 1);
        finishTest(2, "порожньо, вставити 5 на 0 -> 5", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[1] = {5};
        buildList(l, s, 1);
        bool ok = removeAt(l, 0) && keysAre(l, s, 0);
        finishTest(3, "5, видалити позицію 0 -> порожньо", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[2] = {1, 3}, e[3] = {1, 2, 3};
        buildList(l, s, 2);
        bool ok = insertAt(l, 1, createTestNode(2)) && keysAre(l, e, 3);
        finishTest(4, "1, 3, вставити 2 на 1 -> 1, 2, 3", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[2] = {1, 2}, e[3] = {0, 1, 2};
        buildList(l, s, 2);
        bool ok = insertAt(l, 0, createTestNode(0)) && keysAre(l, e, 3);
        finishTest(5, "1, 2, вставити 0 на 0 -> 0, 1, 2", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[2] = {1, 2}, e[3] = {1, 2, 3};
        buildList(l, s, 2);
        bool ok = insertAt(l, 2, createTestNode(3)) && keysAre(l, e, 3);
        finishTest(6, "1, 2, вставити 3 на 2 -> 1, 2, 3", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3}, e[2] = {2, 3};
        buildList(l, s, 3);
        bool ok = removeAt(l, 0) && keysAre(l, e, 2);
        finishTest(7, "1, 2, 3, видалити позицію 0 -> 2, 3", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3}, e[2] = {1, 3};
        buildList(l, s, 3);
        bool ok = removeAt(l, 1) && keysAre(l, e, 2);
        finishTest(8, "1, 2, 3, видалити позицію 1 -> 1, 3", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3}, e[2] = {1, 2};
        buildList(l, s, 3);
        bool ok = removeAt(l, 2) && keysAre(l, e, 2);
        finishTest(9, "1, 2, 3, видалити позицію 2 -> 1, 2", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[4] = {1, 2, 2, 3}, e[3] = {1, 2, 3};
        buildList(l, s, 4);
        bool ok = removeFirstKey(l, 2) && keysAre(l, e, 3);
        finishTest(10, "1, 2, 2, 3, видалити перше 2 -> 1, 2, 3", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[4] = {2, 2, 1, 2}, e[1] = {1};
        buildList(l, s, 4);
        bool ok = removeAllKeys(l, 2) == 3 && keysAre(l, e, 1);
        finishTest(11, "2, 2, 1, 2, видалити всі 2 -> 1", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3};
        buildList(l, s, 3);
        int n = snapshot(l, before);
        Node* h = l.head;
        Node* t = l.tail;
        long long created = createdNodes;
        bool ok = !insertValueAt(l, 5, 9) && createdNodes == created && unchanged(l, before, n, h, t);
        finishTest(12, "1, 2, 3, вставити на позицію 5 -> відмова", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3};
        buildList(l, s, 3);
        int n = snapshot(l, before);
        Node* h = l.head;
        Node* t = l.tail;
        bool ok = !removeAt(l, -1) && unchanged(l, before, n, h, t);
        finishTest(13, "1, 2, 3, видалити позицію -1 -> відмова", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3}, e[4] = {1, 2, 3, 9};
        buildList(l, s, 3);
        bool ok = insertAfter(l, 3, 9) && keysAre(l, e, 4);
        finishTest(14, "1, 2, 3, вставити 9 після 3 -> 1, 2, 3, 9", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3}, e[4] = {0, 1, 2, 3};
        buildList(l, s, 3);
        bool ok = insertBefore(l, 1, 0) && keysAre(l, e, 4);
        finishTest(15, "1, 2, 3, вставити 0 перед 1 -> 0, 1, 2, 3", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[8] = {7, 7, 1, 7, 7, 2, 7, 7}, e[2] = {1, 2};
        buildList(l, s, 8);
        bool ok = removeAllKeys(l, 7) == 6 && keysAre(l, e, 2);
        finishTest(16, "7, 7, 1, 7, 7, 2, 7, 7, видалити всі 7 -> 1, 2", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3}, e[2] = {4, 5};
        buildList(l, s, 3);
        bool ok = removeAt(l, 0) && removeAt(l, 0) && removeAt(l, 0) && checkList(l) == CHECK_OK
                  && l.head == nullptr && activeNodes() == b;
        ok = ok && insertAt(l, 0, createTestNode(4)) && insertAt(l, 1, createTestNode(5)) && keysAre(l, e, 2);
        finishTest(17, "повне видалення і повторне заповнення -> 4, 5", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3};
        buildList(l, s, 3);
        bool ok = true;
        ok = ok && insertAt(l, 3, createTestNode(4));
        ok = ok && insertAt(l, 0, createTestNode(0));
        ok = ok && removeAt(l, 2);
        ok = ok && insertAfter(l, 1, 2);
        ok = ok && insertBefore(l, 4, 3);
        ok = ok && removeFirstKey(l, 0);
        ok = ok && !removeAt(l, 10);
        ok = ok && insertAt(l, 2, createTestNode(2));
        ok = ok && removeAllKeys(l, 2) == 2;
        ok = ok && insertAt(l, 1, createTestNode(2));
        bool allChecks = checkList(l) == CHECK_OK;
        int e[5] = {1, 2, 3, 3, 4};
        ok = ok && allChecks && keysAre(l, e, 5);
        finishTest(18, "серія з 10 змішаних операцій -> 1, 2, 3, 3, 4", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3};
        buildList(l, s, 3);
        int n = snapshot(l, before);
        Node* h = l.head;
        Node* t = l.tail;
        long long created = createdNodes;
        bool ok = !insertAfter(l, 99, 5) && !insertBefore(l, 99, 5) && !removeFirstKey(l, 99)
                  && removeAllKeys(l, 99) == 0 && createdNodes == created && unchanged(l, before, n, h, t);
        finishTest(19, "1, 2, 3, операції з відсутнім ключем 99 -> без змін", ok, l, b);
    }
    {
        List l;
        long long b = activeNodes();
        int s[4] = {2, 1, 2, 2};
        buildList(l, s, 4);
        int pos, visited, pos2, visited2, vAll;
        int positions[10];
        bool ok = findFirst(l, 2, pos, visited) == l.head && pos == 0 && visited == 1;
        ok = ok && findFirst(l, 5, pos2, visited2) == nullptr && visited2 == 4;
        ok = ok && findAll(l, 2, positions, 10, vAll) == 3 && positions[0] == 0 && positions[1] == 2
             && positions[2] == 3 && vAll == 4 && findAll(l, 5, positions, 10, vAll) == 0;
        finishTest(20, "2, 1, 2, 2, пошук першого, відсутнього і всіх 2", ok, l, b);
    }
    cout << "Успішних сценаріїв: " << passed << " з " << total << "\n\n";
}

void showState(const List& l, long long base) {
    cout << "  стан: ";
    printKeys(l);
    cout << ", кількість " << l.count;
    if (l.head != nullptr) cout << ", head " << l.head->code << ", tail " << l.tail->code;
    else cout << ", head = tail = nullptr";
    cout << " | " << checkText(checkList(l)) << ", активних вузлів " << activeNodes() - base << "\n";
}

void runVariant() {
    cout << "=== Варіант 26 ===\n";
    long long base = activeNodes();
    List l;
    int keys[5] = {21, 7, 17, 7, 12};
    const char* names[5] = {"Пузата хата", "Мафія", "Козачок", "Сушія", "Челентано"};
    const char* cuisines[5] = {"українська", "італійська", "українська", "японська", "італійська"};
    double ratings[5] = {4.2, 4.0, 4.7, 4.4, 3.9};
    for (int i = 0; i < 5; i++) addToEnd(l, createNode(keys[i], names[i], cuisines[i], ratings[i]));
    cout << "Початковий список:\n";
    printList(l);
    showState(l, base);

    int pos, visited;
    Node* found = findFirst(l, 17, pos, visited);
    cout << "Пошук 17: ";
    if (found != nullptr) cout << "позиція " << pos << ", " << found->name << ", переглянуто вузлів " << visited << "\n";
    else cout << "не знайдено, переглянуто вузлів " << visited << "\n";

    int positions[10];
    int count = findAll(l, 7, positions, 10, visited);
    cout << "Пошук усіх 7: знайдено " << count << " (позиції";
    for (int i = 0; i < count; i++) cout << " " << positions[i];
    cout << "), переглянуто вузлів " << visited << "\n";
    showState(l, base);

    cout << "Вставлення 9 на позицію 2: "
         << (insertAt(l, 2, createNode(9, "Мама Манана", "грузинська", 4.6)) ? "успішно" : "відмова") << "\n";
    printList(l);
    showState(l, base);

    int removed = removeAllKeys(l, 7);
    cout << "Видалення всіх вузлів з ключем 7: видалено " << removed << "\n";
    printList(l);
    showState(l, base);

    clearList(l);
    cout << "Після очищення:";
    showState(l, base);
    cout << "\n";
}

struct Command {
    char type;
    int a;
    int b;
};

void runCommands() {
    cout << "=== Журнал команд ===\n";
    long long base = activeNodes();
    List l;
    int keys[5] = {21, 7, 17, 7, 12};
    buildList(l, keys, 5);
    cout << "Початок: ";
    showState(l, base);
    Command commands[] = {
        {'F', 17, 0}, {'F', 50, 0}, {'A', 7, 0}, {'I', 2, 9}, {'I', 0, 1},
        {'I', 7, 30}, {'I', 9, 5}, {'B', 1, 0}, {'B', 17, 16}, {'J', 30, 31},
        {'J', 100, 2}, {'D', 0, 0}, {'D', 3, 0}, {'D', 20, 0}, {'K', 12, 0},
        {'K', 100, 0}, {'R', 7, 0}, {'I', 2, 7}, {'I', 3, 7}, {'R', 7, 0},
        {'D', -1, 0}, {'F', 31, 0},
    };
    int n = sizeof(commands) / sizeof(commands[0]);
    int okCount = 0;
    int before[MAX_SNAP];
    for (int c = 0; c < n; c++) {
        Command cmd = commands[c];
        int snapN = snapshot(l, before);
        Node* h = l.head;
        Node* t = l.tail;
        long long created = createdNodes;
        bool success = false;
        int pos, visited;
        cout << c + 1 << ". ";
        if (cmd.type == 'F') {
            Node* f = findFirst(l, cmd.a, pos, visited);
            success = f != nullptr;
            cout << "пошук " << cmd.a << ": " << (success ? "позиція " + to_string(pos) : string("не знайдено"))
                 << ", переглянуто " << visited;
        } else if (cmd.type == 'A') {
            int positions[50];
            int k = findAll(l, cmd.a, positions, 50, visited);
            success = k > 0;
            cout << "пошук усіх " << cmd.a << ": " << k << " (позиції";
            for (int i = 0; i < k; i++) cout << " " << positions[i];
            cout << "), переглянуто " << visited;
        } else if (cmd.type == 'I') {
            success = insertValueAt(l, cmd.a, cmd.b);
            cout << "вставити " << cmd.b << " на позицію " << cmd.a << ": " << (success ? "успішно" : "відмова");
        } else if (cmd.type == 'B') {
            success = insertBefore(l, cmd.a, cmd.b);
            cout << "вставити " << cmd.b << " перед " << cmd.a << ": " << (success ? "успішно" : "відмова");
        } else if (cmd.type == 'J') {
            success = insertAfter(l, cmd.a, cmd.b);
            cout << "вставити " << cmd.b << " після " << cmd.a << ": " << (success ? "успішно" : "відмова");
        } else if (cmd.type == 'D') {
            success = removeAt(l, cmd.a);
            cout << "видалити позицію " << cmd.a << ": " << (success ? "успішно" : "відмова");
        } else if (cmd.type == 'K') {
            success = removeFirstKey(l, cmd.a);
            cout << "видалити перше " << cmd.a << ": " << (success ? "успішно" : "відмова");
        } else {
            int k = removeAllKeys(l, cmd.a);
            success = k > 0;
            cout << "видалити всі " << cmd.a << ": видалено " << k;
        }
        bool structOk = checkList(l) == CHECK_OK && memoryOk(l, base);
        bool readOnly = cmd.type == 'F' || cmd.type == 'A';
        if (!success || readOnly) structOk = structOk && unchanged(l, before, snapN, h, t) && createdNodes == created;
        if (structOk) okCount++;
        cout << " | перевірка " << (structOk ? "пройдена" : "НЕ пройдена") << "\n";
        showState(l, base);
    }
    clearList(l);
    cout << "Команд: " << n << ", перевірок пройдено: " << okCount << " з " << n << "\n";
    cout << "Після очищення:";
    showState(l, base);
    cout << "\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    runTests();
    runVariant();
    runCommands();
    cout << "Створено вузлів: " << createdNodes << ", звільнено: " << freedNodes
         << ", активних: " << activeNodes() << "\n";
    return 0;
}
