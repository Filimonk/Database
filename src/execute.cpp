namespace memdb {



void findAndBorderAll(std::string& input, const std::string& replaceWord) {
    std::string replaceBy = " " + replaceWord + " ";
    
    size_t pos = input.find(replaceWord);
    
    while (pos != std::string::npos) {
        //if ((replaceWord != "|" || *(pos + 1) != '|')) {
            input.replace(pos, replaceWord.size(), replaceBy);
        //}

        pos = input.find(replaceWord, pos + replaceBy.size());
    }
}

std::stringstream splittingRequests(const std::string& query) {
    std::stringstream tempRequests{query};
    std::stringstream splitRequests;

    bool first = 1;
    for (std::string word; tempRequests >> word; ) {
        std::string word_cp = word;
        std::transform(word_cp.begin(), word_cp.end(), word_cp.begin(), [](unsigned char c) { return std::tolower(c); });
        if (first == 0) {
            if (word_cp == "create" ||
                word_cp == "insert" ||
                word_cp == "select" ||
                word_cp == "update" ||
                word_cp == "delete") {

                try {
                    splitRequests << ";";
                }
                catch (...) {
                    throw;
                }
            }
            
            try {
                splitRequests << word;
            }
            catch (...) {
                throw;
            }
        }
    }
    
    return splitRequests;
}

Database::executionResult Database::execute(std::string query) noexcept {
    lastExecutionResult.setStatus(true);
    
    try {
        std::vector <std::string> separators = { "(", ")", ",", "\"", "=", "{", "}", "[", "]", ":",
                                                 "+", "-", "*", "/", "%", "<", ">", "<=", ">=",
                                                 "!=", "&&", "||", "!", "^^", "|" };
        for (auto separator: separators) {
            findAndBorderAll(query, separator);
        }
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed in the division into tokens"});
        return lastExecutionResult;
    }
    
    std::stringstream requests;
    try {
        requests = splittingRequests(query);
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed in the splitting into requests"});
        return lastExecutionResult;
    }
    
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
        else if (word == "select") {
            std::string nameTable;
            std::vector <std::string> columnsSelect;
            condition* conditionSelect = nullptr;
            
            parseSelect(requests, nameTable, columnsSelect, conditionSelect);
            if (lastExecutionResult.is_ok() == false) { 
                delete conditionSelect;
                return lastExecutionResult;
            }
            
            select(nameTable, columnsSelect, conditionSelect);
            if (lastExecutionResult.is_ok() == false) { 
                delete conditionSelect;
                return lastExecutionResult;
            }
            
            delete conditionSelect;
        }
        else {
            lastExecutionResult.setStatus(std::string{"The command is not recognized"});
            return lastExecutionResult;
        }
    }
    
    return lastExecutionResult;
}



} // memdb
