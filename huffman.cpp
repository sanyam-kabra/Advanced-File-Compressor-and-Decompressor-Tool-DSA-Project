#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <bits/stdc++.h>

using namespace std;

// Node class representing each character and its frequency in the Huffman Tree
class Node {
public:
    char ch;            // Character
    int freq;           // Frequency of the character
    Node *left, *right; // Left and right child pointers

    // Constructor for initializing the Node with a character and its frequency
    Node(char ch, int freq) {
        this->ch = ch;
        this->freq = freq;
        left = right = nullptr;
    }
};

// MinHeap class for managing priority queue operations for Huffman Tree construction
class MinHeap {
    vector<Node*> heap;

    // Helper function for heapifying upward (maintaining heap property on insert)
    void heapifyUp(int index) {
        while (index > 0 && heap[(index - 1) / 2]->freq > heap[index]->freq) {
            swap(heap[index], heap[(index - 1) / 2]);
            index = (index - 1) / 2;
        }
    }

    // Helper function for heapifying downward (maintaining heap property on removal)
    void heapifyDown(int index) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heap.size() && heap[left]->freq < heap[smallest]->freq) {
            smallest = left;
        }
        if (right < heap.size() && heap[right]->freq < heap[smallest]->freq) {
            smallest = right;
        }
        if (smallest != index) {
            swap(heap[index], heap[smallest]);
            heapifyDown(smallest);
        }
    }

public:
    // Constructor
    MinHeap() {}

    // Returns true if the heap is empty
    bool empty() const {
        return heap.empty();
    }

    // Returns the size of the heap
    int size() const {
        return heap.size();
    }

    // Inserts a node into the heap
    void insert(Node* node) {
        heap.push_back(node);
        heapifyUp(heap.size() - 1);
    }

    // Removes and returns the node with the minimum frequency
    Node* extractMin() {
        if (heap.empty()) return nullptr;
        Node* minNode = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
        return minNode;
    }
};

// Main class for Huffman Coding operations: encoding and decoding files
class HuffmanCoding {
    Node* root;                              // Root node of the Huffman Tree
    unordered_map<char, string> huffmanCodes; // Map for storing the Huffman codes
    unordered_map<char, int> frequencies;     // Map for storing character frequencies
    string encodedStr;                        // Encoded string representation of input text

    // Builds a frequency table from the input text
    void buildFrequencyTable(const string &text) {
        frequencies.clear();
        for (char ch : text) {
            frequencies[ch]++;
        }
    }

    // Builds the Huffman Tree based on character frequencies
    void buildHuffmanTree() {
        deleteTree(root); // Clean up previous tree if it exists
        root = nullptr;
        huffmanCodes.clear();

        vector<pair<char, int>> freqVec(frequencies.begin(), frequencies.end());
        sort(freqVec.begin(), freqVec.end());

        // Initialize the min-heap with nodes for each character and frequency
        MinHeap minHeap;
        for (const auto& pair : freqVec) {
            minHeap.insert(new Node(pair.first, pair.second));
        }

        // Build the tree by extracting two nodes with minimum frequency and combining them
        while (minHeap.size() > 1) {
            Node *left = minHeap.extractMin();
            Node *right = minHeap.extractMin();

            Node *newNode = new Node('\0', left->freq + right->freq);
            newNode->left = left;
            newNode->right = right;
            minHeap.insert(newNode);
        }

        root = minHeap.empty() ? nullptr : minHeap.extractMin();
        buildHuffmanCodes(root, ""); // Generate codes for each character
    }

    // Recursively deletes the Huffman Tree to release memory
    void deleteTree(Node* node) {
        if (node == nullptr) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

    // Recursively builds Huffman codes for each character by traversing the tree
    void buildHuffmanCodes(Node* node, string str) {
        if (!node) return;

        if (!node->left && !node->right) {
            huffmanCodes[node->ch] = str;
        }

        buildHuffmanCodes(node->left, str + "0");
        buildHuffmanCodes(node->right, str + "1");
    }

    // Encodes the given text into a binary string using Huffman codes
    string getEncodedString(const string &text) {
        string encodedStr = "";
        for (char ch : text) {
            if (huffmanCodes.find(ch) != huffmanCodes.end()) {
                encodedStr += huffmanCodes[ch];
            }
        }
        return encodedStr;
    }

    // Decodes the encoded binary string back into the original text
    string decodeString(Node* node, const string &encodedStr) {
        string decodedStr = "";
        Node* current = node;
        
        for (char bit : encodedStr) {
            if (!current) {
                throw runtime_error("Invalid tree state during decoding");
            }

            current = (bit == '0') ? current->left : current->right;

            if (current && !current->left && !current->right) {
                decodedStr += current->ch;
                current = node;
            }
        }

        if (current != node) {
            throw runtime_error("Invalid encoding - incomplete sequence");
        }
        
        return decodedStr;
    }

    // Saves the encoded binary string to a file along with the frequency table for decoding
    void saveEncodedToFile(const string& encodedStr, const string& encodedFile) {
        ofstream outFile(encodedFile, ios::binary);
        if (!outFile) {
            throw runtime_error("Cannot open output file");
        }

        int freqSize = frequencies.size();
        outFile.write(reinterpret_cast<const char*>(&freqSize), sizeof(freqSize));
        
        vector<pair<char, int>> freqVec(frequencies.begin(), frequencies.end());
        sort(freqVec.begin(), freqVec.end());
        
        for (const auto& pair : freqVec) {
            outFile.put(pair.first);
            outFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
        }

        int encodedLength = encodedStr.length();
        outFile.write(reinterpret_cast<const char*>(&encodedLength), sizeof(encodedLength));

        unsigned char buffer = 0;
        int bitCount = 0;
        for (char bit : encodedStr) {
            buffer = (buffer << 1) | (bit == '1');
            bitCount++;
            if (bitCount == 8) {
                outFile.put(buffer);
                buffer = 0;
                bitCount = 0;
            }
        }

        if (bitCount > 0) {
            buffer <<= (8 - bitCount);
            outFile.put(buffer);
        }

        outFile.close();
    }

    // Loads encoded data from file and rebuilds the Huffman Tree for decoding
    void loadEncodedFromFile(const string& encodedFile) {
        ifstream inFile(encodedFile, ios::binary);
        if (!inFile) {
            throw runtime_error("Cannot open input file");
        }

        int freqSize;
        inFile.read(reinterpret_cast<char*>(&freqSize), sizeof(freqSize));
        
        frequencies.clear();
        for (int i = 0; i < freqSize; i++) {
            char ch;
            int freq;
            inFile.get(ch);
            inFile.read(reinterpret_cast<char*>(&freq), sizeof(freq));
            frequencies[ch] = freq;
        }

        int encodedLength;
        inFile.read(reinterpret_cast<char*>(&encodedLength), sizeof(encodedLength));

        encodedStr = "";
        unsigned char buffer;
        int bitsRead = 0;
        while (inFile.get(reinterpret_cast<char&>(buffer)) && bitsRead < encodedLength) {
            for (int i = 7; i >= 0 && bitsRead < encodedLength; i--) {
                encodedStr += ((buffer >> i) & 1) ? '1' : '0';
                bitsRead++;
            }
        }

        inFile.close();
        buildHuffmanTree();
    }

public:
    // Constructor initializes the HuffmanCoding object
    HuffmanCoding() : root(nullptr) {}
    
    // Destructor to release memory by deleting the Huffman Tree
    ~HuffmanCoding() {
        deleteTree(root);
    }

    // Encodes input file text and saves encoded output to a file
    void encodeToFile(const string& inputFile, const string& encodedFile) {
        ifstream inFile(inputFile);
        if (!inFile) {
            throw runtime_error("Cannot open input file");
        }

        string text((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
        inFile.close();

        buildFrequencyTable(text);
        buildHuffmanTree();
        string encodedStr = getEncodedString(text);
        saveEncodedToFile(encodedStr, encodedFile);
        
        cout << "File successfully compressed" << endl;
    }

    // Decodes the encoded file back into its original text and saves it to a file
    void decodeFromFile(const string& encodedFile, const string& outputFile) {
        loadEncodedFromFile(encodedFile);
        string decodedStr = decodeString(root, encodedStr);

        ofstream outFile(outputFile);
        if (!outFile) {
            throw runtime_error("Cannot open output file");
        }
        outFile << decodedStr;
        outFile.close();
        
        cout << "File successfully decompressed" << endl;
    }
};

// Main function for user interaction: compress or decompress a file
int main() {
    try {
        HuffmanCoding huffman;
        cout << "Do you want to compress or decompress?: ";
        string process;
        cin >> process;

        if (process == "compress") {
            string inputFile, encodedFile;
            cout << "Enter input file name: ";
            cin >> inputFile;
            cout << "Enter output file name: ";
            cin >> encodedFile;
            huffman.encodeToFile(inputFile, encodedFile);
        }
        else if (process == "decompress") {
            string inputFile, decodedFile;
            cout << "Enter input file name: ";
            cin >> inputFile;
            cout << "Enter output file name: ";
            cin >> decodedFile;
            huffman.decodeFromFile(inputFile, decodedFile);
        }
        else {
            cout << "Invalid option. Please choose 'compress' or 'decompress'" << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
