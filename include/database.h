//#include <sys/types.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstring>

#ifndef DATABASE
#define DATABASE


namespace memdb {



void findAndBorderAll(std::string&, const std::string&); // добавляет пробелы вокруг подстрок-разделителей (токенов)
std::stringstream splittingRequests(const std::string&); // весь запрос делит на команды, ставя между ними " ; "
    
class Database {
    
    class row;

public:
    
    class executionResult {
    public:
        
        executionResult()  = default;
        ~executionResult() = default; // если определять и писать без smart ptr, то необходимо
                                      // будет еще писать конструктор копирования для execute
        /*
        ~executionResult() {
            delete baseRowOfTempTable;
            for (auto currentRow: tempTable) {
                delete currentRow;
            }
        }
        */
        void close() {
            delete baseRowOfTempTable;
            for (auto currentRow: tempTable) {
                delete currentRow;
            }
        } // функция закрытия (очещения динамической памяти временной таблицы). обязательно вызвать по окончании работы с БД
        
        void setStatus(bool, const std::string& = "") noexcept; // устанавливает статус и ошибку, если статус false
        void setStatus(const std::string&) noexcept; // устанавливает статус и ошибку, если статус false
        
        bool         is_ok() const noexcept { return status_; } // возвращает статус
        std::string  what()  const noexcept { return error_;  } // возвращает ошибку
        
        void createTempTable(const row* const baseRow, const std::vector <std::string> &columns) noexcept; // создает временную таблицу
        void insert(const row* const currentRow, const std::vector <std::string> &columns) noexcept; // вставляет строку во
                                                                                                     // временную таблицу
        
        std::vector<row*>::iterator begin() { return tempTable.begin(); } // возвращает итератор на первую строку временной таблицы
        std::vector<row*>::iterator end()   { return tempTable.end();   } // возвращает end-итератор временной таблицы
        
    private:
        bool status_;
        std::string error_; 
        
        std::vector <row*> tempTable; // таблица, возвращаемая на запрос
        row* baseRowOfTempTable; // макет строк этой таблицы
                           
    };
    
    Database() = default;
    ~Database();

    executionResult execute(std::string) noexcept; // функция для выполнения запросов

private:
    static inline executionResult lastExecutionResult;
    
    enum class types {INT32, BOOL, STR, BYTES}; // типы значений ячеек
    
    struct cell {
        size_t sizeName;
        std::string name;
        types type;
        char* begin = nullptr;
        size_t size;
        
        bool unique = false;
        bool autoincrement = false;
        inline static size_t counter = 1;
        bool key = false;
    };
    
    class row {
    public:
        friend class Database;
        
        row(const std::vector <cell> &); // конструктор по массиву описаний
        row(const row*); // конструктор копирования по указателю на строку
        
        row() = delete;
        row(const row&) = delete;
        row& operator=(const row&) = delete;
        
        ~row() {
            delete[] rowData;
        }
        
        size_t size() const noexcept { return columnsDescription_.size(); } // возвращает количество столбцов в строке
        
        template<class T>
        T get(const std::string& nameCol) const noexcept {
            cell column = getCell(nameCol);
            if (lastExecutionResult.is_ok() == false) {
                lastExecutionResult.setStatus(true); // востановление состояния для дальнейшего получения ячеек
                return nullptr; 
            }
            return reinterpret_cast<T>(column.begin);
        } // часть интерфейса - получение значения ячейки по имени
        
    private:
        /* Основные (базовые) атрибуты строки конкретной таблицы */
        char* rowData = nullptr;
        std::vector <cell> columnsDescription_;
        
        /* Атрибуты, выводящиеся из базовых */
        size_t sizeOfRow;
        std::map <std::string, size_t> cellNameToIndex;
        /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
        
        
        char* getBegin() const noexcept { return rowData; } // получение указателя на начало значений строки
        size_t getNumberOfCells() const noexcept { return columnsDescription_.size(); } // количество столбцов в строке
        size_t getIndexByCellName(const std::string&) const noexcept; // получения индекса ячейки в строке по ее имени
        
        cell getCell(const size_t) const noexcept; // получение дефолтной копии ячейки
        cell getCell(const std::string&) const noexcept; // получение дефолтной копии ячейки

    };
    
    std::map <std::string, row*> baseTablesRows; // тут храняться все строки, которые получились из запросов create, на их
                                                 // основе создаются другие строки таблиц
    std::map <std::string, std::vector <row*> > tables; // тут храняться векторы строк - таблицы

    
    /* Общие функции парсеров */
    size_t getSize(std::stringstream&) const noexcept; // получение размера заданного типа в байтах
    char* getValue(std::stringstream&, types, size_t) const noexcept; // получение заданного значения ячейки
    void getValues(std::stringstream&, std::vector <char*> &, const row* const) const noexcept; // получение списка значений
                                                                                                // в 2х вариантах
    /* ~~~~~~~~~~~~~~~~~~~~~~ */
    
    
    void parseCreate(std::stringstream&, std::string&, std::vector <cell> &, std::vector <char*> &) const noexcept; // распаршивание
                                                                                                                    // запроса создания
                                                                                                                    // таблицы
    void createTable(const std::string&, const std::vector <cell> &, const std::vector <char*> &) noexcept; // создание таблицы
    
    void parseInsert(std::stringstream&, std::string&, std::vector <char*> &) const noexcept; // распаршивание запроса вставки строки
    void insert(const std::string&, const std::vector <char*> &) noexcept; // вставка строки
    
    
    std::vector <std::string> operations = { "||", "&&", "^^", "=", "!=", "<", "<=", ">", ">=", "+",
                                             "-", "*", "/", "%", "!" }; // всевозможные операторы выражений из запросов
    
    
    class condition {
    public:
        condition() = default;
        ~condition() {
            if (leftChild != nullptr) {
                delete leftChild;
            }
            if (rightChild != nullptr) {
                delete rightChild;
            }
            
            if (value != nullptr) {
                delete[] value->begin;
            }
            delete value;
        } // очищает константные значения, представленные в виде cell
        
        void setOperator(std::string&) noexcept; // установка в узел состояния оператора
        
        void setLeft(condition*) noexcept; // установка левого соседа
        void setRight(condition*) noexcept; // установка правого соседа
        
        void setConst(cell*) noexcept; // установка в узел состояния константы
        void setVariable(std::string&) noexcept; // установка в узел состояния переменной

        int cmp(cell&, cell&) const noexcept; // вспомогательная функция для сравнения велечин в descent
        void descent(condition*, const row*) noexcept; // рекурсивный спуск, который проверяет строку по выражению
        bool check(const row*) noexcept; // проверка строки на выражение, вызывающая рекурсивный спуск descent
        
    private:
        enum class conditionTypes {PLUS, MINUS, MUL, DEV, MOD, LESS, EQUAL, MORE, ROUND, DIRECT,
                                   NOTMORE, NOTLESS, NOTEQUAL, AND, OR, NOT, XOR, CONST, VAR};
        conditionTypes conditionType;
        
        condition* leftChild = nullptr;
        condition* rightChild = nullptr;
        
        cell* value = nullptr;
        std::string nameCell = "";

    };
    
    condition* getCondition(std::vector <std::string> &, size_t, size_t) const noexcept; // получение из запроса дерево выражений
    
    
    void parseSelect(std::stringstream&, std::string&, std::vector <std::string> &, condition*&) const noexcept; // распаршивание
                                                                                                                 // запроса select
    void select(const std::string&, const std::vector <std::string> &, condition* const) noexcept; // выполнение запроса select
    
    void parseUpdate(std::stringstream&, std::string&, std::vector <char*> &, condition*&) const noexcept; // распаршивание
                                                                                                           // запроса update
    void update(const std::string&, const std::vector <char*> &, condition* const) noexcept; // выполнение запроса update
    
    void parseDelete(std::stringstream&, std::string&, condition*&) const noexcept; // распаршивание
                                                                                    // запроса delete
    void deleteRows(const std::string&, condition* const) noexcept; // выполнение запроса delete
    
};



} // memdb

#include "../src/database.cpp"
#include "../src/execute.cpp"
#include "../src/commonParse.cpp"
#include "../src/parseCreate.cpp"
#include "../src/parseInsert.cpp"
#include "../src/parseSelect.cpp"
#include "../src/parseUpdate.cpp"
#include "../src/parseDelete.cpp"

#endif // DATABASE
