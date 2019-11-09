#include <cstdio>
#include <cstdlib>
#include <clocale>
#include <wchar.h>
#include <cstring>

const int ANS_LEN = 2, STR_LEN = 100;
const wchar_t *ANS_LET = L"ДдНн";

typedef char *Elem_t;

typedef struct Node {
    Node *parent = nullptr;
    Elem_t data = 0;
    Node *left = nullptr;
    Node *right = nullptr;
} Node;

Node *NodeInit (Node *parent, Elem_t data);
void NodeDestructTree (Node *node);
Node *TreeLoad (FILE *readfile, Node *parent);
Node *TreeTour (Node *node);
void TreeSave (Node *node, FILE *writefile);

int main () {
    setlocale(LC_ALL, "ru_RU.utf8");

    FILE *readfile = fopen ("../input.txt", "rb");
    Node *root = TreeLoad (readfile, nullptr);
    fclose (readfile);
    /*
    Node *root = NodeInit (nullptr, "Сосед пидор?");
    root->left = NodeInit (root, "Сосед гандон?");
    root->left->left = NodeInit (root->left, "Руслан");
    root->left->right = NodeInit (root->left, "Тёлка?");
    root->left->right->right = NodeInit (root->left->right, "Зайка");
    root->left->right->left = NodeInit (root->left->right, "Галим");
    root->right = NodeInit (root, "Игнат");
    */
    TreeTour (root);

    FILE *writefile = fopen ("../output.txt", "w");
    TreeSave (root, writefile);
    fclose (writefile);

    NodeDestructTree (root);
}

Node *NodeInit (Node *parent, Elem_t data) {
    Node *node = (Node *) calloc (1, sizeof (Node));
    node->parent = parent;
    node->data = (char *) calloc (strlen (data), sizeof (char));
    strcpy (node->data, data);
    return node;
}

void NodeDestructTree (Node *node) {
    if (node->left) NodeDestructTree (node->left);
    if (node->right) NodeDestructTree (node->right);
    if (node->parent && node->parent->left == node)
        node->parent->left = nullptr;
    if (node->parent && node->parent->right == node)
        node->parent->right = nullptr;
    free (node);
}

Node *TreeTour (Node *node) {
    if (!node->left || !node->right)
        printf ("%s", node->data);
    else {
        printf("%s [Д/н]\n", node->data);
        wchar_t ans[ANS_LEN];
        wscanf(L"%ls", ans);
        if (ans[0] == ANS_LET[0] || ans[0] == ANS_LET[1]) //если ответ "Да"
            TreeTour(node->right);
        else if (ans[0] == ANS_LET[2] || ans[0] == ANS_LET[3]) //если ответ "Нет"
            TreeTour(node->left);
    }
}

Node *TreeLoad (FILE *readfile, Node *parent) {
    char str[STR_LEN];
    Node *node = nullptr;
    if (fscanf (readfile, "{ \"%[^\"]\" ", str) > 0) {
        node = NodeInit (parent, str);
        //printf ("%s\n", str);
        node->left = TreeLoad (readfile, node);
        fscanf (readfile, "} ");
        if (fscanf (readfile, "{ \"%[^\"]\" ", str) > 0) {
            //printf ("%s\n", str);
            node->parent->right = NodeInit(node->parent, str);
            node->parent->right->left = TreeLoad (readfile, node->parent);
            fscanf (readfile, "} ");
        }
    }
    return node;
}

void TreeSave (Node *node, FILE *writefile) {
    fprintf (writefile, "{ \"%s\" ", node->data);
    if (node->left)
        TreeSave (node->left, writefile);
    if (node->right)
        TreeSave (node->right, writefile);
    fprintf (writefile, "} ");
}