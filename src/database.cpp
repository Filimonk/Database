namespace memdb {



void findAndBorderAll(std::string& input, const std::string& replaceWord) {
    std::string replaceBy = " " + replaceWord + " ";
    
    size_t pos = input.find(replaceWord);
    
    while (pos != std::string::npos) {
        input.replace(pos, replaceWord.size(), replaceBy);

        pos = input.find(replaceWord, pos + replaceBy.size());
    }
}


Database::~Database() {
    for (auto& pair_string_rowptr: baseTablesRows) {
        delete pair_string_rowptr.second;
    }
    
    for (auto& pair_string_vec: tables) {
        std::vector <row*> &rows_ptrs = pair_string_vec.second;
        for (auto row_ptr: rows_ptrs) {
            delete row_ptr;
        }
    }
}


void Database::executionResult::setStatus(bool status, const std::string& error) {
    status_ = status;
    error_ = error;
}

void Database::executionResult::setStatus(const std::string& error) {
    status_ = false;
    error_ = error;
}
    

Database::row::row(const std::vector <cell> &columnsDescription) {
    columnsDescription_ = columnsDescription;
    sizeOfRow = 0;
    
    for (size_t i{0}; i < columnsDescription.size(); ++i) {
        try {
            cellNameToIndex[columnsDescription[i].name] = i; 
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"Failed adding a column to a row vector or map"});
            return;
        }

        sizeOfRow += columnsDescription[i].size;
    }
    
    try {
        rowData = new char[sizeOfRow];
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed to allocate dynamic memory"});
        return;
    }
    
    for (size_t i{0}, shift{0}; i < columnsDescription.size(); ++i) {
        columnsDescription_[i].begin = rowData + shift;
        shift += columnsDescription_[i].size;
    }
}

Database::row::row(const row* other) {
    try {
        rowData = new char[other->sizeOfRow];
        memcpy(rowData, other->rowData, other->sizeOfRow);
        
        columnsDescription_ = other->columnsDescription_;
        sizeOfRow = other->sizeOfRow;
        cellNameToIndex = other->cellNameToIndex;
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed allocation mem for new row"});
    }
    
    for (size_t i{0}, shift{0}; i < columnsDescription_.size(); ++i) {
        columnsDescription_[i].begin = rowData + shift;
        shift += columnsDescription_[i].size;
    }
}

Database::cell Database::row::getCell(const size_t index) const noexcept {
    if (index < columnsDescription_.size()) {
        return columnsDescription_[index];
    }
    else {
        lastExecutionResult.setStatus(std::string{"An invalid cell is specified to receive"});
        return cell{};
    }
}

Database::cell Database::row::getCell(const std::string& nameCell) const noexcept {
    size_t index;

    try {
        auto it = cellNameToIndex.find(nameCell);
        if (it != cellNameToIndex.end()) {
            index = (*it).second;
        }
        else {
            lastExecutionResult.setStatus(std::string{"An invalid cell is specified to receive"});
            return cell{};
        }
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed to match the cell name to the index"});
        return cell{};
    }
    
    if (index < columnsDescription_.size()) {
        return columnsDescription_[index];
    }
    else {
        lastExecutionResult.setStatus(std::string{"An invalid cell is specified to receive"});
        return cell{};
    }
}


Database::executionResult Database::execute(std::string query) {
    lastExecutionResult.setStatus(true);
    
    try {
        std::vector <std::string> separators = {"(", ")", ",", "\"", "=", "{", "}", "[", "]", ":"};
        for (auto separator: separators) {
            findAndBorderAll(query, separator);
        }
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed in the division into tokens"});
        return lastExecutionResult;
    }
    
    std::stringstream requests{query};
    for (std::string word = ""; requests >> word; ) {
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
        if (word == "create") {
            std::string nameTable;
            std::vector <cell> columnsDescription;
            std::vector <char*> defaultValues;
            
            parseCreate(requests, nameTable, columnsDescription, defaultValues);
            if (lastExecutionResult.is_ok() == false) {
                for (size_t i{0}; i < defaultValues.size(); ++i) {
                    delete[] defaultValues[i];
                }
                return lastExecutionResult;
            }
            
            createTable(nameTable, columnsDescription, defaultValues);
            
            for (size_t i{0}; i < defaultValues.size(); ++i) {
                delete[] defaultValues[i];
            }
        }
        else if (word == "insert") {
            std::string nameTable;
            std::vector <char*> values;
            
            parseInsert(requests, nameTable, values);
            if (lastExecutionResult.is_ok() == false) {
                for (size_t i{0}; i < values.size(); ++i) {
                    delete[] values[i];
                }
                return lastExecutionResult;
            }
            
            insert(nameTable, values);
            
            for (size_t i{0}; i < values.size(); ++i) {
                delete[] values[i];
            }

        }
        else {
            lastExecutionResult.setStatus(std::string{"The command is not recognized"});
            return lastExecutionResult;
        }
    }
    
    return lastExecutionResult;
}

void Database::createTable(const std::string& nameTable, 
                           const std::vector <cell> &columnsDescription,
                           const std::vector <char*> &defaultValues) noexcept {
    
    row* baseRow = new row{columnsDescription};
    
    if (lastExecutionResult.is_ok() == false) return;

    for (size_t i{0}; i < columnsDescription.size(); ++i) {
        if (defaultValues[i] != nullptr) {
            try {
                memcpy((baseRow->getCell(i)).begin, defaultValues[i], (baseRow->getCell(i)).size);
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"Failed setting default value"});
                return;
            }
        }
    }

    try {
        baseTablesRows[nameTable] = baseRow;
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed adding a base table row to a vector"});
        return;
    }
    
    try {
        tables[nameTable] = {};
    }
    catch (...) {
        baseTablesRows.erase(nameTable);
        lastExecutionResult.setStatus(std::string{"Failed adding a table to a map"});
    }
}

void Database::insert(const std::string& nameTable, const std::vector <char*> &values) noexcept {
    row* newRow = new row{baseTablesRows[nameTable]};
    
    if (lastExecutionResult.is_ok() == false) return;

    for (size_t i{0}; i < values.size(); ++i) {
        if (values[i] != nullptr) {
            try {
                memcpy((newRow->getCell(i)).begin, values[i], (newRow->getCell(i)).size);
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"Failed setting value"});
                return;
            }
        }
    }
    
    try {
        tables[nameTable].push_back(newRow);
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed adding a row to a rows vector of table"});
    }
}



} // memdb
