#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cctype>
/* GREEN */
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace std;

// Отключаем конфликт макросов Windows
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

/* GREEN */
// Заглушки для Windows-специфичных функций (для кроссплатформенности)
// Всегда определяем как заглушки, если они еще не определены
#ifndef SetConsoleOutputCP
#define SetConsoleOutputCP(x)
#endif
#ifndef SetConsoleCP
#define SetConsoleCP(x)
#endif


// 1. Структурный тип данных (Запись)

struct Book {
    unsigned int invNumber;       // Инвентарный номер (Уникальный ключ 1)
    string title;                 // Название (Ключ 2)
    string author;                // Автор
    unsigned int year;            // Год издания
    string publisher;             // Издательство
    string genre;                 // Жанр
    unsigned int pages;           // Количество страниц
    double cost;                  // Стоимость
    unsigned int ageLimit;        // Возрастное ограничение
    string status;                // Статус (Доступна/Выдана)

    // Конструктор по умолчанию
    Book() : invNumber(0), year(0), pages(0), cost(0.0), ageLimit(0) {}
};


// 2. Структуры для массивов-индексов

// Индекс для сортировки по Инвентарному номеру (числовой)
struct IndexByInvNum {
    unsigned int key;      // Значение инв. номера
    unsigned int recNum;   // Номер записи в основном массиве
};

// Индекс для сортировки по Названию (строковый)
struct IndexByTitle {
    string key;            // Название книги
    unsigned int recNum;   // Номер записи в основном массиве
};

/* GREEN */
// Индекс для сортировки по Составному ключу (Автор + Название)
struct IndexByComposite {
    string key;            // Конкатенация: author + " | " + title
    unsigned int recNum;   // Номер записи в основном массиве
};


// Константы и Глобальные переменные (для упрощения передачи в функции)
    
const unsigned int MAX_BOOKS = 100;
Book library[MAX_BOOKS];             // Основной массив записей
IndexByInvNum idxInv[MAX_BOOKS];     // Массив-индекс по инв. номеру
IndexByTitle idxTitle[MAX_BOOKS];    // Массив-индекс по названию
/* GREEN */
IndexByComposite idxComposite[MAX_BOOKS]; // Массив-индекс по составному ключу
unsigned int bookCount = 0;          // Текущее количество книг


// Вспомогательные функции ввода/очистки

void clearInputStream() {
    cin.clear();
    cin.ignore(10000, '\n');
}

// Проверка, что строка не пуста
bool validateString(const string& s, const string& fieldName) {
    if (s.empty()) {
        cout << "  [Ошибка] Поле '" << fieldName << "' не может быть пустым!" << endl;
        return false;
    }
    return true;
}


// 3. Ввод данных (Клавиатура и Файл)
// Ввод одной книги с клавиатуры с валидацией
void inputOneBook(Book& b, int num) {
    cout << "\n--- Ввод книги #" << num << " ---" << endl;

    // Инвентарный номер
    while (true) {
        cout << "Инвентарный номер: ";
        if (cin >> b.invNumber) break;
        clearInputStream();
        cout << "  [Ошибка] Введите число!" << endl;
    }

    // Название
    cin.ignore(); // очистка буфера после числа
    while (true) {
        cout << "Название книги: ";
        getline(cin, b.title);
        if (validateString(b.title, "Название")) break;
    }

    // Автор
    while (true) {
        cout << "Автор: ";
        getline(cin, b.author);
        if (validateString(b.author, "Автор")) break;
    }

    // Год
    while (true) {
        cout << "Год издания: ";
        if (cin >> b.year) {
            if (b.year > 1400 && b.year <= 2026) break;
            cout << "  [Ошибка] Неверный год!" << endl;
        }
        else {
            clearInputStream();
            cout << "  [Ошибка] Введите число!" << endl;
        }
    }

    // Издательство
    cin.ignore();
    while (true) {
        cout << "Издательство: ";
        getline(cin, b.publisher);
        if (validateString(b.publisher, "Издательство")) break;
    }

    // Жанр
    while (true) {
        cout << "Жанр: ";
        getline(cin, b.genre);
        if (validateString(b.genre, "Жанр")) break;
    }

    // Страницы
    while (true) {
        cout << "Количество страниц: ";
        if (cin >> b.pages) {
            if (b.pages > 0) break;
            cout << "  [Ошибка] Страниц должно быть > 0!" << endl;
        }
        else {
            clearInputStream();
            cout << "  [Ошибка] Введите число!" << endl;
        }
    }

    // Стоимость
    while (true) {
        cout << "Стоимость (руб): ";
        if (cin >> b.cost) {
            if (b.cost >= 0) break;
            cout << "  [Ошибка] Стоимость не может быть отрицательной!" << endl;
        }
        else {
            clearInputStream();
            cout << "  [Ошибка] Введите число!" << endl;
        }
    }

    // Возраст
    while (true) {
        cout << "Возрастное ограничение (0, 6, 12, 16, 18): ";
        if (cin >> b.ageLimit) break;
        clearInputStream();
        cout << "  [Ошибка] Введите число!" << endl;
    }
    cin.ignore();

    // Статус
    while (true) {
        cout << "Статус (Доступна/Выдана): ";
        getline(cin, b.status);
        // Приводим к нижнему регистру для проверки или просто принимаем
        if (!b.status.empty()) break;
        cout << "  [Ошибка] Введите статус!" << endl;
    }
}

// Ввод массива с клавиатуры
void inputFromKeyboard() {
    if (bookCount >= MAX_BOOKS) {
        cout << "Библиотека заполнена!" << endl;
        return;
    }

    int n;
    cout << "Сколько книг добавить? (макс " << (MAX_BOOKS - bookCount) << "): ";
    cin >> n;
    if (n <= 0 || n + bookCount > MAX_BOOKS) {
        cout << "Некорректное количество." << endl;
        clearInputStream();
        return;
    }
    clearInputStream();

    for (int i = 0; i < n; ++i) {
        inputOneBook(library[bookCount], bookCount + 1);

        // Заполняем индексы сразу при вводе
        idxInv[bookCount].key = library[bookCount].invNumber;
        idxInv[bookCount].recNum = bookCount;

        idxTitle[bookCount].key = library[bookCount].title;
        idxTitle[bookCount].recNum = bookCount;

        bookCount++;
    }
    cout << "Ввод завершен." << endl;
}

// Ввод из файла
bool inputFromFile(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cout << "[Ошибка] Файл не найден: " << filename << endl;
        return false;
    }

    string line;
    bookCount = 0;
    // Пропускаем заголовок, если он есть (первая строка)
    // getline(fin, line); 

    while (getline(fin, line) && bookCount < MAX_BOOKS) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        Book b;

        try {
            getline(ss, token, '|'); b.invNumber = stoul(token);
            getline(ss, b.title, '|');
            getline(ss, b.author, '|');
            getline(ss, token, '|'); b.year = stoul(token);
            getline(ss, b.publisher, '|');
            getline(ss, b.genre, '|');
            getline(ss, token, '|'); b.pages = stoul(token);
            getline(ss, token, '|'); b.cost = stod(token);
            getline(ss, token, '|'); b.ageLimit = stoul(token);
            getline(ss, b.status, '|');

            // Убираем \r если есть
            if (!b.status.empty() && b.status.back() == '\r') b.status.pop_back();

            library[bookCount] = b;

            // Заполняем индексы
            idxInv[bookCount].key = b.invNumber;
            idxInv[bookCount].recNum = bookCount;
            idxTitle[bookCount].key = b.title;
            idxTitle[bookCount].recNum = bookCount;

            bookCount++;
        }
        catch (...) {
            cout << "[Предупреждение] Ошибка чтения строки, пропуск." << endl;
        }
    }
    fin.close();
    cout << "Загружено книг: " << bookCount << endl;
    return true;
}


// 4. Вывод данных


// Вывод одной книги
void printBook(const Book& b) {
    cout << left
        << setw(6) << b.invNumber
        << " | " << setw(25) << b.title
        << " | " << setw(20) << b.author
        << " | " << setw(6) << b.year
        << " | " << setw(15) << b.status
        << endl;
}

// Вывод всей библиотеки (в порядке хранения в памяти)
void printAllBooks() {
    if (bookCount == 0) {
        cout << "Список пуст." << endl;
        return;
    }
    cout << "\n=== ВСЕ КНИГИ (порядок ввода) ===" << endl;
    cout << left << setw(6) << "Inv#" << " | " << setw(25) << "Title" << " | " << setw(20) << "Author" << " | " << setw(6) << "Year" << " | " << setw(15) << "Status" << endl;
    cout << string(80, '-') << endl;
    for (unsigned int i = 0; i < bookCount; ++i) {
        printBook(library[i]);
    }
}

// Вывод по индексу (сортированный список)

void printSorted(int type) {
    if (bookCount == 0) return;

    cout << "\n=== ОТСТОРТИРОВАННЫЙ СПИСОК ===" << endl;
    if (type == 0) {
        cout << "Сортировка: по Инвентарному номеру (возрастание)" << endl;
    }
    else {
        cout << "Сортировка: по Названию (лексикографически)" << endl;
    }

    cout << left << setw(6) << "Inv#" << " | " << setw(25) << "Title" << " | " << setw(20) << "Author" << " | " << setw(6) << "Year" << " | " << setw(15) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (unsigned int i = 0; i < bookCount; ++i) {
        unsigned int realIndex;
        if (type == 0) {
            realIndex = idxInv[i].recNum;
        }
        else {
            realIndex = idxTitle[i].recNum;
        }
        printBook(library[realIndex]);
    }
}

// Сохранение в файл
void saveToFile(const string& filename) {
    ofstream fout(filename);
    if (!fout.is_open()) {
        cout << "[Ошибка] Не удалось создать файл." << endl;
        return;
    }
    for (unsigned int i = 0; i < bookCount; ++i) {
        fout << library[i].invNumber << "|"
            << library[i].title << "|"
            << library[i].author << "|"
            << library[i].year << "|"
            << library[i].publisher << "|"
            << library[i].genre << "|"
            << library[i].pages << "|"
            << library[i].cost << "|"
            << library[i].ageLimit << "|"
            << library[i].status << endl;
    }
    fout.close();
    cout << "Данные сохранены в " << filename << endl;
}


// 5. СОРТИРОВКА ИНДЕКСОВ

/* GREEN */
// Факультативно: Функции сравнения для qsort
int compareInvNum(const void* a, const void* b) {
    const IndexByInvNum* ia = (const IndexByInvNum*)a;
    const IndexByInvNum* ib = (const IndexByInvNum*)b;
    if (ia->key < ib->key) return -1;
    if (ia->key > ib->key) return 1;
    return 0;
}

int compareTitleAsc(const void* a, const void* b) {
    const IndexByTitle* ia = (const IndexByTitle*)a;
    const IndexByTitle* ib = (const IndexByTitle*)b;
    return ia->key.compare(ib->key);
}

int compareCompositeDesc(const void* a, const void* b) {
    const IndexByComposite* ia = (const IndexByComposite*)a;
    const IndexByComposite* ib = (const IndexByComposite*)b;
    // Обратный порядок (убывание)
    return ib->key.compare(ia->key);
}

// 5.1 Итерационная сортировка: Вставками (Insertion Sort) для Инв. Номера
void sortIndexInvNum() {
    // Сортируем массив idxInv по полю key (invNumber)
    for (unsigned int i = 1; i < bookCount; ++i) {
        IndexByInvNum temp = idxInv[i];
        int j = i - 1;
        while (j >= 0 && idxInv[j].key > temp.key) {
            idxInv[j + 1] = idxInv[j];
            j--;
        }
        idxInv[j + 1] = temp;
    }
}

/* GREEN */
// Сортировка через qsort для сравнения результатов (факультативно)
void sortIndexInvNumQsort() {
    qsort(idxInv, bookCount, sizeof(IndexByInvNum), compareInvNum);
}

// Рекурсивная сортировка: QuickSort для Названия
// Вспомогательная функция разбиения
int partitionTitle(int low, int high) {
    string pivot = idxTitle[high].key;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        // Сравнение строк (лексикографическое)
        if (idxTitle[j].key < pivot) {
            i++;
            swap(idxTitle[i], idxTitle[j]);
        }
    }
    swap(idxTitle[i + 1], idxTitle[high]);
    return (i + 1);
}

void quickSortTitle(int low, int high) {
    if (low < high) {
        int pi = partitionTitle(low, high);
        quickSortTitle(low, pi - 1);
        quickSortTitle(pi + 1, high);
    }
}

void sortIndexTitle() {
    if (bookCount > 0) {
        quickSortTitle(0, bookCount - 1);
    }
}

/* GREEN */
// Сортировка через qsort для сравнения результатов (факультативно)
void sortIndexTitleQsort() {
    qsort(idxTitle, bookCount, sizeof(IndexByTitle), compareTitleAsc);
}

/* GREEN */
// Сортировка по составному ключу (Автор + Название) - рекурсивная быстрая сортировка (по убыванию)
int partitionComposite(int low, int high) {
    string pivot = idxComposite[high].key;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        // Сортировка в ОБРАТНОМ порядке (убывание) по сравнению с первым ключом
        if (idxComposite[j].key > pivot) {
            i++;
            swap(idxComposite[i], idxComposite[j]);
        }
    }
    swap(idxComposite[i + 1], idxComposite[high]);
    return (i + 1);
}

void quickSortComposite(int low, int high) {
    if (low < high) {
        int pi = partitionComposite(low, high);
        quickSortComposite(low, pi - 1);
        quickSortComposite(pi + 1, high);
    }
}

void sortIndexComposite() {
    if (bookCount > 0) {
        quickSortComposite(0, bookCount - 1);
    }
}

// Общая функция пересчета индексов (вызывается после любого изменения данных)
void rebuildIndices() {
    // Сначала просто копируем текущие ссылки
    for (unsigned int i = 0; i < bookCount; ++i) {
        idxInv[i].key = library[i].invNumber;
        idxInv[i].recNum = i;

        idxTitle[i].key = library[i].title;
        idxTitle[i].recNum = i;
        
        /* GREEN */
        // Формируем составной ключ: Автор + " | " + Название
        idxComposite[i].key = library[i].author + " | " + library[i].title;
        idxComposite[i].recNum = i;
    }
    // Потом сортируем
    sortIndexInvNum();
    sortIndexTitle();
    /* GREEN */
    sortIndexComposite();
}

// 6. ПОИСК (Задание 2, п.9)

// Итерационный бинарный поиск по Инв. номеру
int binarySearchIterative(unsigned int target) {
    int left = 0;
    int right = bookCount - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (idxInv[mid].key == target) {
            return idxInv[mid].recNum; // Возвращаем реальный индекс в массиве library
        }

        if (idxInv[mid].key < target) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }
    return -1; // Не найдено
}

// Рекурсивный бинарный поиск по Названию
int binarySearchRecursiveHelper(const string& target, int left, int right) {
    if (left > right) return -1;

    int mid = left + (right - left) / 2;

    int cmp = idxTitle[mid].key.compare(target);

    if (cmp == 0) {
        return idxTitle[mid].recNum;
    }

    if (cmp < 0) {
        return binarySearchRecursiveHelper(target, mid + 1, right);
    }
    else {
        return binarySearchRecursiveHelper(target, left, mid - 1);
    }
}

int binarySearchRecursiveTitle(const string& target) {
    return binarySearchRecursiveHelper(target, 0, bookCount - 1);
}

/* GREEN */
// Функции поиска с выводом ВСЕХ совпадений при неуникальных ключах

// Итерационный поиск по invNumber с выводом всех совпадений (хотя invNumber уникален)
void binarySearchIterativeAll(unsigned int target) {
    int left = 0;
    int right = bookCount - 1;
    bool found = false;
    
    // Сначала находим любое совпадение
    int foundPos = -1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (idxInv[mid].key == target) {
            foundPos = mid;
            break;
        }
        if (idxInv[mid].key < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    if (foundPos == -1) {
        cout << "Книга с таким инвентарным номером не найдена." << endl;
        return;
    }
    
    // Распространяемся влево и вправо для поиска всех совпадений
    int start = foundPos, end = foundPos;
    while (start > 0 && idxInv[start - 1].key == target) start--;
    while (end < (int)bookCount - 1 && idxInv[end + 1].key == target) end++;
    
    cout << "Найдено записей: " << (end - start + 1) << endl;
    for (int i = start; i <= end; i++) {
        printBook(library[idxInv[i].recNum]);
    }
}

// Рекурсивный поиск по названию с выводом всех совпадений
void binarySearchRecursiveAllHelper(const string& target, int left, int right, vector<int>& results) {
    if (left > right) return;
    
    int mid = left + (right - left) / 2;
    int cmp = idxTitle[mid].key.compare(target);
    
    if (cmp == 0) {
        // Нашли совпадение, добавляем и ищем слева и справа
        results.push_back(idxTitle[mid].recNum);
        
        // Ищем слева
        int l = mid - 1;
        while (l >= left && idxTitle[l].key == target) {
            results.push_back(idxTitle[l].recNum);
            l--;
        }
        
        // Ищем справа
        int r = mid + 1;
        while (r <= right && idxTitle[r].key == target) {
            results.push_back(idxTitle[r].recNum);
            r++;
        }
    } else if (cmp < 0) {
        binarySearchRecursiveAllHelper(target, mid + 1, right, results);
    } else {
        binarySearchRecursiveAllHelper(target, left, mid - 1, results);
    }
}

void binarySearchRecursiveTitleAll(const string& target) {
    vector<int> results;
    binarySearchRecursiveAllHelper(target, 0, bookCount - 1, results);
    
    if (results.empty()) {
        cout << "Книга с таким названием не найдена." << endl;
    } else {
        cout << "Найдено записей: " << results.size() << endl;
        for (size_t i = 0; i < results.size(); i++) {
            printBook(library[results[i]]);
        }
    }
}

// Бинарный поиск по составному ключу (итерационный)
int binarySearchComposite(const string& target) {
    int left = 0;
    int right = bookCount - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = idxComposite[mid].key.compare(target);
        
        if (cmp == 0) {
            return idxComposite[mid].recNum;
        }
        
        if (cmp < 0) {
            // Составной ключ отсортирован по убыванию, поэтому меняем направление
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return -1;
}

// Поиск по составному ключу с выводом всех совпадений
void binarySearchCompositeAll(const string& target) {
    int left = 0;
    int right = bookCount - 1;
    bool found = false;
    
    int foundPos = -1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = idxComposite[mid].key.compare(target);
        
        if (cmp == 0) {
            foundPos = mid;
            break;
        }
        
        if (cmp < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    
    if (foundPos == -1) {
        cout << "Книга с таким составным ключом не найдена." << endl;
        return;
    }
    
    // Распространяемся влево и вправо для поиска всех совпадений
    int start = foundPos, end = foundPos;
    while (start > 0 && idxComposite[start - 1].key == target) start--;
    while (end < (int)bookCount - 1 && idxComposite[end + 1].key == target) end++;
    
    cout << "Найдено записей: " << (end - start + 1) << endl;
    for (int i = start; i <= end; i++) {
        printBook(library[idxComposite[i].recNum]);
    }
}

// Сравнение с std::binary_search из <algorithm>
#include <algorithm>

bool stdBinarySearchByInv(unsigned int target) {
    // Создаем временный массив для сравнения
    vector<unsigned int> keys(bookCount);
    for (unsigned int i = 0; i < bookCount; i++) {
        keys[i] = idxInv[i].key;
    }
    return std::binary_search(keys.begin(), keys.end(), target);
}

bool stdBinarySearchByTitle(const string& target) {
    vector<string> keys(bookCount);
    for (unsigned int i = 0; i < bookCount; i++) {
        keys[i] = idxTitle[i].key;
    }
    return std::binary_search(keys.begin(), keys.end(), target);
}

void searchMenu() {
    cout << "\n--- ПОИСК ---" << endl;
    cout << "1. Поиск по инвентарному номеру (итерационный)" << endl;
    cout << "2. Поиск по названию (рекурсивный)" << endl;
    /* GREEN */
    cout << "3. Поиск по инвентарному номеру (все совпадения)" << endl;
    cout << "4. Поиск по названию (все совпадения)" << endl;
    cout << "5. Поиск по составному ключу (автор + название)" << endl;
    cout << "6. Сравнение с std::binary_search" << endl;
    cout << "0. Назад" << endl;
    cout << "Выбор: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
        unsigned int inv;
        cout << "Введите инвентарный номер: ";
        cin >> inv;
        int idx = binarySearchIterative(inv);
        if (idx != -1) {
            cout << "Найдено:" << endl;
            printBook(library[idx]);
        }
        else {
            cout << "Книга с таким инвентарным номером не найдена." << endl;
        }
    }
    else if (choice == 2) {
        string title;
        cin.ignore();
        cout << "Введите название: ";
        getline(cin, title);
        int idx = binarySearchRecursiveTitle(title);
        if (idx != -1) {
            cout << "Найдено:" << endl;
            printBook(library[idx]);
        }
        else {
            cout << "Книга с таким названием не найдена." << endl;
        }
    }
    /* GREEN */
    else if (choice == 3) {
        unsigned int inv;
        cout << "Введите инвентарный номер: ";
        cin >> inv;
        binarySearchIterativeAll(inv);
    }
    else if (choice == 4) {
        string title;
        cin.ignore();
        cout << "Введите название: ";
        getline(cin, title);
        binarySearchRecursiveTitleAll(title);
    }
    else if (choice == 5) {
        string author, title;
        cin.ignore();
        cout << "Введите автора: ";
        getline(cin, author);
        cout << "Введите название: ";
        getline(cin, title);
        string compositeKey = author + " | " + title;
        binarySearchCompositeAll(compositeKey);
    }
    else if (choice == 6) {
        unsigned int inv;
        string title;
        cout << "Введите инвентарный номер для сравнения: ";
        cin >> inv;
        bool myResult = (binarySearchIterative(inv) != -1);
        bool stdResult = stdBinarySearchByInv(inv);
        cout << "Мой поиск: " << (myResult ? "найдено" : "не найдено") << endl;
        cout << "std::binary_search: " << (stdResult ? "найдено" : "не найдено") << endl;
        cout << "Результаты совпадают: " << (myResult == stdResult ? "ДА" : "НЕТ") << endl;
        
        cin.ignore();
        cout << "Введите название для сравнения: ";
        getline(cin, title);
        myResult = (binarySearchRecursiveTitle(title) != -1);
        stdResult = stdBinarySearchByTitle(title);
        cout << "Мой поиск: " << (myResult ? "найдено" : "не найдено") << endl;
        cout << "std::binary_search: " << (stdResult ? "найдено" : "не найдено") << endl;
        cout << "Результаты совпадают: " << (myResult == stdResult ? "ДА" : "НЕТ") << endl;
    }
}


// 7. РЕДАКТИРОВАНИЕ и УДАЛЕНИЕ


void editBook() {
    cout << "\n--- РЕДАКТИРОВАНИЕ ---" << endl;
    unsigned int inv;
    cout << "Введите инвентарный номер книги для редактирования: ";
    cin >> inv;

    int idx = binarySearchIterative(inv);
    if (idx == -1) {
        cout << "Книга не найдена." << endl;
        return;
    }

    cout << "Текущие данные:" << endl;
    printBook(library[idx]);
    cout << "Введите новые данные:" << endl;



    cin.ignore();
    cout << "Новое название: "; getline(cin, library[idx].title);
    cout << "Новый автор: "; getline(cin, library[idx].author);
    cout << "Новый статус (Доступна/Выдана): "; getline(cin, library[idx].status);

    // После редактирования нужно перестроить индексы, т.к. могло измениться название
    rebuildIndices();
    cout << "Данные обновлены. Индексы перестроены." << endl;
}

void deleteBook() {
    cout << "\n--- УДАЛЕНИЕ ---" << endl;
    unsigned int inv;
    cout << "Введите инвентарный номер книги для удаления: ";
    cin >> inv;

    int idx = binarySearchIterative(inv);
    if (idx == -1) {
        cout << "Книга не найдена." << endl;
        return;
    }

    cout << "Удалить книгу: ";
    printBook(library[idx]);
    char confirm;
    cout << "Вы уверены? (y/n): ";
    cin >> confirm;

    if (confirm == 'y' || confirm == 'Y') {
        // Сдвигаем элементы массива влево
        for (int i = idx; i < bookCount - 1; ++i) {
            library[i] = library[i + 1];
        }
        bookCount--;

        // Перестраиваем индексы
        rebuildIndices();
        cout << "Книга удалена." << endl;
    }
}

/* GREEN */
// Редактирование по названию (второй ключ)
void editBookByTitle() {
    cout << "\n--- РЕДАКТИРОВАНИЕ ПО НАЗВАНИЮ ---" << endl;
    string title;
    cin.ignore();
    cout << "Введите название книги для редактирования: ";
    getline(cin, title);

    int idx = binarySearchRecursiveTitle(title);
    if (idx == -1) {
        cout << "Книга не найдена." << endl;
        return;
    }

    cout << "Текущие данные:" << endl;
    printBook(library[idx]);
    cout << "Введите новые данные:" << endl;

    cout << "Новый инвентарный номер: "; 
    cin >> library[idx].invNumber;
    cin.ignore();
    cout << "Новое название: "; getline(cin, library[idx].title);
    cout << "Новый автор: "; getline(cin, library[idx].author);
    cout << "Новый статус (Доступна/Выдана): "; getline(cin, library[idx].status);

    // После редактирования нужно перестроить индексы
    rebuildIndices();
    cout << "Данные обновлены. Индексы перестроены." << endl;
}

// Удаление по названию (второй ключ)
void deleteBookByTitle() {
    cout << "\n--- УДАЛЕНИЕ ПО НАЗВАНИЮ ---" << endl;
    string title;
    cin.ignore();
    cout << "Введите название книги для удаления: ";
    getline(cin, title);

    int idx = binarySearchRecursiveTitle(title);
    if (idx == -1) {
        cout << "Книга не найдена." << endl;
        return;
    }

    cout << "Удалить книгу: ";
    printBook(library[idx]);
    char confirm;
    cout << "Вы уверены? (y/n): ";
    cin >> confirm;

    if (confirm == 'y' || confirm == 'Y') {
        // Сдвигаем элементы массива влево
        for (int i = idx; i < bookCount - 1; ++i) {
            library[i] = library[i + 1];
        }
        bookCount--;

        // Перестраиваем индексы
        rebuildIndices();
        cout << "Книга удалена." << endl;
    }
}

// Вывод массива-индекса по инвентарному номеру (п.14)
void printIndexByInvNum() {
    if (bookCount == 0) {
        cout << "Индекс пуст." << endl;
        return;
    }
    cout << "\n=== ИНДЕКС ПО ИНВЕНТАРНОМУ НОМЕРУ (возрастание) ===" << endl;
    cout << left << setw(6) << "Key" << " | " << setw(10) << "RecNum" << endl;
    cout << string(20, '-') << endl;
    for (unsigned int i = 0; i < bookCount; i++) {
        cout << left << setw(6) << idxInv[i].key << " | " << setw(10) << idxInv[i].recNum << endl;
    }
}

// Вывод массива-индекса по названию (п.14)
void printIndexByTitle() {
    if (bookCount == 0) {
        cout << "Индекс пуст." << endl;
        return;
    }
    cout << "\n=== ИНДЕКС ПО НАЗВАНИЮ (лексикографически) ===" << endl;
    cout << left << setw(30) << "Key" << " | " << setw(10) << "RecNum" << endl;
    cout << string(45, '-') << endl;
    for (unsigned int i = 0; i < bookCount; i++) {
        string shortKey = idxTitle[i].key.substr(0, 28);
        cout << left << setw(30) << shortKey << " | " << setw(10) << idxTitle[i].recNum << endl;
    }
}

// Вывод массива-индекса по составному ключу (факультативно)
void printIndexByComposite() {
    if (bookCount == 0) {
        cout << "Индекс пуст." << endl;
        return;
    }
    cout << "\n=== ИНДЕКС ПО СОСТАВНОМУ КЛЮЧУ (автор+название, убывание) ===" << endl;
    cout << left << setw(40) << "Key" << " | " << setw(10) << "RecNum" << endl;
    cout << string(55, '-') << endl;
    for (unsigned int i = 0; i < bookCount; i++) {
        string shortKey = idxComposite[i].key.substr(0, 38);
        cout << left << setw(40) << shortKey << " | " << setw(10) << idxComposite[i].recNum << endl;
    }
}

// Вывод отсортированного списка по составному ключу
void printSortedByComposite() {
    if (bookCount == 0) return;

    cout << "\n=== ОТСТОРТИРОВАННЫЙ СПИСОК ПО СОСТАВНОМУ КЛЮЧУ (убывание) ===" << endl;
    cout << left << setw(6) << "Inv#" << " | " << setw(25) << "Title" << " | " << setw(20) << "Author" << " | " << setw(6) << "Year" << " | " << setw(15) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (unsigned int i = 0; i < bookCount; ++i) {
        unsigned int realIndex = idxComposite[i].recNum;
        printBook(library[realIndex]);
    }
}

// Оптимизированная переиндексация после редактирования (факультативно)
void updateIndicesAfterEdit(unsigned int editedIndex) {
    // Обновляем только измененные записи в индексах
    for (unsigned int i = 0; i < bookCount; i++) {
        if (idxInv[i].recNum == editedIndex) {
            idxInv[i].key = library[editedIndex].invNumber;
        }
        if (idxTitle[i].recNum == editedIndex) {
            idxTitle[i].key = library[editedIndex].title;
        }
        if (idxComposite[i].recNum == editedIndex) {
            idxComposite[i].key = library[editedIndex].author + " | " + library[editedIndex].title;
        }
    }
    // Пересортировываем индексы (можно использовать более эффективную сортировку, 
    // но для простоты используем полную пересортировку)
    sortIndexInvNum();
    sortIndexTitle();
    sortIndexComposite();
}

// ГЛАВНОЕ МЕНЮ

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    setlocale(LC_ALL, "Russian");

    bool running = true;
    while (running) {
        cout << "\n========================================" << endl;
        cout << "   КНИГИ В БИБЛИОТЕКЕ" << endl;
        cout << "========================================" << endl;
        cout << "1. Загрузить из файла" << endl;
        cout << "2. Ввести данные с клавиатуры" << endl;
        cout << "3. Показать все книги (как в памяти)" << endl;
        cout << "4. Показать отсортированные списки (Индексы)" << endl;
        /* GREEN */
        cout << "9. Показать индексы (массивы-индексы)" << endl;
        cout << "10. Сортировка по составному ключу" << endl;
        cout << "11. Редактировать по названию" << endl;
        cout << "12. Удалить по названию" << endl;
        cout << "5. Поиск книг" << endl;
        cout << "6. Редактировать запись (по инв. номеру)" << endl;
        cout << "7. Удалить запись (по инв. номеру)" << endl;
        cout << "8. Сохранить в файл" << endl;
        cout << "0. Выход" << endl;
        cout << "Выбор: ";

        int choice;
        cin >> choice;
        clearInputStream();

        switch (choice) {
        case 1: {
            string fname;
            cout << "Имя файла: ";
            getline(cin, fname);
            inputFromFile(fname);
            rebuildIndices(); 
            break;
        }
        case 2:
            inputFromKeyboard();
            rebuildIndices(); 
            break;
        case 3:
            printAllBooks();
            break;
        case 4:
            if (bookCount == 0) {
                cout << "Нет данных." << endl;
            }
            else {
                printSorted(0); // По инв. номеру
                cout << "\n";
                printSorted(1); // По названию
            }
            break;
        /* GREEN */
        case 9:
            if (bookCount == 0) {
                cout << "Нет данных." << endl;
            }
            else {
                printIndexByInvNum();
                printIndexByTitle();
                printIndexByComposite();
            }
            break;
        case 10:
            printSortedByComposite();
            break;
        case 11:
            editBookByTitle();
            break;
        case 12:
            deleteBookByTitle();
            break;
        case 5:
            searchMenu();
            break;
        case 6:
            editBook();
            break;
        case 7:
            deleteBook();
            break;
        case 8: {
            string fname;
            cout << "Имя файла для сохранения: ";
            getline(cin, fname);
            saveToFile(fname);
            break;
        }
        case 0:
            running = false;
            cout << "Выход из программы." << endl;
            break;
        default:
            cout << "Неверный выбор." << endl;
        }
    }

    return 0;
}