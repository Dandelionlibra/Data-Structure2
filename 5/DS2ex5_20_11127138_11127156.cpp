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
    // vector<pair<float, int>> index;
    unordered_map<float, pair<Graph, int>> indexMap; // index
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
        bool operator()(float a, float b) {
            return a > b;
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
                // cout << "\033[1;32mput graph2\n\033[0m" << endl; // green
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
                    // newBlockFiles.insert(newBlockFiles.begin(), blockFiles[i]);
                    odd = true;
                }
            }

            // remove block files
            for(int filename = 0 ; filename < blockFiles.size() ; filename++){
                if(odd && filename == blockFiles.size()-1) // if the number of runs is even, the last run is not removed
                    break;;
                // cout << "\033[1;31mremove: " << blockFiles[filename] << "\033[0m" << endl;
                remove(blockFiles[filename].c_str());
            }

            blockFiles = newBlockFiles; // update block files

            cout << "Now there are " << blockFiles.size() << " runs.\n\n";
        }

        // check the output filename exists or not
        if (rename(blockFiles[0].c_str(), outputFile.c_str()) != 0){
            // cout << "\033[1;31mrename failed\n\033[0m";
            remove(outputFile.c_str());
        }
        
        // rename the last merged file to the output file
        rename(blockFiles[0].c_str(), outputFile.c_str());

    }

    void buildAndPrintIndex(const string& orderFile) {
        ifstream inFile(orderFile, ios::in | ios::binary);

        // FILE* inFile = fopen(orderFile.c_str(), "rb");
        Graph record;
        float last = -1;
        int pos = 0;
        int idx = 1;

        cout << "\n<Primary index>: (key, offset)" << endl;
        while (inFile.read(reinterpret_cast<char*>(&record), sizeof(Graph))) { 
            
            
            
            if(last!=record.weight){
                last = record.weight;

                cout << "[" << idx << "] (" << record.weight << ", " << pos << ")" << endl;
                // index.push_back({record.weight, pos});

                // indexMap.insert(pair<float, pair>(record.weight, make_pair(record, pos)));
                
                // cout << "\033[1;33m[" << idx << "] (" << record.putID << " " << record.getID << " " << record.weight << ", " << pos << ")\033[0m" << endl;
                // system("pause");
                indexMap[record.weight] = make_pair(record, pos);

                idx++;
            }
            pos++;
        }

        inFile.close();


        // cout << "test\n";
        // for(auto it = indexMap.begin(); it != indexMap.end(); it++){
        //     cout << "key: " << it->first << " offset: " << it->second.second << endl;
        // }
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

        auto end = chrono::high_resolution_clock::now();
        ExternalTime = chrono::duration_cast<chrono::microseconds>(end - start); // calculate the elapsed time
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

    void printTime(){
        cout << "The execution time ..." << endl;
        cout << "Internal Sort = " << fixed << setprecision(3) << float(InternalTime.count())/1000 << " ms" << endl;
        cout << "External Sort = " << float(ExternalTime.count())/1000 << " ms" << endl;
        cout << "Total Execution Time = " << float(InternalTime.count() + ExternalTime.count())/1000 << " ms" << endl ;
        cout << "\n";
        cout.unsetf(ios::fixed);
    }

    void Mission_3(){
        string input;
        float weight;
        bool continueFlag = true;
        do{
            cout << endl << "Please input a threshold in the range [0,1]: " << endl;
            cin >> input;

            // weight = stof(input);

            try {
                cout << fixed << setprecision(6);
                cout << "input: " << input << endl;
                weight = stof(input); // 尝试将输入转换为浮点数
                // 四舍五入到小数点后第六位
                cout << "weight " << weight << endl;
                // weight = round(stof(input) * 1e6) / 1e6;
                // cout << "The weight value rounded to six decimal places is: " << weight << endl;
            } catch (const invalid_argument& e) {

                cout << "Invalid input: " << e.what() << ". Please enter a valid floating-point number." << endl;
                continue;
            } catch (const out_of_range& e) {

                cout << "Input out of range: " << e.what() << ". The value is too large or too small for a float." << endl;
                continue;
            }

            cout << endl << "[0]Quit or [Any other key]continue?" << endl;
            
            cin >> input;
            if(input == "0")
                continueFlag = false;
            else
                continueFlag = true;


            /*
            bool isNumber = true;
            int bigger_than_one = 0;
            for(int i = 0, dot = 0 ; i < input.length(); i++){
                if(input[i] == '.')
                    dot++;
                if(dot > 1){ // more than one dot
                    isNumber = false;
                    break;
                }
                if(!isdigit(input[i]) && input[i] != '.'){ // not a number or dot
                    isNumber = false;
                    break;
                }
                if(dot == 0){
                    bigger_than_one++;
                }
            }

            if(!isNumber)
                continue;
            else if(input.length() > 4){
                cout << "\nThe threshold is not in the range [0,1]!\n";
                continue;
            }

            weight = stof(input);
            if(weight <= 0 || weight > 1){
                cout << "\nThe threshold is not in the range [0,1]!\n";
            }
            else{
                create_Mission3_order(weight);
            }
            
            cout << endl << "[0]Quit or [Any other key]continue?" << endl;
            cin >> input;
            if(input == "0")
                continueFlag = false;
            else
                continueFlag = true;*/
        }
        while(continueFlag);

        // if(continueFlag)
        //     create_Mission3_order(weight);


    }

    void create_Mission3_order(float weight){
        // string orderFile = "order" + fileID + ".bin";
        string orderFile = "order" + fileID + ".txt";
        ofstream outFile;
        outFile.open(orderFile.c_str(), ios::out);

        // count search time 
        // --------------------------------------------------
        chrono::microseconds recordTime;
        auto start = chrono::high_resolution_clock::now();
        for (auto it = indexMap.begin(); it != indexMap.end(); it++) {
            if (it->first == weight)
                break;
        }
        auto end = chrono::high_resolution_clock::now();
        recordTime = chrono::duration_cast<chrono::microseconds>(end - start);
        // --------------------------------------------------

        cout.unsetf(ios::fixed);
        
        vector<float> keys;
        for(auto it = indexMap.begin(); it != indexMap.end(); it++){
            // cout << "key: " << it->first << " offset: " << it->second.second << endl;
            if(it->first >= weight)
                keys.insert(upper_bound(keys.begin(), keys.end(), it->first, CompareNodes()), it->first);

                // keys.push_back(it->first);
        }

        if(weight == 0.01)
            outFile << "Search " << weight << " : " << dataSize << " records, " << float(recordTime.count())/1000 << " ms" << endl;
        else
            outFile << "Search " << weight << " : " << indexMap.at(weight-0.01).second << " records, " << float(recordTime.count())/1000 << " ms" << endl;
        
        for(auto key : keys){
            cout << "\033[1;33mweight " << weight << " key: " << key << "\033[0m" << endl;
            cout << "[" << indexMap.at(key).second << "] " << indexMap.at(key).first.putID << " " << indexMap.at(key).first.getID << " " << indexMap.at(key).first.weight << endl;
            outFile << "[" << indexMap.at(key).second+1 << "] " << indexMap.at(key).first.putID << " " << indexMap.at(key).first.getID << " " << indexMap.at(key).first.weight << endl;
        }


        
        // for( float w = 1 ; w > 0 ; w = w - 0.01){
        //     cout << "\033[1;33mweight " << weight << " w: " << w << "\033[0m" << endl;
        //     cout << "indexMap.at(w).first.weight: " << indexMap.at(w).first.weight << endl;
        //     if(w >= weight){
        //         outFile << "[" << indexMap.at(w).second << "] " << indexMap.at(w).first.putID << " " << indexMap.at(w).first.getID << " " << indexMap.at(w).first.weight << endl;
        //     }
        //     else{
        //         cout << "\033[1;32mweight" << weight << " w: " << w << "\033[0m" << endl;
        //         break;
        //     }
            
        // }

        // inFile.close();
        outFile.close();
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


            cout << endl << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
            cout << endl << "Mission 3: Threshold search on primary index ";
            cout << endl << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
            cout << endl;

            glist.Mission_3();

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
