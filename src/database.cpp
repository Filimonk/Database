namespace memdb {



void findAndBorderAll(std::string& input, const std::string& replaceWord) {
    std::string replaceBy = " " + replaceWord + " ";
    
    size_t pos = input.find(replaceWord);
    
    while (pos != std::string::npos) {
        input.replace(pos, replaceWord.size(), replaceBy);

        pos = input.find(replaceWord, pos + replaceBy.size());
    }
}
    
void Database::executionResult::set(bool status, const std::string& error) {
    status_ = status;
    error_ = error;
}

void Database::executionResult::set(const std::string& error) {
    status_ = false;
    error_ = error;
}
    
/*
Database::executionResult Database::execute(std::string query) {
    lastExecutionResult.set(true);
    
    std::vector <std::string> separators = {"(", ")", ",", "\"", "=", "0x", "{", "}", "[", "]", ":"};
    for (auto separator: separators) {
        findAndBorderAll(query, separator);
    }
    
    std::stringstream requests{query};
    for (std::string word = ""; requests >> word; ) {
        std::string copy_word = word;
        
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
        if (word == "create") {
            std::string nameTable;
            std::vector <cell> columnsDescription;
            std::vector <char*> defaultValues;

            std::vector <std::string> keywords = {"table", }
            while (word != ")") {
                
            }
        }
    }
    
    return lastExecutionResult;
}
*/

Database::row::row(const std::vector <cell> &columnsDescription) {
    columnsDescription_ = columnsDescription;
    sizeOfRow = 0;
    
    for (size_t i{0}; i < columnsDescription.size(); ++i) {
        try {
            cellStartAddress.push_back(sizeOfRow);
            cellNameToIndex[columnsDescription[i].cellName] = i; 
        }
        catch (...) {
            lastExecutionResult.set("Failed adding a column to a row vector or map");
            return;
        }

        sizeOfRow += columnsDescription[i].size;
    }
    
    try {
        rowData = new char[sizeOfRow];
    }
    catch (...) {
        lastExecutionResult.set("Failed to allocate dynamic memory");
    }
}

Database::row::row(const row& other) {
    try {
        rowData = new char[other.sizeOfRow];
        memcpy(rowData, other.rowData, other.sizeOfRow);
        
        columnsDescription_ = other.columnsDescription_;
        sizeOfRow = other.sizeOfRow;
        cellStartAddress = other.cellStartAddress;
        cellNameToIndex = other.cellNameToIndex;
    }
    catch (...) {
        lastExecutionResult.set("Failed allocation mem for new row");
    }
}

void Database::createTable(const std::string& nameTable, 
                           const std::vector <cell> &columnsDescription,
                           const std::vector <char*> &defaultValues) noexcept {
    
    row baseRow{columnsDescription};
    
    if (lastExecutionResult.is_ok() == false) return;

    char* rowBegin = baseRow.getBegin();
    for (size_t i{0}, shift{0}; i < columnsDescription.size(); ++i) {
        if (defaultValues[i] != nullptr) {
            char* sectionBegin = rowBegin + shift;
            try {
                memcpy(sectionBegin, defaultValues[i], columnsDescription[i].size);
            }
            catch (...) {
                lastExecutionResult.set("Failed setting default value");
                return;
            }
        }
    }

    try {
        baseTablesRows.insert({nameTable, baseRow});
    }
    catch (...) {
        lastExecutionResult.set("Failed adding a base table row to a vector");
        return;
    }
    
    try {
        tables[nameTable] = {};
    }
    catch (...) {
        baseTablesRows.erase(nameTable);
        lastExecutionResult.set("Failed adding a table to a map");
    }
}

void Database::insert(const std::string& nameTable, const std::vector <char*> &values) noexcept {
    auto it = baseTablesRows.find(nameTable);
    row newRow = (*it).second;
    
    if (lastExecutionResult.is_ok() == false) return;

    char* rowBegin = newRow.getBegin();
    for (size_t i{0}, shift{0}; i < values.size(); ++i) {
        if (values[i] != nullptr) {
            char* sectionBegin = rowBegin + shift;
            try {
                memcpy(sectionBegin, values[i], newRow.columnsDescription_[i].size);
            }
            catch (...) {
                lastExecutionResult.set("Failed setting value");
                return;
            }
        }
    }
    
    try {
        //tables[nameTable].push_back(newRow);
    }
    catch (...) {
        lastExecutionResult.set("Failed adding a row to a rows vector of table");
    }
}

void Database::select(const std::string& nameTable, const std::vector <char*> &values) noexcept {
    auto it = baseTablesRows.find(nameTable);
    row newRow = (*it).second;
    
    if (lastExecutionResult.is_ok() == false) return;

    char* rowBegin = newRow.getBegin();
    for (size_t i{0}, shift{0}; i < values.size(); ++i) {
        if (values[i] != nullptr) {
            char* sectionBegin = rowBegin + shift;
            try {
                memcpy(sectionBegin, values[i], newRow.columnsDescription_[i].size);
            }
            catch (...) {
                lastExecutionResult.set("Failed setting value");
                return;
            }
        }
    }
    
    try {
        //tables[nameTable].push_back(newRow);
    }
    catch (...) {
        lastExecutionResult.set("Failed adding a row to a rows vector of table");
    }
}

void Database::update(const std::string& nameTable, const std::vector <char*> &values) noexcept {
    auto it = baseTablesRows.find(nameTable);
    row newRow = (*it).second;
    
    if (lastExecutionResult.is_ok() == false) return;

    char* rowBegin = newRow.getBegin();
    for (size_t i{0}, shift{0}; i < values.size(); ++i) {
        if (values[i] != nullptr) {
            char* sectionBegin = rowBegin + shift;
            try {
                memcpy(sectionBegin, values[i], newRow.columnsDescription_[i].size);
            }
            catch (...) {
                lastExecutionResult.set("Failed setting value");
                return;
            }
        }
    }
    
    try {
        //tables[nameTable].push_back(newRow);
    }
    catch (...) {
        lastExecutionResult.set("Failed adding a row to a rows vector of table");
    }
}

void Database::deleteRows(const std::string& nameTable, const std::vector <char*> &values) noexcept {
    auto it = baseTablesRows.find(nameTable);
    row newRow = (*it).second;
    
    if (lastExecutionResult.is_ok() == false) return;

    char* rowBegin = newRow.getBegin();
    for (size_t i{0}, shift{0}; i < values.size(); ++i) {
        if (values[i] != nullptr) {
            char* sectionBegin = rowBegin + shift;
            try {
                memcpy(sectionBegin, values[i], newRow.columnsDescription_[i].size);
            }
            catch (...) {
                lastExecutionResult.set("Failed setting value");
                return;
            }
        }
    }
    
    try {
        //tables[nameTable].push_back(newRow);
    }
    catch (...) {
        lastExecutionResult.set("Failed adding a row to a rows vector of table");
    }
}



} // memdb
