#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

const int LIST_SIZE = 100; //Размер списка
const int NUM_SIZE = 3;   //Макс количество разрядов в числе операций со списком
const int SERVICE = 118;  //Служебная константа (для построения графов)

//Тип содержимого списка
typedef char * Elem_t;

//Структура элемента списка
typedef struct {
    Elem_t data = 0;
    size_t prev = 0, next = 0;
} List_it;

//Структура cамого списка
struct List_t {
    List_it *items = nullptr;
    size_t size = 0;
    size_t head = 0, tail = 0;
    size_t free = 1;
    size_t dump_count = 1;
};

//Операции со списком
List_t ListInit (size_t size);                                      //Конструктор
void ListDestruct (List_t *list);                                   //Деструктор
void ListOK (List_t *list);                                         //Верификатор
size_t ListValSearch (List_t *list, Elem_t val);                    //Поиск элемента по значению
size_t ListIdxSearch (List_t *list, size_t idx);                    //Поиск элемента по логическому номеру
void ListPushFront (List_t *list, Elem_t val);                      //Вставить элемент в начало списка
void ListPushBack (List_t *list, Elem_t val);                       //Вставить элемент в конец списка
void ListPushBefore (List_t *list, Elem_t val_s, Elem_t val_i);     //Вставить элемент перед заданным
void ListPushAfter (List_t *list, Elem_t val_s, Elem_t val_i);      //Вставить элемен  после заданного
void ListDelete (List_t *list, Elem_t val);                         //Удалить элемент
void ListClear (List_t *list);                                      //Очистить список
void ListPrint (List_t *list, FILE *writelist);                     //Вывод содержимого списка подряд
void ListDump (List_t *list);                                       //Дамп списка
void ListSort (List_t *list);                                       //Пересчет индексов элементов