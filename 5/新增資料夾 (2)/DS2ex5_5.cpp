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
#include <queue> // queue
#include <cmath>
#include <chrono>
using namespace std;

struct Graph {
    char putID[10] = {'\0'}; // Outdegree student id
    char getID[10] = {'\0'}; // Indegree student id
    float weight = 0.0; // (0,1] the weight of the edge
};
/*struct HeapNode {
    Graph element;
    int fileIndex;
};*/

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


    struct CompareNodes {
        /*bool operator()(HeapNode const& a, HeapNode const& b) {
            if (a.element.weight == b.element.weight) // keep stability when sorting
                return a.fileIndex > b.fileIndex;
            return a.element.weight < b.element.weight; // according to the weight sorting
        }*/
        bool operator()(const WithIndex& a, const WithIndex& b) {
            if (a.graph.weight == b.graph.weight) { // keep stability when sorting
                return a.originalIndex < b.originalIndex;
                // return string(a.graph.putID).compare(string(b.graph.putID)) < 0;
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

    void mergeFiles(string file1, string file2, string outputFile){
        ifstream input1(file1, std::ios::binary);
        ifstream input2(file2, std::ios::binary);
        ofstream output(outputFile, std::ios::binary);
        Graph graph1, graph2;
        
        bool hasGraph1 = input1.read(reinterpret_cast<char*>(&graph1), sizeof(Graph)).eof() == 0;
        bool hasGraph2 = input2.read(reinterpret_cast<char*>(&graph2), sizeof(Graph)).eof() == 0;
        while (hasGraph1 && hasGraph2) {
            // cout << "graph1.weight: " << graph1.weight << " graph2.weight: " << graph2.weight << endl;
            // system("pause");

            if (graph1.weight >= graph2.weight) {
                // cout << "\033[1;33mput graph1\n\033[0m" << endl; // yellow
                output.write(reinterpret_cast<const char*>(&graph1), sizeof(Graph));
                hasGraph1 = input1.read(reinterpret_cast<char*>(&graph1), sizeof(Graph)).eof() == 0;
            } else {
                // cout << "\033[1;32mput graph2\n\033[0m" << endl; // green
                output.write(reinterpret_cast<const char*>(&graph2), sizeof(Graph));
                hasGraph2 = input2.read(reinterpret_cast<char*>(&graph2), sizeof(Graph)).eof() == 0;
            }
        }

        // put the remaining records into the output file
        while (hasGraph1) {
            output.write(reinterpret_cast<const char*>(&graph1), sizeof(Graph));
            hasGraph1 = input1.read(reinterpret_cast<char*>(&graph1), sizeof(Graph)).eof() == 0;
        }
        while (hasGraph2) {
            output.write(reinterpret_cast<const char*>(&graph2), sizeof(Graph));
            hasGraph2 = input2.read(reinterpret_cast<char*>(&graph2), sizeof(Graph)).eof() == 0;
        }

        input1.close();
        input2.close();
        output.close();
    }

    void merge(const string& outputFile) {
        while(blockFiles.size() > 1){
            vector<string> newBlockFiles;

            // print the current runs
            cout << "Now there are " << blockFiles.size() << " runs.\n\n";
            int i;
            bool even = false; // check if the number of runs is even
            for (i = 0 ; i < blockFiles.size(); i += 2) {
                if (i + 1 < blockFiles.size()) {
                    // make different file name for merged file
                    string mergedFileName = to_string(blockFiles.size())+"_merged_" + to_string(i / 2) + ".bin";

                    // cout << "Merge " << blockFiles[i] << " and " << blockFiles[i + 1] << " into " << mergedFileName << endl;
                    mergeFiles(blockFiles[i], blockFiles[i + 1], mergedFileName);

                    // system("pause");
                    newBlockFiles.push_back(mergedFileName);
                } else {
                    // cout << "\033[1;33mput " << blockFiles[i] << " into newBlockFiles\033[0m" << endl;
                    newBlockFiles.push_back(blockFiles[i]);
                    even = true;
                }
            }

            // remove block files
            for(int filename = 0 ; filename < blockFiles.size() ; filename++){
                if(even && filename == blockFiles.size()-1) // if the number of runs is even, the last run is not removed
                    break;;
                // cout << "\033[1;31mremove: " << blockFiles[filename] << "\033[0m" << endl;
                remove(blockFiles[filename].c_str());
            }

            blockFiles = newBlockFiles; // update block files
        }
        // print the final run
        cout << "Now there are " << blockFiles.size() << " runs.\n\n";

        // rename the last merged file
        rename(blockFiles[0].c_str(), outputFile.c_str());


        
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
            if(last!=record.weight){
                last = record.weight;
                cout << "[" << seq << "] (" << record.weight << ", " << pos << ")" << endl;
                seq++;

                // cout << "\033[1;31mchange\n\033[0m" << endl;
                // cout << "last: " << last << " record.weight: " << record.weight << " pos: " << pos << endl;
            }

            // system("pause");
            // cout << "last: " << last << " record.weight: " << record.weight << " pos: " << pos << endl;
            
            pos++;
            
        }

        inFile.close();
    }
    
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

    void externalSort(){
        // start count external sort time
        auto start = chrono::high_resolution_clock::now();

        merge("order" + fileID + ".bin");

        // remove block files
        // for (const auto& filename : blockFiles)
            // cout << "remove: " << filename << endl;
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


    void Mission_1(){
        // perform the internal sort
        internalSort();
        cout << endl << "The internal sort is completed. Check the initial sorted runs! \n" << endl;

        // perform the external sort
        externalSort();

        printTime();

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
        blockFiles.clear();
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
            glist.Mission_1();

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
