#include "Huffman.h"


using namespace std;
void buildHuffmanTree(const unordered_map<char, int>& frequencies, Node*& root) {
    priority_queue<Node*, vector<Node*>, CompareNodes> minHeap;


    for (const auto& pair : frequencies) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        Node* left = minHeap.top();
        minHeap.pop();

        Node* right = minHeap.top();
        minHeap.pop();

        Node* newNode = new Node('#', left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;

        minHeap.push(newNode);
    }

    root = minHeap.top();
}

void generateHuffmanCodes(Node* root, string code, unordered_map<char, string>& huffmanCodes) {
    if (root == nullptr) {
        return;
    }

    if (root->left == nullptr && root->right == nullptr) {
        huffmanCodes[root->symbol] = code;
    }

    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}


std::unordered_map<char, int> get_frequency_from_file(const std::string& filename)
{
    std::unordered_map<char, int> frequencies;

    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return frequencies;
    }

    char current_char;

    while (file >> std::noskipws >> current_char)
    {
        // Проверка есть ли символ уже в словаре
        auto it = frequencies.find(current_char);
        if (it == frequencies.end())
        {
            frequencies[current_char] = 1;
        }
        else
        {
            it->second++;
        }
    }

    file.close();

    return frequencies;
}

void serializeTree(Node* root, const std::string& filename_output, long long sizeorig) {
    std::ofstream output_file(filename_output, std::ios::app | ios::binary);

    if (!output_file.is_open()) {
        std::cerr << "Error opening file: " << filename_output << std::endl;
        return;
    }
    if(sizeorig != -1)
        output_file << sizeorig << '@';

    if (root != nullptr) {
        if (root->left == nullptr && root->right == nullptr) {
            // Лист - записываем бит 1 и символ
            output_file << '1';
            output_file << root->symbol;
            output_file.close();
        } else {
            // Не лист - записываем бит 0 и рекурсивно обходим левое и правое поддеревья
            output_file << '0';
            output_file.close();
            serializeTree(root->left, filename_output, -1);
            serializeTree(root->right, filename_output, -1);

        }
    }
}

char GetPow(unsigned n) {
    switch (n) {
        case 0:
            return (char)1;
        case 1:
            return (char)2;
        case 2:
            return (char)4;
        case 3:
            return (char)8;
        case 4:
            return (char)16;
        case 5:
            return (char)32;
        case 6:
            return (char)64;
        default:
            return (char)0;
    }
}

char BinaryToChar(std::string& bin) {
    if (bin == "10000000") {
        return -128;
    }
    char res = 0;
    for (unsigned i = bin.length() - 1; i >= 1; --i) {
        if (bin[i] == '1') {
            res += GetPow(bin.length() - 1 - i);
        }
    }
    if (bin[0] == '1') {
        res *= -1;
    }
    return res;
}

std::string ToBinaryString(char n) {
    std::string buffer;
    bool negative = n < 0;
    if (negative) {
        n *= -1;
    }
    do
    {
        buffer += char('0' + n % 2);
        n /= 2;
    } while (n > 0);
    buffer = std::string(buffer.rbegin(), buffer.rend());
    while (buffer.length() < (8 - 1)) {
        buffer = '0' + buffer;
    }
    if (negative) {
        buffer = '1' + buffer;
    } else {
        buffer = '0' + buffer;
    }
    return buffer;
}


void CodeHuffman(const std::string& filename_input, const std::unordered_map<char, std::string>& huffmanCodes,
                 Node* model, const std::string& filename_output) {
    std::ifstream file(filename_input, ios::binary);
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.close();
    std::ifstream input_file(filename_input, ios::binary);
    std::ofstream output_file(filename_output, std::ios::app | ios::binary);



    if (!input_file.is_open())
    {
        std::cerr << "Error opening input file for serialize: " << filename_input << std::endl;
        return;
    }

    if (!output_file.is_open()) {
        std::cerr << "Error opening output file for serialize: " << filename_output << std::endl;
        return;
    }

    // Сериализуем дерево в файл
    serializeTree(model, filename_output, fileSize);

    char current_char;
    string temp;
    while(input_file >> std::noskipws >> current_char){
        temp += huffmanCodes.at(current_char);
        while (temp.length() >= 8) {
            std::string binByte = temp.substr(0, 8);
            char temp2 = BinaryToChar(binByte);
            output_file << temp2;
            temp = temp.substr(8, temp.length() - 8);
        }
    }
    if (!temp.empty()) {
        while (temp.length() != 8) {
            temp += '0';
        }
        output_file << BinaryToChar(temp);
    }

    input_file.close();
    output_file.close();
}



Node* deserializeTree(std::ifstream& file) {



    char bit = static_cast<char>(file.get());
    if (bit == '1') {
        char symbol = static_cast<char>(file.get());
        return new Node(symbol, 0);
    } else {
        Node* left = deserializeTree(file);
        Node* right = deserializeTree(file);
        Node* newNode = new Node('#', 0);
        newNode->left = left;
        newNode->right = right;
        return newNode;
    }
}


void decodeHuffman(const string& filename_input, const string& filename_output) {

    std::ifstream input_file(filename_input,  ios::binary);
    std::ofstream output_file(filename_output, std::ios::trunc);

    string strsize="";
    char c = (char)input_file.get();
    while(c != '@'){
        strsize+=c;
        c = (char)input_file.get();
    }
    int size = stoi(strsize);
    Node* deserializedRoot = deserializeTree(input_file);

    Node* current = deserializedRoot;

    std::string text(std::istreambuf_iterator<char>(input_file), {});


    std::string binaryText;
    for(char c: text) {
        std::string temp = ToBinaryString(c);
        binaryText += temp;
    }
    long long size_uncompressed= 0;
    for(char bit: binaryText){
        if(size_uncompressed == size)
            break;
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }

        if (current->left == nullptr && current->right == nullptr) {
            // Достигнут лист - добавляем символ к декодированному тексту
            output_file << current->symbol;
            size_uncompressed++;
            current = deserializedRoot;  // Возвращаемся в корень для следующего символа
        }
    }
}


std::string readFileToString(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}





