#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;


void toLowercase(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}

void Read(vector<string> &pattern, vector<string> &text, vector<pair<int,int>> &strandnumber){
    string curLine;
    string curWorld;
    pair<int,int> numstrnumword;
    getline(cin, curLine, '\n');

    istringstream strstream(curLine);


    while(strstream >> curWorld){
        toLowercase(curWorld);
        pattern.push_back(curWorld);
    }

    int numstr = 0;
    int numword=0;
    while(getline(cin, curLine, '\n')){
        numstr++;
        istringstream strstream2(curLine);
        while(strstream2 >> curWorld){
            numword++;
            toLowercase(curWorld);
            text.push_back(curWorld);
            numstrnumword.first = numstr;
            numstrnumword.second = numword;
            strandnumber.push_back(numstrnumword);
        }
        numword=0;
    }
}

void GoodBadCharacter(const vector<std::string> &pattern, map<string, std::vector<int>> &word_positions){
    for (int i = 0; i < pattern.size(); i++) {
        word_positions[pattern[i]].push_back(i);
    }
}

int shiftBadCharacter(int notmatchpospat, string notmatchword, map<string, std::vector<int>> word_positions){
    if(word_positions.count(notmatchword)==0)
        return 1;
    int posBadinPat=-1;
    for (int i = word_positions[notmatchword].size()-1; i >= 0; i--) {
        if(word_positions[notmatchword][i]<notmatchpospat){
            posBadinPat=word_positions[notmatchword][i];
            break;
        }
    }
    if(posBadinPat==-1)
        return 1;
    int shift = notmatchpospat - posBadinPat;
    return shift;
}




std::vector<int> Zfunction(const std::vector<std::string> &pattern) {
    int n = (int)pattern.size();
    int left = 0;
    int right = 0;
    std::vector<int> zFunction(n, 0);
    for (int i = 1; i < n; ++i) {
        if (i <= right)
            zFunction[i] = std::min(right - i + 1, zFunction[i - left]);

        while (i + zFunction[i] < n && (pattern[n - 1 - zFunction[i]] == pattern[n - 1 - (i + zFunction[i])]))
            zFunction[i]++;

        if (i + zFunction[i] - 1 > right) {
            left = i;
            right = i + zFunction[i] - 1;
        }
    }
    return zFunction;
}




void GoodSuf(const std::vector<std::string> &pattern, std::vector<int> &goodSuffix){
    // z-функция
    std::vector<int> zFunction = Zfunction(pattern);

    int n = (int)pattern.size();
    std::vector<int> N(zFunction.rbegin(), zFunction.rend());
    std::vector<int> L(n + 1, n);

    int j;
    for (int i = 0; i < n - 1; ++i) {
        j = n - N[i];
        L[j] = i;
    }

    std::vector<int> l(n + 1, n);
    for (int i = n - 1; i >= 0; i--) {
        j = n - i;
        if (N[j - 1] == j)
            l[i] = (j - 1);
        else
            l[i] = l[i + 1];
    }

    for (int i = 0; i < n + 1; ++i)
        if (L[i] == n)
            L[i] = l[i];

    for (auto &x : L)
        if (x != n)
            x = n - 1 - x;

    goodSuffix = L;
}

std::vector<int> CalcPrefSuff(const std::vector<std::string> Pattern, std::vector<int> N) {
    unsigned long size = Pattern.size();
    std::vector<int> PrefSuff(size, 0);
    PrefSuff.resize(size);
    for (int i = size - 1; i >= 0; i--) {
        int j = size - i - 1;
        if (N[j] == j + 1) {
            PrefSuff[i] = j + 1;
        } else {
            if (i == size - 1) {
                PrefSuff[i] = 0;
            } else {
                PrefSuff[i] = PrefSuff[i + 1];
            }
        }
    }
    return PrefSuff;
}



int main(){

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);

    vector<string> pattern; //паттерн
    vector<string> text; //текст
    vector<pair<int,int>> strandnumber; //массив пар - <номер строки, порядковый номер слова в этой строке>
    Read(pattern, text, strandnumber);
    std::vector<int> goodSuf(pattern.size() + 1, pattern.size());
    GoodSuf(pattern, goodSuf);
    map<string, std::vector<int>> word_positions;
    GoodBadCharacter(pattern, word_positions);
    std::vector<int> zFunction = Zfunction(pattern);
    std::vector<int> N(zFunction.rbegin(), zFunction.rend());
    std::vector<int> PrefSuff=CalcPrefSuff(pattern,N);
//    cout << shiftBadCharacter(8, "c",word_positions);


    std::vector<int> occurPos;
    std::vector<unsigned long> M(text.size());
    unsigned long k = 0, n = pattern.size(), i = 0, h = 0;
    int line = 1;
    k = n;
    while (k <= text.size()) {
        i = n-1;
        h = k-1;
        while (true) {
            if (M[h] == 0) {
                if (pattern[i] == text[h]) {
                    if (i == 0) {
                        occurPos.push_back(k-n);
                        M[k-1] = n;
                        if (n > 2) {
                            k += n - PrefSuff[1];
                        } else {
                            k++;
                        }
                        break;
                    } else {
                        i--;
                        h--;
                    }
                } else {
                    M[k-1] = k - h -1;
                    k = k +  std::max({goodSuf[i+1], shiftBadCharacter(i, text[h], word_positions)});
                    break;
                }
            } else if (M[h] < N[i]) {
                i -= M[h];
                h -= M[h];
            } else if (M[h] >= N[i] && N[i] >= i) {
                occurPos.push_back(k-n);
                M[k-1] = k-1 - h;
                if (n > 2) {
                    k += n - PrefSuff[1];
                } else {
                    k++;
                }
                break;
            } else if (M[h] > N[i] && N[i] < i) {
                M[k-1] = k - h -1;
                h -= N[i];
                i -= N[i];
                k = k +  std::max({goodSuf[i+1], shiftBadCharacter(i, text[h], word_positions)});
                break;
            } else if (M[h] == N[i] && N[i] < i) {
                i -= M[h];
                h -= M[h];
            }
        }
    }
    for (const auto &oc : occurPos)
        std::cout << strandnumber[oc].first << ", " << strandnumber[oc].second << '\n';

    return 0;

}