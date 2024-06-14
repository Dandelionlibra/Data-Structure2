#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <string>
#include <cstring>

// Graph structure
struct Graph {
    char putID[10] = {'\0'}; // Outdegree student id
    char getID[10] = {'\0'}; // Indegree student id
    float weight = 0.0; // (0,1] the weight of the edge

    // Comparison operator for sorting by weight in descending order
    bool operator<(const Graph& other) const {
        return weight > other.weight;
    }
};

// Read a chunk of data from the binary file
std::vector<Graph> readChunk(std::ifstream& inputFile, size_t chunkSize) {
    std::vector<Graph> buffer;
    buffer.reserve(chunkSize);
    Graph temp;
    while (buffer.size() < chunkSize && inputFile.read(reinterpret_cast<char*>(&temp), sizeof(Graph))) {
        buffer.push_back(temp);
    }
    return buffer;
}

// Write a chunk of data to a binary file
void writeChunk(const std::vector<Graph>& chunk, const std::string& fileName) {
    std::ofstream outputFile(fileName, std::ios::binary);
    for (const auto& graph : chunk) {
        outputFile.write(reinterpret_cast<const char*>(&graph), sizeof(Graph));
    }
}

// Merge two sorted files into one sorted file
void mergeFiles(const std::string& file1, const std::string& file2, const std::string& outputFile) {
    std::ifstream input1(file1, std::ios::binary);
    std::ifstream input2(file2, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);

    Graph graph1, graph2;
    bool hasGraph1 = input1.read(reinterpret_cast<char*>(&graph1), sizeof(Graph)).eof() == 0;
    bool hasGraph2 = input2.read(reinterpret_cast<char*>(&graph2), sizeof(Graph)).eof() == 0;

    while (hasGraph1 && hasGraph2) {
        if (graph1 < graph2) {
            output.write(reinterpret_cast<const char*>(&graph1), sizeof(Graph));
            hasGraph1 = input1.read(reinterpret_cast<char*>(&graph1), sizeof(Graph)).eof() == 0;
        } else {
            output.write(reinterpret_cast<const char*>(&graph2), sizeof(Graph));
            hasGraph2 = input2.read(reinterpret_cast<char*>(&graph2), sizeof(Graph)).eof() == 0;
        }
    }

    while (hasGraph1) {
        output.write(reinterpret_cast<const char*>(&graph1), sizeof(Graph));
        hasGraph1 = input1.read(reinterpret_cast<char*>(&graph1), sizeof(Graph)).eof() == 0;
    }

    while (hasGraph2) {
        output.write(reinterpret_cast<const char*>(&graph2), sizeof(Graph));
        hasGraph2 = input2.read(reinterpret_cast<char*>(&graph2), sizeof(Graph)).eof() == 0;
    }
}


// Split phase: read the input file and create sorted chunks
std::vector<std::string> splitAndSortChunks(const std::string& inputFile, size_t chunkSize) {
    std::ifstream input(inputFile, std::ios::binary);
    std::vector<std::string> chunkFiles;
    size_t chunkIndex = 0;

    while (input.peek() != EOF) {
        std::vector<Graph> chunk = readChunk(input, chunkSize);
        std::sort(chunk.begin(), chunk.end());
        std::string chunkFileName = "chunk_" + std::to_string(chunkIndex++) + ".bin";
        writeChunk(chunk, chunkFileName);
        chunkFiles.push_back(chunkFileName);
    }

    return chunkFiles;
}

// Merge phase: iteratively merge files until one sorted file remains
void mergeSortChunks(std::vector<std::string>& chunkFiles) {
    while (chunkFiles.size() > 1) {
        std::vector<std::string> newChunkFiles;
        for (size_t i = 0; i < chunkFiles.size(); i += 2) {
            if (i + 1 < chunkFiles.size()) {
                std::string mergedFileName = "merged_" + std::to_string(i / 2) + ".bin";
                mergeFiles(chunkFiles[i], chunkFiles[i + 1], mergedFileName);
                newChunkFiles.push_back(mergedFileName);
            } else {
                newChunkFiles.push_back(chunkFiles[i]);
            }
        }
        chunkFiles = std::move(newChunkFiles);
    }
}

int main() {
    const std::string inputFile = "pairs501.bin";
    const size_t chunkSize = 300; // Number of Graph entries per chunk

    // Step 1: Split and sort chunks
    std::vector<std::string> chunkFiles = splitAndSortChunks(inputFile, chunkSize);

    // Step 2: Merge sorted chunks
    mergeSortChunks(chunkFiles);

    if (!chunkFiles.empty()) {
        std::cout << "Sorted file: " << chunkFiles[0] << std::endl;
    } else {
        std::cerr << "No chunks found!" << std::endl;
    }

    return 0;
}