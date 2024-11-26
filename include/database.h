//#include <sys/types.h>
#include <cstring>
#include <string>
#include <vector>
#include <map>

#ifndef DATABASE
#define DATABASE


namespace memdb {



class Database {
public:
    
    class executionResult {
    public:
        executionResult()  = default;
        ~executionResult() = default;
        
        void set(bool, const std::string& = "");
        void set(const std::string&);
        
        bool         is_ok() const noexcept { return status_; }
        std::string  what()  const noexcept { return error_;  }
        
    private:
        bool status_;
        std::string error_; 
                           
    };
    
    Database() = default;
    ~Database() = default;

    executionResult execute(const std::string&);

private:
    static inline executionResult lastExecutionResult;
    
    enum class types {INT32, BOOL, STR, BYTES};
    
    struct cell {
        std::string cellName;
        types type;
        size_t size;
        
        bool unique;
        bool autoincrement;
        bool key;
    };
    
    class row {
    public:
        row(const std::vector <cell> &);
        row() = delete;
        
        ~row() {
            delete[] rowData;
        }
        
        char* getBegin() const noexcept { return rowData; }
        
    private:
        /* Основные (базовые) атрибуты строки конкретной таблицы */
        std::vector <cell> columnsDescription_;
        char* rowData = nullptr;
        
        /* Атрибуты, выводящиеся из базовых */
        size_t sizeOfRow;
        std::vector <size_t> cellStartAddress;
        std::map <std::string, size_t> cellNameToIndex;
        
    };
    
    std::map <std::string, row> baseTablesRows;
    std::map <std::string, std::vector <row> > tables;

    void createTable(const std::string&, const std::vector <cell> &, const std::vector <char*> &) noexcept;
    
};



} // memdb

#include "../src/database.cpp"

#endif // DATABASE
