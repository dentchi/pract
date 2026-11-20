#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const int NAME_LEN = 60;
const int CUISINE_LEN = 40;
const int MAX_STEPS = 100000;
const int MAX_OUT = 200;

struct DNode {
    int code = 0;
    char name[NAME_LEN] = "";
    char cuisine[CUISINE_LEN] = "";
    double rating = 0;
    DNode* prev = nullptr;
    DNode* next = nullptr;
};

struct DList {
    DNode* head = nullptr;
    DNode* tail = nullptr;
    int count = 0;
};

struct SNode {
    int code = 0;
    char name[NAME_LEN] = "";
    char cuisine[CUISINE_LEN] = "";
    double rating = 0;
    SNode* next = nullptr;
};

struct CList {
    SNode* head = nullptr;
    SNode* tail = nullptr;
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

DNode* createDNode(int code, const char name[], const char cuisine[], double rating) {
    DNode* node = new DNode;
    createdNodes++;
    node->code = code;
    copyText(node->name, name, NAME_LEN);
    copyText(node->cuisine, cuisine, CUISINE_LEN);
    node->rating = rating;
    return node;
}

SNode* createSNode(int code, const char name[], const char cuisine[], double rating) {
    SNode* node = new SNode;
    createdNodes++;
    node->code = code;
    copyText(node->name, name, NAME_LEN);
    copyText(node->cuisine, cuisine, CUISINE_LEN);
    node->rating = rating;
    return node;
}

void freeDNode(DNode*& node) {
    delete node;
    freedNodes++;
    node = nullptr;
}

void freeSNode(SNode*& node) {
    delete node;
    freedNodes++;
    node = nullptr;
}

void printCodes(const int codes[], int n) {
    if (n == 0) cout << "порожньо";
    for (int i = 0; i < n; i++) {
        cout << codes[i];
        if (i + 1 < n) cout << ", ";
    }
}

bool sameCodes(const int a[], int na, const int b[], int nb) {
    if (na != nb) return false;
    for (int i = 0; i < na; i++)
        if (a[i] != b[i]) return false;
    return true;
}

void dPushBack(DList& l, DNode* node) {
    node->next = nullptr;
    node->prev = l.tail;
    if (l.tail == nullptr) l.head = node;
    else l.tail->next = node;
    l.tail = node;
    l.count++;
}

bool dInsertAt(DList& l, int pos, int code) {
    if (pos < 0 || pos > l.count) return false;
    DNode* node = createDNode(code, "Ресторан", "українська", 4.0);
    if (pos == l.count) {
        dPushBack(l, node);
        return true;
    }
    DNode* after = l.head;
    for (int i = 0; i < pos; i++) after = after->next;
    DNode* before = after->prev;
    node->next = after;
    node->prev = before;
    after->prev = node;
    if (before == nullptr) l.head = node;
    else before->next = node;
    l.count++;
    return true;
}

void dUnlink(DList& l, DNode* target) {
    DNode* before = target->prev;
    DNode* after = target->next;
    if (before == nullptr) l.head = after;
    else before->next = after;
    if (after == nullptr) l.tail = before;
    else after->prev = before;
    freeDNode(target);
    l.count--;
}

bool dRemoveAt(DList& l, int pos) {
    if (pos < 0 || pos >= l.count) return false;
    DNode* target = l.head;
    for (int i = 0; i < pos; i++) target = target->next;
    dUnlink(l, target);
    return true;
}

bool dRemoveKey(DList& l, int key) {
    DNode* current = l.head;
    while (current != nullptr && current->code != key) current = current->next;
    if (current == nullptr) return false;
    dUnlink(l, current);
    return true;
}

int dForward(const DList& l, int out[]) {
    int n = 0;
    DNode* current = l.head;
    while (current != nullptr && n < MAX_OUT) {
        out[n] = current->code;
        n++;
        current = current->next;
    }
    return n;
}

int dBackward(const DList& l, int out[]) {
    int n = 0;
    DNode* current = l.tail;
    while (current != nullptr && n < MAX_OUT) {
        out[n] = current->code;
        n++;
        current = current->prev;
    }
    return n;
}

void dClear(DList& l) {
    DNode* current = l.head;
    while (current != nullptr) {
        DNode* nextNode = current->next;
        freeDNode(current);
        current = nextNode;
    }
    l.head = nullptr;
    l.tail = nullptr;
    l.count = 0;
}

const int INV_OK = 0;
const int INV_EMPTY = 1;
const int INV_BREAK = 2;
const int INV_BACK = 3;
const int INV_TAIL = 4;
const int INV_COUNT = 5;
const int INV_NOT_CLOSED = 6;
const int INV_LENGTH = 7;
const int INV_BOUNDS = 8;

const char* invText(int code) {
    if (code == INV_OK) return "інваріанти виконано";
    if (code == INV_EMPTY) return "неузгоджений порожній стан";
    if (code == INV_BREAK) return "розрив";
    if (code == INV_BACK) return "неправильний зворотний зв'язок";
    if (code == INV_TAIL) return "хибний кінець";
    if (code == INV_COUNT) return "неправильна кількість";
    if (code == INV_NOT_CLOSED) return "незамкнений цикл";
    if (code == INV_LENGTH) return "цикл неправильної довжини";
    return "неправильні зовнішні зв'язки меж";
}

int dCheck(const DList& l) {
    if (l.count == 0 || l.head == nullptr || l.tail == nullptr) {
        if (l.count == 0 && l.head == nullptr && l.tail == nullptr) return INV_OK;
        return INV_EMPTY;
    }
    if (l.head->prev != nullptr || l.tail->next != nullptr) return INV_BOUNDS;
    DNode* current = l.head;
    DNode* previous = nullptr;
    int steps = 0;
    while (current != nullptr) {
        if (steps >= MAX_STEPS) return INV_LENGTH;
        if (current->prev != previous) return INV_BACK;
        previous = current;
        current = current->next;
        steps++;
    }
    if (previous != l.tail) return steps < l.count ? INV_BREAK : INV_TAIL;
    if (steps != l.count) return INV_COUNT;
    return INV_OK;
}

void dDiagnostics(const DList& l) {
    DNode* current = l.head;
    int k = 0;
    if (current == nullptr) cout << "  Список порожній\n";
    while (current != nullptr && k < l.count + 1) {
        bool ok = (current->next == nullptr || current->next->prev == current);
        cout << "  " << k << " | код " << current->code << " | prev ";
        if (current->prev == nullptr) cout << "nullptr";
        else cout << current->prev;
        cout << " | адреса " << current << " | next ";
        if (current->next == nullptr) cout << "nullptr";
        else cout << current->next;
        cout << " | " << (ok ? "зв'язок узгоджено" : "ПОРУШЕНО: next->prev != вузол") << "\n";
        current = current->next;
        k++;
    }
}

void cPushBack(CList& l, SNode* node) {
    if (l.head == nullptr) {
        l.head = node;
        l.tail = node;
    } else {
        l.tail->next = node;
        l.tail = node;
    }
    l.tail->next = l.head;
    l.count++;
}

int cFullTraversal(const CList& l, int out[]) {
    if (l.head == nullptr) return 0;
    int n = 0;
    SNode* current = l.head;
    do {
        out[n] = current->code;
        n++;
        current = current->next;
    } while (current != l.head && n < MAX_OUT);
    return n;
}

int cWalk(const CList& l, int steps, int out[], SNode*& finish) {
    finish = l.head;
    if (l.head == nullptr) return 0;
    SNode* current = l.head;
    int n = 0;
    for (int i = 0; i < steps; i++) {
        if (n < MAX_OUT) {
            out[n] = current->code;
            n++;
        }
        current = current->next;
    }
    finish = current;
    return n;
}

int cCheck(const CList& l) {
    if (l.count == 0 || l.head == nullptr || l.tail == nullptr) {
        if (l.count == 0 && l.head == nullptr && l.tail == nullptr) return INV_OK;
        return INV_EMPTY;
    }
    SNode* current = l.head;
    for (int i = 0; i < l.count; i++) {
        if (current == nullptr) return INV_NOT_CLOSED;
        if (i > 0 && current == l.head) return INV_LENGTH;
        if (i == l.count - 1 && current != l.tail) return INV_TAIL;
        current = current->next;
    }
    if (current == nullptr) return INV_NOT_CLOSED;
    if (current != l.head) return INV_LENGTH;
    return INV_OK;
}

void cClear(CList& l) {
    if (l.head == nullptr) return;
    l.tail->next = nullptr;
    SNode* current = l.head;
    while (current != nullptr) {
        SNode* nextNode = current->next;
        freeSNode(current);
        current = nextNode;
    }
    l.head = nullptr;
    l.tail = nullptr;
    l.count = 0;
}

void cdPushBack(DList& l, DNode* node) {
    if (l.head == nullptr) {
        l.head = node;
        l.tail = node;
    } else {
        node->prev = l.tail;
        l.tail->next = node;
        l.tail = node;
    }
    l.tail->next = l.head;
    l.head->prev = l.tail;
    l.count++;
}

bool cdRemoveAt(DList& l, int pos) {
    if (pos < 0 || pos >= l.count) return false;
    DNode* target = l.head;
    for (int i = 0; i < pos; i++) target = target->next;
    if (l.count == 1) {
        freeDNode(target);
        l.head = nullptr;
        l.tail = nullptr;
        l.count = 0;
        return true;
    }
    target->prev->next = target->next;
    target->next->prev = target->prev;
    if (target == l.head) l.head = target->next;
    if (target == l.tail) l.tail = target->prev;
    freeDNode(target);
    l.count--;
    return true;
}

int cdForward(const DList& l, int out[]) {
    if (l.head == nullptr) return 0;
    int n = 0;
    DNode* current = l.head;
    do {
        out[n] = current->code;
        n++;
        current = current->next;
    } while (current != l.head && n < MAX_OUT);
    return n;
}

int cdBackward(const DList& l, int out[]) {
    if (l.tail == nullptr) return 0;
    int n = 0;
    DNode* current = l.tail;
    do {
        out[n] = current->code;
        n++;
        current = current->prev;
    } while (current != l.tail && n < MAX_OUT);
    return n;
}

int cdCheck(const DList& l) {
    if (l.count == 0 || l.head == nullptr || l.tail == nullptr) {
        if (l.count == 0 && l.head == nullptr && l.tail == nullptr) return INV_OK;
        return INV_EMPTY;
    }
    if (l.head->prev != l.tail || l.tail->next != l.head) return INV_NOT_CLOSED;
    DNode* current = l.head;
    for (int i = 0; i < l.count; i++) {
        if (current == nullptr || current->next == nullptr) return INV_NOT_CLOSED;
        if (i > 0 && current == l.head) return INV_LENGTH;
        if (current->next->prev != current) return INV_BACK;
        if (i == l.count - 1 && current != l.tail) return INV_TAIL;
        current = current->next;
    }
    if (current != l.head) return INV_LENGTH;
    current = l.tail;
    for (int i = 0; i < l.count; i++) {
        if (current == nullptr) return INV_NOT_CLOSED;
        current = current->prev;
    }
    if (current != l.tail) return INV_LENGTH;
    return INV_OK;
}

void cdClear(DList& l) {
    if (l.head == nullptr) return;
    l.tail->next = nullptr;
    l.head->prev = nullptr;
    dClear(l);
}

void dBuild(DList& l, const int keys[], int n) {
    for (int i = 0; i < n; i++) dPushBack(l, createDNode(keys[i], "Ресторан", "українська", 4.0));
}

void cBuild(CList& l, const int keys[], int n) {
    for (int i = 0; i < n; i++) cPushBack(l, createSNode(keys[i], "Ресторан", "українська", 4.0));
}

void cdBuild(DList& l, const int keys[], int n) {
    for (int i = 0; i < n; i++) cdPushBack(l, createDNode(keys[i], "Ресторан", "українська", 4.0));
}

bool dIs(const DList& l, const int expected[], int n) {
    int fw[MAX_OUT], bw[MAX_OUT];
    int nf = dForward(l, fw);
    int nb = dBackward(l, bw);
    if (!sameCodes(fw, nf, expected, n) || nb != n) return false;
    for (int i = 0; i < n; i++)
        if (bw[i] != expected[n - 1 - i]) return false;
    return dCheck(l) == INV_OK && l.count == n;
}

int passed = 0, total = 0;

void report(int num, const char* type, const char* name, bool ok, long long base) {
    bool memOk = activeNodes() == base;
    bool result = ok && memOk;
    total++;
    if (result) passed++;
    cout << num << ". [" << type << "] " << name << " | активних після очищення " << activeNodes() - base
         << " | " << (result ? "успішно" : "ПОМИЛКА") << "\n";
}

void runTests() {
    cout << "=== Автоматизовані тести ===\n";
    int out[MAX_OUT], out2[MAX_OUT];
    {
        DList l;
        long long b = activeNodes();
        bool ok = dForward(l, out) == 0 && dBackward(l, out2) == 0 && dCheck(l) == INV_OK;
        report(1, "двонапрямлений", "порожньо, обхід -> 0 вузлів", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[1] = {5};
        dBuild(l, s, 1);
        bool ok = dIs(l, s, 1) && l.head == l.tail && l.head->prev == nullptr && l.head->next == nullptr;
        dClear(l);
        report(2, "двонапрямлений", "5, два обходи -> 5 і 5", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3}, r[3] = {3, 2, 1};
        dBuild(l, s, 3);
        int n = dBackward(l, out);
        bool ok = sameCodes(out, n, r, 3) && dForward(l, out2) == n && dIs(l, s, 3);
        dClear(l);
        report(3, "двонапрямлений", "1, 2, 3, зворотний обхід -> 3, 2, 1", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[2] = {1, 3}, e[3] = {1, 2, 3};
        dBuild(l, s, 2);
        bool ok = dInsertAt(l, 1, 2) && dIs(l, e, 3);
        dClear(l);
        report(4, "двонапрямлений", "1, 3, вставити 2 -> 1, 2, 3", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3}, e[2] = {1, 3};
        dBuild(l, s, 3);
        bool ok = dRemoveKey(l, 2) && dIs(l, e, 2);
        dClear(l);
        report(5, "двонапрямлений", "1, 2, 3, видалити 2 -> 1, 3", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[1] = {7};
        dBuild(l, s, 1);
        bool ok = dRemoveAt(l, 0) && dIs(l, s, 0) && l.head == nullptr && l.tail == nullptr;
        report(6, "двонапрямлений", "7, видалити -> порожньо", ok, b);
    }
    {
        CList l;
        long long b = activeNodes();
        int s[1] = {4};
        cBuild(l, s, 1);
        SNode* finish;
        cWalk(l, 4, out, finish);
        bool ok = finish == l.head && l.head->next == l.head && cCheck(l) == INV_OK;
        cClear(l);
        report(7, "циклічний однонапрямлений", "4, 4 кроки -> повернення до початку", ok, b);
    }
    {
        CList l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3}, e[5] = {1, 2, 3, 1, 2};
        cBuild(l, s, 3);
        SNode* finish;
        int n = cWalk(l, 5, out, finish);
        bool ok = sameCodes(out, n, e, 5) && cCheck(l) == INV_OK;
        cClear(l);
        report(8, "циклічний однонапрямлений", "1, 2, 3, 5 кроків -> 1, 2, 3, 1, 2", ok, b);
    }
    {
        CList l;
        long long b = activeNodes();
        int s[2] = {1, 2}, e[3] = {1, 2, 3};
        cBuild(l, s, 2);
        cPushBack(l, createSNode(3, "Ресторан", "українська", 4.0));
        int n = cFullTraversal(l, out);
        SNode* finish;
        cWalk(l, 3, out2, finish);
        bool ok = sameCodes(out, n, e, 3) && l.count == 3 && finish == l.head && cCheck(l) == INV_OK;
        cClear(l);
        report(9, "циклічний однонапрямлений", "1, 2, вставити 3 -> цикл довжини 3", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3};
        cdBuild(l, s, 3);
        int n = cdForward(l, out);
        bool ok = sameCodes(out, n, s, 3) && cdCheck(l) == INV_OK;
        cdClear(l);
        report(10, "циклічний двонапрямлений", "1, 2, 3, прямий цикл -> 1, 2, 3", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3}, r[3] = {3, 2, 1};
        cdBuild(l, s, 3);
        int n = cdBackward(l, out);
        bool ok = sameCodes(out, n, r, 3) && cdCheck(l) == INV_OK;
        cdClear(l);
        report(11, "циклічний двонапрямлений", "1, 2, 3, зворотний цикл -> 3, 2, 1", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[1] = {1};
        cdBuild(l, s, 1);
        bool ok = l.head->next == l.head && l.head->prev == l.head && cdCheck(l) == INV_OK;
        cdClear(l);
        report(12, "циклічний двонапрямлений", "1, перевірити зв'язки -> обидва ведуть до себе", ok, b);
    }
    {
        long long b = activeNodes();
        int s[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        DList d, cd;
        CList c;
        dBuild(d, s, 10);
        cBuild(c, s, 10);
        cdBuild(cd, s, 10);
        bool ok = activeNodes() - b == 30;
        dClear(d);
        cClear(c);
        cdClear(cd);
        ok = ok && dCheck(d) == INV_OK && cCheck(c) == INV_OK && cdCheck(cd) == INV_OK;
        report(13, "усі типи", "10 вузлів, очистити -> 0 активних", ok, b);
    }
    {
        long long b = activeNodes();
        int s[3] = {4, 5, 6};
        DList d, cd;
        CList c;
        dBuild(d, s, 3);
        cBuild(c, s, 3);
        cdBuild(cd, s, 3);
        dClear(d);
        cClear(c);
        cdClear(cd);
        dBuild(d, s, 3);
        cBuild(c, s, 3);
        cdBuild(cd, s, 3);
        int n1 = cFullTraversal(c, out), n2 = cdForward(cd, out2);
        bool ok = dIs(d, s, 3) && sameCodes(out, n1, s, 3) && sameCodes(out2, n2, s, 3)
                  && cCheck(c) == INV_OK && cdCheck(cd) == INV_OK;
        dClear(d);
        cClear(c);
        cdClear(cd);
        report(14, "усі типи", "після очищення створити повторно -> коректна структура", ok, b);
    }
    {
        CList l;
        long long b = activeNodes();
        int s[3] = {1, 2, 3};
        cBuild(l, s, 3);
        SNode* finish;
        int n = cWalk(l, 100, out, finish);
        bool ok = n == 100 && out[99] == 1 && finish == l.head->next && cCheck(l) == INV_OK;
        cClear(l);
        report(15, "циклічний", "3 вузли, контроль 100 кроків -> завершення без зависання", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[2] = {2, 3}, e[4] = {1, 2, 3, 4};
        dBuild(l, s, 2);
        bool ok = dInsertAt(l, 0, 1) && l.head->code == 1 && dInsertAt(l, l.count, 4) && l.tail->code == 4
                  && dIs(l, e, 4);
        dClear(l);
        report(16, "двонапрямлений", "2, 3, вставити 1 на початок і 4 в кінець -> 1, 2, 3, 4", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[4] = {1, 2, 3, 4}, e[2] = {2, 3};
        dBuild(l, s, 4);
        bool ok = dRemoveAt(l, 0) && dRemoveAt(l, l.count - 1) && l.head->prev == nullptr && l.tail->next == nullptr
                  && dIs(l, e, 2);
        dClear(l);
        report(17, "двонапрямлений", "1, 2, 3, 4, видалити початок і кінець -> 2, 3", ok, b);
    }
    {
        DList l;
        long long b = activeNodes();
        int s[4] = {1, 2, 3, 4}, e[2] = {2, 3};
        cdBuild(l, s, 4);
        bool ok = cdRemoveAt(l, 0) && cdRemoveAt(l, l.count - 1) && l.head->prev == l.tail && l.tail->next == l.head
                  && cdForward(l, out) == 2 && sameCodes(out, 2, e, 2) && cdCheck(l) == INV_OK;
        cdClear(l);
        report(18, "циклічний двонапрямлений", "1, 2, 3, 4, видалити обидві межі -> 2, 3", ok, b);
    }
    {
        long long b = activeNodes();
        int s[5] = {1, 2, 3, 4, 5};
        DList d, cd;
        CList c;
        dBuild(d, s, 5);
        cBuild(c, s, 5);
        cdBuild(cd, s, 5);
        dClear(d);
        cClear(c);
        cdClear(cd);
        long long freedAfterFirst = freedNodes;
        dClear(d);
        cClear(c);
        cdClear(cd);
        bool ok = freedNodes == freedAfterFirst && dCheck(d) == INV_OK && cCheck(c) == INV_OK && cdCheck(cd) == INV_OK;
        report(19, "усі типи", "два послідовні очищення -> друге нічого не звільняє", ok, b);
    }
    {
        long long b = activeNodes();
        int s[3] = {1, 2, 3};
        DList d, cd;
        CList c;
        dBuild(d, s, 3);
        cBuild(c, s, 3);
        cdBuild(cd, s, 3);
        bool ok = true;
        DNode* savedPrev = d.tail->prev;
        d.tail->prev = d.head;
        cout << "  Діагностика пошкодженого двонапрямленого списку:\n";
        dDiagnostics(d);
        ok = ok && dCheck(d) == INV_BACK;
        d.tail->prev = savedPrev;
        DNode* savedNext = d.head->next;
        d.head->next = nullptr;
        ok = ok && dCheck(d) == INV_BREAK;
        d.head->next = savedNext;
        c.tail->next = nullptr;
        ok = ok && cCheck(c) == INV_NOT_CLOSED;
        c.tail->next = c.head->next;
        ok = ok && cCheck(c) == INV_LENGTH;
        c.tail->next = c.head;
        DNode* savedCdPrev = cd.head->next->prev;
        cd.head->next->prev = cd.tail;
        ok = ok && cdCheck(cd) == INV_BACK;
        cd.head->next->prev = savedCdPrev;
        ok = ok && dCheck(d) == INV_OK && cCheck(c) == INV_OK && cdCheck(cd) == INV_OK;
        dClear(d);
        cClear(c);
        cdClear(cd);
        report(20, "усі типи", "навмисні пошкодження -> розрив, зворотний зв'язок, незамкнений цикл, довжина", ok, b);
    }
    cout << "Успішних тестів: " << passed << " з " << total << "\n\n";
}

void printDList(const DList& l) {
    DNode* current = l.head;
    int pos = 0;
    while (current != nullptr) {
        cout << "  " << pos << " | код " << current->code << " | " << current->name << " | "
             << current->cuisine << " | " << current->rating << "\n";
        current = current->next;
        pos++;
    }
}

void showBoth(const DList& l) {
    int fw[MAX_OUT], bw[MAX_OUT];
    int nf = dForward(l, fw);
    int nb = dBackward(l, bw);
    cout << "  прямий обхід: ";
    printCodes(fw, nf);
    cout << " (вузлів: " << nf << ")\n  зворотний обхід: ";
    printCodes(bw, nb);
    cout << " (вузлів: " << nb << ")\n  перевірка: " << invText(dCheck(l)) << ", кількість " << l.count << "\n";
}

void runVariant() {
    cout << "=== Варіант 26 ===\n";
    long long base = activeNodes();
    int keys[4] = {21, 7, 17, 12};
    const char* names[4] = {"Пузата хата", "Мафія", "Козачок", "Челентано"};
    const char* cuisines[4] = {"українська", "італійська", "українська", "італійська"};
    double ratings[4] = {4.2, 4.0, 4.7, 3.9};
    DList d;
    for (int i = 0; i < 4; i++) dPushBack(d, createDNode(keys[i], names[i], cuisines[i], ratings[i]));
    cout << "Двонапрямлений список:\n";
    printDList(d);
    showBoth(d);
    cout << "Діагностика зв'язків:\n";
    dDiagnostics(d);

    cout << "Операція: видалити кінець -> " << (dRemoveAt(d, d.count - 1) ? "успішно" : "відмова") << "\n";
    printDList(d);
    showBoth(d);
    cout << "  head " << d.head->code << ", tail " << d.tail->code << ", tail->next = "
         << (d.tail->next == nullptr ? "nullptr" : "НЕ nullptr") << "\n";

    int fw[MAX_OUT];
    int nf = dForward(d, fw);
    CList c;
    DList cd;
    for (int i = 0; i < nf; i++) {
        cPushBack(c, createSNode(fw[i], "Ресторан", "українська", 4.0));
        cdPushBack(cd, createDNode(fw[i], "Ресторан", "українська", 4.0));
    }
    int out[MAX_OUT] = {};
    SNode* finish;
    int n = cWalk(c, 15, out, finish);
    cout << "Циклічний однонапрямлений, обхід 15 кроків: ";
    printCodes(out, n);
    cout << "\n  після 15 кроків покажчик на вузлі " << finish->code
         << (finish == c.head ? " (початок)" : "") << " | " << invText(cCheck(c)) << "\n";
    int k = cFullTraversal(c, out);
    cout << "  один повний обхід: ";
    printCodes(out, k);
    cout << " (кроків: " << k << ")\n";

    DNode* current = cd.head;
    cout << "Циклічний двонапрямлений, 15 кроків уперед: ";
    for (int i = 0; i < 15; i++) {
        cout << current->code << (i < 14 ? ", " : "");
        current = current->next;
    }
    cout << "\n  після 15 кроків уперед покажчик на " << current->code << (current == cd.head ? " (початок)" : "") << "\n";
    current = cd.tail;
    cout << "Циклічний двонапрямлений, 15 кроків назад від кінця: ";
    for (int i = 0; i < 15; i++) {
        cout << current->code << (i < 14 ? ", " : "");
        current = current->prev;
    }
    cout << "\n  після 15 кроків назад покажчик на " << current->code << (current == cd.tail ? " (кінець)" : "") << "\n";
    cout << "  перевірка: " << invText(cdCheck(cd)) << "\n";

    dClear(d);
    cClear(c);
    cdClear(cd);
    cout << "Після очищення всіх трьох списків: активних вузлів " << activeNodes() - base << "\n\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    runTests();
    runVariant();
    cout << "Створено вузлів: " << createdNodes << ", звільнено: " << freedNodes
         << ", активних: " << activeNodes() << "\n";
    return 0;
}
