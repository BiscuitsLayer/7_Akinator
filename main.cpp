#include <cstdlib>
#include <clocale>
#include <cwchar>
#include <cstring>
#include <stdio_ext.h>
#include <cwctype>
#include "third-party-libraries/ListLib.h"

#define PRINT(STR) { printf (STR"\n"); system ("echo "#STR" | festival --tts --language russian"); }
#define SAY(USR, STR) { char temp_say[STR_LEN]; \
    sprintf (temp_say, "%s%s%s%s", "echo ", USR, STR, " | festival --tts --language russian"); \
    system (temp_say); }

const int ANS_LEN = 2, STR_LEN = 1000, BUF_EXTRA_SIZE = 1;
const wchar_t *ANS_LET = L"ДдНн";
const wchar_t *NEGATION = L"Нне ";
const char *TREE = "../tree.txt";
const char *LIST = "../list.txt";

typedef char *Elem_t;

typedef struct Node {
    Node *parent = nullptr;
    Elem_t data = "";
    size_t level = 0;
    Node *right = nullptr;
    Node *left = nullptr;
} Node;

wchar_t* gets_wc (unsigned bufSize);
Node *NodeInit (Node *parent, Elem_t data);
void NodeDestructTree (Node *node);
bool NodeAdd (Node *node_old, Node *node_new, Node *question, List_t *list);
int NodeNegationSearch (Node *node);
void NodeReverse (Node *node, size_t idx);
void FillList (List_t *list, Node *node);
void GiveDefinition (char *data, List_t *list, Node *node);
void GiveDifference (char *data1, char *data2, List_t *list, Node *node);
Node *TreeSearch (char *data, Node *node);
Node *TreeLoad (FILE *readtree, Node *parent);
Node *TreeTour (Node *node, List_t *list);
void TreeSave (Node *node, FILE *writetree);

int main () {
    setlocale(LC_ALL, "ru_RU.utf8");

    FILE *readtree = fopen (TREE, "rb");
    Node *root = TreeLoad (readtree, nullptr);
    fclose (readtree);

    FILE *writelist = fopen (LIST, "w");
    List_t list = ListInit (LIST_SIZE);
    FillList (&list, root);
    ListPrint (&list, writelist);
    fclose (writelist);

    GiveDefinition ("Тройка", &list, root);
    GiveDifference("КСП", "Шестёрка", &list, root);
    TreeTour (root, &list);

    FILE *writetree = fopen (TREE, "w");
    TreeSave (root, writetree);
    fclose (writetree);

    NodeDestructTree (root);
    ListDestruct (&list);
}

Node *NodeInit (Node *parent, Elem_t data) {
    Node *node = (Node *) calloc (1, sizeof (Node));
    if (parent) {
        node->parent = parent;
        node->level = node->parent->level + 1;
    }
    node->data = (char *) calloc (strlen (data) + BUF_EXTRA_SIZE, sizeof (char));
    strcpy (node->data, data);
    return node;
}

void NodeDestructTree (Node *node) {
    if (node->right) NodeDestructTree (node->right);
    if (node->left) NodeDestructTree (node->left);
    if (node->parent && node->parent->right == node)
        node->parent->right = nullptr;
    else if (node->parent && node->parent->left == node)
        node->parent->left = nullptr;
    free (node->data);
    free (node);
}

Node *TreeTour (Node *node, List_t *list) {
    wchar_t ans[ANS_LEN];
    if (!node->right || !node->left) {
        printf ("Это %s? [Д/н]\n", node->data);
        SAY("Это ", node->data)
        wscanf (L"%ls", ans);
        if (ans[0] == ANS_LET[0] || ans[0] == ANS_LET[1]) //если ответ "Да"
            PRINT("Ну вот, я снова угадал. Я знаю столовки физтеха лучше тебя!")

        else if (ans[0] == ANS_LET[2] || ans[0] == ANS_LET[3]) { //если ответ "Нет"
            wchar_t *data_new = nullptr;
            char data_new_char[STR_LEN];

            PRINT("Хм, я таких не знаю. Напиши, какую столовку ты загадывал.")
            data_new = gets_wc(256);
            sprintf (data_new_char, "%S", data_new);
            Node *node_new = NodeInit (nullptr, data_new_char);

            printf ("И чем же она отличается от %s?\n", node->data);
            SAY ("И чем же она отличается от ", node->data)
            data_new = gets_wc(256);
            sprintf (data_new_char, "%S", data_new);
            data_new_char[strlen(data_new_char) + 1] = '\0';
            data_new_char[strlen(data_new_char)] = '?';
            Node *question = NodeInit (nullptr, data_new_char);

            if (NodeAdd (node, node_new, question, list)) {
                PRINT("Новая столовка успешно добавлена.")
                int idx = 0;
                while (idx >= 0) {
                    idx = NodeNegationSearch(question);
                    if (idx >= 0)
                        NodeReverse(question, idx);
                }
            }
            else
                PRINT("Такая столовка уже есть в акинаторе.")
            free (data_new);
        }
    }
    else {
        printf("%s [Д/н]\n", node->data);
        SAY("", node->data)
        wscanf(L"%ls", ans);
        if (ans[0] == ANS_LET[0] || ans[0] == ANS_LET[1]) //если ответ "Да"
            TreeTour(node->left, list);
        else if (ans[0] == ANS_LET[2] || ans[0] == ANS_LET[3]) //если ответ "Нет"
            TreeTour(node->right, list);
    }
    return nullptr;
}

bool NodeAdd (Node *node_old, Node *node_new, Node *question, List_t *list) {
    if (ListValSearch (list, node_new->data)) {
        return false;
    }
    question->level = node_old->level;
    node_new->level = node_old->level = question->level + 1;
    question->right = node_old;
    question->left = node_new;
    if (node_old->parent->right == node_old)
        node_old->parent->right = question;
    else if (node_old->parent->left == node_old)
        node_old->parent->left = question;
    node_old->parent = question;
    node_new->parent = question;
    return true;
}

int NodeNegationSearch (Node *node) {
    wchar_t temp[STR_LEN];
    swprintf (temp, strlen (node->data), L"%s", node->data);
    size_t len = wcslen (temp);

    for (size_t i = 0; i < len; ++i) {
        if ( (temp[i] == NEGATION[0] || temp[i] == NEGATION[1] ) && temp[i + 1] == NEGATION[2] && temp[i + 2] == NEGATION[3])
            return i;
    }
    return -1;
}

void NodeReverse (Node *node, size_t idx) {
    wchar_t temp[STR_LEN];
    swprintf (temp, strlen (node->data), L"%s", node->data);
    size_t len = wcslen (temp);
    for (size_t i = idx; i < len - 3; ++i) {
        temp[i] = temp[i + 3];
    }
    temp[len - 3] = '\0';
    temp[0] = towupper (temp[0]);
    sprintf (node->data, "%S", temp);

    Node *ptr = node->right;
    node->right = node->left;
    node->left = ptr;
}

void FillList (List_t *list, Node *node) {
    if (node->right)
        FillList (list, node->right);
    if (node->left)
        FillList (list, node->left);
    if (!node->right || !node->left)
        ListPushBack (list, node->data);
}

void GiveDefinition (char *data, List_t *list, Node *node) {
    if (!ListValSearch (list, data))
        PRINT("Такой столовки нет в акинаторе.")
    else {
        Node *temp = TreeSearch (data, node);
        printf ("%s это столовка, которая ", temp->data);
        SAY("", temp->data)
        SAY("это столовка, которая","")
        while (temp->parent) {
            if (temp->parent->right == temp) {
                printf("не ");
                SAY ("не", "")
            }
            wchar_t temp_str[STR_LEN];
            swprintf (temp_str, strlen (temp->parent->data), L"%s", temp->parent->data);
            temp_str[wcslen (temp_str) - 1] = L' ';
            temp_str[wcslen (temp_str) - 1] = '\0';
            temp_str[0] = towlower (temp_str[0]);
            printf ("%S; ", temp_str);
            SAY ("", temp->parent->data)
            temp = temp->parent;
        }
        printf ("\n");
    }
}

void GiveDifference (char *data1, char *data2, List_t *list, Node *node) {
    if (!ListValSearch(list, data1)) PRINT("Первой столовки нет в акинаторе.")
    else if (!ListValSearch(list, data1)) PRINT("Второй столовки нет в акинаторе.")
    else {
        Node *temp1 = TreeSearch(data1, node);
        Node *temp2 = TreeSearch(data2, node);
        if (temp1 == temp2) {
            PRINT ("Столовка введена повторно.")
            return;
        }
        Node *temp3 = temp1;
        Node *temp4 = temp2;
        while (temp3->level > temp2->level)
            temp3 = temp3->parent;
        while (temp4->level > temp1->level)
            temp4 = temp4->parent;
        while (temp3 != temp4) {
            temp3 = temp3->parent;
            temp4 = temp4->parent;
        }
        if (temp3->parent) {
            printf ("Обе столовки похожи тем, что они ");
            SAY ("Обе столовки похожи тем, что они", "")
        } else {
            printf ("Столовки ни в чем не похожи, ");
            SAY ("Столовки ни в чем не похожи", "")
        }
        while (temp3->parent) {
            if (temp3->parent->right == temp3) {
                printf("не ");
                SAY ("не", "")
            }
            wchar_t temp_str[STR_LEN];
            swprintf(temp_str, strlen(temp3->parent->data), L"%s", temp3->parent->data);
            temp_str[wcslen(temp_str) - 1] = L' ';
            temp_str[wcslen(temp_str) - 1] = '\0';
            temp_str[0] = towlower(temp_str[0]);
            printf("%S; ", temp_str);
            SAY ("", temp3->parent->data)
            temp3 = temp3->parent;
        }
        printf ("а различны тем, что %s ", data1);
        SAY ("а различны тем, что", data1)
        while (temp1->parent != temp4->parent) {
            if (temp1->parent->right == temp1) {
                printf("не ");
                SAY ("не", "")
            }
            wchar_t temp_str[STR_LEN];
            swprintf(temp_str, strlen(temp1->parent->data), L"%s", temp1->parent->data);
            temp_str[wcslen(temp_str) - 1] = L' ';
            temp_str[wcslen(temp_str) - 1] = '\0';
            temp_str[0] = towlower(temp_str[0]);
            printf("%S; ", temp_str);
            SAY ("", temp1->parent->data)
            temp1 = temp1->parent;
        }
        printf ("а %s ", data2);
        SAY ("а", data2)
        while (temp2->parent != temp4->parent) {
            if (temp2->parent->right == temp2) {
                printf("не ");
                SAY ("не", "")
            }
            wchar_t temp_str[STR_LEN];
            swprintf(temp_str, strlen(temp2->parent->data), L"%s", temp2->parent->data);
            temp_str[wcslen(temp_str) - 1] = L' ';
            temp_str[wcslen(temp_str) - 1] = '\0';
            temp_str[0] = towlower(temp_str[0]);
            printf("%S; ", temp_str);
            SAY ("", temp2->parent->data)
            temp2 = temp2->parent;
        }
        printf ("\n");
    }
}

Node *TreeSearch (char *data, Node *node) {
    Node *ans = nullptr;
    if (node->right && !ans)
        ans = TreeSearch (data, node->right);
    if (node->left && !ans)
        ans = TreeSearch (data, node->left);
    if (!node->right || !node->left)
        ans = ( (strcmp (data, node->data) == 0) ? node : nullptr );
    return ans;
}

Node *TreeLoad (FILE *readtree, Node *parent) {
    char str[STR_LEN];
    Node *node = nullptr;
    if (fscanf (readtree, "{ \"%[^\"]\" ", str) > 0) {
        node = NodeInit (parent, str);
        node->left = TreeLoad (readtree, node);
        fscanf (readtree, "} ");
        if (fscanf (readtree, "{ \"%[^\"]\" ", str) > 0) {
            parent->right = NodeInit(node->parent, str);
            parent->right->left = TreeLoad (readtree, parent->right);
            fscanf (readtree, "} ");
        }
    }
    return node;
}

void TreeSave (Node *node, FILE *writetree) {
    fprintf (writetree, "{ \"%s\" ", node->data);
    if (node->left)
        TreeSave (node->left, writetree);
    if (node->right)
        TreeSave (node->right, writetree);
    fprintf (writetree, "} ");
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