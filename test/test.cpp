#ifndef DO_NOT_INCLUDE_SYS_LIBS

// Area for system headers
#include <bits/stdc++.h>

#endif // DO_NOT_INCLUDE_SYS_LIBS

#include "../include/database.h"

using namespace std;
 
void solve() {
    memdb::Database db;
    auto result = db.execute("create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)");
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    result = db.execute("insert (,\"vasya1\", 0xdeadbeefdeadbeef) to users");
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    result = db.execute("insert (login = \"vasya2\", password_hash = 0xdeadbeefdeadbeef) to users");
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    result = db.execute("insert (,\"admin1\", 0x0000000000000000, true) to users");
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    result = db.execute("insert ( \
                            is_admin = true, \
                            login = \"admin2\", \
                            password_hash = 0x0000000000000000 \
                         ) to users select id, login, password_hash, is_admin from users where is_admin || id < 10");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* password_hash = line->get<char*>("password_hash");
        if (password_hash != nullptr) {
            cout << password_hash << " ";
        }
        bool* is_admin = line->get<bool*>("is_admin");
        if (is_admin != nullptr) {
            cout << *is_admin << " ";
        }
        cout << "\n";
    }
    
    result = db.execute("select id, login, password_hash, is_admin from users where is_admin ^^ id < 10");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* password_hash = line->get<char*>("password_hash");
        if (password_hash != nullptr) {
            cout << password_hash << " ";
        }
        bool* is_admin = line->get<bool*>("is_admin");
        if (is_admin != nullptr) {
            cout << *is_admin << " ";
        }
        cout << "\n";
    }
    

    cout << "\n";
    memdb::Database mydb;
    result = mydb.execute("crEaTE TABLE first ({autoincrement} id : int32, is_admin: bool = false, \
                                               {unique} login: string[32], password_hash:bytes[8], text: string[100] = \"\") \
                           crEaTE TABLE second ({autoincrement} id: int32, is_admin: bool = false, \
                                                {unique} login: string[32], password_hash:bytes[8], text: string[100] = \"\")");
    
    if (result.is_ok() == true) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }

    result = mydb.execute("insert (, true, \"admin1\", 0x0102030405060708, \"abcd\") to first \
                           insert (, true, \"admin2\", 0x0102030405060708, \"abc\") to first \
                           insert (, true, \"admin3\", 0x0102030405060708, \"abcc\") to first \
                           insert (, true, \"admin4\", 0x0102030405060708, \"abccc\") to first \
                           insert (, false, \"vasya\", 0x0102030405060708, \"abc\") to first \
                           insert (, true, \"admin1\", 0x0102030405060708, \"abcd\") to second \
                           insert (, true, \"admin2\", 0x0102030405060708, \"abc\") to second \
                           insert (, false, \"vasya2\", 0x0102030405060708, \"abc\") to second \
                           insert (, false, \"vasya1\", 0x0102030405060708, \"abcd\") to second");
    
    if (result.is_ok() == true) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    result = mydb.execute("insert (, true, \"admin1\", 0x0102030405060708, \"abcd\") to wrong_first");
    
    if (result.is_ok() == true) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad (SUCCESSFUL)\n" << result.what() << "\n";
    }
    
    result = mydb.execute("select id, login, text from first where is_admin && login + \"abcd\" = login + text");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* text = line->get<char*>("text");
        if (text != nullptr) {
            cout << text << " ";
        }
        cout << "\n";
    }
    
    result = mydb.execute("select id, login, text from second where is_admin >= (\"abcd\" = text)");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* text = line->get<char*>("text");
        if (text != nullptr) {
            cout << text << " ";
        }
        cout << "\n";
    }
    
    result = mydb.execute("select id, login, text from second where !(is_admin >= (\"abcd\" = text))");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* text = line->get<char*>("text");
        if (text != nullptr) {
            cout << text << " ";
        }
        cout << "\n";
    }
    
    result = mydb.execute("select id, login, text from first where is_admin_worng && login + \"abcd\" = login + text");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad (SUCCESSFUL)\n" << result.what() << "\n";
    }
    
    result = mydb.execute("select id, login, text from first where is_admin_worng && |login + \"abcd\"| = login + text");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad (SUCCESSFUL)\n" << result.what() << "\n";
    }
    
    result = mydb.execute("select id, login, text from first where !is_admin || |login + text| % 3 != 1");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* text = line->get<char*>("text");
        if (text != nullptr) {
            cout << text << " ";
        }
        cout << "\n";
    }
    
    cout << "\n";
    result = mydb.execute("select id, login, text from first where !is_admin || (|login + text| - 1 + 2) % 3 != 1");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* text = line->get<char*>("text");
        if (text != nullptr) {
            cout << text << " ";
        }
        cout << "\n";
    }
    
    result = mydb.execute("select id, login, text from first where !is_admin || |login + text| % 0 != 1");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad (SUCCESSFUL)\n" << result.what() << "\n";
    }
    
    result = mydb.execute("select id, login, text from first where is_admin && !(|login + \"abcd\"| = |login + text|)");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* text = line->get<char*>("text");
        if (text != nullptr) {
            cout << text << " ";
        }
        cout << "\n";
    }
    
    result = mydb.execute("select id, login, text from first where is_admin && |login + \"abcd\"| = |login + text|");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* text = line->get<char*>("text");
        if (text != nullptr) {
            cout << text << " ";
        }
        cout << "\n";
    }
    
    result = mydb.execute("deLETE first where is_admin && |login + \"abcd\"| = |login + text| \
                           select id, login, text from first where TRue");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* text = line->get<char*>("text");
        if (text != nullptr) {
            cout << text << " ";
        }
        cout << "\n";
    }
    
    result = mydb.execute("Update first set text = \"aaaaa\" where |login| / 2 = 3 \
                           select id, login, text from first where TRue");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    for (auto& line: result) {
        int* id = line->get<int*>("id");
        if (id != nullptr) {
            cout << *id << " ";
        }
        char* login = line->get<char*>("login");
        if (login != nullptr) {
            cout << login << " ";
        }
        char* text = line->get<char*>("text");
        if (text != nullptr) {
            cout << text << " ";
        }
        cout << "\n";
    }
    
    result.close();
}

int32_t main() {
//ios::sync_with_stdio(false);
//cin.tie(0);

//freopen("input.txt", "r", stdin);

    int t = 1;
    //cin >> t;
    while (t--) {
        solve();
    }

    return 0;
}
