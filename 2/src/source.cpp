#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>

using namespace std;

const unsigned int SIZE = 300;

struct Node {
    char* key; //слово
    uint64_t value; // номер
    int height; // высота поддерева
    Node* left; // левое поддерево
    Node* right; // правое поддерево
    Node(char* k, uint64_t v) {
        key = k;
        value = v;
        height = 1;
        left = nullptr;
        right = nullptr;
    }
};



class AVLTree {
public:
    AVLTree() {
        root = nullptr;
    }

    // добавление узла
    bool addNode(char* key, uint64_t value) {
        if(_findHelper(root,key,&value)){
            return false;
        }
        char* pastekey=(char*)malloc((strlen(key)+1)*sizeof(char));
        strcpy(pastekey,key);
        root = _addNode(root, pastekey, value);
        return true;
    }

    // удаление узла
    bool removeNode(char* key) {
        if(_findKey(root,key)){
            root = _removeNode(root, key);
            return true;
        }
        return false;
    }

    // поиск узла
    bool find(char* key, uint64_t* value) {
        return _findHelper(root, key, value);
    }


    void clearAVL() {
        _clearAVLTree(root);
    }

    void Deserialize(std::ifstream &is) {
        root = _DeserializeRecursive(is);
    }

    void Serialize(std::ofstream &ofs) {
        _SerializeRecursive(root, ofs);
    }


private:
    Node *root;

    unsigned char _height(Node *p) {
        return p == nullptr ? 0 : p->height;
    }

    int _editbalance(Node *p) {
        return _height(p->right) - _height(p->left);
    }

    void _fixheight(Node *p) {
        unsigned char hl = _height(p->left);
        unsigned char hr = _height(p->right);
        p->height = (hl>hr?hl:hr)+1;
    }


    Node *_rotateright(Node *p) // правый поворот вокруг p
    {
        Node *q = p->left;
        p->left = q->right;
        q->right = p;
        _fixheight(p);
        _fixheight(q);
        return q;
    }

    Node *_rotateleft(Node *q) // левый поворот вокруг q
    {
        Node *p = q->right;
        q->right = p->left;
        p->left = q;
        _fixheight(q);
        _fixheight(p);
        return p;
    }

    Node *balance(Node *p) // балансировка узла p
    {
        _fixheight(p);
        if (_editbalance(p) == 2) {
            if (_editbalance(p->right) < 0)
                p->right = _rotateright(p->right);
            return _rotateleft(p);
        }
        if (_editbalance(p) == -2) {
            if (_editbalance(p->left) > 0)
                p->left = _rotateleft(p->left);
            return _rotateright(p);
        }
        return p; //балансировка не нужна
    }


    Node *_addNode(Node *p, char* key, uint64_t value) // вставка ключа key в дерево с корнем p
    {
        if (!p) return new Node(key, value);
        if (strcmp(key, p->key)<=0) {
            p->left = _addNode(p->left, key, value);
            if (p->left == nullptr)
                return nullptr;
        }else {
            p->right = _addNode(p->right, key, value);
            if (p->right == nullptr)
                return nullptr;
        }
        return balance(p);
    }

    Node *_findmin(Node *p) // поиск узла с минимальным ключом в дереве p
    {
        return p->left ? _findmin(p->left) : p;
    }

    Node *_removemin(Node *p) // удаление узла с минимальным ключом из дерева p
    {
        if (p->left == nullptr)
            return p->right;
        p->left = _removemin(p->left);
        return balance(p);
    }

    Node *_removeNode(Node *p, char* key) // удаление ключа key из дерева p
    {
        if (!p) return nullptr;
        if (strcmp(key, p->key)<0)
            p->left = _removeNode(p->left, key);
        else if(strcmp(key, p->key)>0)
            p->right = _removeNode(p->right, key);
        else
        {
            Node *q = p->left;
            Node *r = p->right;
            delete p;
            if (!r) return q;
            Node *min = _findmin(r);
            min->right = _removemin(r);
            min->left = q;
            return balance(min);
        }
        return balance(p);
    }

    bool _findHelper(Node* node, char* key, uint64_t* value) {
        if (node == nullptr) {
            return false;
        }
        if (strcmp(key, node->key)==0) {
            *value = node->value;
            return true;
        }
        if (strcmp(key, node->key)<=0) {
            return _findHelper(node->left, key, value);
        } else {
            return _findHelper(node->right, key, value);
        }
    }

    bool _findKey(Node* node, char* key) {
        if (node == nullptr) {
            return false;
        }
        if (strcmp(key, node->key)==0) {
            return true;
        }
        if (strcmp(key, node->key)<=0) {
            return _findKey(node->left, key);
        } else {
            return _findKey(node->right, key);
        }
    }

    void _clearAVLTree(Node* node) {
        if (node == NULL) {
            return;
        }
        _clearAVLTree(node->left);
        _clearAVLTree(node->right);
        free(node);
    }


    void _SerializeRecursive(Node *tree, std::ofstream &ofs) {
        bool isEmpty = tree == nullptr;
        ofs.write(reinterpret_cast<const char*>(&isEmpty), sizeof(bool));
        if (isEmpty) {
            return;
        }
        size_t size = strlen(tree->key);
        ofs.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
        ofs.write(tree->key, sizeof(char) * size);
        ofs.write(reinterpret_cast<const char*>(&tree->value), sizeof(uint64_t));
        ofs.write(reinterpret_cast<const char*>(&tree->height), sizeof(int));
        _SerializeRecursive(tree->left, ofs);
        _SerializeRecursive(tree->right, ofs);
    }

    Node* _DeserializeRecursive(std::ifstream& is) {
        bool isEmpty;
        is.read(reinterpret_cast<char*>(&isEmpty), sizeof(bool));
        if (isEmpty) {
            return nullptr;
        }
        size_t size = 0;
        is.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        char* buffer = new char[size + 1];
        is.read(buffer, sizeof(char) * size);
        buffer[size] = '\0'; // добавляем завершающий нуль
        char* key = static_cast<char *>(malloc(sizeof(char) * (size + 1)));
        strcpy(key, buffer);
        uint64_t value;
        is.read(reinterpret_cast<char*>(&value), sizeof(uint64_t));
        int height;
        is.read(reinterpret_cast<char*>(&height), sizeof(int));
        auto* newNode = new Node(key, value);
        newNode->height = height;
        newNode->left = _DeserializeRecursive(is);
        newNode->right = _DeserializeRecursive(is);
        delete[] buffer;
        return newNode;
    }

};




int main() {
    std::ios_base::sync_with_stdio(false);
    AVLTree tree;
    char command[300] = {'\0'};
    char key[257]={'\0'};
    unsigned long long value;

    while (true) {
        cin >> std::ws;
        if (cin.eof()) {
            break;
        }
        cin >> command;
        if (command[0] == '+') {
            cin >> key;
            int i=0;
            while(key[i]) {
                key[i] = tolower(key[i]);
                i++;
            }
            cin >> value;
            if (tree.addNode(key, value)) {
                cout << "OK" << endl;
            } else {
                cout << "Exist" << endl;
            }
        } else if (command[0] == '-') {
            cin >> key;
            int i=0;
            while(key[i]) {
                key[i] = tolower(key[i]);
                i++;
            }
            if (tree.removeNode(key)) {
                cout << "OK" << endl;
            } else {
                cout << "NoSuchWord" << endl;
            }
        } else if (command[0] == '!') {
            cin >> key;
            if (key[0] == 'S') {
                cin >> command;
                std::ofstream ofs(command, std::ios::binary);
                tree.Serialize(ofs);
                std::cout << "OK" << std::endl;
            } else if (key[0] == 'L') {
                cin >> command;
                tree.clearAVL();
                std::ifstream ifs(command, std::ios::binary);
                if(ifs.good() && ifs.peek() != EOF)
                    tree.Deserialize(ifs);
                std::cout << "OK" << std::endl;
            }
        } else {
            uint64_t val;
            int i=0;
            while(command[i]) {
                command[i] = tolower(command[i]);
                i++;
            }
            if (tree.find(command, &val)) {
                cout << "OK: " << val << endl;
            } else {
                cout << "NoSuchWord" << endl;
            }
        }
    }
    return 0;
}