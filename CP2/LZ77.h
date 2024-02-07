#ifndef LZ77_H
#define LZ77_H
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <deque>

struct NodeLZ {
    int offset = 0;
    int length = 0;
    char next = '\0';
};

std::vector<NodeLZ> LZcodeFromFile(const std::string& filename_input);
NodeLZ LZmatchFind(std::string& buffer, std::ifstream& file);
void LZcode(const std::string& filename_input, const std::string& filename_output, int maxBufferSize);
std::string LZdecode(const std::vector<NodeLZ>& code);
void writeStringtoFile(const std::string& data, const std::string& filename);


#endif
