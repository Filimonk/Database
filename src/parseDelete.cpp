namespace memdb {



void Database::parseDelete(std::stringstream& request,
                           std::string& nameTable,
                           condition*& conditionSelect) const noexcept {
    
    std::string word;
    
    if (!(request >> nameTable)) {
        lastExecutionResult.setStatus(std::string{"The delete request is incorrect"});
        return;
    }
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The delete request is incorrect"});
        return;
    }
    std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
    if (word != "where") {
        lastExecutionResult.setStatus(std::string{"The delete request is incorrect"});
        return;
    }
    
    std::vector <std::string> expression;
    for (; request >> word && word != ";"; ) {
        try {
            expression.push_back(word);
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The delete request is incorrect"});
            return;
        }
    }
    
    if (word != ";") {
        lastExecutionResult.setStatus(std::string{"The delete request is incorrect"});
        return;
    }
    
    conditionSelect = getCondition(expression, 0, expression.size());
}



} // memdb
