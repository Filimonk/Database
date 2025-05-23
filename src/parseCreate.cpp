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
            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
            
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
                
                if (lastExecutionResult.is_ok() == false) return; 
                
                try {
                    defaultValues.push_back(value);
                } 
                catch (...) {
                    delete[] value;
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return;
                }
                
                if (!(request >> word)) {
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return;
                }
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
            
            try {
                columnsDescription.push_back(newCell);
            } 
            catch (...) {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return;
            }
            
            if (word == ")") {
                break;
            }
            
            descriptionConstructionCounter = -1; 
            
            newCell.unique = false;
            newCell.autoincrement = false;
            newCell.key = false;
            
            if (word != ",") {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return;
            }
        }
        else {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return;
        }
        
        if (!(request >> word)) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return;
        }
        ++descriptionConstructionCounter;
    }
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return;
    }
    if (word != ";") {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return;
    }
    
    if (descriptionConstructionCounter != 4) {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return;
    }
}



} // memdb
