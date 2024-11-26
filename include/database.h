// Названия могут быть длиной не больше (MAX_NAME_SIZE - 1)

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
        std::string error_; // возможно стоит переделать на char[const (исходя из размера максимальной ошибки)]
                            // и добавить noexcept для set-ов 
    };
    
    Database() = default;
    ~Database() = default;

    executionResult execute(std::string&);

private:
    static executionResult lastExecutionResult;
    
    static const size_t MAX_NAME_SIZE = 21;
    
    enum class types {INT32, BOOL, STR, BYTES};
    
    struct cell {
        char cellName[MAX_NAME_SIZE];
        types type;
        size_t size;
        
        bool unique;
        bool autoincrement;
        bool key;
    };
    
    class row {
    public:
        row(std::vector <cell> &);
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
        std::map <char[MAX_NAME_SIZE], size_t> cellNameToIndex;
        
    };
    
    std::map <char[MAX_NAME_SIZE], row> baseTablesRows;
    std::map <char[MAX_NAME_SIZE], std::vector <row> > tables;

    void createTable(char[MAX_NAME_SIZE], std::vector <cell> &, std::vector <char*> &) noexcept;
    
};



} // memdb

#include "../src/database.cpp"

#endif // DATABASE
