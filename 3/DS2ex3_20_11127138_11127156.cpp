// 11127138 林雨臻
// 11127156 王佩諭
#include <cstdlib>  // atoi, system
#include <fstream>  // open, is_open
#include <iomanip>  // setw, setprecision
#include <iostream> // cout, endl, fixed
#include <string>   // string
#include <cstring>   // strcpy
#include <vector>   // vector
#include <algorithm> // find
#include <cmath>
using namespace std;


struct studentType {
    char sid[10] = {'\0'}; // student id
    char sname[10] = {'\0'}; // student name
    unsigned char score[6] = {0}; // a set of scores in [0,100]
    float mean = 0.0; // the average of scores
};

struct student_hash {
    char sid[10] = {'\0'}; // student id
    char sname[10] = {'\0'}; // student name
    // unsigned char score[6]; // a set of scores in [0,100]
    float mean = 0.0; // the average of scores

    int hashvalue;
    bool isempty;

    student_hash() : hashvalue(-1), isempty(true) {}
};

class HashFunction {
public:
    HashFunction() {}
    int find_prime(int size, int prime) {
        // int prime = ceil(size * 1.15); //  bigger than 1.15 times of the size

        while(true){
            bool isPrime = true;
            for (int tmp = sqrt(prime) ; isPrime!=false && tmp > 1 ; tmp--) {
                if (prime % tmp == 0)
                    isPrime = false;
            }

            if(isPrime == false) // current prime is not a prime number
                prime++;
            else
                break;
        }

        return prime;
    }

    int Function(string sid, int table_size) {
        int key = 1;
        for (int i = 0; i < sid.length(); ++i){
            // cout << key << endl;
            int tmp = int(sid[i]);
            key = (tmp * key) % table_size;
        }

        return key ;
    }


    void write_file(string Type, string fileout, vector<student_hash> aList){ // declaration: write all as file
        fileout = Type+fileout ;
        ofstream sortedout( fileout.c_str() ) ;
        if(!sortedout){
            cout << "### Cannot create "<< fileout <<" ###\n" ;
            return ;
        }

        if(Type == "quadratic")
            sortedout << " --- Hash table created by Quadratic probing ---\n" ;
        else if(Type == "double")
            sortedout << " --- Hash table created by Double hashing    ---\n" ;

        for(int i = 0; i < aList.size(); i++){
            sortedout << "[" << setw(3) << i << "] " ;
            if(!aList.at(i).isempty)
                sortedout << setw(10) << aList.at(i).hashvalue << ", " << setw(10) << aList.at(i).sid << ", " << setw(10) << aList.at(i).sname << ", " << setw(10) << float(aList.at(i).mean) ;
                
            sortedout << endl ;
        }

        sortedout << " ----------------------------------------------------- " << endl ;
        sortedout.close() ;
    }

    ~HashFunction() {}
};

class Quadratic : public HashFunction {
private:
    vector<student_hash> student_quadratic;
    int table_size;
    int data_size;
    float successful;
    float unsuccessful;

    void setting_size(int size){
        data_size = size;

        int prime = ceil(size * 1.15); //  bigger than 1.15 times of the size
        table_size = find_prime(size, prime);

        student_quadratic.resize(table_size);
    }

    void probe_quadratic(studentType student) { // vector<studentType>& hashSet, , const int table_size
        int hashvalue = Function(string(student.sid), table_size); // by hash function to get the hashvalue

        int hash_idx = hashvalue;

        if (student_quadratic.at(hash_idx).isempty == true){ // if the place is empty
            successful++;
            strcpy(student_quadratic.at(hash_idx).sid, student.sid);
            strcpy(student_quadratic.at(hash_idx).sname, student.sname);
            student_quadratic.at(hash_idx).mean = student.mean;
            student_quadratic.at(hash_idx).hashvalue = hashvalue;
            student_quadratic.at(hash_idx).isempty = false;
        }
        else{ // if the place is not empty
            int i = 1;
            do{
                successful++;
                hash_idx = (hashvalue + (i % table_size) * (i % table_size));

                hash_idx = hash_idx % table_size;
                i++;
            }
            while(student_quadratic.at(hash_idx).isempty == false && hash_idx != hashvalue); // && i < table_size/2
            if (hash_idx == hashvalue)
                cout << student.sname << " Fail put in.\n" ;

            else if (student_quadratic.at(hash_idx).isempty == true){
                strcpy(student_quadratic.at(hash_idx).sid, student.sid);
                strcpy(student_quadratic.at(hash_idx).sname, student.sname);
                student_quadratic.at(hash_idx).mean = student.mean;
                student_quadratic.at(hash_idx).hashvalue = hashvalue;
                student_quadratic.at(hash_idx).isempty = false;
                successful++;
            }
        }

        // return hashvalue;
    }


public:
    Quadratic() : table_size(0), data_size(0), unsuccessful(0.0), successful(0.0) {}

    void Hash_Quadratic(vector<studentType> pSet) { // , string fileID
        setting_size(pSet.size()); // set the table size

        for (int i = 0; i < pSet.size(); ++i)
            probe_quadratic(pSet.at(i));
        

        // unsuccessful times calculation
        for (int i = 0 ; i < table_size ; i++) {
            int count = 0;
            int idx = 0;
            if (student_quadratic[i].isempty == false) { // have data
                do{
                    count++ ;
                    // idx = i + pow(count,2);
                    idx = (i + (count % table_size) * (count % table_size));

                    idx = idx % table_size;
                    unsuccessful++ ;
                }
                while(student_quadratic[idx].isempty == false && count < table_size);
            }
            
        }
		// cout << "unsuccessful: " << unsuccessful << " successful: " << successful << "\n" ;

    }

    void showAll() { // display all the records on screen
        cout << "\n\033[1;31mshowAll:\033[0m"<<endl;
        for (int i = 0; i < student_quadratic.size(); i++){
            cout << right << "[" << setw(2) << i << "]\t";
            if(student_quadratic.at(i).isempty == false){
                cout << left << student_quadratic.at(i).sid << "\t" << student_quadratic.at(i).sname
                     << "\thatshvalue: " << student_quadratic.at(i).hashvalue
                     << "\tMean: " << fixed << setprecision(2) << student_quadratic.at(i).mean
                     << "  isempty:" << student_quadratic.at(i).isempty ;
            }
            cout << endl;
        }
    }


    void calculation(string fileID){
        cout << endl << "Hash table has been successfully created by Quadratic probing" << endl ;

        float unsuccess = unsuccessful / table_size;
        cout << "unsuccessful search: " ;
		cout << fixed << setprecision(4) << unsuccess << " comparisons on average" << endl ;

		float success = successful / data_size;
        cout << "successful search: " ;
		cout << fixed << setprecision(4) << success << " comparisons on average" << endl ;


        write_file("quadratic", (fileID+".txt"), student_quadratic);
    }
    void clearUp() {
        table_size = 0;
        data_size = 0;
        unsuccessful = 0.0;
        successful = 0.0;
        student_quadratic.clear();
    }

    ~Quadratic() {
        clearUp();
    }
};

class doubleHash : public HashFunction {
private:
    vector<student_hash> student_double;
    int table_size; // is a prime
    int data_size;
    int highest_step; // is a prime
    float successful;
    float unsuccessful;

    void setting_size(int size){
        data_size = size;
        int prime = ceil(size * 1.15); //  bigger than 1.15 times of the size
        table_size = find_prime(size, prime);
        
        prime = ceil(float(size) / 5); //  bigger than (size / 5)
        highest_step = find_prime(size, prime); // set the highest step
        student_double.resize(table_size);
    }

    int step_func(string sid){
        int step = 1;
        for (int i = 0; i < sid.length(); ++i){
            int tmp = int(sid[i]);
            step = (tmp * step) % highest_step;
        }
        step = highest_step - step;
        return step;
    }

    int probe_double(studentType student) { // vector<studentType>& hashSet, , const int table_size
        int hashvalue = Function(string(student.sid), table_size); // by hash function to get the hashvalue
        int step = step_func(string(student.sid));
        int hash_idx = hashvalue;
        if (student_double.at(hash_idx).isempty == true){ // if the place is empty
            
            successful++;
            strcpy(student_double.at(hash_idx).sid, student.sid);
            strcpy(student_double.at(hash_idx).sname, student.sname);
            student_double.at(hash_idx).mean = student.mean;
            student_double.at(hash_idx).hashvalue = hashvalue;
            student_double.at(hash_idx).isempty = false;

        }
        else{ // if the place is not empty
            int i = 1;
            do{
                successful++;
                hash_idx = (hash_idx + step) % table_size;
                i++;
            }
            while(student_double.at(hash_idx).isempty == false && hash_idx != hashvalue); // && i < table_size/2

            if (hash_idx == hashvalue)
                cout << student.sname << " Fail put in.\n" ;

            if(student_double.at(hash_idx).isempty == true){
                strcpy(student_double.at(hash_idx).sid, student.sid);
                strcpy(student_double.at(hash_idx).sname, student.sname);
                student_double.at(hash_idx).mean = student.mean;
                student_double.at(hash_idx).hashvalue = hashvalue;
                student_double.at(hash_idx).isempty = false;
                successful++;
            }
        }
        return hashvalue;
    }


public:
    doubleHash() : table_size(0), data_size(0), highest_step(0), unsuccessful(0.0), successful(0.0) {}

    void Hash_double(vector<studentType> pSet) { // , string fileID
        setting_size(pSet.size()); // set the table size

        for (int i = 0; i < pSet.size(); ++i)
            int hashvalue = probe_double(pSet.at(i));//, table_size
        

    }


    void showAll() { // display all the records on screen
        cout << "\n\033[1;31mshowAll:\033[0m"<<endl;
        for (int i = 0; i < student_double.size(); i++){
            cout << right << "[" << setw(2) << i << "]\t";
            if(student_double.at(i).isempty == false){
                cout << left << student_double.at(i).sid << "\t" << student_double.at(i).sname
                     << "\thatshvalue: " << student_double.at(i).hashvalue
                     << "\tMean: " << fixed << setprecision(2) << student_double.at(i).mean
                     << "  isempty:" << student_double.at(i).isempty ;
            }
            cout << endl;
        }

    }


    void calculation(string fileID){
        cout << endl << "Hash table has been successfully created by Double hashing   \n" ;
        
		float success = successful / data_size;
        cout << "successful search: " ;
		cout << fixed << setprecision(4) << success << " comparisons on average" << endl ;

        write_file("double", (fileID+".txt"), student_double);
    }
    void clearUp() {
        table_size = 0;
        data_size = 0;
        highest_step = 0;
        unsuccessful = 0.0;
        successful = 0.0;
        student_double.clear();
    }

    ~doubleHash() {
        clearUp();
    }
};




class StudentList {
private:
    vector<studentType> pSet; // set of data records
    string fileID; // number to form a file name
    bool fileexist; // already enter a file or not

    Quadratic q_hashTable;
    doubleHash d_hashTable;

    void show(int i, const vector<studentType>& p) const { // display one record on screen
        cout << right << "[" << setw(2) << i + 1 << "]\t" << left << p.at(i).sid << "\t" << p.at(i).sname << "\tScores: ";
        for (int j = 0; j < 6; ++j) {
            cout << static_cast<int>(p.at(i).score[j]) << " ";
        }
        cout << "\tMean: " << fixed << setprecision(2) << p.at(i).mean << endl;
    }

    bool BinExist() {
        fstream binFile(("input" + fileID + ".bin").c_str(), fstream::in | fstream::binary);
        bool success = binFile.is_open();
        if(!success)
            cout << "\n### input" << fileID << ".bin does not exist! ###\n";
        binFile.close();
        return success;
    }


public:
    StudentList() : fileexist(false) {}

    bool isEmpty() {
        return pSet.empty();
    }

    bool fileExist() {
        return fileexist;
    }
    
    bool q_Exist() {
        return pSet.size();
    }

    bool readFile() {
        clearUp();
        if (Test2Binary()) // = 0 is false else is true
            fileexist = true;
        else
            return false;

        readBinary("input" + fileID + ".bin");
        if(pSet.size() == 0)
            fileexist = false;
        return fileexist;
    }

    int Test2Binary() {
        fstream inFile, outFile;
        string fileName;
        int stNo = 0;
        do{
            cout << "\nInput a file number ([0] Quit): ";
            cin >> fileID;
            if(!fileID.compare("0")) // No file to open, so quit
                return false;

            // bool bin = BinExist(); // check if the binary file exists
            if(BinExist()) // if the binary file exists, then quit
                return true;

            inFile.open(("input" + fileID + ".txt").c_str(), fstream::in); // Open a file
            if(inFile.is_open())
                break; // successfully open a file, so leave the loop
            else{
                cout << "\n### input" << fileID << ".txt does not exist! ###\n\n";
                return false;
            }

        } while(true); // end while

        fileName = "input" + fileID + ".bin";
        outFile.open(fileName.c_str(), fstream::out | fstream::binary); // create a binary file
        if(outFile.is_open()){
            char rBuf[255]; // buffer to keep one whole record

            while(inFile.getline(rBuf, 255, '\n')){ // retrieve one entire record from file
                string temp; // duplicate a string in another format
                studentType oneSt; // keep one record with all the required fields
                int cNo = 0, pre = 0, pos = 0; //indicators to extract each field in a record

                stNo++; // number of records
                temp.assign(rBuf); // make a duplicate of the entire record
                pos = temp.find_first_of('\t', pre); // move to the end of the query field

                while(pos != string::npos){
                    switch(++cNo){
                        case 1: // student id
                            strcpy(oneSt.sid, temp.substr(pre, pos - pre).c_str());
                            break;
                        case 2: // student name
                            strcpy(oneSt.sname, temp.substr(pre, pos - pre).c_str());
                            break;
                        default: // scores
                            oneSt.score[cNo - 3] = atoi(temp.substr(pre, pos - pre).c_str());
                            break;
                    } // end switch
                    pre = ++pos;
                    pos = temp.find_first_of('\t', pre); // move to the end of the next field
                } // end inner while

                pos = temp.find_last_of('\t'); // position of the rightmost tab
                oneSt.mean = atof(temp.substr(pos + 1).c_str()); // copy the average of score
                outFile.write((char*)&oneSt, sizeof(oneSt)); // write the entire record into a binary file
            } // end outer while

            outFile.close(); // write out the binary file
        } // end else
        inFile.close(); // close the text file
        return stNo; // it succeeds if there is at least one records
    }

    void readBinary(string fileName) {
        fstream binFile; // input file handle
        studentType oneSt; // keep one record with the required fields
        int stNo = 0; // total number of students

        binFile.open(fileName.c_str(), fstream::in | fstream::binary); // open a binary file
        if(binFile.is_open()){
            binFile.seekg(0, binFile.end);
            stNo = binFile.tellg() / sizeof(oneSt);
            binFile.seekg(0, binFile.beg);
            for(int i = 0 ; i < stNo ; i++){
                binFile.read((char*)&oneSt, sizeof(oneSt)); // read the entire record from a binary file
                // cout << "[" << i + 1 << "] " << oneSt.sid << "\t" << oneSt.sname << endl;
                pSet.push_back(oneSt); // keep the record in the vector
            } // end for
        } // end if
        binFile.close(); // close input file
    }

    void build_quadratic() {
        q_hashTable.clearUp();
        q_hashTable.Hash_Quadratic(pSet);

        q_hashTable.calculation(fileID);
    }

    void build_double() {
        d_hashTable.clearUp();
        d_hashTable.Hash_double(pSet);

        d_hashTable.calculation(fileID);

    }


    void showAll() { // display all the records on screen
        for (int i = 0; i < pSet.size(); ++i)
            show(i, pSet);
    }

    void clearUp() {
        fileexist = false;
        fileID.clear();
        pSet.clear();
        q_hashTable.clearUp();
    }
    ~StudentList(){ clearUp(); }
};

int main() {
    StudentList slist;
    int command = -1;
    bool isEnd = false;
    string input;
    while (!isEnd) {
        cout << "\n******* Hash Table *****";
        cout << "\n* 0. QUIT              *";
        cout << "\n* 1. Quadratic probing *";
        cout << "\n* 2. Double hashing    *";
        cout << "\n************************";
        cout << "\nInput a choice(0, 1, 2): ";

        cin >> input;

        if (input == "0")
            command = 0;
        else if (input == "1")
            command = 1;
        else if (input == "2")
            command = 2;
        else
            command = -1;

        switch (command) {
            case 0:
                isEnd = true;
                break;
            case 1:
                if (slist.readFile()) { // if file exist
                    slist.build_quadratic();
                }
                break;
            case 2:
                if (slist.fileExist() && slist.q_Exist()) {
                    slist.build_double();
                }
                else
                    cout << "### Command 1 first. ###\n" << endl;
                break;
            default:
                cout << "\nCommand does not exist!\n" << endl;
                break;
        } // end switch case
    } // end while

    // cout << "Program ended." << endl;

    return 0;
}
