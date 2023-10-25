#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#define TERMINATION_SYMBOL '$'

using namespace std;

class SuffixTree;

class Node
{
public:
    friend SuffixTree;

    Node(Node *link, int start, int *end) : Node(link, start, end, -1,0,0)
    {
    }

    Node(Node *link, int start, int *end, int ind, int in_1, int in_2) : suffix_link(link),
                                                                         start(start),
                                                                         end(end),
                                                                         suffix_index(ind),
                                                                         in_1(in_1),
                                                                         in_2(in_2)
    {
    }

private:
    map<char, Node *> children;
    Node *suffix_link;
    int start;
    int *end;
    int suffix_index;
    int in_1=-1;
    int in_2=-1;
};

class SuffixTree
{
public:
    SuffixTree(string &text, int sep);
    void BuildSuffixTree();
    ~SuffixTree()
    {
        DeleteSuffixTree(root);
    }
    void CountLeafIndices(vector<int> &vec)
    {
        CountIndex(root, vec);
    }
    void depth()
    {
        func_depth(root, 0);
    }
    void depth2()
    {
        func_depth2(root, 0);
    }

private:
    void ExtendSuffixTree(int pos);
    void DeleteSuffixTree(Node *node)
    {
        for (auto it : node->children)
            DeleteSuffixTree(it.second);
        if (node->suffix_index == -1)
            delete node->end;
        delete node;
    }
    void CountIndex(Node *node, vector<int> &vec);
    void func_depth(Node *node, int d);
    void func_depth2(Node *node, int d);
    int EdgeLength(Node *node);

    Node *root = new Node(nullptr, -1, new int(-1));
    Node *lastCreatedInternalNode = nullptr;

    string text;

    Node *activeNode = nullptr;
    int activeEdge = -1;
    int activeLength = 0;
    int remainingSuffixCount = 0;
    int leafEnd = -1;
    int sep;
};

SuffixTree::SuffixTree(string &str, int separator) : text(str), sep(separator)
{
    text += TERMINATION_SYMBOL;
    BuildSuffixTree();
}

void SuffixTree::BuildSuffixTree()
{
    activeNode = root;
    for (size_t i = 0; i < text.length(); i++)
        ExtendSuffixTree(i);
}

int SuffixTree::EdgeLength(Node *node)
{
    return *(node->end) - (node->start) + 1;
}

void SuffixTree::ExtendSuffixTree(int phase)
{

    lastCreatedInternalNode = nullptr;

    leafEnd++;

    remainingSuffixCount++;

    while (remainingSuffixCount > 0)
    {

        if (activeLength == 0)
            activeEdge = phase;

        auto find = activeNode->children.find(text[activeEdge]);

        if (find == activeNode->children.end())
        {

            if(phase - remainingSuffixCount + 1 > sep){
                activeNode->children.insert(make_pair(
                        text[activeEdge],
                        new Node(root, phase, &leafEnd, phase - remainingSuffixCount + 1,0,1)));
            }else{
                activeNode->children.insert(make_pair(
                        text[activeEdge],
                        new Node(root, phase, &leafEnd, phase - remainingSuffixCount + 1, 1, 0)));
            }

            if (lastCreatedInternalNode != nullptr)
            {
                lastCreatedInternalNode->suffix_link = activeNode;
                lastCreatedInternalNode = nullptr;
            }
        }
        else
        {

            Node *next = find->second;
            int edge_length = EdgeLength(next);

            if (activeLength >= edge_length)
            {
                activeEdge += edge_length;
                activeLength -= edge_length;
                activeNode = next;
                continue;
            }

            if (text[next->start + activeLength] == text[phase])
            {

                if (lastCreatedInternalNode != nullptr && activeNode != root)
                    lastCreatedInternalNode->suffix_link = activeNode;
                activeLength++;
                break;
            }

            Node *split = new Node(root, next->start, new int(next->start + activeLength - 1));

            activeNode->children[text[activeEdge]] = split;

            next->start += activeLength;

            if(phase - remainingSuffixCount + 1 > sep){
                split->children.insert(make_pair(text[phase], new Node(root, phase, &leafEnd, phase - remainingSuffixCount + 1,0,1)));
            }else{
                split->children.insert(make_pair(text[phase], new Node(root, phase, &leafEnd, phase - remainingSuffixCount + 1,1,0)));
            }

            split->children.insert(make_pair(text[next->start], next));

            if (lastCreatedInternalNode != nullptr)
                lastCreatedInternalNode->suffix_link = split;
            lastCreatedInternalNode = split;
        }

        remainingSuffixCount--;

        if (activeNode == root && activeLength > 0)
        {
            activeLength--;
            activeEdge++;
        }
        else if (activeNode != root)
            activeNode = activeNode->suffix_link;
    }
}

void SuffixTree::CountIndex(Node *node, vector<int> &vec)
{
    if (!node)
        return;
    for (auto it : node->children)
        CountIndex(it.second, vec);
    if (node->suffix_index != -1)
        vec.push_back(node->suffix_index);
}

int Max = 0;
vector<int> v;

void SuffixTree::func_depth(Node *node, int d) {
    int k;
    if (node->children.empty())
        return;
    for (auto it: node->children) {
        if (!it.second->children.empty()) {
            k = *(it.second->end) - it.second->start + 1;
        }
        func_depth(it.second, d + k);
    }
    for (auto it: node->children) {
        node->in_1 |= it.second->in_1;
        node->in_2 |= it.second->in_2;
    }
    if((node->in_1) == 1 && (node->in_2 == 1)){
        if(Max == d){
            v.push_back(*(node->end));
        }else if(d>Max){
            Max=d;
            v.resize(0);
            v.push_back(*(node->end));
        }
    }
}



int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    std::string inputText1, inputText2;
    cin >> inputText1 >> inputText2;
    std::string inputText= inputText1+"#"+inputText2;
    int separator = static_cast<int>(inputText1.size());
    SuffixTree suffixTree(inputText, separator);
    suffixTree.depth();
    cout << Max << '\n';
    for(auto i: v){
        for(int j = i - Max+1; j<i+1; j++){
            cout<<inputText[j];
        }
        cout << '\n';
    }

    return 0;
}

