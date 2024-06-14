// 11127138 林雨臻
// 11127156 王佩諭
#include <cstdlib>  // atoi, system
#include <fstream>  // open, is_open
#include <iomanip>  // setw, setprecision
#include <iostream> // cout, endl, fixed
#include <string>   // string
#include <cstring>   // strcpy
#include <vector>   // vector
#include <algorithm> // find, binary_search
#include <map> // map
#include <unordered_map> // unordered_map
#include <set> // set
#include <unordered_set> // unordered_set
#include <queue> // queue
#include <stack> // stack
#include <chrono>
using namespace std;



struct Graph {
    char putID[10] = {'\0'}; // Outdegree student id
    char getID[10] = {'\0'}; // Indegree student id
    float weight = 0.0; // (0,1] the weight of the edge
};
struct MinHeapNode {
    Graph element;
    int fileIndex;
};

struct WithIndex {
    Graph graph;
    int originalIndex;
};

class Sort {
private:
    vector<Graph> gSet;
    vector<string> blockFiles;
    string fileID; // number to form a file name
    bool fileexist; // already enter a file or not
    int bufferSize; // buffer size for sorting

    chrono::milliseconds InternalTime; // calculate the elapsed time
    chrono::milliseconds ExternalTime; // calculate the elapsed time
    // adjacencyList adjList;

    struct CompareNodes {
        bool operator()(MinHeapNode const& a, MinHeapNode const& b) {
            return a.element.weight < b.element.weight; // 按weight由大到小排序
        }
        /*bool operator()(const Graph& a, const Graph& b) {
            return a.weight > b.weight;
        }*/
        bool operator()(const WithIndex& a, const WithIndex& b) {
            if (a.graph.weight == b.graph.weight) {
                return a.originalIndex < b.originalIndex; // 保持穩定性
            }
            return a.graph.weight > b.graph.weight;
        }
    };
    
    void writeBlock(const vector<Graph>& block, const string& filename) {
        ofstream outFile(filename, ios::out | ios::binary);
        for (const auto& record : block) {
            outFile.write(reinterpret_cast<const char*>(&record), sizeof(Graph));
        }
        outFile.close();
    }

    // read binary file and split it into sorted blocks
    /*void splitAndSortBlocks(const string& fileName) {
        ifstream inFile(fileName, ios::in | ios::binary);
        vector<Graph> buffer;
        buffer.reserve(bufferSize);
        Graph oneRecord;
        size_t blockIndex = 0;

        if(!inFile.is_open()){
            cout << "\n### " << fileName << " does not exist! ###\n";
            fileexist = false;
            return;
        }
        else
            fileexist = true;
        
        while (inFile.read(reinterpret_cast<char*>(&oneRecord), sizeof(Graph))) {
            buffer.push_back(oneRecord);
            if (buffer.size() == bufferSize) {
                sort(buffer.begin(), buffer.end(), CompareNodes());
                string blockFileName = "Block" + to_string(blockIndex++) + ".bin";
                writeBlock(buffer, blockFileName);
                blockFiles.push_back(blockFileName);
                buffer.clear();
            }
        }

        // deal with the last incomplete block
        if (!buffer.empty()) {
            sort(buffer.begin(), buffer.end(), CompareNodes());
            string blockFileName = "Block" + to_string(blockIndex++) + ".bin";
            writeBlock(buffer, blockFileName);
            blockFiles.push_back(blockFileName);
        }

        inFile.close();
    }

    */

    void split(const string& fileName) {
        fstream inFile;
        inFile.open(fileName.c_str(), fstream::in | fstream::binary);
        vector<WithIndex> buffer;
        buffer.reserve(bufferSize); // reserve buffer size for sorting
        Graph oneRecord;
        int blockIndex = 0;
        int recordIndex = 0;

        while (inFile.read(reinterpret_cast<char*>(&oneRecord), sizeof(Graph))) {
            buffer.push_back({oneRecord, recordIndex++});
            if (buffer.size() == bufferSize) {
                stable_sort(buffer.begin(), buffer.end(), CompareNodes());
                vector<Graph> block;
                block.reserve(bufferSize);
                for (const auto& elem : buffer) {
                    block.push_back(elem.graph);
                }
                string blockFileName = "block" + to_string(blockIndex++) + ".bin";
                writeBlock(block, blockFileName);
                blockFiles.push_back(blockFileName);
                buffer.clear();
            }
        }

        // deal with the last incomplete block
        if (!buffer.empty()) {
            stable_sort(buffer.begin(), buffer.end(), CompareNodes());
            vector<Graph> block;
            block.reserve(buffer.size());
            for (const auto& elem : buffer) {
                block.push_back(elem.graph);
            }
            string blockFileName = "block" + to_string(blockIndex++) + ".bin";
            writeBlock(block, blockFileName);
            blockFiles.push_back(blockFileName);
        }

        inFile.close();
    }



    void merge(const string& outputFile) {
        priority_queue<MinHeapNode, vector<MinHeapNode>, CompareNodes> minHeap;
        vector<ifstream> blockStreams(blockFiles.size());
        ofstream outFile(outputFile, ios::out | ios::binary);
        vector<Graph> buffer(bufferSize);
        int bufferIndex = 0;

        // Open all block files and read the first element
        for (int i = 0; i < blockFiles.size(); ++i) {
            blockStreams[i].open(blockFiles[i], ios::in | ios::binary);
            Graph element;
            if (blockStreams[i].read(reinterpret_cast<char*>(&element), sizeof(Graph))) {
                minHeap.push({element, static_cast<int>(i)});
            }
        }

        // Merge all files
        while (!minHeap.empty()) {
            MinHeapNode minNode = minHeap.top();
            minHeap.pop();

            buffer[bufferIndex++] = minNode.element;
            if (bufferIndex == bufferSize) {
                outFile.write(reinterpret_cast<char*>(buffer.data()), bufferSize * sizeof(Graph));
                bufferIndex = 0;
            }

            if (blockStreams[minNode.fileIndex].read(reinterpret_cast<char*>(&minNode.element), sizeof(Graph))) {
                minHeap.push(minNode);
            }
        }

        // Write the remaining data to the output file
        if (bufferIndex > 0) {
            outFile.write(reinterpret_cast<char*>(buffer.data()), bufferIndex * sizeof(Graph));
        }

        // Close all file streams
        for (auto& stream : blockStreams) {
            stream.close();
        }
        outFile.close();

        cout << "The sorted file is saved as " << outputFile << endl;
    }

    /*
    void mergeSortedBlocks(const string& outputFile) {
        priority_queue<MinHeapNode, vector<MinHeapNode>, CompareNodes> minHeap;
        vector<ifstream> chunkStreams(blockFiles.size());
        ofstream outFile(outputFile, ios::out | ios::binary);
        vector<Graph> buffer(bufferSize);
        int bufferIndex = 0;

        // 打開所有chunk文件並讀取第一個元素
        for (int i = 0; i < blockFiles.size(); ++i) {
            chunkStreams[i].open(blockFiles[i], ios::in | ios::binary);
            Graph element;
            if (chunkStreams[i].read(reinterpret_cast<char*>(&element), sizeof(Graph))) {
                minHeap.push({element, static_cast<int>(i)});
            }
        }

        // 合併所有文件
        while (!minHeap.empty()) {
            MinHeapNode minNode = minHeap.top();
            minHeap.pop();

            buffer[bufferIndex++] = minNode.graph;
            if (bufferIndex == bufferSize) {
                outFile.write(reinterpret_cast<char*>(buffer.data()), bufferSize * sizeof(Graph));
                bufferIndex = 0;
            }

            if (chunkStreams[minNode.fileIndex].read(reinterpret_cast<char*>(&minNode.graph), sizeof(Graph))) {
                minHeap.push(minNode);
            }
        }

        // 將剩餘的數據寫入輸出文件
        if (bufferIndex > 0) {
            outFile.write(reinterpret_cast<char*>(buffer.data()), bufferIndex * sizeof(Graph));
        }

        // 關閉所有文件流
        for (auto& stream : chunkStreams) {
            stream.close();
        }
        outFile.close();
    }*/
    


public:
    Sort() : fileexist(false), bufferSize(300) {}

    bool fileExist() { return fileexist; }

    bool readFile() {
        clearUp();
        do{
            cout << "\nInput the file name: [0]Quit\n";
            cin >> fileID;

            if(!fileID.compare("0")) // No file to open, so quit
                return false;
            else{
                existBinary("pairs" + fileID + ".bin");
                return true;
            }

        } while(true); // end while


    }

    bool existBinary(string fileName){
        fstream binFile;
        binFile.open(fileName.c_str(), fstream::in | fstream::binary); // open a binary file
        if(binFile.is_open()){
            fileexist = true;
        }
        else{
            cout << "\npairs" << fileID << ".bin does not exist!!!\n";                                                                                                          
            fileexist = false; // file does not exist
        }
        binFile.close(); // close input file

        return fileexist;
    }

    /*void readBinary(string fileName) {
        fstream binFile; // input file handle
        Graph oneSt; // keep one record with the required fields
        int stNo = 0; // total number of students

        binFile.open(fileName.c_str(), fstream::in | fstream::binary); // open a binary file
        if(binFile.is_open()){
            fileexist = true; // file exist

            binFile.seekg(0, binFile.end); // get the start to end position
            stNo = binFile.tellg() / sizeof(oneSt);
            binFile.seekg(0, binFile.beg);
            for(int i = 0 ; i < stNo ; i++){
                binFile.read((char*)&oneSt, sizeof(oneSt)); // read the entire record from a binary file

                gSet.push_back(oneSt); // keep the record in the vector
            } // end for
            cout << endl;
        } // end if
        else{
            cout << "\n### pairs" << fileID << ".bin does not exist! ###\n";
            fileexist = false; // file does not exist
        }
        binFile.close(); // close input file
    }*/

    bool readFileName(){
        // clearUp();
        cout << "\nInput the file name: [0]Quit\n";
        cin >> fileID;
        if(!fileID.compare("0")) // No file to open, so quit
            return false;
        return true;
    }

    void externalSort(){
        // start count external sort time
        auto start = chrono::high_resolution_clock::now();
        merge("order" + fileID + ".bin");

        // remove block files
        // for (const auto& filename : blockFiles)
            // remove(filename.c_str());
        auto end = chrono::high_resolution_clock::now();
        ExternalTime = chrono::duration_cast<chrono::milliseconds>(end - start); // calculate the elapsed time
        
    }


    void internalSort(){
        // start count internal sort time
        auto start = chrono::high_resolution_clock::now();
        split("pairs" + fileID + ".bin");
        auto end = chrono::high_resolution_clock::now();
        InternalTime = chrono::duration_cast<chrono::milliseconds>(end - start); // calculate the elapsed time

    }
    void buildAndPrintIndex(const string& orderFile) {
        ifstream inFile(orderFile, ios::in | ios::binary);
        Graph record;
        // streampos pos;
        float last = -1;
        int pos = 0;
        int seq = 1;

        cout << "\n<Primary index>: (key, offset)" << endl;
        while (inFile.read(reinterpret_cast<char*>(&record), sizeof(Graph))) {
            // pos = inFile.tellg();
            // pos -= sizeof(Graph);
            if(last!=record.weight){
                last = record.weight;
                cout << "[" << seq << "] (" << record.weight << ", " << pos << ")" << endl;
                seq++;
                // cout << "change\n" << endl;
                
                // cout << "last: " << last << " record.weight: " << record.weight << " pos: " << pos << endl;
            }

            // system("pause");
            // cout << "last: " << last << " record.weight: " << record.weight << " pos: " << pos << endl;
            
            pos++;
            
        }

        inFile.close();
    }
    void Mission_2(){
        string orderFile = "order" + fileID + ".bin";
        buildAndPrintIndex(orderFile);
    }

    void printTime(){
        cout << "The execution time ..." << endl;
        cout << "Internal Sort = " << InternalTime.count() << " ms" << endl;
        cout << "External Sort = " << ExternalTime.count() << " ms" << endl;
        cout << "Total Execution Time = " << InternalTime.count() + ExternalTime.count() << " ms" << endl ;
        cout << "\n";
    }

    void showData(){
        cout << "\033[1;31mshowData: \n\033[0m";
        for (int i = 0; i < gSet.size(); i++){
            cout << gSet[i].putID << " " << gSet[i].getID << " " << gSet[i].weight << endl;
        }

        writetxt("graph" + fileID + ".txt");
    }

    void writetxt(string fileName){
        ofstream txtFile;
        txtFile.open(fileName.c_str(), ios::out);
        if(txtFile.is_open()){
            for(int i = 0 ; i < gSet.size() ; i++){
                txtFile << gSet[i].putID << " " << gSet[i].getID << " " << gSet[i].weight << endl;
            }
            cout << "\n### " << fileName << " is created! ###\n";
        }
        else
            cout << "\n### " << fileName << " cannot be created! ###\n";
        txtFile.close();
    }

    void clearUp(){
        fileexist = false;
        fileID.clear();
        gSet.clear();
        // adjList.clearUp(); // clear the adjacency list
    }
    ~Sort(){ clearUp(); }
};



int main(){
    Sort glist;
    // int command = -1;
    bool continueFlag = true;
    bool fileExistFlag = true;
    string input;
    
    while (continueFlag) {
    	cout << endl << "*** The buffer size is 300" ;
    	cout << endl << "***********************************************" ;
    	cout << endl << " On-machine Exercise 05                       *" ;
    	cout << endl << " Mission 1: External Merge Sort on a Big File *" ;
    	cout << endl << " Mission 2: Construction of Primary Index     *" ;
    	cout << endl << "***********************************************" ;

        cout << endl << "########################################################" ;
        cout << endl << "Mission 1: External merge sort " ;
        cout << endl << "########################################################" << endl ;


        do{
            continueFlag  = glist.readFile();
            fileExistFlag = glist.fileExist();
        } while(!fileExistFlag && continueFlag);



        if(fileExistFlag){
            // cout << "\033[1;31mstart internal sort ...\033[0m" << endl;

            // glist.Mission_1();

            glist.internalSort();
            cout << "The internal sort is completed. Check the initial sorted runs! " << endl;
            glist.externalSort();

            cout << endl;
            glist.printTime();


            cout << endl << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
            cout << endl << "Mission 2: Build the primary index";
            cout << endl << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";

            cout << endl;

            glist.Mission_2();




        }


        
        cout << "\n[0]Quit or [Any other key]continue?" << endl;
        cin >> input;
        if(input == "0")
            continueFlag = false;
        else
            continueFlag = true;
        

    } // end while

    return 0;
}
