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


class GraphList{
private:
    vector<Graph> gSet;
    // string fileID; // number to form a file name
    bool fileexist; // already enter a file or not

public:
    string fileID; // number to form a file name
    GraphList() : fileexist(false) {}

    bool fileExist() { return fileexist; }

    bool readFile() {
        clearUp();
        do{
            cout << "\nInput a file number ([0] Quit): ";
            cin >> fileID;

            if(!fileID.compare("0")) // No file to open, so quit
                return false;
            else{
                readBinary("order"+fileID+".bin");
                if(gSet.size() == 0)
                    fileexist = false;
                return fileexist;
            }

        } while(true); // end while


    }

    void readBinary(string fileName) {
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
    }

    void showData(){
        
        cout << "\033[1;31mshowData: \n\033[0m";
        for (int i = 0; i < gSet.size(); i++)
            cout << gSet[i].putID << " " << gSet[i].getID << " " << gSet[i].weight << endl;
        
        

        // writetxt("block" + fileID + ".txt");
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
        
    }
    ~GraphList(){ clearUp(); }
};



int main(){
    GraphList glist;
    int command = -1;
    bool isEnd = false;
    string input;
    while (!isEnd) {
    	cout << endl << "**** Graph data manipulation *****" ;
        cout << endl << "* 0. QUIT                        *" ;
        cout << endl << "* 1. Build adjacency lists       *" ;
        // cout << endl << "* 2. Compute connection counts   *" ;
        // cout << endl << "* 3. Estimate influence values   *" ;
        // cout << endl << "* 4. Probability-based influence *" ;
        cout << endl << "**********************************" ;
        cout << endl << "Input a choice(0, 1): " ;
        cin >> input;

        if (input == "0")
            command = 0;
        else if (input == "1")
            command = 1;
        else
            command = -1;

        switch (command) {
            case 0:
                isEnd = true;
                break;
            case 1:
                if (glist.readFile()) { // if file exist
                    // glist.showData();
                    glist.writetxt("order" + glist.fileID + ".txt");
                }
                break;
            default:
                cout << "\nCommand does not exist!" << endl;
                break;
        } // end switch case
    } // end while

    return 0;
}
