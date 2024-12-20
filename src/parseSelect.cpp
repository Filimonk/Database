namespace memdb {



void Database::parseSelect(std::stringstream& request,
                           std::string& nameTable,
                           std::vector <std::string> &columnsSelect,
                           condition*& conditionSelect) const noexcept {
    
    std::string word;
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        return;
    }
    
    std::string word_cp;
    try {
        word_cp = word;
        std::transform(word_cp.begin(), word_cp.end(), word_cp.begin(), [](unsigned char c) { return std::tolower(c); });
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        return;
    }
    
    if (word_cp == "from") {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect: there are no columns to select"});
        return;
    }
    
    try {
        columnsSelect.push_back(word);
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed pushing the word to selecting vector"});
        return;
    }
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        return;
    }
    
    while (word == ",") {
        if (!(request >> word)) {
            lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
            return;
        }
        
        try {
            columnsSelect.push_back(word);
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"Failed pushing the word to selecting vector"});
            return;
        }
        
        if (!(request >> word)) {
            lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
            return;
        }
        
        try {
            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
            return;
        }
    }
    
    if (word != "from") {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        return;
    }
    
    if (!(request >> nameTable)) {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        return;
    }
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        return;
    }
    std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
    if (word != "where") {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        return;
    }
    
    std::vector <std::string> expression;
    for (; request >> word && word != ";"; ) {
        try {
            expression.push_back(word);
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
            return;
        }
    }
    
    conditionSelect = getCondition(expression, 0, expression.size());
}



} // memdb
