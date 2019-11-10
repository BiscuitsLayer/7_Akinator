#include <cstdio>
#include <cstdlib>
#include <clocale>
#include <wchar.h>
#include <cstring>
#include <stdio_ext.h>

const int ANS_LEN = 2, STR_LEN = 100, BUF_EXTRA_SIZE = 1;
const wchar_t *ANS_LET = L"ДдНн";

typedef char *Elem_t;

typedef struct Node {
    Node *parent = nullptr;
    Elem_t data = "";
    Node *left = nullptr;
    Node *right = nullptr;
} Node;

wchar_t* gets_wc (unsigned bufSize);
Node *NodeInit (Node *parent, Elem_t data);
void NodeDestructTree (Node *node);
void NodeAdd (Node *node_old, Node *node_new, Node *question);
Node *TreeLoad (FILE *readfile, Node *parent);
Node *TreeTour (Node *node);
void TreeSave (Node *node, FILE *writefile);

int main () {
    setlocale(LC_ALL, "ru_RU.utf8");

    FILE *readfile = fopen ("../input.txt", "rb");
    Node *root = TreeLoad (readfile, nullptr);
    fclose (readfile);
    TreeTour (root);

    FILE *writefile = fopen ("../output.txt", "w");
    TreeSave (root, writefile);
    fclose (writefile);

    NodeDestructTree (root);
}

Node *NodeInit (Node *parent, Elem_t data) {
    Node *node = (Node *) calloc (1, sizeof (Node));
    node->parent = parent;
    node->data = (char *) calloc (strlen (data) + BUF_EXTRA_SIZE, sizeof (char));
    strcpy (node->data, data);
    return node;
}

void NodeDestructTree (Node *node) {
    if (node->left) NodeDestructTree (node->left);
    if (node->right) NodeDestructTree (node->right);
    if (node->parent && node->parent->left == node)
        node->parent->left = nullptr;
    else if (node->parent && node->parent->right == node)
        node->parent->right = nullptr;
    free (node->data);
    free (node);
}

Node *TreeTour (Node *node) {
    wchar_t ans[ANS_LEN];
    if (!node->left || !node->right) {
        printf ("Это %s? [Д/н]\n", node->data);
        wscanf (L"%ls", ans);

        if (ans[0] == ANS_LET[0] || ans[0] == ANS_LET[1]) //если ответ "Да"
            printf ("Ну вот, я снова угадал. Я знаю твоих соседей лучше тебя!");
        else if (ans[0] == ANS_LET[2] || ans[0] == ANS_LET[3]) { //если ответ "Нет"
            wchar_t *data_new = nullptr;
            char data_new_char[STR_LEN];

            printf ("Хм, я таких не знаю. Напиши, кого ты загадывал.\n");
            data_new = gets_wc(256);
            sprintf (data_new_char, "%S", data_new);
            Node *node_new = NodeInit (nullptr, data_new_char);

            printf ("И чем же он/она отличается от %s?\n", node->data);
            data_new = gets_wc(256);
            sprintf (data_new_char, "%S", data_new);
            data_new_char[strlen(data_new_char) + 1] = '\0';
            data_new_char[strlen(data_new_char)] = '?';
            Node *question = NodeInit (nullptr, data_new_char);

            NodeAdd (node, node_new, question);
            printf ("Новый персонаж успешно добавлен.\n");
        }
    }
    else {
        printf("%s [Д/н]\n", node->data);
        wscanf(L"%ls", ans);
        if (ans[0] == ANS_LET[0] || ans[0] == ANS_LET[1]) //если ответ "Да"
            TreeTour(node->right);
        else if (ans[0] == ANS_LET[2] || ans[0] == ANS_LET[3]) //если ответ "Нет"
            TreeTour(node->left);
    }
}

void NodeAdd (Node *node_old, Node *node_new, Node *question) {
    question->left = node_old;
    question->right = node_new;
    if (node_old->parent->left == node_old)
        node_old->parent->left = question;
    else if (node_old->parent->right == node_old)
        node_old->parent->right = question;
    node_old->parent = question;
    node_new->parent = question;
}

Node *TreeLoad (FILE *readfile, Node *parent) {
    char str[STR_LEN];
    Node *node = nullptr;
    if (fscanf (readfile, "{ \"%[^\"]\" ", str) > 0) {
        node = NodeInit (parent, str);
        node->left = TreeLoad (readfile, node);
        fscanf (readfile, "} ");
        if (fscanf (readfile, "{ \"%[^\"]\" ", str) > 0) {
            node->parent->right = NodeInit(node->parent, str);
            node->parent->right->left = TreeLoad (readfile, node->parent->right);
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

wchar_t* gets_wc (unsigned bufSize) {
    wchar_t* buf = (wchar_t*) calloc (bufSize, sizeof (wchar_t));
    unsigned idx = 0;
    wchar_t wc = L'm';

    while (true) {
        wc = getwchar();
        if (wc != L'\n') {
            buf[idx] = wc;
            ++idx;
        } else if (idx)
            break;
    }

    return buf;
}