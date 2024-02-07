#include "LZ77.h"

using namespace std;
std::vector<NodeLZ> LZcodeFromFile(const string& filename_input) {
    std::ifstream input_file(filename_input, ios::binary);
  std::vector<NodeLZ> code;
  std::string input;
    while ((input_file.get()) != EOF) {
        input_file.seekg(-1, std::ios::cur);
        NodeLZ NodeLZ;
        input_file >> NodeLZ.offset;
        input_file.get();
        input_file >> NodeLZ.length;
        input_file.get();
        NodeLZ.next = static_cast<char>(input_file.get());
        input_file.get();
        code.push_back(NodeLZ);
    }
  input_file.close();
  return code;
}


NodeLZ LZmatchFind(std::string& buffer, std::ifstream& file) {
  int buffer_size = (int)buffer.size();
  string cur_buffer = buffer;
  NodeLZ NodeLZ;
  std::string sub;
  int a = 0;
  int pos = -1;
  char next = file.peek();
    std::streampos n = file.tellg();
  for (int i = 0; i <= static_cast<int>(cur_buffer.size()); ++i) {
    if (static_cast<int>(a + sub.size()) >= buffer_size && (int)cur_buffer.size() > 0) {
        std::streampos curpos = file.tellg();
        file.seekg(n);
        cur_buffer.push_back(static_cast<char>(file.get()));
        n = file.tellg();
        file.seekg(curpos);
    }
    if(file.peek() == EOF){
        next = '\0';
        break;
    }
    sub.push_back(static_cast<char>(file.get()));
    a = (int)cur_buffer.rfind(sub);
    if (a >= 0) {
      pos = a;
      continue;
    }
    next = sub.back();
//    if(next == -1)
//        next = '\0';
    sub.pop_back();
    break;
  }
  if (pos != -1) {
    NodeLZ.offset = buffer_size - pos;
    NodeLZ.length = (int)(sub.size());
    NodeLZ.next = next;
  } else {
    NodeLZ.next = next;
  }
  for (char c : sub) {
      buffer.push_back(c);
  }
    buffer.push_back(NodeLZ.next );
  return NodeLZ;
}

void LZcode(const std::string& filename_input, const std::string& filename_output, int maxBufferSize) {

    std::ifstream input_file(filename_input);
    std::ofstream outputFile(filename_output, std::ios::trunc | ios::binary);

    if (!outputFile.is_open()) {
        std::cerr << "Error opening file: " << filename_output << std::endl;
        return;
    }
    NodeLZ curNodeLZ;
    std::string buffer;
    while (!input_file.eof()) {
        curNodeLZ = LZmatchFind(buffer, input_file);
        outputFile << curNodeLZ.offset << " " << curNodeLZ.length << " " << curNodeLZ.next << '\n';

        while(static_cast<int>(buffer.size())>maxBufferSize)
            buffer.erase();
    }


}

std::string LZdecode(const std::vector<NodeLZ>& code) {
  std::string answer;
  for (NodeLZ curNodeLZ : code) {
    if (curNodeLZ.offset == 0) {
//        if(curNodeLZ.next != '\0')
            answer += curNodeLZ.next;
    } else {
      if (curNodeLZ.offset < curNodeLZ.length) {
        int last_len = curNodeLZ.length;
        while (last_len != 0) {
          answer += answer.substr(answer.size() - curNodeLZ.offset,
                                  std::min(curNodeLZ.offset, last_len));
          last_len -= std::min(curNodeLZ.offset, last_len);
        }
      } else {
        answer += answer.substr(answer.size() - curNodeLZ.offset, curNodeLZ.length);
      }
//        if(curNodeLZ.next != '\0')
            answer += curNodeLZ.next;
    }
  }
  return answer;
}



void writeStringtoFile(const string& data, const string& filename) {
    ofstream output(filename, ios::trunc);
    if (!output.is_open()) {
        cerr << "Error opening file to write LZ: " << filename << endl;
        return;
    }

    output.write(data.c_str(), data.size()-1);

    output.close();
}

