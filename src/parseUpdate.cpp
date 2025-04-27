namespace memdb {



void Database::parseUpdate(std::stringstream& request,
                           std::string& nameTable,
                           std::vector <char*> &values,
                           condition*& conditionUpdate) const noexcept {
    
    std::string word;
    
    if (!(request >> nameTable)) {
        lastExecutionResult.setStatus(std::string{"The update request is incorrect"});
        return;
    }
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The update request is incorrect"});
        return;
    }
    std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
    if (word != "set") {
        lastExecutionResult.setStatus(std::string{"The update request is incorrect"});
        return;
    }
    
    std::stringstream streamValues; // отправляется в функцию getValues и должна содержать
                                    // данные от ( до ) не включительно + ',' в конце
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The update request is incorrect"});
        return;
    }
    std::string word_cp = word;
    std::transform(word_cp.begin(), word_cp.end(), word_cp.begin(), [](unsigned char c) { return std::tolower(c); });
    while (word_cp != "where") {
        try {
            if (!(streamValues << " " << word << " ")) {
                throw;
            }
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"Failed in adding the word to streamValues in the update request"});
            return;
        }

        if (!(request >> word)) {
            lastExecutionResult.setStatus(std::string{"The update request is incorrect"});
            return;
        }   
        word_cp = word;
        std::transform(word_cp.begin(), word_cp.end(), word_cp.begin(), [](unsigned char c) { return std::tolower(c); });
    }
    
    try {
        if (!(streamValues << " , ")) {
            throw;
        }
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed in adding the word to streamValues in the update request"});
        return;
    }
    
    
    std::vector <std::string> expression;
    for (; request >> word && word != ";"; ) {
        try {
            expression.push_back(word);
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The update request is incorrect"});
            return;
        }
    }
    
    if (word != ";") {
        lastExecutionResult.setStatus(std::string{"The update request is incorrect"});
        return;
    }
    
    conditionUpdate = getCondition(expression, 0, expression.size());
    
    
    auto it = baseTablesRows.find(nameTable);
    if (it == baseTablesRows.end()) {
        lastExecutionResult.setStatus(std::string{"The update request is incorrect: the table wasn't found"});
        return;
    }
    row* baseRow = (*it).second;
    
    getValues(streamValues, values, baseRow);
}



} // memdb
