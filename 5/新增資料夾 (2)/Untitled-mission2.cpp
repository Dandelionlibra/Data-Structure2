#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Graph {
    char putID[10] = {'\0'}; // Outdegree student id
    char getID[10] = {'\0'}; // Indegree student id
    float weight = 0.0; // (0,1] the weight of the edge
};

void buildAndPrintIndex(const string& sortedFile) {
    ifstream inFile(sortedFile, ios::in | ios::binary);
    Graph record;
    streampos pos;
    int seq = 1;

    while (inFile.read(reinterpret_cast<char*>(&record), sizeof(Graph))) {
        pos = inFile.tellg();
        pos -= sizeof(Graph);
        cout << seq++ << ". Weight: " << record.weight << ", Offset: " << pos << endl;
    }

    inFile.close();
}

int main() {
    string sortedFile = "pairs.bin";

    // 计时开始
    auto start = high_resolution_clock::now();

    // 建立并输出索引
    buildAndPrintIndex(sortedFile);

    // 计时结束
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);

    cout << "Index building and printing completed in " << duration.count() << " seconds." << endl;

    return 0;
}