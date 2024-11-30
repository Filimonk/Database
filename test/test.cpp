#ifndef DO_NOT_INCLUDE_SYS_LIBS

// Area for system headers
#include <bits/stdc++.h>

#endif // DO_NOT_INCLUDE_SYS_LIBS

#include "../include/database.h"

using namespace std;

#define int long long
 
void solve() {
    memdb::Database db;
    auto result = db.execute("create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)");
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    result = db.execute("insert (,\"vasya\", 0xdeadbeefdeadbeef) to users");
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    result = db.execute("insert (login = \"vasya\", password_hash = 0xdeadbeefdeadbeef) to users");
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    result = db.execute("insert (,\"admin\", 0x0000000000000000, true) to users");
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
    
    result = db.execute("insert ( \
                            is_admin = true, \
                            login = \"admin\", \
                            password_hash = 0x0000000000000000 \
                         ) to users");
    
    if (result.is_ok()) {
        cout << "Ok\n";
    }
    else {
        cout << "Bad\n" << result.what() << "\n";
    }
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
