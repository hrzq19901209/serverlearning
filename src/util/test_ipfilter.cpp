#include <iostream>
#include <string>
#include "ip_filter.h"

using namespace std;

int main(int argc, char* argv[]){
    IpFilter f;
    f.add_allow("10.16.51.127");
    set<string> s = f.getAllow();
    for(string v: s){
        cout << v << endl;    
    }
    return 0;
}
