namespace memdb {



size_t Database::getSize(std::stringstream& request) const noexcept {
    std::string word;
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return 0;
    }
    if (word != "[") {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return 0;
    }
    
    int size;
    if (!(request >> size)) {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return 0;
    }
    
    if (!(request >> word)) {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return 0;
    }
    if (word != "]") {
        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
        return 0;
    }
    
    return size + 1;
}

char* Database::getValue(std::stringstream& request, types type, size_t size) const noexcept {
    char* ret;
    
    if (type == types::INT32) {
        int value;
        if (!(request >> value)) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
        }
        
        try {
            ret = new char[4];
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
        }
        
        int* ret_intptr = reinterpret_cast <int*> (ret);
        *(ret_intptr) = value;
    }
    else if (type == types::BOOL) {
        std::string value;
        if (!(request >> value)) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
        }
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return std::tolower(c); });
        
        try {
            ret = new char[1];
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
        }
        
        if (value == "true") {
            bool* ret_boolptr = reinterpret_cast <bool*> (ret);
            *(ret_boolptr) = true;
        }
        else if (value == "false") {
            bool* ret_boolptr = reinterpret_cast <bool*> (ret);
            *(ret_boolptr) = false;
        }
        else {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return ret;
        }
    }
    else if (type == types::STR) {
        std::string value = "";
        
        std::string word;
        if (!(request >> word)) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
        }
        if (word != "\"") {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
        }
        
        if (!(request >> word)) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
        }
        while (word != "\"") {
            value += word;

            if (!(request >> word)) {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return nullptr;
            }
        }
        
        try {
            ret = new char[size];
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
        }
        
        try {
            size = std::min(size, value.size() + 1);
            memcpy(ret, value.data(), size - 1);
            *(ret + (size - 1)) = '\0';
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return ret;
        }
    }
    else if (type == types::BYTES) {
        std::string value = "";
        
        std::string word;
        if (!(request >> word)) {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
        }
        if (word == "\"") {
            if (!(request >> word)) {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return nullptr;
            }
            while (word != "\"") {
                value += word;

                if (!(request >> word)) {
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return nullptr;
                }
            }

            try {
                ret = new char[size];
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return nullptr;
            }

            try {
                size = std::min(size, value.size() + 1);
                memcpy(ret, value.data(), size - 1);
                *(ret + (size - 1)) = '\0';
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return ret;
            }
        }
        else if (word.size() >= 2 && word[0] == '0' && word[1] == 'x') {
            try {
                ret = new char[size];
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return nullptr;
            }
            
            *(ret + (size - 1)) = '\0';
            
            for (size_t i{0}; i < 2 * (size - 1) - (word.size() - 2); ++i) {
                try {
                    word += '0';
                }
                catch (...) {
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return ret;
                }
            }
            
            for (size_t i{0}; i < size-1; ++i) {
                std::string byte_str = "";
                byte_str = word[i] + word[i+1];
                char byte_char;
                std::stringstream converter;
                
                if (!(converter << std::hex << byte_str)) {
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return ret;
                }
                if (!(converter >> byte_char)) {
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return ret;
                }
                
                *(ret + i) = byte_char;
            }
        }
        else {
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
        }
    }
    
    return ret;
}

void Database::getValues(std::stringstream& streamValues, std::vector <char*> &values, const row* const baseRow) const noexcept {
    std::stringstream copy_streamValues;
    for (std::string word = ""; streamValues >> word; copy_streamValues << word);

    bool secondTypeList = 0;
    for (std::string word = ""; copy_streamValues >> word; ) {
        if (word == "=") {
            secondTypeList = 1;
            break;
        }
    }
    
    if (secondTypeList == 0) {
        size_t i{0};
        for (std::string word = ""; streamValues >> word; ++i) {
            std::stringstream section;
            size_t sizeSection = 0;
            while (word != ",") {
                section << word;

                if (!(streamValues >> word)) {
                    lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                    return;
                }
                
                ++sizeSection;
            }
            
            cell newCell = baseRow->getCell(i);
            if (lastExecutionResult.is_ok() == false) return; 
            
            if (sizeSection) {
                char* value = getValue(section, newCell.type, newCell.size);
                
                try {
                    values.push_back(value);
                } 
                catch (...) {
                    delete[] value;
                    lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                    return;
                }
                
                if (lastExecutionResult.is_ok() == false) return; 
            }
            else {
                try {
                    values.push_back(nullptr);
                } 
                catch (...) {
                    lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                    return;
                }
            }
        }
    }
    else {
        for (std::string word = ""; streamValues >> word; ) {
            cell newCell = baseRow->getCell(word);
            if (lastExecutionResult.is_ok() == false) return; 
            
            if (!(streamValues >> word)) {
                lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                return;
            }
            
            if (word == "=") {
                lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                return;
            }
            
            char* value = getValue(streamValues, newCell.type, newCell.size);
            
            try {
                values.push_back(value);
            } 
            catch (...) {
                delete[] value;
                lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                return;
            }
            
            if (lastExecutionResult.is_ok() == false) return; 
        }
    }
}



}; // memdb
