//#include <sys/types.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstring>

#ifndef DATABASE
#define DATABASE


namespace memdb {



void findAndBorderAll(std::string&, const std::string&);
std::stringstream splittingRequests(const std::string&);
    
class Database {
    
    class row;

public:
    
    class executionResult {
    public:
        
        /*
        class iterator {
        public:

            iterator() = default;
            ~iterator() = default;
        
            return 
        }
        */
        
        executionResult()  = default;
        ~executionResult() = default; // если определять и писать без smart ptr, то необходимо
                                      // будет еще писать конструктор копирования для execute
        /*
        void zeroing() {
            baseRowOfTempTable = nullptr;
            for (size_t i{0}; i < tempTable.size(); ++i) {
                tempTable[i] = nullptr;
            }
        }
        ~executionResult() {
            delete baseRowOfTempTable;
            for (auto currentRow: tempTable) {
                delete currentRow;
            }
        }
        */
        
        void setStatus(bool, const std::string& = "") noexcept;
        void setStatus(const std::string&) noexcept;
        
        bool         is_ok() const noexcept { return status_; }
        std::string  what()  const noexcept { return error_;  }
        
        void createTempTable(const row* const baseRow, const std::vector <std::string> &columns) noexcept;
        void insert(const row* const currentRow, const std::vector <std::string> &columns) noexcept;
        
        std::vector<row*>::iterator begin() { return tempTable.begin(); }
        std::vector<row*>::iterator end()   { return tempTable.end();   }
        
    private:
        bool status_;
        std::string error_; 
        
        std::vector <row*> tempTable;
        row* baseRowOfTempTable;
                           
    };
    
    Database() = default;
    ~Database();

    executionResult execute(std::string) noexcept;
    //void recover() { lastExecutionResult.setStatus(true); }

private:
    static inline executionResult lastExecutionResult;
    
    enum class types {INT32, BOOL, STR, BYTES};
    
    struct cell {
        size_t sizeName;
        std::string name;
        types type;
        char* begin = nullptr;
        size_t size;
        
        bool unique = false;
        bool autoincrement = false;
        bool key = false;
    };
    
    class row {
    public:
        friend class Database;
        
        row(const std::vector <cell> &);
        row(const row*);
        
        row() = delete;
        row(const row&) = delete;
        row& operator=(const row&) = delete;
        
        ~row() {
            delete[] rowData;
        }
        
        size_t size() const noexcept { return columnsDescription_.size(); }
        
        template<class T>
        T get(const std::string& nameCol) const noexcept {
            cell column = getCell(nameCol);
            if (lastExecutionResult.is_ok() == false) {
                lastExecutionResult.setStatus(true); // востановление состояния для дальнейшего получения ячеек
                return nullptr; 
            }
            return reinterpret_cast<T>(column.begin);
        }
        
    private:
        /* Основные (базовые) атрибуты строки конкретной таблицы */
        char* rowData = nullptr;
        std::vector <cell> columnsDescription_;
        
        /* Атрибуты, выводящиеся из базовых */
        size_t sizeOfRow;
        std::map <std::string, size_t> cellNameToIndex;
        /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
        
        
        char* getBegin() const noexcept { return rowData; }
        size_t getNumberOfCells() const noexcept { return columnsDescription_.size(); }
        size_t getIndexByCellName(const std::string&) const noexcept;
        
        cell getCell(const size_t) const noexcept;
        cell getCell(const std::string&) const noexcept;

    };
    
    std::map <std::string, row*> baseTablesRows;
    std::map <std::string, std::vector <row*> > tables;

    
    /* Общие функции парсеров */
    size_t getSize(std::stringstream&) const noexcept;
    char* getValue(std::stringstream&, types, size_t) const noexcept;
    void getValues(std::stringstream&, std::vector <char*> &, const row* const) const noexcept;
    /* ~~~~~~~~~~~~~~~~~~~~~~ */
    
    
    void parseCreate(std::stringstream&, std::string&, std::vector <cell> &, std::vector <char*> &) const noexcept;
    void createTable(const std::string&, const std::vector <cell> &, const std::vector <char*> &) noexcept;
    
    void parseInsert(std::stringstream&, std::string&, std::vector <char*> &) const noexcept;
    void insert(const std::string&, const std::vector <char*> &) noexcept;
    
    
    std::vector <std::string> operations = { "||", "&&", "^^", "=", "!=", "<", "<=", ">", ">=", "+",
                                             "-", "*", "/", "%", "!" };
    
    
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
        
        void setOperator(std::string&) noexcept;
        
        void setLeft(condition*) noexcept;
        void setRight(condition*) noexcept;
        
        void setConst(cell*) noexcept;
        void setVariable(std::string&) noexcept;

        int cmp(cell&, cell&) const noexcept; // вспомогательная функция для сравнения велечин в descent
        void descent(condition*, const row*) noexcept;
        bool check(const row*) noexcept;
        
    private:
        enum class conditionTypes {PLUS, MINUS, MUL, DEV, MOD, LESS, EQUAL, MORE, ROUND, DIRECT,
                                   NOTMORE, NOTLESS, NOTEQUAL, AND, OR, NOT, XOR, CONST, VAR};
        conditionTypes conditionType;
        
        condition* leftChild = nullptr;
        condition* rightChild = nullptr;
        
        cell* value = nullptr;
        std::string nameCell = "";

    };
    
    condition* getCondition(std::vector <std::string> &, size_t, size_t) const noexcept;
    
    
    void parseSelect(std::stringstream&, std::string&, std::vector <std::string> &, condition*&) const noexcept;
    void select(const std::string&, const std::vector <std::string> &, condition* const) noexcept;
    
    /*
    void parseUpdate(std::stringstream&, std::string&, std::vector <char*> &, condition*) const noexcept;
    void update(const std::string&, const std::vector <char*> &, const condition*) noexcept;
    
    void parseDelete(std::stringstream&, std::string&, condition*) const noexcept;
    void deleteRows(const std::string&, const condition*) noexcept;
    */
    
};



} // memdb

#include "../src/database.cpp"
#include "../src/execute.cpp"
#include "../src/commonParse.cpp"
#include "../src/parseCreate.cpp"
#include "../src/parseInsert.cpp"
#include "../src/parseSelect.cpp"

#endif // DATABASE
