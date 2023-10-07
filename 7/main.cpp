#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
using namespace std;


int main() {
    int64_t n, m;
    cin >> n >> m;
    string n_str = to_string(n);
    int length = n_str.length();
    int flag;
    flag = (n % m == 0) ? 1 : 0;
    vector<int64_t> count(length);
    count[0] = n/m-(static_cast<int64_t>(pow(10,length-1))-1)/m;
    n/=10;
    length--;
    for(int i=1; n>0;i++) {
        count[i] = n/m-(static_cast<int64_t>(pow(10,length-1))-1)/m + count[i-1];
        n/=10;
        length--;
    }
    cout <<count.back() - flag;
}