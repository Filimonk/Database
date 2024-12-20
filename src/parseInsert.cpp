namespace memdb {



void Database::parseInsert(std::stringstream& request,
                           std::string& nameTable,
                           std::vector <char*> &values) const noexcept {
    
    std::string word;
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
        return;
    }
    std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
    if (word != "(") {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
        return;
    }
    
    std::stringstream streamValues; // отправляется в функцию getValues и должна содержать
                                    // данные от ( до ) не включительно + ',' в конце
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
        return;
    }
    while (word != ")") {
        try {
            if (!(streamValues << " " << word << " ")) {
                throw;
            }
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"Failed in adding the word to streamValues in the insert request"});
            return;
        }

        if (!(request >> word)) {
            lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
            return;
        }   
    }
    
    try {
        if (!(streamValues << " , ")) {
            throw;
        }
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed in adding the word to streamValues in the insert request"});
        return;
    }
    
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
        return;
    }
    std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
    if (word != "to") {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
        return;
    }
    
    if (!(request >> nameTable)) {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
        return;
    }
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
        return;
    }
    if (word != ";") {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
        return;
    }
    
    auto it = baseTablesRows.find(nameTable);
    if (it == baseTablesRows.end()) {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect: the table wasn't found"});
        return;
    }
    row* baseRow = (*it).second;
    
    getValues(streamValues, values, baseRow);
}



} // memdb
