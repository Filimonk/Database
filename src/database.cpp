namespace memdb {



void Database::executionResult::set(bool status, const std::string& error) {
    status_ = status;
    error_ = error;
}

void Database::executionResult::set(const std::string& error) {
    status_ = false;
    error_ = error;
}
    
Database::executionResult Database::execute(std::string& query) {
    lastExecutionResult.set(true);
    
    return lastExecutionResult;
}

Database::row::row(std::vector <cell> &columnsDescription) : columnsDescription_{columnsDescription} {
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

void Database::createTable(char nameTable[MAX_NAME_SIZE], std::vector <cell> &columnsDescription,
                           std::vector <char*> &defaultValues) noexcept {
    
    row baseRow{columnsDescription};
    
    if (lastExecutionResult.is_ok() == false) return;

    char* rowBegin = baseRow.getBegin();
    for (size_t i{0}, shift{0}; i < columnsDescription.size(); ++i) {
        if (defaultValues[i] != nullptr) {
            /*
            if (columnsDescription[i].type == INT32) {
                int* sectionBegin = reinterpret_cast <int*> (rowBegin + shift);
            }
            else if (columnsDescription[i].type == BOOL) {
                bool* sectionBegin = reinterpret_cast <bool*> (rowBegin + shift);
            }
            else if (columnsDescription[i].type == STR) {
                char* sectionBegin = reinterpret_cast <char*> (rowBegin + shift);
            }
            else if (columnsDescription[i].type == BYTES) {
                char* sectionBegin = reinterpret_cast <char*> (rowBegin + shift);
            }
            */
            
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
        baseTablesRows[nameTable] = baseRow;
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



} // memdb
