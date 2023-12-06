#include <iostream>
#include <sstream>
#include <vector>

struct Node {
  int offset = 0;
  int length = 0;
  char next = '#';
};

std::vector<Node> codeFromInput();
std::string textFromInput();
Node matchFind(std::string buffer, std::string subString);
std::vector<Node> code(const std::string& text);
std::string decode(const std::vector<Node>& code);
void printCode(const std::vector<Node>& answer);

int main() {
  std::string option;
  getline(std::cin, option);
  if (option == "compress") {
    std::string text = textFromInput();
    std::vector<Node> answer = code(text);
    printCode(answer);
  } else if (option == "decompress") {
    std::vector<Node> code = codeFromInput();
    std::string answer = decode(code);
    std::cout << answer;
  } else {
    std::cout << "Unknown option";
  }
  return 0;
}

std::vector<Node> codeFromInput() {
  std::vector<Node> code;
  std::string input;
  Node node;
  while (getline(std::cin, input)) {
    node.next = '#';
    std::istringstream stream(input);
    stream >> node.offset >> node.length;
    if (std::isalpha(input.back())) stream >> node.next;
    code.push_back(node);
  }
  return code;
}

std::string textFromInput() {
  std::string text;
  std::string line;
  getline(std::cin, text);
  while (getline(std::cin, line)) {
    text += '#';
    text += line;
  }
  return text;
}

Node matchFind(std::string buffer, std::string subString) {
  int buffer_size = (int)buffer.size();
  Node node;
  std::string sub;
  int a = 0;
  int pos = -1;
  char next = subString[0];
  int n = 0;
  for (int i = 0; i <= buffer.size(); ++i) {
    if (a + sub.size() >= buffer_size && (int)buffer.size() > 0) {
      buffer += subString.substr(n, 1);
      n++;
    }
    sub.push_back(subString[i]);
    a = (int)buffer.rfind(sub);
    if (a >= 0) {
      pos = a;
      continue;
    }
    if (sub.back() == '\0')
      next = '#';
    else
      next = sub.back();
    sub.pop_back();
    break;
  }
  if (pos != -1) {
    node.offset = buffer_size - pos;
    node.length = (int)(sub.size());
    node.next = next;
  } else {
    node.next = next;
  }
  return node;
}

std::vector<Node> code(const std::string& text) {
  int curPos = 0;
  Node curNode;
  std::vector<Node> answer;
  while (curPos < text.size()) {
    curNode = matchFind(text.substr(0, curPos), text.substr(curPos));
    curPos = curPos + curNode.length + 1;
    answer.push_back(curNode);
  }
  return answer;
}

std::string decode(const std::vector<Node>& code) {
  std::string answer;
  for (Node curNode : code) {
    if (curNode.offset == 0) {
      answer.push_back(curNode.next);
    } else {
      if (curNode.offset < curNode.length) {
        int last_len = curNode.length;
        while (last_len != 0) {
          answer += answer.substr(answer.size() - curNode.offset,
                                  std::min(curNode.offset, last_len));
          last_len -= std::min(curNode.offset, last_len);
        }
      } else {
        answer += answer.substr(answer.size() - curNode.offset, curNode.length);
      }
      if (curNode.next != '#')
        answer += curNode.next;
      else
        answer += '\n';
    }
  }
  size_t pos = 0;
  while ((pos = answer.find('#', pos)) != std::string::npos) {
    answer.replace(pos, 1, "\n");
  }
  return answer;
}

void printCode(const std::vector<Node>& answer) {
  for (Node node : answer) {
    if (node.next != '#') {
      std::cout << node.offset << " " << node.length << " " << node.next
                << '\n';
    } else {
      std::cout << node.offset << " " << node.length << '\n';
    }
  }
}
