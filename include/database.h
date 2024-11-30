//#include <sys/types.h>
//#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <string>

#ifndef DATABASE
#define DATABASE


namespace memdb {



void findAndBorderAll(std::string&, const std::string&);
    
class Database {
public:
    
    class executionResult {
    public:
        executionResult()  = default;
        ~executionResult() = default;
        
        void setStatus(bool, const std::string& = "");
        void setStatus(const std::string&);
        
        bool         is_ok() const noexcept { return status_; }
        std::string  what()  const noexcept { return error_;  }
        
    private:
        bool status_;
        std::string error_; 
                           
    };
    
    Database() = default;
    ~Database();

    executionResult execute(std::string);

private:
    static inline executionResult lastExecutionResult;
    
    enum class types {INT32, BOOL, STR, BYTES};
    
    struct cell {
        size_t sizeName;
        std::string name;
        types type;
        char* begin;
        size_t size;
        
        bool unique;
        bool autoincrement;
        bool key;
    };
    
    class row {
    public:
        row(const std::vector <cell> &);
        row(const row*);
        
        row() = delete;
        row(const row&) = delete;
        row& operator=(const row&) = delete;
        
        ~row() {
            delete[] rowData;
        }
        
        char* getBegin() const noexcept { return rowData; }
        
        cell getCell(const size_t) const noexcept;
        cell getCell(const std::string&) const noexcept;
        
    private:
        /* Основные (базовые) атрибуты строки конкретной таблицы */
        char* rowData = nullptr;
        std::vector <cell> columnsDescription_;
        
        /* Атрибуты, выводящиеся из базовых */
        size_t sizeOfRow;
        std::map <std::string, size_t> cellNameToIndex;
        
    };
    
    std::map <std::string, row*> baseTablesRows;
    std::map <std::string, std::vector <row*> > tables;

    size_t getSize(std::stringstream&) const noexcept;
    char* getValue(std::stringstream&, types, size_t) const noexcept;
    void getValues(std::stringstream&, std::vector <char*> &, const row* const) const noexcept;
    
    void parseCreate(std::stringstream&, std::string&, std::vector <cell> &, std::vector <char*> &) const noexcept;
    void createTable(const std::string&, const std::vector <cell> &, const std::vector <char*> &) noexcept;
    
    void parseInsert(std::stringstream&, std::string&, std::vector <char*> &) const noexcept;
    void insert(const std::string&, const std::vector <char*> &) noexcept;
    
    
    void select(const std::string& nameTable, const std::vector <char*> &values) noexcept;
    void update(const std::string& nameTable, const std::vector <char*> &values) noexcept;
    void deleteRows(const std::string& nameTable, const std::vector <char*> &values) noexcept;
    
};



} // memdb

#include "../src/database.cpp"
#include "../src/commonParse.cpp"
#include "../src/parseCreate.cpp"
#include "../src/parseInsert.cpp"

#endif // DATABASE
