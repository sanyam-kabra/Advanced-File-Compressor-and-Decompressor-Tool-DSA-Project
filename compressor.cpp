#include <bits/stdc++.h>
#include <jpeglib.h>

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
    while (index > 0) {
        int parentIndex = (index - 1) / 2;
        if (heap[parentIndex]->freq <= heap[index]->freq) {
            break;
        }
        swap(heap[parentIndex], heap[index]);
        index = parentIndex;
    }
}

void heapifyDown(int index) {
    int size = heap.size();
    while (index < size) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int smallest = index;

        if (leftChild < size && heap[leftChild]->freq < heap[smallest]->freq) {
            smallest = leftChild;
        }

        if (rightChild < size && heap[rightChild]->freq < heap[smallest]->freq) {
            smallest = rightChild;
        }

        if (smallest == index) {
            break;
        }

        swap(heap[index], heap[smallest]);
        index = smallest;
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

//Function to calculate file size
long getFileSize(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return -1;
    }
    
    // Move the file pointer to the end
    file.seekg(0, std::ios::end);
    
    // Get the position of the file pointer (which is the file size)
    long size = file.tellg();
    
    // Close the file
    file.close();
    
    return size;
}

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
        long inputsize = getFileSize(inputFile);
        long outputsize = getFileSize(encodedFile);
        cout<<"original file size:"<<inputsize<<" bytes"<<endl;
        cout<<"compressed file size:"<<outputsize<<" bytes"<<endl;
        double compressionPercentage = (static_cast<double>(outputsize) / inputsize) * 100;
        cout << "Compression Percentage: " << compressionPercentage << "%" << endl;

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
class JPEGCompressor {
public:
    JPEGCompressor(const std::string& inputFile, const std::string& outputFile, int quality)
        : inputFile(inputFile), outputFile(outputFile), quality(quality) {}

    void compress() {
        // Open the input file
        FILE* infile = fopen(inputFile.c_str(), "rb");
        if (!infile) {
            throw std::runtime_error("Can't open input file: " + inputFile);
        }

        // Create and initialize the JPEG decompressor
        jpeg_decompress_struct cinfo;
        jpeg_error_mgr jerr;
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

        // Specify the data source for decompression
        jpeg_stdio_src(&cinfo, infile);
        jpeg_read_header(&cinfo, TRUE);
        jpeg_start_decompress(&cinfo);

        // Allocate memory for the image
        std::vector<unsigned char> buffer(cinfo.output_width * cinfo.output_height * cinfo.output_components);
        while (cinfo.output_scanline < cinfo.output_height) {
            unsigned char* row_pointer = buffer.data() + cinfo.output_scanline * cinfo.output_width * cinfo.output_components;
            jpeg_read_scanlines(&cinfo, &row_pointer, 1);
        }

        // Clean up the decompressor
        jpeg_finish_decompress(&cinfo);
        fclose(infile);

        // Create and initialize the JPEG compressor
        jpeg_compress_struct cinfo_out;
        jpeg_error_mgr jerr_out;
        cinfo_out.err = jpeg_std_error(&jerr_out);
        jpeg_create_compress(&cinfo_out);

        // Open the output file
        FILE* outfile = fopen(outputFile.c_str(), "wb");
        if (!outfile) {
            throw std::runtime_error("Can't open output file: " + outputFile);
        }
        jpeg_stdio_dest(&cinfo_out, outfile);

        // Set compression parameters
        cinfo_out.image_width = cinfo.output_width;
        cinfo_out.image_height = cinfo.output_height;
        cinfo_out.input_components = cinfo.output_components;
        cinfo_out.in_color_space = cinfo.out_color_space;

        jpeg_set_defaults(&cinfo_out);
        jpeg_set_quality(&cinfo_out, quality, TRUE);

        // Start compression
        jpeg_start_compress(&cinfo_out, TRUE);
        while (cinfo_out.next_scanline < cinfo_out.image_height) {
            unsigned char* row_pointer = buffer.data() + cinfo_out.next_scanline * cinfo_out.image_width * cinfo_out.input_components;
            jpeg_write_scanlines(&cinfo_out, &row_pointer, 1);
        }

        // Finish compression
        jpeg_finish_compress(&cinfo_out);
        fclose(outfile);

        // Clean up
        jpeg_destroy_compress(&cinfo_out);
    }

private:
    std::string inputFile;
    std::string outputFile;
    int quality;
};

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "Usage: compressor.exe text <compress/decompress> <input_file> <output_file>" << endl;
        return 1;
    }

    string fileType = argv[1];
    string operation = argv[2];
    string inputFile = argv[3];
    string outputFile = argv[4];

    if (fileType == "text") {
        if (operation == "compress") {
            HuffmanCoding huffman;
            huffman.encodeToFile(inputFile, outputFile);
            cout << "Text file compression completed successfully!" << endl;

        } else if (operation == "decompress") {
            HuffmanCoding huffman;
            huffman.decodeFromFile(inputFile, outputFile);
            cout << "Text file decompression completed successfully!" << endl;
        } else {
            cerr << "Invalid operation. Use 'compress' or 'decompress'." << endl;
            return 1;
        }
    } else if (fileType == "jpeg") {
        string quality = argv[5];
               
        JPEGCompressor jpegCompressor(inputFile, outputFile, stoi(quality));
        jpegCompressor.compress();
        cout << "JPEG compression completed successfully!" << endl;
        long inputsize = getFileSize(inputFile);
        long outputsize = getFileSize(outputFile);
        cout<<"original file size:"<<inputsize<<" bytes"<<endl;
        cout<<"compressed file size:"<<outputsize<<" bytes"<<endl;
        double compressionPercentage = (static_cast<double>(outputsize) / inputsize) * 100;
        cout << "Compression Percentage: " << compressionPercentage << "%" << endl;
    } else {
        cerr << "Invalid file type. Use 'text' or 'jpeg'." << endl;
        return 1;
    }

    return 0;
}
