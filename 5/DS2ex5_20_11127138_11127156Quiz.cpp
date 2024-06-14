// 11127138 林雨臻
// 11127156 王佩諭
#include <cstdlib>  // atoi, system, strtol
#include <fstream>  // open, is_open
#include <iomanip>  // setw, setprecision
#include <iostream> // cout, endl, fixed
#include <string>   // string
#include <cstring>   // strcpy
#include <vector>   // vector
#include <algorithm> // find, binary_search
#include <unordered_map> // unordered_map
#include <queue> // queue
#include <cmath>
#include <chrono>
#include <map>
using namespace std;

struct Graph {
    char putID[10] = {'\0'}; // Outdegree student id
    char getID[10] = {'\0'}; // Indegree student id
    float weight = 0.0; // (0,1] the weight of the edge
};

struct WithIndex {
    Graph graph;
    int originalIndex;
};

class Sort {
private:
    vector<Graph> gSet;
    vector<string> blockFiles;
    map<float, int, greater<float>> indexMap;
    int dataSize;
    string fileID; // number to form a file name
    bool fileexist; // already enter a file or not
    int bufferSize; // buffer size for sorting

    chrono::microseconds InternalTime; // calculate the elapsed time
    chrono::microseconds ExternalTime; // calculate the elapsed time


    struct CompareNodes {
        bool operator()(const WithIndex& a, const WithIndex& b) {
            if (a.graph.weight == b.graph.weight) { // keep stability when sorting
                return a.originalIndex < b.originalIndex;
            }
            return a.graph.weight > b.graph.weight;
        }
    };

    void writeBlock(const vector<Graph>& block, const string& filename) {
        ofstream outFile(filename, ios::out | ios::binary);
        for (const auto& record : block)
            outFile.write(reinterpret_cast<const char*>(&record), sizeof(Graph));
        outFile.close();
    }

    // read binary file and split it into sorted blocks
    void split(const string& fileName) {
        FILE* inFile = fopen(fileName.c_str(), "rb");
        vector<WithIndex> buffer;
        buffer.reserve(bufferSize); // reserve buffer size for sorting
        Graph oneRecord;
        int blockIndex = 0;
        int recordIndex = 0;

        while (fread(&oneRecord, sizeof(Graph), 1, inFile)) {
            buffer.push_back({oneRecord, recordIndex++});
            dataSize++;
            if (buffer.size() == bufferSize) {
                stable_sort(buffer.begin(), buffer.end(), CompareNodes());
                vector<Graph> block;
                for (const auto& elem : buffer)
                    block.push_back(elem.graph);

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

        fclose(inFile);
    }

    void mergeFiles(string file1, string file2, string outputFile){
        FILE* inFile1 = fopen(file1.c_str(), "rb");
        FILE* inFile2 = fopen(file2.c_str(), "rb");
        FILE* outFile = fopen(outputFile.c_str(), "wb");

        Graph data1, data2;
        bool hasData1 = fread(&data1, sizeof(Graph), 1, inFile1);
        bool hasData2 = fread(&data2, sizeof(Graph), 1, inFile2);

        vector<Graph> buffer(bufferSize);
        int bufferIndex = 0;

        while (hasData1 && hasData2) {
            if (data1.weight >= data2.weight) {
                buffer[bufferIndex++] = data1;
                hasData1 = fread(&data1, sizeof(Graph), 1, inFile1);
            } else {
                buffer[bufferIndex++] = data2;
                hasData2 = fread(&data2, sizeof(Graph), 1, inFile2);
            }
            if (bufferIndex == bufferSize) {
                fwrite(buffer.data(), sizeof(Graph), bufferIndex, outFile);
                bufferIndex = 0;
            }
        }

        // put the remaining records into the output file
        while (hasData1) {
            buffer[bufferIndex++] = data1;
            if (bufferIndex == bufferSize) {
                fwrite(buffer.data(), sizeof(Graph), bufferIndex, outFile);
                bufferIndex = 0;
            }
            hasData1 = fread(&data1, sizeof(Graph), 1, inFile1);
        }

        while (hasData2) {
            buffer[bufferIndex++] = data2;
            if (bufferIndex == bufferSize) {
                fwrite(buffer.data(), sizeof(Graph), bufferIndex, outFile);
                bufferIndex = 0;
            }
            hasData2 = fread(&data2, sizeof(Graph), 1, inFile2);
        }

        // put the remaining records into the output file
        if (bufferIndex > 0)
            fwrite(buffer.data(), sizeof(Graph), bufferIndex, outFile);

        fclose(inFile1);
        fclose(inFile2);
        fclose(outFile);
    }

    void merge(const string& outputFile) {
        cout << endl << "The internal sort is completed. Check the initial sorted runs! \n" << endl;
        // print the initial runs
        cout << "Now there are " << blockFiles.size() << " runs.\n\n";

        while(blockFiles.size() > 1){
            vector<string> newBlockFiles;
            bool odd = false; // check if the number of runs is odd
            for (int i = 0 ; i < blockFiles.size(); i = i+2) {
                // even number of runs
                if (i + 1 < blockFiles.size()) {
                    // make different file name for merged file
                    string mergedFileName = to_string(blockFiles.size())+"_merged_" + to_string(i / 2) + ".bin";
                    mergeFiles(blockFiles[i], blockFiles[i + 1], mergedFileName);
                    newBlockFiles.push_back(mergedFileName);
                }
                // odd number of runs
                else {
                    newBlockFiles.push_back(blockFiles[i]);
                    odd = true;
                }
            }

            // remove block files
            for(int filename = 0 ; filename < blockFiles.size() ; filename++){
                if(odd && filename == blockFiles.size()-1) // if the number of runs is even, the last run is not removed
                    break;;
                remove(blockFiles[filename].c_str());
            }

            blockFiles = newBlockFiles; // update block files

            cout << "Now there are " << blockFiles.size() << " runs.\n\n";
        }

        // check the output filename exists or not
        if (rename(blockFiles[0].c_str(), outputFile.c_str()) != 0){
            remove(outputFile.c_str());
        }

        // rename the last merged file to the output file
        rename(blockFiles[0].c_str(), outputFile.c_str());

    }

    void buildAndPrintIndex(const string& orderFile) {
        FILE* inFile = fopen(orderFile.c_str(), "rb");
        Graph record;
        float last = -1;
        int pos = 0;
        int idx = 1;

        cout << "\n<Primary index>: (key, offset)" << endl;
        while (fread(&record, sizeof(Graph), 1, inFile)) {
            if(last!=record.weight){
                last = record.weight;
                cout << "[" << idx << "] (" << record.weight << ", " << pos << ")" << endl;
                indexMap.insert(pair<float, int>(record.weight, pos));
                idx++;
            }
            pos++;
        }

        fclose(inFile);
    }

    void searchPrimy ( float threshold, const string& orderFile ) {
        map<int, Graph> searchMap;
        FILE* inFile = fopen(orderFile.c_str(), "rb");
        Graph record;
        int pos = 0;
        int records = 0;
        auto start = chrono::high_resolution_clock::now();
        for (const auto& pair : indexMap) {
            if (pair.first >= threshold) {
                for ( ; pos < pair.second  ; pos++ ) {
                    fread(&record, sizeof(Graph), 1, inFile);
                }
                fread(&record, sizeof(Graph), 1, inFile);
                pos = pos + 1;
                // cout << "[" << pair.second + 1 << "] " << record.putID << " " << record.getID << " " << record.weight << endl;
                searchMap.insert({pair.second + 1, record});
            } // if
            else {
                if ( threshold > 0.01 )
                    records = pair.second;
                break;
            } // else
        } // for


        if ( threshold <= 0.01 )
            records = dataSize;
        auto end = chrono::high_resolution_clock::now();
        chrono::microseconds SearchTime = chrono::duration_cast<chrono::microseconds>(end - start);
        writeFile(searchMap, threshold, records, float(SearchTime.count())/1000 );

    }

    void writeFile(const map<int, Graph>& searchMap, float threshold, int records, float time ) {
        string filename = "order" + fileID + ".txt";
        ofstream outFile(filename);

        if (!outFile) {
            return;
        }

        outFile << "Search " << threshold << " : " << records << " recods, "
                << time << " ms" << endl;  ;
        // outFile << fixed << setprecision(3) << float(SearchTime.count())/1000 << " ms" << endl;

        cout.unsetf(ios::fixed);
        cout.precision(6);


        for (const auto& pair : searchMap) {
            if ( pair.first < 10 )
                outFile << "[ " << pair.first << "] ";
            else
                outFile << "[" << pair.first << "] " ;

            outFile << pair.second.putID << " "
                    << pair.second.getID << " " << pair.second.weight << endl;
        }

        outFile.close();
    }
public:
    Sort() : fileexist(false), bufferSize(300), dataSize(0) {}

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
        if(binFile.is_open())
            fileexist = true;
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

        auto end = chrono::high_resolution_clock::now();
        ExternalTime = chrono::duration_cast<chrono::microseconds>(end - start); // calculate the elapsed time
    }


    void internalSort(){
        // start count internal sort time
        auto start = chrono::high_resolution_clock::now();
        split("pairs" + fileID + ".bin");
        auto end = chrono::high_resolution_clock::now();
        InternalTime = chrono::duration_cast<chrono::microseconds>(end - start); // calculate the elapsed time
    }

    void Mission_1(){
        internalSort(); // perform the internal sort

        externalSort(); // perform the external sort

        printTime(); // print the execution time
    }

    void Mission_2(){
        string orderFile = "order" + fileID + ".bin";
        buildAndPrintIndex(orderFile);
    }

    void Mission_3( float threshold ){
        string orderFile = "order" + fileID + ".bin";
        searchPrimy(threshold, orderFile);
    }


    void printTime(){
        cout << "The execution time ..." << endl;
        cout << "Internal Sort = " << fixed << setprecision(3) << float(InternalTime.count())/1000 << " ms" << endl;
        cout << "External Sort = " << float(ExternalTime.count())/1000 << " ms" << endl;
        cout << "Total Execution Time = " << float(InternalTime.count() + ExternalTime.count())/1000 << " ms" << endl ;
        cout << "\n";
        cout.unsetf(ios::fixed);
    }

    void settingBuffer(){
        string input;
        cout << endl << "*** The buffer size is " << bufferSize;

        do{
            cout << endl << "Input a number in [1, 60000]: ";
            cin >> input;

            bool isNumber = true;
            for(int i = 0; i < input.length(); i++){
                if(!isdigit(input[i])){
                    isNumber = false;
                    break;
                }
            }
            if(!isNumber)
                continue;
            else if(input.length() > 5){
                cout << "\n### It is NOT in [1,60000] ###\n";
                continue;
            }
            else
                bufferSize = stoi(input);


            // cout << "\033[1;31mBuffer size: " << bufferSize << "\033[0m" << endl;
            if(bufferSize < 1 || bufferSize > 60000){
                cout << "\n### It is NOT in [1,60000] ###\n";
                continue;
            }
            else
                break;


        } while(true);
    }

    void clearUp(){
        fileexist = false;
        fileID.clear();
        gSet.clear();
        blockFiles.clear();
        // index.clear();
        indexMap.clear();
        dataSize = 0;
    }
    ~Sort(){ clearUp(); }
};

int main(){
    Sort glist;
    bool continueFlag = true;
    bool fileExistFlag = true;
    string input;

    while (continueFlag) {
    	glist.settingBuffer();

    	cout << endl << "***********************************************" ;
    	cout << endl << " On-machine Exercise 05                       *" ;
    	cout << endl << " Mission 1: External Merge Sort on a Big File *" ;
    	cout << endl << " Mission 2: Construction of Primary Index     *" ;
    	cout << endl << " Mission 3: Range Search by Primary Index     *" ;
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
            cout << endl << "Mission 2: Build the primary index ";
            cout << endl << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
            cout << endl;
            glist.Mission_2();

            cout << endl << endl << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
            cout << endl << "Mission 3: Threshold search on primary index ";
            cout << endl << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;;

            float threshold = 0;
            while ( continueFlag ) {

                cout << endl << "Please input a threshold in the range [0,1]: " << endl;
                cin >> threshold;

                if ( cin.fail() ) {
                    cout << endl << "Invalid input. Please enter a number." << endl;
                    cin.clear(); // 清除错误状态
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 清理缓冲区
                }
                else if (threshold > 1 || threshold <= 0)
                    cout << endl << "The threshold is not in the range [0,1]!" << endl;
                else
                   glist.Mission_3( threshold );

                cout << "\n[0]Quit or [Any other key]continue?" << endl;
                cin >> input;
                if(input == "0")
                    continueFlag = false;
            } // while

        } // if

        cout << "\n[0]Quit or [Any other key]continue?" << endl;
        cin >> input;

        if(input == "0")
            continueFlag = false;
        else
            continueFlag = true;


    } // end while

    return 0;
}
