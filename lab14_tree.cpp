#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const int NAME_LEN = 60;
const int MAX_NODES = 1000;
const int MAX_TOKENS = 2000;
const int TOKEN_LEN = 16;

struct TreeNode {
    int code = 0;
    char name[NAME_LEN] = "";
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
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
    node->left = nullptr;
    node->right = nullptr;
    return node;
}

bool isEmpty(const TreeNode* root) {
    return root == nullptr;
}

void clearTree(TreeNode*& root) {
    if (root == nullptr) return;
    clearTree(root->left);
    clearTree(root->right);
    delete root;
    freedNodes++;
    root = nullptr;
}

int splitTokens(const char text[], char tokens[][TOKEN_LEN]) {
    int count = 0, i = 0;
    while (text[i] != '\0') {
        while (text[i] == ' ') i++;
        if (text[i] == '\0') break;
        int len = 0;
        while (text[i] != ' ' && text[i] != '\0') {
            if (len < TOKEN_LEN - 1) tokens[count][len++] = text[i];
            i++;
        }
        tokens[count][len] = '\0';
        if (count < MAX_TOKENS - 1) count++;
    }
    return count;
}

bool parseNumber(const char t[], int& value) {
    int i = 0, sign = 1;
    if (t[0] == '-') {
        sign = -1;
        i = 1;
    }
    if (t[i] == '\0') return false;
    value = 0;
    while (t[i] != '\0') {
        if (t[i] < '0' || t[i] > '9') return false;
        value = value * 10 + (t[i] - '0');
        i++;
    }
    value *= sign;
    return true;
}

bool buildFromPrefix(char tokens[][TOKEN_LEN], int count, int& pos, TreeNode*& node) {
    node = nullptr;
    if (pos >= count) return false;
    if (tokens[pos][0] == '#' && tokens[pos][1] == '\0') {
        pos++;
        return true;
    }
    int value;
    if (!parseNumber(tokens[pos], value)) return false;
    pos++;
    node = createNode(value);
    if (!buildFromPrefix(tokens, count, pos, node->left)) return false;
    if (!buildFromPrefix(tokens, count, pos, node->right)) return false;
    return true;
}

bool createTree(const char description[], TreeNode*& root) {
    static char tokens[MAX_TOKENS][TOKEN_LEN];
    int count = splitTokens(description, tokens);
    int pos = 0;
    root = nullptr;
    bool ok = buildFromPrefix(tokens, count, pos, root) && pos == count;
    if (!ok) clearTree(root);
    return ok;
}

void preorder(const TreeNode* node, int out[], int& n) {
    if (node == nullptr) return;
    out[n++] = node->code;
    preorder(node->left, out, n);
    preorder(node->right, out, n);
}

void inorder(const TreeNode* node, int out[], int& n) {
    if (node == nullptr) return;
    inorder(node->left, out, n);
    out[n++] = node->code;
    inorder(node->right, out, n);
}

void postorder(const TreeNode* node, int out[], int& n) {
    if (node == nullptr) return;
    postorder(node->left, out, n);
    postorder(node->right, out, n);
    out[n++] = node->code;
}

int inorderIterative(const TreeNode* root, int out[], int& maxStack) {
    const TreeNode* stack[MAX_NODES];
    int top = 0, n = 0;
    maxStack = 0;
    const TreeNode* current = root;
    while (current != nullptr || top > 0) {
        while (current != nullptr) {
            stack[top++] = current;
            if (top > maxStack) maxStack = top;
            current = current->left;
        }
        current = stack[--top];
        out[n++] = current->code;
        current = current->right;
    }
    return n;
}

int countNodes(const TreeNode* node) {
    if (node == nullptr) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

int countLeaves(const TreeNode* node) {
    if (node == nullptr) return 0;
    if (node->left == nullptr && node->right == nullptr) return 1;
    return countLeaves(node->left) + countLeaves(node->right);
}

int countInternal(const TreeNode* node) {
    if (node == nullptr) return 0;
    if (node->left == nullptr && node->right == nullptr) return 0;
    return 1 + countInternal(node->left) + countInternal(node->right);
}

int height(const TreeNode* node) {
    if (node == nullptr) return 0;
    int hl = height(node->left);
    int hr = height(node->right);
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

bool checkStructure(const TreeNode* root, int& visited) {
    const TreeNode* seen[MAX_NODES];
    visited = 0;
    return collectAddresses(root, seen, visited) && visited == countNodes(root);
}

void toPrefix(const TreeNode* node, char buf[], int& len) {
    char tmp[16];
    if (node == nullptr) {
        buf[len++] = '#';
        buf[len++] = ' ';
        buf[len] = '\0';
        return;
    }
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

void insertModel(TreeNode*& node, int value) {
    if (node == nullptr) {
        node = createNode(value);
        return;
    }
    if (value < node->code) insertModel(node->left, value);
    else insertModel(node->right, value);
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

void sortCopy(const int a[], int n, int out[]) {
    for (int i = 0; i < n; i++) out[i] = a[i];
    for (int i = 1; i < n; i++) {
        int key = out[i], j = i - 1;
        while (j >= 0 && out[j] > key) {
            out[j + 1] = out[j];
            j--;
        }
        out[j + 1] = key;
    }
}

struct TreeTest {
    const char* name;
    const char* description;
    int n;
    int pre[10];
    int in[10];
    int post[10];
    int leaves;
    int internal;
    int h;
};

int traversalsOk = 0, characteristicsOk = 0, iterativeOk = 0, memoryOk = 0;

void runTest(int num, const TreeTest& t) {
    long long base = activeNodes();
    TreeNode* root = nullptr;
    bool created = createTree(t.description, root);
    int pre[MAX_NODES], in[MAX_NODES], post[MAX_NODES], it[MAX_NODES], s1[MAX_NODES], s2[MAX_NODES];
    int np = 0, ni = 0, nq = 0, maxStack;
    preorder(root, pre, np);
    inorder(root, in, ni);
    postorder(root, post, nq);
    int nit = inorderIterative(root, it, maxStack);
    sortCopy(pre, np, s1);
    sortCopy(in, ni, s2);
    bool trav = created && sameSeq(pre, np, t.pre, t.n) && sameSeq(in, ni, t.in, t.n) && sameSeq(post, nq, t.post, t.n)
                && sameSeq(s1, np, s2, ni);
    if (t.n > 0) trav = trav && pre[0] == root->code && post[nq - 1] == root->code;
    int nodes = countNodes(root), leaves = countLeaves(root), internal = countInternal(root), h = height(root);
    bool chars = nodes == t.n && leaves == t.leaves && internal == t.internal && h == t.h
                 && (t.n == 0 || leaves + internal == nodes);
    int visited;
    bool structure = checkStructure(root, visited) && visited == t.n;
    bool iter = sameSeq(it, nit, in, ni);
    clearTree(root);
    bool mem = root == nullptr && activeNodes() == base;
    if (trav) traversalsOk++;
    if (chars && structure) characteristicsOk++;
    if (iter) iterativeOk++;
    if (mem) memoryOk++;
    cout << num << ". " << t.name << " [" << t.description << "]\n";
    cout << "   прямий: ";
    printSeq(pre, np);
    cout << " | симетричний: ";
    printSeq(in, ni);
    cout << " | зворотний: ";
    printSeq(post, nq);
    cout << "\n   вузлів " << nodes << ", листків " << leaves << ", внутрішніх " << internal << ", висота " << h
         << " | обходи " << (trav ? "так" : "НІ") << ", характеристики " << (chars && structure ? "так" : "НІ")
         << ", ітеративний " << (iter ? "так" : "НІ") << ", пам'ять " << (mem ? "так" : "НІ") << "\n";
}

void runTests() {
    cout << "=== Автоматизовані тести ===\n";
    TreeTest tests[] = {
        {"порожнє дерево", "#", 0, {}, {}, {}, 0, 0, 0},
        {"один вузол", "5 # #", 1, {5}, {5}, {5}, 1, 0, 1},
        {"корінь з двома нащадками", "2 1 # # 3 # #", 3, {2, 1, 3}, {1, 2, 3}, {1, 3, 2}, 2, 1, 2},
        {"ланцюжок ліворуч", "3 2 1 # # # #", 3, {3, 2, 1}, {1, 2, 3}, {1, 2, 3}, 1, 2, 3},
        {"ланцюжок праворуч", "1 # 2 # 3 # #", 3, {1, 2, 3}, {1, 2, 3}, {3, 2, 1}, 1, 2, 3},
        {"три вузли", "4 2 # # 6 # #", 3, {4, 2, 6}, {2, 4, 6}, {2, 6, 4}, 2, 1, 2},
        {"сім вузлів", "5 3 2 # # 4 # # 7 6 # # 8 # #", 7, {5, 3, 2, 4, 7, 6, 8}, {2, 3, 4, 5, 6, 7, 8},
         {2, 4, 3, 6, 8, 7, 5}, 4, 3, 3},
        {"неповне", "1 2 # # 3 4 # # #", 4, {1, 2, 3, 4}, {2, 1, 4, 3}, {2, 4, 3, 1}, 2, 2, 3},
        {"від'ємні числа", "0 -2 # # 5 # #", 3, {0, -2, 5}, {-2, 0, 5}, {-2, 5, 0}, 2, 1, 2},
        {"однакові значення", "4 4 # # 4 # #", 3, {4, 4, 4}, {4, 4, 4}, {4, 4, 4}, 2, 1, 2},
        {"власне: повне", "4 2 1 # # 3 # # 6 5 # # 7 # #", 7, {4, 2, 1, 3, 6, 5, 7}, {1, 2, 3, 4, 5, 6, 7},
         {1, 3, 2, 5, 7, 6, 4}, 4, 3, 3},
        {"власне: вироджене ліворуч", "4 3 2 1 # # # # #", 4, {4, 3, 2, 1}, {1, 2, 3, 4}, {1, 2, 3, 4}, 1, 3, 4},
        {"власне: вироджене праворуч", "1 # 2 # 3 # 4 # #", 4, {1, 2, 3, 4}, {1, 2, 3, 4}, {4, 3, 2, 1}, 1, 3, 4},
        {"власне: неповне", "8 4 # 6 # # 12 10 # # #", 5, {8, 4, 6, 12, 10}, {4, 6, 8, 10, 12}, {6, 4, 10, 12, 8},
         2, 3, 3},
        {"власне: з повтореннями", "7 7 7 # # # 7 # #", 4, {7, 7, 7, 7}, {7, 7, 7, 7}, {7, 7, 7, 7}, 2, 2, 3},
    };
    int total = sizeof(tests) / sizeof(tests[0]);
    for (int t = 0; t < total; t++) runTest(t + 1, tests[t]);
    long long base = activeNodes();
    TreeNode* bad = nullptr;
    bool rejected = !createTree("1 2 #", bad) && !createTree("1 # # 5", bad) && !createTree("1 x # #", bad)
                    && bad == nullptr && activeNodes() == base;
    cout << "16. неправильні описи [1 2 #], [1 # # 5], [1 x # #]: " << (rejected ? "відхилено, витоків немає" : "ПОМИЛКА")
         << "\n";
    cout << "Дерев з правильними трьома обходами: " << traversalsOk << " з " << total
         << " (" << traversalsOk * 3 << " обходів)\n";
    cout << "Перевірок характеристик і структури: " << characteristicsOk << " з " << total << "\n";
    cout << "Ітеративний симетричний збігся з рекурсивним: " << iterativeOk << " з " << total << "\n";
    cout << "Очищень без активних вузлів: " << memoryOk << " з " << total << "\n\n";
}

void runVariant() {
    cout << "=== Варіант 26 ===\n";
    long long base = activeNodes();
    int values[7] = {53, 29, 75, 14, 42, 63, 87};
    TreeNode* model = nullptr;
    for (int i = 0; i < 7; i++) insertModel(model, values[i]);
    char description[1000];
    int len = 0;
    description[0] = '\0';
    toPrefix(model, description, len);
    if (len > 0) description[len - 1] = '\0';
    clearTree(model);
    cout << "Контрольна модель (менше ліворуч, більше праворуч) для 53, 29, 75, 14, 42, 63, 87\n";
    cout << "Префіксний опис: " << description << "\n";

    TreeNode* root = nullptr;
    bool ok = createTree(description, root);
    cout << "Створення за описом: " << (ok ? "успішно" : "ПОМИЛКА") << ", корінь " << (root ? root->code : 0) << "\n";
    int pre[MAX_NODES], in[MAX_NODES], post[MAX_NODES], it[MAX_NODES];
    int np = 0, ni = 0, nq = 0, maxStack;
    preorder(root, pre, np);
    inorder(root, in, ni);
    postorder(root, post, nq);
    cout << "Прямий: ";
    printSeq(pre, np);
    cout << "\nСиметричний: ";
    printSeq(in, ni);
    cout << "\nЗворотний: ";
    printSeq(post, nq);
    cout << "\n";
    int nit = inorderIterative(root, it, maxStack);
    cout << "Ітеративний симетричний: ";
    printSeq(it, nit);
    cout << " | збігається з рекурсивним: " << (sameSeq(it, nit, in, ni) ? "так" : "НІ")
         << ", максимальна глибина стеку " << maxStack << "\n";
    int visited;
    cout << "Вузлів " << countNodes(root) << ", листків " << countLeaves(root) << ", внутрішніх "
         << countInternal(root) << ", висота " << height(root) << "\n";
    cout << "Перевірка структури: " << (checkStructure(root, visited) ? "кожен вузол відвідано один раз" : "ПОМИЛКА")
         << " (" << visited << " вузлів)\n";
    clearTree(root);
    cout << "Після очищення: корінь " << (isEmpty(root) ? "порожній" : "НЕ порожній") << ", активних вузлів "
         << activeNodes() - base << "\n";
    ok = createTree(description, root);
    int again[MAX_NODES], na = 0;
    inorder(root, again, na);
    cout << "Повторне створення: " << (ok && sameSeq(again, na, in, ni) ? "коректне" : "ПОМИЛКА");
    clearTree(root);
    cout << ", після очищення активних вузлів " << activeNodes() - base << "\n\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    runTests();
    runVariant();
    cout << "Створено вузлів: " << createdNodes << ", звільнено: " << freedNodes << ", активних: " << activeNodes()
         << "\n";
    return 0;
}
