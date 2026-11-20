#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const int NAME_LEN = 60;
const int MAX_NODES = 1000;

struct TreeNode {
    int code = 0;
    char name[NAME_LEN] = "";
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

struct BST {
    TreeNode* root = nullptr;
    int count = 0;
};

long long createdNodes = 0;
long long freedNodes = 0;

long long activeNodes() {
    return createdNodes - freedNodes;
}

TreeNode* createNode(int code) {
    TreeNode* node = new TreeNode;
    createdNodes++;
    node->code = code;
    const char base[] = "Ресторан";
    int i = 0;
    while (base[i] != '\0' && i < NAME_LEN - 1) {
        node->name[i] = base[i];
        i++;
    }
    node->name[i] = '\0';
    return node;
}

void freeNode(TreeNode*& node) {
    delete node;
    freedNodes++;
    node = nullptr;
}

void clearNodes(TreeNode*& node) {
    if (node == nullptr) return;
    clearNodes(node->left);
    clearNodes(node->right);
    freeNode(node);
}

void clearTree(BST& t) {
    clearNodes(t.root);
    t.count = 0;
}

void inorder(const TreeNode* node, int out[], int& n) {
    if (node == nullptr) return;
    inorder(node->left, out, n);
    out[n++] = node->code;
    inorder(node->right, out, n);
}

int countNodes(const TreeNode* node) {
    if (node == nullptr) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

int height(const TreeNode* node) {
    if (node == nullptr) return 0;
    int hl = height(node->left), hr = height(node->right);
    return 1 + (hl > hr ? hl : hr);
}

bool collectAddresses(const TreeNode* node, const TreeNode* seen[], int& n) {
    if (node == nullptr) return true;
    for (int i = 0; i < n; i++)
        if (seen[i] == node) return false;
    if (n >= MAX_NODES) return false;
    seen[n++] = node;
    return collectAddresses(node->left, seen, n) && collectAddresses(node->right, seen, n);
}

bool noCycles(const TreeNode* root) {
    const TreeNode* seen[MAX_NODES];
    int n = 0;
    return collectAddresses(root, seen, n);
}

int badValue = 0, badBound = 0;
bool badIsUpper = false;

bool checkRange(const TreeNode* node, bool hasLow, int low, bool hasHigh, int high) {
    if (node == nullptr) return true;
    if (hasLow && node->code <= low) {
        badValue = node->code;
        badBound = low;
        badIsUpper = false;
        return false;
    }
    if (hasHigh && node->code >= high) {
        badValue = node->code;
        badBound = high;
        badIsUpper = true;
        return false;
    }
    return checkRange(node->left, hasLow, low, true, node->code) && checkRange(node->right, true, node->code, hasHigh, high);
}

bool isBST(const TreeNode* root) {
    return checkRange(root, false, 0, false, 0);
}

TreeNode* searchIterative(const BST& t, int key, int& comparisons, int path[], int& pathLen) {
    comparisons = 0;
    pathLen = 0;
    TreeNode* current = t.root;
    while (current != nullptr) {
        path[pathLen++] = current->code;
        comparisons++;
        if (key == current->code) return current;
        comparisons++;
        if (key < current->code) current = current->left;
        else current = current->right;
    }
    return nullptr;
}

TreeNode* searchRecursive(TreeNode* node, int key, int& comparisons, int path[], int& pathLen) {
    if (node == nullptr) return nullptr;
    path[pathLen++] = node->code;
    comparisons++;
    if (key == node->code) return node;
    comparisons++;
    if (key < node->code) return searchRecursive(node->left, key, comparisons, path, pathLen);
    return searchRecursive(node->right, key, comparisons, path, pathLen);
}

const int INS_OK = 0;
const int INS_DUPLICATE = 1;

int insertKey(BST& t, int key) {
    TreeNode** link = &t.root;
    while (*link != nullptr) {
        if (key == (*link)->code) return INS_DUPLICATE;
        if (key < (*link)->code) link = &(*link)->left;
        else link = &(*link)->right;
    }
    *link = createNode(key);
    t.count++;
    return INS_OK;
}

const int DEL_NOT_FOUND = 0;
const int DEL_LEAF = 1;
const int DEL_ONE = 2;
const int DEL_TWO = 3;

const char* deleteText(int code) {
    if (code == DEL_LEAF) return "листок";
    if (code == DEL_ONE) return "один нащадок";
    if (code == DEL_TWO) return "два нащадки";
    return "не знайдено";
}

int removeAtLink(TreeNode** link) {
    TreeNode* target = *link;
    if (target->left == nullptr && target->right == nullptr) {
        *link = nullptr;
        freeNode(target);
        return DEL_LEAF;
    }
    if (target->left == nullptr || target->right == nullptr) {
        *link = (target->left != nullptr) ? target->left : target->right;
        freeNode(target);
        return DEL_ONE;
    }
    TreeNode** succLink = &target->right;
    while ((*succLink)->left != nullptr) succLink = &(*succLink)->left;
    TreeNode* successor = *succLink;
    target->code = successor->code;
    for (int i = 0; i < NAME_LEN; i++) target->name[i] = successor->name[i];
    *succLink = successor->right;
    freeNode(successor);
    return DEL_TWO;
}

int removeKey(BST& t, int key) {
    TreeNode** link = &t.root;
    while (*link != nullptr && (*link)->code != key) {
        if (key < (*link)->code) link = &(*link)->left;
        else link = &(*link)->right;
    }
    if (*link == nullptr) return DEL_NOT_FOUND;
    int result = removeAtLink(link);
    t.count--;
    return result;
}

void toPrefix(const TreeNode* node, char buf[], int& len) {
    if (node == nullptr) {
        buf[len++] = '#';
        buf[len++] = ' ';
        buf[len] = '\0';
        return;
    }
    char tmp[16];
    int v = node->code, k = 0;
    bool neg = v < 0;
    if (neg) v = -v;
    do {
        tmp[k++] = char('0' + v % 10);
        v /= 10;
    } while (v > 0);
    if (neg) buf[len++] = '-';
    while (k > 0) buf[len++] = tmp[--k];
    buf[len++] = ' ';
    buf[len] = '\0';
    toPrefix(node->left, buf, len);
    toPrefix(node->right, buf, len);
}

void printSeq(const int a[], int n) {
    if (n == 0) cout << "порожньо";
    for (int i = 0; i < n; i++) cout << a[i] << (i + 1 < n ? ", " : "");
}

bool sameSeq(const int a[], int na, const int b[], int nb) {
    if (na != nb) return false;
    for (int i = 0; i < na; i++)
        if (a[i] != b[i]) return false;
    return true;
}

bool isSorted(const int a[], int n) {
    for (int i = 1; i < n; i++)
        if (a[i - 1] >= a[i]) return false;
    return true;
}

bool structureOk(const BST& t, long long base) {
    int seq[MAX_NODES], n = 0;
    inorder(t.root, seq, n);
    return isBST(t.root) && noCycles(t.root) && countNodes(t.root) == t.count && n == t.count && isSorted(seq, n)
           && activeNodes() - base == t.count;
}

void buildTree(BST& t, const int keys[], int n) {
    for (int i = 0; i < n; i++) insertKey(t, keys[i]);
}

int passed = 0, total = 0;

void report(int num, const char* name, bool ok, BST& t, long long base) {
    int seq[MAX_NODES], n = 0;
    inorder(t.root, seq, n);
    cout << num << ". " << name << " -> ";
    printSeq(seq, n);
    bool structure = structureOk(t, base);
    clearTree(t);
    bool mem = t.root == nullptr && activeNodes() == base;
    bool result = ok && structure && mem;
    total++;
    if (result) passed++;
    cout << " | операція " << (ok ? "так" : "НІ") << ", структура " << (structure ? "так" : "НІ") << ", пам'ять "
         << (mem ? "так" : "НІ") << " | " << (result ? "успішно" : "ПОМИЛКА") << "\n";
}

bool inorderIs(const BST& t, const int e[], int ne) {
    int seq[MAX_NODES], n = 0;
    inorder(t.root, seq, n);
    return sameSeq(seq, n, e, ne);
}

bool searchBoth(BST& t, int key, bool expectFound, int expectPath) {
    int c1, c2 = 0, p1[MAX_NODES], p2[MAX_NODES], n1, n2 = 0;
    char before[4000], after[4000];
    int lb = 0, la = 0;
    toPrefix(t.root, before, lb);
    TreeNode* r1 = searchIterative(t, key, c1, p1, n1);
    TreeNode* r2 = searchRecursive(t.root, key, c2, p2, n2);
    toPrefix(t.root, after, la);
    bool unchanged = lb == la;
    for (int i = 0; i < lb && unchanged; i++)
        if (before[i] != after[i]) unchanged = false;
    return r1 == r2 && (r1 != nullptr) == expectFound && c1 == c2 && sameSeq(p1, n1, p2, n2) && n1 == expectPath
           && unchanged;
}

void runTests() {
    cout << "=== Автоматизовані сценарії ===\n";
    {
        BST t;
        long long b = activeNodes();
        bool ok = searchBoth(t, 5, false, 0) && t.root == nullptr;
        report(1, "порожньо, пошук 5", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int e[1] = {5};
        bool ok = insertKey(t, 5) == INS_OK && inorderIs(t, e, 1) && t.count == 1;
        report(2, "порожньо, вставити 5", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[1] = {5};
        buildTree(t, s, 1);
        bool ok = removeKey(t, 5) == DEL_LEAF && t.root == nullptr && t.count == 0;
        report(3, "5, видалити 5 (листок-корінь)", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[3] = {5, 3, 7}, e[4] = {3, 4, 5, 7};
        buildTree(t, s, 3);
        bool ok = insertKey(t, 4) == INS_OK && inorderIs(t, e, 4) && t.count == 4;
        report(4, "5, 3, 7, вставити 4", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[3] = {5, 3, 7};
        buildTree(t, s, 3);
        long long created = createdNodes;
        bool ok = insertKey(t, 5) == INS_DUPLICATE && createdNodes == created && t.count == 3;
        int e[3] = {3, 5, 7};
        ok = ok && inorderIs(t, e, 3);
        report(5, "5, 3, 7, вставити 5 (повтор заборонено)", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[3] = {5, 3, 7}, e[2] = {5, 7};
        buildTree(t, s, 3);
        bool ok = removeKey(t, 3) == DEL_LEAF && inorderIs(t, e, 2);
        report(6, "5, 3, 7, видалити 3 (листок)", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[4] = {5, 3, 7, 6}, e[3] = {3, 5, 6};
        buildTree(t, s, 4);
        bool ok = removeKey(t, 7) == DEL_ONE && inorderIs(t, e, 3) && t.root->right->code == 6;
        report(7, "5, 3, 7, 6, видалити 7 (один нащадок)", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[5] = {5, 3, 8, 7, 9}, e[4] = {3, 5, 7, 9};
        buildTree(t, s, 5);
        bool ok = removeKey(t, 8) == DEL_TWO && inorderIs(t, e, 4) && t.root->right->code == 9;
        report(8, "5, 3, 8, 7, 9, видалити 8 (два нащадки)", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[5] = {5, 3, 8, 7, 9}, e[4] = {3, 7, 8, 9};
        buildTree(t, s, 5);
        bool ok = removeKey(t, 5) == DEL_TWO && inorderIs(t, e, 4) && t.root->code == 7;
        report(9, "5, 3, 8, 7, 9, видалити корінь 5", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[3] = {5, 3, 7}, e[3] = {3, 5, 7};
        buildTree(t, s, 3);
        TreeNode* oldRoot = t.root;
        bool ok = removeKey(t, 10) == DEL_NOT_FOUND && inorderIs(t, e, 3) && t.root == oldRoot && t.count == 3;
        report(10, "5, 3, 7, видалити 10 (відсутній)", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[4] = {1, 2, 3, 4};
        buildTree(t, s, 4);
        bool ok = searchBoth(t, 4, true, 4);
        report(11, "1, 2, 3, 4, пошук 4 (шлях 4 вузли)", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[7] = {4, 2, 6, 1, 3, 5, 7};
        buildTree(t, s, 7);
        bool ok = searchBoth(t, 7, true, 3);
        report(12, "4, 2, 6, 1, 3, 5, 7, пошук 7 (шлях 3 вузли)", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[4] = {5, 3, 7, 6}, e[3] = {3, 6, 7};
        buildTree(t, s, 4);
        bool ok = removeKey(t, 5) == DEL_TWO && inorderIs(t, e, 3) && t.root->code == 6;
        report(13, "5, 3, 7, 6, видалити корінь 5", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[2] = {2, 1}, e[1] = {1};
        buildTree(t, s, 2);
        bool ok = removeKey(t, 2) == DEL_ONE && inorderIs(t, e, 1) && t.root->code == 1;
        report(14, "2, 1, видалити корінь 2", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[2] = {2, 3}, e[1] = {3};
        buildTree(t, s, 2);
        bool ok = removeKey(t, 2) == DEL_ONE && inorderIs(t, e, 1) && t.root->code == 3;
        report(15, "2, 3, видалити корінь 2", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[3] = {5, 3, 7}, e[2] = {5, 7};
        buildTree(t, s, 3);
        bool ok = removeKey(t, 3) == DEL_LEAF && removeKey(t, 3) == DEL_NOT_FOUND && inorderIs(t, e, 2) && t.count == 2;
        report(16, "власний: повторне видалення 3", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[6] = {5, 3, 8, 6, 9, 7}, e[5] = {3, 6, 7, 8, 9};
        buildTree(t, s, 6);
        bool ok = removeKey(t, 5) == DEL_TWO && inorderIs(t, e, 5) && t.root->code == 6
                  && t.root->right->left->code == 7;
        report(17, "власний: замінник 6 з нащадком 7", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[7] = {5, 3, 8, 2, 4, 7, 9}, e[4] = {2, 3, 4, 9};
        buildTree(t, s, 7);
        bool ok = removeKey(t, t.root->code) == DEL_TWO && t.root->code == 7 && structureOk(t, b);
        ok = ok && removeKey(t, t.root->code) == DEL_TWO && t.root->code == 8 && structureOk(t, b);
        ok = ok && removeKey(t, t.root->code) == DEL_TWO && t.root->code == 9 && inorderIs(t, e, 4);
        report(18, "власний: три послідовні видалення кореня", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[7] = {53, 29, 75, 14, 42, 63, 87};
        buildTree(t, s, 7);
        bool ok = t.count == 7 && activeNodes() - b == 7;
        clearTree(t);
        ok = ok && t.root == nullptr && t.count == 0 && activeNodes() == b;
        report(19, "власний: очищення 7 вузлів", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[5] = {4, 2, 6, 1, 3}, e[5] = {1, 2, 3, 4, 6};
        buildTree(t, s, 5);
        clearTree(t);
        buildTree(t, s, 5);
        bool ok = inorderIs(t, e, 5) && t.count == 5;
        report(20, "власний: повторне створення після очищення", ok, t, b);
    }
    {
        BST t;
        long long b = activeNodes();
        int s[3] = {5, 3, 7};
        buildTree(t, s, 3);
        bool ok = isBST(t.root);
        TreeNode* wrong = createNode(6);
        t.root->left->right = wrong;
        t.count++;
        bool detected = !isBST(t.root) && badValue == 6 && badBound == 5 && badIsUpper;
        cout << "  виявлено: вузол " << badValue << " порушує " << (badIsUpper ? "верхню" : "нижню") << " межу "
             << badBound << " (глибина 3)\n";
        t.root->left->right = nullptr;
        freeNode(wrong);
        t.count--;
        ok = ok && detected && isBST(t.root);
        report(21, "перевірка властивості: помилка на глибині 3", ok, t, b);
    }
    cout << "Успішних сценаріїв: " << passed << " з " << total << "\n\n";
}

void showTree(const BST& t, long long base) {
    char buf[4000];
    int len = 0;
    buf[0] = '\0';
    toPrefix(t.root, buf, len);
    int seq[MAX_NODES], n = 0;
    inorder(t.root, seq, n);
    cout << "  опис: " << buf << "\n  симетричний: ";
    printSeq(seq, n);
    cout << "\n  вузлів " << t.count << ", висота " << height(t.root) << ", корінь "
         << (t.root ? t.root->code : 0) << " | структура " << (structureOk(t, base) ? "правильна" : "ПОМИЛКА") << "\n";
}

void runVariant() {
    cout << "=== Варіант 26 ===\n";
    long long base = activeNodes();
    BST t;
    int keys[7] = {53, 29, 75, 14, 42, 63, 87};
    buildTree(t, keys, 7);
    cout << "Початкове дерево:\n";
    showTree(t, base);

    int c1, c2 = 0, p1[MAX_NODES], p2[MAX_NODES], n1, n2 = 0;
    TreeNode* r1 = searchIterative(t, 14, c1, p1, n1);
    TreeNode* r2 = searchRecursive(t.root, 14, c2, p2, n2);
    cout << "Пошук 14: " << (r1 ? "знайдено" : "не знайдено") << ", шлях ";
    printSeq(p1, n1);
    cout << ", порівнянь " << c1 << " | рекурсивний: шлях ";
    printSeq(p2, n2);
    cout << ", порівнянь " << c2 << " | збіг " << (r1 == r2 && c1 == c2 ? "так" : "НІ") << "\n";
    showTree(t, base);

    cout << "Вставлення 69: " << (insertKey(t, 69) == INS_OK ? "успішно" : "повтор, відмова") << "\n";
    showTree(t, base);

    int result = removeKey(t, 29);
    cout << "Видалення 29: " << deleteText(result) << "\n";
    showTree(t, base);

    cout << "Демонстрація випадків видалення на дереві варіанта:\n";
    result = removeKey(t, 14);
    cout << "Видалення 14: " << deleteText(result) << "\n";
    showTree(t, base);
    result = removeKey(t, 63);
    cout << "Видалення 63: " << deleteText(result) << "\n";
    showTree(t, base);
    result = removeKey(t, 53);
    cout << "Видалення кореня 53: " << deleteText(result) << "\n";
    showTree(t, base);
    result = removeKey(t, 100);
    cout << "Видалення 100: " << deleteText(result) << "\n";
    showTree(t, base);

    clearTree(t);
    cout << "Після очищення: корінь " << (t.root == nullptr ? "порожній" : "НЕ порожній") << ", активних вузлів "
         << activeNodes() - base << "\n\n";
}

void insertBalanced(BST& t, const int sorted[], int lo, int hi) {
    if (lo > hi) return;
    int mid = lo + (hi - lo) / 2;
    insertKey(t, sorted[mid]);
    insertBalanced(t, sorted, lo, mid - 1);
    insertBalanced(t, sorted, mid + 1, hi);
}

unsigned long long rng = 12345;

int nextRandom(int bound) {
    rng = (rng * 1103515245ULL + 12345ULL) % 2147483648ULL;
    return int((rng / 65536) % (unsigned long long)bound);
}

void runExperiment() {
    cout << "=== Експеримент із формою дерева (ключі 1..100) ===\n";
    const int N = 100;
    int sorted[N], mixed[N];
    for (int i = 0; i < N; i++) {
        sorted[i] = i + 1;
        mixed[i] = i + 1;
    }
    for (int i = N - 1; i > 0; i--) {
        int j = nextRandom(i + 1);
        int tmp = mixed[i];
        mixed[i] = mixed[j];
        mixed[j] = tmp;
    }
    const char* names[3] = {"майже збалансована", "вироджена (зростання)", "змішана"};
    for (int shape = 0; shape < 3; shape++) {
        long long base = activeNodes();
        BST t;
        if (shape == 0) insertBalanced(t, sorted, 0, N - 1);
        else if (shape == 1) buildTree(t, sorted, N);
        else buildTree(t, mixed, N);
        int c, path[MAX_NODES], pl;
        long long totalPresent = 0;
        int worst = 0;
        for (int k = 1; k <= N; k++) {
            searchIterative(t, k, c, path, pl);
            totalPresent += c;
            if (c > worst) worst = c;
        }
        int cPresent, cAbsent, vPresent, vAbsent;
        searchIterative(t, 100, cPresent, path, vPresent);
        searchIterative(t, 101, cAbsent, path, vAbsent);
        cout << names[shape] << ": висота " << height(t.root) << ", пошук 100: " << cPresent << " пор. (" << vPresent
             << " вузлів), пошук 101: " << cAbsent << " пор. (" << vAbsent << " вузлів), середнє для 1..100: " << double(totalPresent) / N << ", максимум " << worst
             << " | структура " << (structureOk(t, base) ? "правильна" : "ПОМИЛКА") << "\n";
        clearTree(t);
    }
    cout << "\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    runTests();
    runVariant();
    runExperiment();
    cout << "Створено вузлів: " << createdNodes << ", звільнено: " << freedNodes << ", активних: " << activeNodes()
         << "\n";
    return 0;
}
