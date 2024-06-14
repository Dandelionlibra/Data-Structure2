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

class ExternalSort {
private:
    vector<Graph> gSet;
    vector<string> blockFiles;
    string fileID; // number to form a file name
    bool fileexist; // already enter a file or not
    int bufferSize; // buffer size for sorting
    // adjacencyList adjList;

    struct CompareNodes {
        bool operator()(MinHeapNode const& a, MinHeapNode const& b) {
            return a.element.weight < b.element.weight;
        }
        bool operator()(const Graph& a, const Graph& b) {
            return a.weight > b.weight;
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
    void splitAndSortBlocks(const string& fileName) {
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


    void mergeSortedBlocks(const string& outputFile) {
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
    }

public:
    ExternalSort() : fileexist(false), bufferSize(300) {}

    bool fileExist() { return fileexist; }

    bool readFile() {
        clearUp();
        do{
            cout << "\nInput a file number ([0] Quit): ";
            cin >> fileID;

            if(!fileID.compare("0")) // No file to open, so quit
                return false;
            else{
                existBinary("pairs" + fileID + ".bin");
                return fileexist;
            }

        } while(true); // end while


    }

    bool readFileName(){
        clearUp();
        cout << "\nInput a file number ([0] Quit): ";
        cin >> fileID;
        if(!fileID.compare("0")) // No file to open, so quit
            return false;
        return true;
    }

    bool existBinary(string fileName){
        fstream binFile;
        binFile.open(fileName.c_str(), fstream::in | fstream::binary); // open a binary file
        if(binFile.is_open()){
            fileexist = true;
        }
        else{
            cout << "\n### pairs" << fileID << ".bin does not exist! ###\n";
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

    void externalSort(){
        // enter a file name or quit
        if(readFileName() == false)
            return;

        // if the file does not exist
        if(!existBinary("pairs" + fileID + ".bin"))
            return;

        // start time
        auto start = chrono::high_resolution_clock::now();

        // split and sort blocks
        splitAndSortBlocks("pairs" + fileID + ".bin");
        mergeSortedBlocks("order"+fileID+".bin");

        // remove block files
        for (const auto& filename : blockFiles)
            remove(filename.c_str());
        
        // end time
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start); // calculate the elapsed time

        cout << "External sort completed in " << duration.count() << " ms." << endl;
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
    ~ExternalSort(){ clearUp(); }
};



int main(){
    ExternalSort glist;
    int command = -1;
    bool isEnd = false;
    string input;
    while (!isEnd) {
    	cout << endl << "**** Graph data manipulation *****" ;
        cout << endl << "* 0. QUIT                        *" ;
        cout << endl << "* 1. external sort       *" ;
        cout << endl << "* 2. primary index   *" ;
        // cout << endl << "* 3. Estimate influence values   *" ;
        // cout << endl << "* 4. Probability-based influence *" ;
        cout << endl << "**********************************" ;
        cout << endl << "Input a choice(0, 1, 2): " ;
        // cout << endl << "Input a choice(0, 1, 2, 3, 4): " ;
        cin >> input;

        if (input == "0")
            command = 0;
        else if (input == "1")
            command = 1;
        else if (input == "2")
            command = 2;
        /*else if (input == "3")
            command = 3;
        else if (input == "4")
            command = 4;*/

        else
            command = -1;

        switch (command) {
            case 0:
                isEnd = true;
                break;
            case 1:
                if (glist.readFile()) { // if file exist
                    cout << "external sort" << endl;
                    if(glist.fileExist())
                        glist.externalSort();

                    // glist.showData();
                    
                }
                break;
            case 2:
                if (glist.fileExist()) {
                    cout << "primary index" << endl;
                    
                }
                else
                    cout << "### There is no graph and choose 1 first. ###" << endl;
                break;
            /*case 3:
                if (glist.fileExist()) {
                    
                }
                else
                    cout << "### There is no graph and choose 1 first. ###" << endl;
                break;
            case 4:
                if (glist.fileExist()) {
                    // cout << "probability point\n";
                    // glist.random_influence();
                }
                else
                    cout << "### There is no graph and choose 1 first. ###" << endl;
                break;*/
            default:
                cout << "\nCommand does not exist!" << endl;
                break;
        } // end switch case
    } // end while

    return 0;
}
