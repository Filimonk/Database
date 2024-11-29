namespace memdb {



void Database::parseCreate(std::stringstream& request,
                           std::string& nameTable,
                           std::vector <cell> &columnsDescription,
                           std::vector <char*> &defaultValues) const noexcept {
    
    std::string word;
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return;
    }
    std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
    if (word != "table") {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return;
    }
    
    if (!(request >> nameTable)) {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return;
    }
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return;
    }
    if (word != "(") {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return;
    }
    
    cell newCell;
    
    int descriptionConstructionCounter = 0;
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return;
    }
    std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
    while (word != ")") {
        if (word == "{" && descriptionConstructionCounter == 0) {
            if (!(request >> word)) {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return;
            }
            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
            
            int newSection = 1;
            while (word != "}") {
                if (word == "unique" && newSection == 1) {
                    newCell.unique = true;
                }
                else if (word == "autoincrement" && newSection == 1) {
                    newCell.autoincrement = true;
                }
                else if (word == "key" && newSection == 1) {
                    newCell.key = true;
                }
                else if (word == "," && newSection == 0) {}
                else {
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return;
                }
                
                newSection = (newSection + 1) % 2;
                
                if (!(request >> word)) {
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return;
                }
                std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
            }
            
            if (!(request >> word)) {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return;
            }
            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
            descriptionConstructionCounter = 1;
        }
        else if (descriptionConstructionCounter == 0) {
            descriptionConstructionCounter = 1;
        }
        else if (word == "{") {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return;
        }
        
        if (descriptionConstructionCounter == 1) {
            newCell.name = word;
            newCell.sizeName = word.size();
        }
        else if (descriptionConstructionCounter == 2) {
            if (word != ":") {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return;
            }
        }
        else if (descriptionConstructionCounter == 3) {
            if (word == "int32") {
                newCell.type = types::INT32;
                newCell.size = 4;
            }
            else if (word == "bool") {
                newCell.type = types::BOOL;
                newCell.size = 1;
            }
            else if (word == "string") {
                newCell.type = types::STR;
                newCell.size = getSize(request);
                if (lastExecutionResult.is_ok() == false) return;
            }
            else if (word == "bytes") {
                newCell.type = types::BYTES;
                newCell.size = getSize(request);
                if (lastExecutionResult.is_ok() == false) return;
            }
            else {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return;
            }
        }
        else if (descriptionConstructionCounter == 4) {
            if (word == "=") {
                char* value = getValue(request, newCell.type, newCell.size);
                
                try {
                    defaultValues.push_back(value);
                } 
                catch (...) {
                    delete[] value;
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return;
                }
                
                if (lastExecutionResult.is_ok() == false) return; 
                
                if (!(request >> word)) {
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return;
                }
                std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
            }
            else {
                try {
                    defaultValues.push_back(nullptr);
                } 
                catch (...) {
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return;
                }
            }
            
            if (word == ")") {
                break;
            }
            
            descriptionConstructionCounter = -1; 
            
            if (word != ",") {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return;
            }
        }
        
        if (!(request >> word)) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return;
        }
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
        ++descriptionConstructionCounter;
    }
    
    if (descriptionConstructionCounter == 4) {
        try {
            columnsDescription.push_back(newCell);
        } 
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return;
        }
    }
    else {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return;
    }
}



} // memdb
