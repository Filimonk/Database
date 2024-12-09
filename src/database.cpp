namespace memdb {



Database::~Database() {
    for (auto& pair_string_rowptr: baseTablesRows) {
        delete pair_string_rowptr.second;
    }
    
    for (auto& pair_string_vec: tables) {
        std::vector <row*> &rows_ptrs = pair_string_vec.second;
        for (auto row_ptr: rows_ptrs) {
            delete row_ptr;
        }
    }
}


void Database::executionResult::setStatus(bool status, const std::string& error) noexcept {
    status_ = status;
    try {
        error_ = error;
    }
    catch (...) {}
}

void Database::executionResult::setStatus(const std::string& error) noexcept {
    status_ = false;
    try {
        error_ = error;
    }
    catch (...) {}
}
    

Database::row::row(const std::vector <cell> &columnsDescription) {
    columnsDescription_ = columnsDescription;
    sizeOfRow = 0;
    
    for (size_t i{0}; i < columnsDescription.size(); ++i) {
        try {
            cellNameToIndex[columnsDescription[i].name] = i; 
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"Failed adding a column to a row vector or map"});
            return;
        }

        sizeOfRow += columnsDescription[i].size;
    }
    
    try {
        rowData = new char[sizeOfRow];
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed to allocate dynamic memory"});
        return;
    }
    
    for (size_t i{0}, shift{0}; i < columnsDescription.size(); ++i) {
        columnsDescription_[i].begin = rowData + shift;
        shift += columnsDescription_[i].size;
    }
}

Database::row::row(const row* other) {
    try {
        rowData = new char[other->sizeOfRow];
        memcpy(rowData, other->rowData, other->sizeOfRow);
        
        columnsDescription_ = other->columnsDescription_;
        sizeOfRow = other->sizeOfRow;
        cellNameToIndex = other->cellNameToIndex;
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed allocation mem for new row"});
    }
    
    for (size_t i{0}, shift{0}; i < columnsDescription_.size(); ++i) {
        columnsDescription_[i].begin = rowData + shift;
        shift += columnsDescription_[i].size;
    }
}

Database::cell Database::row::getCell(const size_t index) const noexcept {
    if (index < columnsDescription_.size()) {
        return columnsDescription_[index];
    }
    else {
        lastExecutionResult.setStatus(std::string{"An invalid cell is specified to receive"});
        return cell{};
    }
}

Database::cell Database::row::getCell(const std::string& nameCell) const noexcept {
    size_t index;

    try {
        auto it = cellNameToIndex.find(nameCell);
        if (it != cellNameToIndex.end()) {
            index = (*it).second;
        }
        else {
            lastExecutionResult.setStatus(std::string{"An invalid cell is specified to receive"});
            return cell{};
        }
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed to match the cell name to the index"});
        return cell{};
    }
    
    if (index < columnsDescription_.size()) {
        return columnsDescription_[index];
    }
    else {
        lastExecutionResult.setStatus(std::string{"An invalid cell is specified to receive"});
        return cell{};
    }
}


void Database::createTable(const std::string& nameTable, 
                           const std::vector <cell> &columnsDescription,
                           const std::vector <char*> &defaultValues) noexcept {
    
    row* baseRow = new row{columnsDescription};
    
    if (lastExecutionResult.is_ok() == false) return;

    for (size_t i{0}; i < columnsDescription.size(); ++i) {
        if (defaultValues[i] != nullptr) {
            try {
                memcpy((baseRow->getCell(i)).begin, defaultValues[i], (baseRow->getCell(i)).size);
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"Failed setting default value"});
                return;
            }
        }
    }

    try {
        baseTablesRows[nameTable] = baseRow;
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed adding a base table row to a vector"});
        return;
    }
    
    try {
        tables[nameTable] = {};
    }
    catch (...) {
        baseTablesRows.erase(nameTable);
        lastExecutionResult.setStatus(std::string{"Failed adding a table to a map"});
    }
}

void Database::insert(const std::string& nameTable, const std::vector <char*> &values) noexcept {
    row* newRow = new row{baseTablesRows[nameTable]};
    
    if (lastExecutionResult.is_ok() == false) return;

    for (size_t i{0}; i < values.size(); ++i) {
        if (values[i] != nullptr) {
            try {
                memcpy((newRow->getCell(i)).begin, values[i], (newRow->getCell(i)).size);
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"Failed setting value"});
                return;
            }
        }
    }
    
    try {
        tables[nameTable].push_back(newRow);
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed adding a row to a rows vector of table"});
    }
}


void Database::condition::setOperator(std::string& operation) noexcept {
    if (operation == "(") {
        conditionType = conditionTypes::ROUND;
    }
    else if (operation == "|") {
        conditionType = conditionTypes::DIRECT;
    }
    else if (operation == "+") {
        conditionType = conditionTypes::PLUS;
    }
    else if (operation == "-") {
        conditionType = conditionTypes::MINUS;
    }
    else if (operation == "*") {
        conditionType = conditionTypes::MUL;
    }
    else if (operation == "/") {
        conditionType = conditionTypes::DEV;
    }
    else if (operation == "%") {
        conditionType = conditionTypes::MOD;
    }
    else if (operation == "<") {
        conditionType = conditionTypes::LESS;
    }
    else if (operation == "=") {
        conditionType = conditionTypes::EQUAL;
    }
    else if (operation == ">") {
        conditionType = conditionTypes::MORE;
    }
    else if (operation == "<=") {
        conditionType = conditionTypes::NOTMORE;
    }
    else if (operation == ">=") {
        conditionType = conditionTypes::NOTLESS;
    }
    else if (operation == "!=") {
        conditionType = conditionTypes::NOTEQUAL;
    }
    else if (operation == "&&") {
        conditionType = conditionTypes::AND;
    }
    else if (operation == "||") {
        conditionType = conditionTypes::OR;
    }
    else if (operation == "!") {
        conditionType = conditionTypes::NOT;
    }
    else if (operation == "^^") {
        conditionType = conditionTypes::XOR;
    }
}

void Database::condition::setLeft(condition* child) noexcept {
    leftChild = child;
}

void Database::condition::setRight(condition* child) noexcept {
    rightChild = child;
}

void Database::condition::setConst(cell* cellValue) noexcept {
    conditionType = conditionTypes::CONST;
    value = cellValue;
}

void Database::condition::setVariable(std::string& name) noexcept {
    conditionType = conditionTypes::VAR;
    try {
        nameCell = name;
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        return;
    }
}

void Database::condition::descent(condition* vertex, const row* rowToCheck) noexcept {
    switch (vertex->conditionType) {
        case conditionTypes::CONST :
            return;
        case conditionTypes::VAR :
            vertex->value = new cell{rowToCheck->getCell(vertex->nameCell)};
            return;
        default:
            break;
    }
    
    
    descent(vertex->leftChild, rowToCheck);
    if (lastExecutionResult.is_ok() == false) { return; }
    descent(vertex->rightChild, rowToCheck);
    if (lastExecutionResult.is_ok() == false) { return; }
    
    
    if (((vertex->leftChild)->value == nullptr && vertex->conditionType != conditionTypes::NOT
                                               && vertex->conditionType != conditionTypes::ROUND
                                               && vertex->conditionType != conditionTypes::DIRECT ) ||
        (vertex->rightChild)->value == nullptr) {
        lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
        return;
    }
    
    cell leftVal = *((vertex->leftChild)->value);
    cell rightVal = *((vertex->rightChild)->value);
    
    delete vertex->value;
    vertex->value = nullptr;
    vertex->value = new cell;
    cell* val = vertex->value;
    
    try {
        switch (vertex->conditionType) {
            case conditionTypes::PLUS :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::INT32;
                    val->size = 4;
                    val->begin = new char[4];
                    int* val_intptr = reinterpret_cast <int*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    *(val_intptr) = *(lval_intptr) + *(rval_intptr);
                }
                return;
            case conditionTypes::MINUS :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::INT32;
                    val->size = 4;
                    val->begin = new char[4];
                    int* val_intptr = reinterpret_cast <int*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    *(val_intptr) = *(lval_intptr) - *(rval_intptr);
                }
                return;
            case conditionTypes::MUL :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::INT32;
                    val->size = 4;
                    val->begin = new char[4];
                    int* val_intptr = reinterpret_cast <int*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    *(val_intptr) = *(lval_intptr) * *(rval_intptr);
                }
                return;
            case conditionTypes::DEV :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::INT32;
                    val->size = 4;
                    val->begin = new char[4];
                    int* val_intptr = reinterpret_cast <int*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    if ((*rval_intptr) == 0) {
                        lastExecutionResult.setStatus(std::string{"The selection request is incorrect: division by zero"});
                        delete vertex->value;
                        vertex->value = nullptr;
                        return;
                    }
                    *(val_intptr) = *(lval_intptr) / *(rval_intptr);
                }
                return;
            case conditionTypes::MOD :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::INT32;
                    val->size = 4;
                    val->begin = new char[4];
                    int* val_intptr = reinterpret_cast <int*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    if ((*rval_intptr) == 0) {
                        lastExecutionResult.setStatus(std::string{"The selection request is incorrect: division by zero"});
                        delete vertex->value;
                        vertex->value = nullptr;
                        return;
                    }
                    *(val_intptr) = *(lval_intptr) % *(rval_intptr);
                }
                return;
            case conditionTypes::LESS :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::BOOL;
                    val->size = 1;
                    val->begin = new char[1];
                    bool* val_boolptr = reinterpret_cast <bool*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    *(val_boolptr) = (*(lval_intptr) < *(rval_intptr));
                }
                return;
            case conditionTypes::EQUAL :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::BOOL;
                    val->size = 1;
                    val->begin = new char[1];
                    bool* val_boolptr = reinterpret_cast <bool*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    *(val_boolptr) = (*(lval_intptr) == *(rval_intptr));
                }
                return;
            case conditionTypes::MORE :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::BOOL;
                    val->size = 1;
                    val->begin = new char[1];
                    bool* val_boolptr = reinterpret_cast <bool*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    *(val_boolptr) = (*(lval_intptr) > *(rval_intptr));
                }
                return;
            case conditionTypes::NOTMORE :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::BOOL;
                    val->size = 1;
                    val->begin = new char[1];
                    bool* val_boolptr = reinterpret_cast <bool*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    *(val_boolptr) = (*(lval_intptr) <= *(rval_intptr));
                }
                return;
            case conditionTypes::NOTLESS :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::BOOL;
                    val->size = 1;
                    val->begin = new char[1];
                    bool* val_boolptr = reinterpret_cast <bool*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    *(val_boolptr) = (*(lval_intptr) >= *(rval_intptr));
                }
                return;
            case conditionTypes::NOTEQUAL :
                if (leftVal.type != types::INT32 || rightVal.type != types::INT32   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::BOOL;
                    val->size = 1;
                    val->begin = new char[1];
                    bool* val_boolptr = reinterpret_cast <bool*> (val->begin);
                    int* lval_intptr = reinterpret_cast <int*> (leftVal.begin);
                    int* rval_intptr = reinterpret_cast <int*> (rightVal.begin);
                    *(val_boolptr) = (*(lval_intptr) != *(rval_intptr));
                }
                return;
            case conditionTypes::AND :
                if (leftVal.type != types::BOOL || rightVal.type != types::BOOL   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::BOOL;
                    val->size = 1;
                    val->begin = new char[1];
                    bool* val_boolptr = reinterpret_cast <bool*> (val->begin);
                    bool* lval_boolptr = reinterpret_cast <bool*> (leftVal.begin);
                    bool* rval_boolptr = reinterpret_cast <bool*> (rightVal.begin);
                    *(val_boolptr) = (*(lval_boolptr) && *(rval_boolptr));
                }
                return;
            case conditionTypes::OR :
                if (leftVal.type != types::BOOL || rightVal.type != types::BOOL   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::BOOL;
                    val->size = 1;
                    val->begin = new char[1];
                    bool* val_boolptr = reinterpret_cast <bool*> (val->begin);
                    bool* lval_boolptr = reinterpret_cast <bool*> (leftVal.begin);
                    bool* rval_boolptr = reinterpret_cast <bool*> (rightVal.begin);
                    *(val_boolptr) = (*(lval_boolptr) || *(rval_boolptr));
                }
                return;
            case conditionTypes::XOR :
                if (leftVal.type != types::BOOL || rightVal.type != types::BOOL   ) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::BOOL;
                    val->size = 1;
                    val->begin = new char[1];
                    bool* val_boolptr = reinterpret_cast <bool*> (val->begin);
                    bool* lval_boolptr = reinterpret_cast <bool*> (leftVal.begin);
                    bool* rval_boolptr = reinterpret_cast <bool*> (rightVal.begin);
                    *(val_boolptr) = (*(lval_boolptr) ^ *(rval_boolptr));
                }
                return;
            case conditionTypes::NOT :
                if (rightVal.type != types::BOOL) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::BOOL;
                    val->size = 1;
                    val->begin = new char[1];
                    bool* val_boolptr = reinterpret_cast <bool*> (val->begin);
                    bool* rval_boolptr = reinterpret_cast <bool*> (rightVal.begin);
                    *(val_boolptr) = !(*(rval_boolptr));
                }
                return;
            case conditionTypes::ROUND :
                val->type = rightVal.type;
                val->size = rightVal.size;
                val->begin = new char[rightVal.size];
                //char* val_charptr = val->begin;
                //char* rval_charptr = rightVal.begin;
                memcpy(val->begin, rightVal.begin, rightVal.size);
                return;
            case conditionTypes::DIRECT :
                if (rightVal.type != types::STR || rightVal.type != types::BYTES) {
                    lastExecutionResult.setStatus(std::string{"The selection request is incorrect"});
                    delete vertex->value;
                    vertex->value = nullptr;
                    return;
                }
                else {
                    val->type = types::INT32;
                    val->size = 4;
                    val->begin = new char[4];
                    int* val_intptr = reinterpret_cast <int*> (val->begin);
                    size_t len = rightVal.size;
                    *(val_intptr) = static_cast <int> (len);
                }
                return;
            default:
                break;
        }
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Memory for the cell value could not be allocated"});
        delete vertex->value;
        vertex->value = nullptr;
        return;
    }
}

bool Database::condition::check(const row* rowToCheck) noexcept {
    descent(this, rowToCheck);
    if (lastExecutionResult.is_ok() == false) return false;
    
    if ((this->value)->type != types::BOOL) {
        lastExecutionResult.setStatus(std::string{"The condition is incorrect"});
        return false;
    }
    else {
        return *((this->value)->begin);
    }
}


void Database::executionResult::createTempTable(const row* const baseRow, const std::vector <std::string> &columns) noexcept {
    delete baseRowOfTempTable;
    tempTable.clear();
    
    std::vector <cell> columnsDescription;
    for (size_t i{0}; i < columns.size(); ++i) {
        cell newCell = baseRow->getCell(columns[i]);
        if (lastExecutionResult.is_ok() == false) return;
        
        try {
            columnsDescription.push_back(newCell);
        }
        catch (...) {
            lastExecutionResult.setStatus(std::string{"The temporary table cannot be created"});
            return;
        }
    }
    
    try {
        baseRowOfTempTable = new row{columnsDescription};
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"The temporary table cannot be created"});
        return;
    }
}

void Database::executionResult::insert(const row* const currentRow, const std::vector <std::string> &columns) noexcept {
    row* newRow = new row{baseRowOfTempTable};
    if (lastExecutionResult.is_ok() == false) return;

    for (size_t i{0}; i < columns.size(); ++i) {
        memcpy((newRow->getCell(i)).begin, (currentRow->getCell(columns[i])).begin, (newRow->getCell(i)).size);
    }
    
    try {
        tempTable.push_back(newRow);
    }
    catch (...) {
        lastExecutionResult.setStatus(std::string{"Failed adding a row to a rows vector of temporary table"});
    }
}

void Database::select(const std::string& nameTable,
                      const std::vector <std::string> &columnsSelect,
                      condition* const conditionSelect) noexcept {
    
    std::vector <row*> &table = tables[nameTable];
    lastExecutionResult.createTempTable(baseTablesRows[nameTable], columnsSelect);
    if (lastExecutionResult.is_ok() == false) return;

    for (size_t i{0}; i < table.size(); ++i) {
        if (conditionSelect->check(table[i])) {
            if (lastExecutionResult.is_ok() == false) return;
            lastExecutionResult.insert(table[i], columnsSelect);
            if (lastExecutionResult.is_ok() == false) return;
        }
    }
}

/*
void Database::update(const std::string& nameTable,
                      const std::vector <char*> &values,
                      const condition &conditionUpdate) noexcept {
    
    std::vector <row*> &table = tables[nameTable];

    for (size_t j{0}; j < table.size(); ++j) {
        if (conditionUpdate.chek(table[j])) {
            row* newRow = table[j];
            
            for (size_t i{0}; i < values.size(); ++i) {
                if (values[i] != nullptr) {
                    try {
                        memcpy((newRow->getCell(i)).begin, values[i], (newRow->getCell(i)).size);
                    }
                    catch (...) {
                        lastExecutionResult.setStatus(std::string{"Failed setting value"});
                        return;
                    }
                }
            }
        }
    }
}

void Database::deleteRows(const std::string& nameTable,
                          const condition &conditionDelete) noexcept {
    
    std::vector <row*> &table = tables[nameTable];

    for (size_t i{0}; i < table.size(); ++i) {
        if (conditionDelete.chek(table[i])) {
            try {
                table.erase(table.begin() + i);
            }
            catch (...) {
                lastExecutionResult.setStatus(std::string{"Failed delete the row"});
                return;
            }
        }
    }
}
*/



} // memdb
