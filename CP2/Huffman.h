#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
#include <unordered_map>

using namespace std;

struct Node {
    char symbol;
    int frequency;
    Node *left, *right;

    Node(char symbol, int frequency) : symbol(symbol), frequency(frequency), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(Node* a, Node* b) {
        return a->frequency > b->frequency;
    }
};

void buildHuffmanTree(const unordered_map<char, int>& frequencies, Node*& root);
void generateHuffmanCodes(Node* root, string code, unordered_map<char, string>& huffmanCodes);
string get_data(const string& filename);
std::unordered_map<char, int> get_frequency_from_file(const std::string& filename);
void serializeTree(Node* root, const std::string& filename_output);
void CodeHuffman(const std::string& filename_input, const std::unordered_map<char, std::string>& huffmanCodes,
                 Node* model, const std::string& filename_output);
Node* deserializeTree(std::ifstream& file);
void decodeHuffman(const string& filename_input, const string& filename_output);
void writeBinaryFile(const string& filename, const string& data);
string readFileToString(const std::string& filename);



#endif
