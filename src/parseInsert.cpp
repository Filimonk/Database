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
    
    std::stringstream streamValues;
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
        return;
    }
    while (word != ")") {
        streamValues << word;

        if (!(request >> word)) {
            lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
            return;
        }   
    }
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
        return;
    }
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
    row* baseRow = (*it).second;
    
    getValues(streamValues, values, baseRow);
}



} // memdb
