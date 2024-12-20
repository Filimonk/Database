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
            delete[] ret;
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
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
            if (value.size() != 0) {
                value += " ";
            }
            
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
            delete[] ret;
            lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
            return nullptr;
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
                if (value.size() != 0) {
                    value += " ";
                }
            
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
                delete[] ret;
                lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                return nullptr;
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
                    delete[] ret;
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return nullptr;
                }
            }
            
            for (size_t i{0}; i < size-1; ++i) {
                std::string byte_str = "";
                byte_str = word[i] + word[i+1];
                char byte_char;
                std::stringstream converter;
                
                try {
                    if (!(converter << std::hex << byte_str)) {
                        delete[] ret;
                        lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                        return nullptr;
                    }
                }
                catch (...) {
                    delete[] ret;
                    lastExecutionResult.setStatus(std::string{"Failed in converting the byte in the create request"});
                    return nullptr;
                }
                
                if (!(converter >> byte_char)) {
                    delete[] ret;
                    lastExecutionResult.setStatus(std::string{"The creation request is incorrect"});
                    return nullptr;
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
    try {
        values.assign(baseRow->getNumberOfCells(), nullptr);
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed to allocate dynamic memory for vector of values"});
        return;
    }
    
    std::stringstream copy_streamValues;
    for (std::string word = ""; streamValues >> word; copy_streamValues << " " << word << " ");
    
    streamValues.clear();
    streamValues.seekg(0);
    streamValues.seekp(0);

    bool secondTypeList = 0;
    for (std::string word = ""; copy_streamValues >> word; ) {
        if (word == "=") {
            secondTypeList = 1;
        }
        
        try {
            if (!(streamValues << " " << word << " ")) {
                throw;
            }
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"Failed in adding the word to streamValues in the insert request"});
            return;
        }
    }
    
    if (secondTypeList == 0) {
        size_t i{0};
        for (std::string word = ""; streamValues >> word; ++i) {
            std::stringstream section;
            size_t sizeSection = 0;
            while (word != ",") {
                section << " " << word << " ";

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
                if (lastExecutionResult.is_ok() == false) {
                    return;
                }
                
                if (i < values.size() && values[i] == nullptr) {
                    values[i] = value;
                }
                else {
                    delete value;
                    lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                    return;
                }
            }
        }
    }
    else {
        for (std::string word = ""; streamValues >> word; ) {
            std::string nameCell;
            try {
                nameCell = word;
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"Cannot get a name"});
                return;
            }
            
            cell newCell = baseRow->getCell(nameCell);
            
            if (lastExecutionResult.is_ok() == false) return; 
            
            if (!(streamValues >> word)) {
                lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                return;
            }
            
            if (word != "=") {
                lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                return;
            }
            
            size_t indexCellInRow = baseRow->getIndexByCellName(nameCell);
            if (lastExecutionResult.is_ok() == false) { return; }
            
            char* value = getValue(streamValues, newCell.type, newCell.size);
            if (lastExecutionResult.is_ok() == false) { return; }
            
            if (indexCellInRow < values.size() && values[indexCellInRow] == nullptr) {
                values[indexCellInRow] = value;
            }
            else {
                delete value;
                lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                return;
            }
            
            if (!(streamValues >> word)) {
                lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                return;
            }
            if (word != ",") {
                lastExecutionResult.setStatus(std::string{"The insert request is incorrect"});
                return;
            }
        }
    }
}

Database::condition* Database::getCondition(std::vector <std::string> &expression, size_t left, size_t right) const noexcept {
    if (left == right) {
        return nullptr;
    }
    
    condition* result;
    try {
         result = new condition;
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        return nullptr;
    }
    
    int balansOfRoundsBrackets = 0;
    int balansOfDirectBrackets = 0;
    
    for (auto operation: operations) {
        for (size_t i{left}; i < right; ++i) {
            if (expression[i] == "(") {
                ++balansOfRoundsBrackets;
            }
            if (expression[i] == ")") {
                --balansOfRoundsBrackets;
            }
            
            if (expression[i] == "|") {
                balansOfDirectBrackets = (balansOfDirectBrackets + 1) % 2;
            }
            
            
            if (expression[i] == operation && balansOfRoundsBrackets == 0 && balansOfDirectBrackets == 0) {
                result->setOperator(operation);
                if (lastExecutionResult.is_ok() == false) {
                    delete result;
                    return nullptr;
                }
                    
                if (operation == "!") {
                    if (i != left) {
                        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                        delete result;
                        return nullptr;
                    }
                    
                    condition* child = getCondition(expression, left + 1, right);
                    if (lastExecutionResult.is_ok() == false) {
                        delete result;
                        return nullptr;
                    }
                    
                    result->setRight(child);
                    if (lastExecutionResult.is_ok() == false) {
                        delete result;
                        return nullptr;
                    }
                    
                    return result;
                }
                else {
                    condition* leftChild = getCondition(expression, left, i);
                    if (lastExecutionResult.is_ok() == false) {
                        delete result;
                        return nullptr;
                    }
                    
                    condition* rightChild = getCondition(expression, i + 1, right);
                    if (lastExecutionResult.is_ok() == false) {
                        delete result;
                        return nullptr;
                    }
                    
                    
                    result->setLeft(leftChild);
                    if (lastExecutionResult.is_ok() == false) {
                        delete result;
                        return nullptr;
                    }
                    
                    result->setRight(rightChild);
                    if (lastExecutionResult.is_ok() == false) {
                        delete result;
                        return nullptr;
                    }
                    
                    return result;
                }
            }
        }
    }
    
    if (expression[left] == "(") {
        if (expression[right - 1] != ")") { 
            lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
            delete result;
            return nullptr;
        }
        
        result->setOperator(expression[left]);   
        if (lastExecutionResult.is_ok() == false) {
            delete result;
            return nullptr;
        }
        
        condition* child = getCondition(expression, left + 1, right - 1);
        if (lastExecutionResult.is_ok() == false) {
            delete result;
            return nullptr;
        }
        
        result->setRight(child);
        if (lastExecutionResult.is_ok() == false) {
            delete result;
            return nullptr;
        }
        
        return result;
    }
    
    if (expression[left] == "|") {
        if (expression[right - 1] != "|") { 
            lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
            delete result;
            return nullptr;
        }
        
        result->setOperator(expression[left]);   
        if (lastExecutionResult.is_ok() == false) {
            delete result;
            return nullptr;
        }
        
        condition* child = getCondition(expression, left + 1, right - 1);
        if (lastExecutionResult.is_ok() == false) {
            delete result;
            return nullptr;
        }
        
        result->setRight(child);
        if (lastExecutionResult.is_ok() == false) {
            delete result;
            return nullptr;
        }
        
        return result;
    }
    
        
    std::stringstream value;
    
    if (expression[left] == "\"") {
        if (expression[right - 1] != "\"") {
            lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
            delete result;
            return nullptr;
        }
        
        size_t size{0};
        for (size_t i{left}; i < right; ++i) {
            try {
                value << " " << expression[i] << " ";
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                delete result;
                return nullptr;
            }

            if (i != left && i != right - 1) {
                size += expression[i].size();
            }
        }
        size += ((right - left - 1) - 1) + 1; // ((количество участков строки) - 1) = разрезов и +1 для \0
        
        char* value_ptr = getValue(value, types::STR, size);
        if (lastExecutionResult.is_ok() == false) {
            delete result;
            return nullptr;
        }
            
        cell* cellValue;
        try {
            cellValue = new cell;
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"Memory for the new cell was not allocated"});
            delete result;
            delete[] value_ptr;
            return nullptr;
        }
        cellValue->type = types::STR;
        cellValue->begin = value_ptr;
        cellValue->size = size;
        
        result->setConst(cellValue);
        /*
        if (lastExecutionResult.is_ok() == false) {
            delete result;
            delete[] value_ptr;
            delete cellValue;
            return nullptr;
        }
        */
    }
    else if (right - left == 1 && expression[left][0] == '0' && expression[left][1] == 'x') {
        try {
            value << expression[left];
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
            delete result;
            return nullptr;
        }
        
        size_t size{((expression[left].size() - 2) + 1) / 2 + 1}; // (количество символов) / 2 с округлением вверх = кол байт и +1 под \0
        
        char* value_ptr = getValue(value, types::BYTES, size);
        if (lastExecutionResult.is_ok() == false) {
            delete result;
            return nullptr;
        }
            
        cell* cellValue;
        try {
            cellValue = new cell;
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"Memory for the new cell was not allocated"});
            delete result;
            delete[] value_ptr;
            return nullptr;
        }
        cellValue->type = types::BYTES;
        cellValue->begin = value_ptr;
        cellValue->size = size;
        
        result->setConst(cellValue);
        /*
        if (lastExecutionResult.is_ok() == false) {
            delete result;
            delete[] value_ptr;
            delete cellValue;
            return nullptr;
        }
        */
    }
    else if (right - left == 1) {
        std::string &word = expression[left];
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
        if ((word.size() == 4 && word[0] == 't' && word[1] == 'r' && word[2] == 'u' && word[3] == 'e') ||
            (word.size() == 5 && word[0] == 'f' && word[1] == 'a' && word[2] == 'l' && word[3] == 's' &&  word[4] == 'e')) {
            value << expression[left];
            
            char* value_ptr = getValue(value, types::BOOL, 1);
            if (lastExecutionResult.is_ok() == false) {
                delete result;
                return nullptr;
            }
            
            cell* cellValue;
            try {
                cellValue = new cell;
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"Memory for the new cell was not allocated"});
                delete result;
                delete[] value_ptr;
                return nullptr;
            }
            cellValue->type = types::BOOL;
            cellValue->begin = value_ptr;
            cellValue->size = 1;
            
            result->setConst(cellValue);
            /*
            if (lastExecutionResult.is_ok() == false) {
                delete result;
                delete[] value_ptr;
                delete cellValue;
                return nullptr;
            }
            */
        }
        else {
            for (auto letter: word) {
                if (letter < '0' || letter > '9') {
                    result->setVariable(expression[left]);
                    if (lastExecutionResult.is_ok() == false) {
                        delete result;
                        return nullptr;
                    }
                    
                    return result;
                }
            }
            
            value << expression[left];
            
            char* value_ptr = getValue(value, types::INT32, 4);
            if (lastExecutionResult.is_ok() == false) {
                delete result;
                return nullptr;
            }
            
            cell* cellValue;
            try {
                cellValue = new cell;
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"Memory for the new cell was not allocated"});
                delete result;
                delete[] value_ptr;
                return nullptr;
            }
            cellValue->type = types::INT32;
            cellValue->begin = value_ptr;
            cellValue->size = 4;
            
            result->setConst(cellValue);
            /*
            if (lastExecutionResult.is_ok() == false) {
                delete result;
                delete[] value_ptr;
                delete cellValue;
                return nullptr;
            }
            */
        }
    }
    else {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        delete result;
        return nullptr;
    }
    
    return result;
}



}; // memdb
