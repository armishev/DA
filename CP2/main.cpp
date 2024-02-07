#include "Huffman.h"
#include "LZ77.h"
#include <getopt.h>
#include <filesystem>
#include <chrono>
using namespace std;


struct options {
    int opt_c;
    int opt_d;
    int opt_k;
    int opt_l;
    int opt_r;
    int opt_t;
    int opt_1;
    int opt_0;
    int opt_9;
};

struct options options;

void get_options(int argc, char* argv[]);
void applying_opt(int i, int argc, char* argv[]);
void compress(const string& input_file);
void decompress(const string& input_file);
void process_directory(const filesystem::path& dir_path);
void printHelp();
void check_lzh(const string& input_file);


int main(int argc, char* argv[]){
    auto start_time = std::chrono::high_resolution_clock::now();
    get_options(argc, argv);
    applying_opt(optind, argc, argv);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Duration of the program: " << duration.count() << " microseconds" << std::endl;

    return 0;

}

void printHelp() {
    std::cout << "Usage: ./myzip [options] [files]\n"
              << "Options:\n"
              << "  -h  Show this help message\n"
              << "  -с  Display the compression result on standard output. Without this option, the compression result is written to a file\n"
              << "  -d  Decompress (unpack) the file instead of compressing it.\n"
              << "  -k  Keep the original file after compression or decompression. Without this option, the original file is usually deleted.\n"
              << "  -l  Display information about the compressed file, such as size before and after compression, compression ratio, etc.\n"
              << "  -r  Recursively process all files in the specified directories.\n"
              << "  -t  Check the integrity of the compressed file.\n"
              << "  -1 -9  Set the compression level. -1 denotes the lowest compression level (fast), and -9 denotes the highest compression level (slow but with higher compression ratio).";
}



void get_options(int argc, char* argv[]) {
    int c;
    while ((c = getopt_long(argc, argv, "hcdklrt190", NULL, NULL)) != -1) {
        switch (c) {
            case 'h':
                printHelp();
                break;
            case 'c':
                options.opt_c = 1;
                break;
            case 'd':
                options.opt_d = 1;
                break;
            case 'k':
                options.opt_k = 1;
                break;
            case 'l':
                options.opt_l = 1;
                break;
            case 'r':
                options.opt_r = 1;
                break;
            case 't':
                options.opt_t = 1;
                break;
            case '1':
                options.opt_1 = 1;
                break;
            case '9':
                options.opt_9 = 1;
                if(options.opt_9)
                break;
            case '0':
                options.opt_0 = 1;
                break;
            case '?':
                fprintf(stderr, "Передана неизвестная опция!\n");
                exit(1);
        }
    }
}

void applying_opt(int i, int argc, char* argv[]){

    for (; i < argc; i++) {
        string directory = argv[i];
        if(options.opt_t){
            check_lzh(directory);
            continue;
        }
        if(options.opt_r){
            process_directory(directory);
            continue;
        }
        if(!(options.opt_d || options.opt_l || options.opt_t)){
            compress(directory);
            continue;
        }
        if(options.opt_d){
            decompress(directory);
            if(!options.opt_k){
                remove(directory.c_str());
            }
            remove("buffer.bin");
            continue;

        }

        if(options.opt_l){
            size_t last_dot_position = directory.find_last_of('.');
            if(directory.substr(last_dot_position,4) != ".lzh"){
                fprintf(stderr, "This file not in lzh format\n");
                exit(1);
            }
            decompress(directory);
            //получение размера
            std::ifstream file_orig(directory.substr(0,last_dot_position), std::ios::binary);
            file_orig.seekg(0, std::ios::end);
            std::streampos fileSize_orig = file_orig.tellg();
            file_orig.close();
            std::ifstream file_compressed(directory, std::ios::binary);
            file_compressed.seekg(0, std::ios::end);
            std::streampos fileSize_compressed = file_compressed.tellg();
            file_orig.close();

            std::cout << "Compressed " << directory << ": " << fileSize_compressed << " bytes" << std::endl;
            std::cout << "Uncompressed " << directory.substr(0,last_dot_position) << ": " << fileSize_orig << " bytes" << std::endl;
            cout << "Ratio: " << (fileSize_compressed * 100) / fileSize_orig << " %" << std::endl;
            remove(directory.substr(0,last_dot_position).c_str());
            continue;
        }
    }

}


void compress(const string& input_file){
    std::ifstream file(input_file);
    if(!file.is_open()){
        fprintf(stderr, "File does not exist!\n");
        exit(1);
    }
    int flag = options.opt_0;
    unordered_map<char, int> frequencies;
    if(!flag) {
        int max_buf = 10000;
        (options.opt_1 == 1) ? max_buf = 250 : 0;
        LZcode(input_file, "buffer.bin", max_buf);
        frequencies = get_frequency_from_file("buffer.bin");
    }else{
        frequencies = get_frequency_from_file(input_file);
    }

    Node* root = nullptr;
    buildHuffmanTree(frequencies, root);

    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
    string output_file = input_file + ".lzh";
    std::ofstream outFile(output_file, std::ios::trunc);//если был удалить
    outFile.close();
    if(!flag)
        CodeHuffman("buffer.bin", huffmanCodes, root, output_file);
    else
        CodeHuffman(input_file, huffmanCodes, root, output_file);
    if(options.opt_c){
        std::ifstream inputFile(output_file, std::ios::binary);
        std::string line;
        while (std::getline(inputFile, line)) {
            std::cout << line << std::endl;
        }
        inputFile.close();
        remove(output_file.c_str());
    }
    if(!flag)
        remove("buffer.bin");
    if(!(options.opt_k || options.opt_c)){
        remove(input_file.c_str());
    }
}

void decompress(const string& input_file){
    size_t last_dot_position = input_file.find_last_of('.');
    if(input_file.substr(last_dot_position,4) != ".lzh"){
        fprintf(stderr, "This file not in lzh format!\n");
        exit(1);
    }
    std::ifstream file(input_file);
    if(!file.is_open()){
        fprintf(stderr, "File does not exist!\n");
        exit(1);
    }

    int flag = options.opt_0;
    last_dot_position = input_file.find_last_of('.');
    string output_file = input_file.substr(0, last_dot_position);
    if(!flag) {
        decodeHuffman(input_file, "buffer.bin");
        string codeHuffman = readFileToString("buffer.bin");
        std::vector<NodeLZ> code = LZcodeFromFile("buffer.bin");
        std::string answer = LZdecode(code);
        remove(output_file.c_str());
        writeStringtoFile(answer, output_file);
    }
    else {
        decodeHuffman(input_file, output_file);
    }

}


void process_directory(const filesystem::path& dir_path) {
    if (!filesystem::is_directory(dir_path)) {
        std::cout << "The directory is invalid: " << dir_path << std::endl;
        exit(0);
    }
    for (const auto& entry : filesystem::directory_iterator(dir_path)) {
        if (entry.is_directory()) {
            // Рекурсивно обрабатываем поддиректории
            process_directory(entry.path());
        } else if (entry.is_regular_file()) {
            auto start_time = std::chrono::high_resolution_clock::now();
            char* mas[1];
            string path = entry.path().string();
            size_t last_dot_position = path.find_last_of('.');
            std::string filename = entry.path().filename().string();

            if((options.opt_d && path.substr(last_dot_position+1,3) != "lzh") || filename[0] == '.'){
                continue;
            }
            mas[0] = const_cast<char*>(path.c_str());
            options.opt_r=0;
            applying_opt(0, 1, mas);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

            std::cout << "File: " << entry.path().filename().string() << " Time: "<<  duration.count() << " microseconds" << std::endl;
        }
    }
}

void check_lzh(const string& input_file){
    size_t last_dot_position = input_file.find_last_of('.');
    if(input_file.substr(last_dot_position,4) != ".lzh"){
        fprintf(stderr, "This file not in lzh format!\n");
        return;
    }
    try {
        options.opt_k = 1;
        decompress(input_file);
        remove((input_file.substr(0,last_dot_position)).c_str());
    } catch (...) {
        fprintf(stderr, "The file in lzh format is corrupted!\n");
        return;
    }
}
