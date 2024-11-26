#ifndef DO_NOT_INCLUDE_SYS_LIBS

// Area for system headers
#include <bits/stdc++.h>

#endif // DO_NOT_INCLUDE_SYS_LIBS

#include "database.h"

using namespace std;
namespace addstd {};
using namespace memdb;

#define int long long
 
void findAndBorderAll(std::string& input, const std::string& replaceWord) {
    std::string replaceBy = " " + replaceWord + " ";
    
    size_t pos = input.find(replaceWord);
    
    while (pos != std::string::npos) {
        input.replace(pos, replaceWord.size(), replaceBy);

        pos = input.find(replaceWord, pos + replaceBy.size());
    }
}
 
void solve() {
    /*
    std::string input{"fiRSt(){}0x     SECOnd\n\n\ttHIRd fourth\t fifth"};
    findAndBorderAll(input, "d");
    
    std::stringstream ss{input};
    for (std::string s = ""; ss >> s; ) {
        std::cout << s << "\n";
    }
    Database db;
    db.execute(input);
    */
}

int32_t main() {
ios::sync_with_stdio(false);
cin.tie(0);

//freopen("input.txt", "r", stdin);

    int t = 1;
    //cin >> t;
    while (t--) {
        solve();
    }

    return 0;
}
