// 11127138 林雨臻
#include <cstdlib>  // atoi, system
#include <fstream>  // open, is_open
#include <iomanip>  // setw, setprecision
#include <iostream> // cout, endl, fixed
#include <string>   // string
#include <vector>   // vector
#include <math.h>
using namespace std;

struct schoolType {
    int no; // initial no
    string sname; // school name
    string dname; // department name
    string type;  // day or night
    string level; // graduate or undergraduate
    int nstud;    // num of students
    int nprof;    // num of professors
    int ngrad;    // num of graduates
};

class Heap{
    vector<schoolType> item;
    schoolType rootItem;
    int size; // number of heap items

    void clearHeap(){
        size = 0;
        item.clear();
    }
public:
    Heap() : size(0) {} // constructor of an empty tree

    bool heapIsEmpty(){
        return (item.size()==0);
    }

    void heapInsertAll( vector<schoolType> pSet ){
        item = pSet; // put new item at the end of heap
        size = item.size();
    }

    void heapInsert( schoolType pSet ){
        //item = pSet; // put new item at the end of heap
        item.push_back(pSet);
        size = item.size();
    }

    void sortInsert(){
        for( int i = item.size()/2 ; i >= 0 ; i-- )
            heapRebuild(i);
    }

    void heapDelete(vector<schoolType> &item){
        if(heapIsEmpty()){
            cout << "Heap empty\n";
            return;
        }
            
        rootItem = item[0];
        size--;
        item[0]=item[size];
        heapRebuild(0);

    }

    void heapRebuild(int root){
        int child = 2*root + 1; // find the left child of root
        if( child < size ){ // root have child
            int rightChild = child + 1;
            if( (rightChild < size) && (item[rightChild].nstud > item[child].nstud) ) // root have rightChild
                child = rightChild ; // the index of the larger child
            if( item[root].nstud < item[child].nstud ){
                schoolType temp = item[root];
                item[root] = item[child];
                item[child] = temp;
                heapRebuild(child);
            }

        }
    }

    void showheap(){
        cout << left << "\t#\t" << setw(32) << "Name" << "Type 1\t\t" << "HP" << "\t" << "Attack" << "\t" << "Defense\n";
        for( int i = 0 ; i < item.size() ; i++ ){
            cout << right << "[" << setw(2) << i << "]\t" ;
            cout << "[" << item.at(i).no << "]\t" << item.at(i).sname << " " << setw(16)
             << item.at(i).nstud << "\t" << item.at(i).nprof << "\n" ;
        }
    }

    void showheight(){
        int tmp = item.size() ;
        int height = 0;
        while( tmp > 0 ){
            tmp = tmp - pow(2, height);
            height++;
        }

       cout << "HP heap height = " << height << "\n";

    }
    
    void showroot(){
        int idx = 0 ;
        cout << "root: ";
        cout << "[" << item.at(idx).no << "] " << item.at(idx).nstud << "\n" ;

    }

    void showleftmost(){
        int increment = 2 ;
        int idx = 0 ;
        for( int i = 0 ; i < item.size() ; i++ ){
            if( idx + pow(increment,i) < item.size() )
                idx = idx + pow(increment, i) ;
            else
                break;
            
        }

        cout << "leftmost bottom: " ;
        cout << "[" << item.at(idx).no << "] " << item.at(idx).nstud << "\n" ;


    }
    void showbottom(){
        int idx = item.size()-1;
        cout << "bottom: ";
        cout << "[" << item.at(idx).no << "] " << item.at(idx).nstud << "\n" ;

    }
    void clearUp() { // cut off the entire tree
        clearHeap();
    } // end clearUp

    ~Heap() { clearUp(); } // destructor
}; // end Heap

class Deap{
    vector<schoolType> item;
    schoolType rootItem;

    bool minormax(int index, int &correspond, int &p_correspond){
        int layer = 1; // layer=0 only have a null node
        int tmp=0;
        int pretmp=0;
        bool min = true;

        while(tmp<index){ // pretmp < index <= tmp
            pretmp = tmp;
            tmp = tmp + pow(2, layer);
            ++layer;
        }
        if (index > tmp-(tmp-pretmp)/2)
            min = false;

        // setting correspond & p_correspond
        int gap = (tmp-pretmp)/2;
        if(min)
            correspond = index + gap;
        else
            correspond = index - gap;
        p_correspond = (correspond-1)/2 ;

        return min;
    }

    void Checkmin( int index ){
        if(index == 1) // avoid 0/0
            return;
        int father = (index-1)/2 ;
        if(father == 0) // avoid goto root which have no data
            return;
        if(item[index].nstud < item[father].nstud){
            schoolType t = item[index] ;
            item[index] = item[father];
            item[father] = t;

            Checkmin( father );
        }
    }

    void Checkmax( int index ){
        int father = (index-1)/2 ;
        if(father == 0) // avoid goto root which have no data
            return;
        if(item[index].nstud > item[father].nstud){
            schoolType t = item[index] ;
            item[index] = item[father];
            item[father] = t;

            Checkmax( father );
        }
    }

    void rearrangeDEAP(int index) {
        int correspond=0;
        int p_correspond=0; // father of correspond
        bool min = minormax(index, correspond, p_correspond); // min = false means max

        if(min){ // min, must dont have correspond
            if(p_correspond != 0 && item[index].nstud > item[p_correspond].nstud){
                schoolType t = item[index] ;
                item[index] = item[p_correspond];
                item[p_correspond] = t;
                Checkmax(p_correspond);
            }
            else
                Checkmin(index);
            
            
        }
        else{ // max, must have correspond
            if(item[index].nstud < item[correspond].nstud){
                schoolType t = item[index] ;
                item[index] = item[correspond];
                item[correspond] = t;
                Checkmin(correspond);
            }
            else
                Checkmax(index);
        }
    
    }
    void clearDeap(){
        //size = 0;
        item.clear();
    }
public:
    Deap(){} // constructor of an empty tree

    bool deapIsEmpty(){
        return (item.size()==0);
    }

    void deapInsertAll( vector<schoolType> pSet ){
        item = pSet;
        //size = item.size();
    }

    void deapInsert( schoolType pSet ){ // Insert new
        if(deapIsEmpty()){
            schoolType p;
            p.sname="0";
            p.nstud=0;
            item.push_back(p);
        }

        item.push_back(pSet); // put new item at the end of heap
        rearrangeDEAP(item.size()-1);
    }

    void showdeap(){
        for( int i = 0, j = pow(2,i), c = 0 ; c < item.size() ; i++, j = pow(2,i) ){
            for( int z = j ; c < item.size()  && z > 0 ; z--, c++ ){
                cout << item.at(c).nstud << " " ;
            }
            cout << "\n" ;
        }
    }

    void showheight(){
        int tmp = item.size() ;
        int height = 0;
        while( tmp > 0 ){
            tmp = tmp - pow(2, height);
            height++;
        }

       cout << "HP heap height = " << height << "\n";
    }
    
    void showleftmost(){
        int increment = 2 ;
        int idx = 0 ;
        for( int i = 0 ; i < item.size() ; i++ ){
            if( idx + pow(increment,i) < item.size() ) idx = idx + pow(increment, i) ;
            else break;
        }

        cout << "leftmost bottom: " ;
        cout << "[" << item.at(idx).no << "] " << item.at(idx).nstud << "\n" ;
    }
    void showbottom(){
        int idx = item.size()-1;
        cout << "bottom: ";
        cout << "[" << item.at(idx).no << "] " << item.at(idx).nstud << "\n" ;

    }
    void clearUp() { // cut off the entire tree
        clearDeap();
    } // end clearUp

    ~Deap() { clearUp(); }

};

class SchoolList{
    vector<schoolType> pSet; // set of data records
    string fileID; // number to form a file name
    bool fileexist ;
    bool heapexist;
    Heap aHeap;
    Deap aDeap;

    void show(int i, vector<schoolType> p){ // display one record on screen 
        cout << right << "[" << setw(2) <<i+1<< "]\t" << left << p.at(i).no << "\t" << p.at(i).sname << "\t" << setw(24) << p[i].dname << "\t" 
             << setw(16) << p[i].type << p[i].level << "\t" << p[i].nstud << "\t" << p[i].nprof << "\n" ;
    }

    public:
    SchoolList(): fileID(""),fileexist(false),heapexist(false){ clearUp(); } // constructor: do nothing
    bool isEmpty(){ // check if there is nothing
        if (pSet.size() == 0)
            return true;
        return false;
    }
    bool fileExist(){
        return fileexist;
    }
    
    bool readFile() { // declaration: read all from a inputfile
        fstream inFile;
        string fileName;
        int index = 0;

        clearUp();

        bool fileExist = false;
        while(!fileExist){ // confirm existence or not
            cout << "\nInput a file number ([0] Quit): ";
            cin >> fileID;
            if(fileID == "0") // ([0]:Quit)
                return  false;

            fileName = "input" + fileID + ".txt";
            inFile.open(fileName.c_str(), fstream::in);
            if (!inFile.is_open())
                cout << "\n### " << fileName << " does not exist! ###\n";
            else
                fileExist = true ;
        }
        fileexist = true;
        char cstr[255];
        int fNo, pre, pos;
        inFile.getline(cstr, 255, '\n'); // skip the 1st header line
        inFile.getline(cstr, 255, '\n'); // skip the 2nd header line
        inFile.getline(cstr, 255, '\n'); // skip the list of column names
        while (inFile.getline(cstr, 255, '\n')) {
            schoolType oneR; // one data record
            string buf, cut; // transform c-string into c++ string

            fNo = 0;
            pre = 0;
            index++;
            oneR.no = index;
            buf.assign(cstr);
            do {
                pos = buf.find_first_of('\t', pre);
                cut = buf.substr(pre, pos - pre);
                switch (++fNo) {
                    case 2:
                        oneR.sname = cut; // get the school name
                        break;
                    case 4:
                        oneR.dname = cut; // get the department name
                        break;
                    case 5:
                        oneR.type = cut; // get either day or night
                        break;
                    case 6:
                        oneR.level = cut; // get graduate or undergraduate
                        break;
                    case 7:
                        cut = sortinput(cut);
                        oneR.nstud = atoi(cut.c_str()); // get the number of students
                        break;
                    case 8:
                        cut = sortinput(cut);
                        oneR.nprof = atoi(cut.c_str()); // get the number of professors
                        break;
                    case 9:
                        cut = sortinput(cut);
                        oneR.ngrad = atoi(cut.c_str()); // get the number of graduates
                        break;
                    default:
                        break;
                }                              // end switch
                pre = ++pos;                   // move to the next field
            } while ((pos > 0) && (fNo < 10)); // continue if there are still fields to be read
            pSet.push_back(oneR);        // save one record into the school list
        } // end outer-while
        inFile.close();

        //} // end else

        if (!pSet.size()) {
            //cout << "\n### Get nothing from the file " << fileName << "!###\n";
            return false;
        } // end if

        return true;
    } // end readFile

    string sortinput(string cut){
        string a = "";
        for( int i = 0 ; i<cut.size() ; i++ ){
            if (cut[i] != '\"' && cut[i] != ',')
                a = a + cut[i];
        }
        return a;
    }
    void showAll(){ // display all the records in screen
        for(int i = 0 ; i < pSet.size() ; i++){
            show(i, pSet) ;
        }
    }

    void buildHeap(){
        for(int i = 0 ; i < pSet.size() ; i++){
            aHeap.heapInsert(pSet[i]);
            aHeap.sortInsert();
        }

        //aHeap.showheap();
        aHeap.showroot();
        aHeap.showbottom();
        aHeap.showleftmost();
    }

    void buildDeap(){
        for(int i = 0 ; i < pSet.size() ; i++){
            aDeap.deapInsert(pSet[i]);
        }

        //aDeap.showdeap();
        aDeap.showbottom();
        aDeap.showleftmost();

    }
    void clearUp(){
        fileexist = false;
        heapexist = false;
        fileID.clear();
        pSet.clear();
        aHeap.clearUp();
        aDeap.clearUp();

    } // end clearUp()

    ~SchoolList(){ clearUp(); } // desturctor: destory the object
}; // end SchoolList


int main() {
    SchoolList slist;
    int command = -1;
    bool isEnd = false;
    string fileName, input;
    while (!isEnd) {
        cout << "\n**** Heap Construction *****";
        cout << "\n* 0. QUIT                  *";
        cout << "\n* 1. Build a max heap      *";
        cout << "\n* 2. Build a DEAP          *";
        cout << "\n****************************";
        cout << "\nInput a choice(0, 1, 2): ";
        cin >> command;
        switch (command) {
            case 0:
                isEnd = true;
                break;
            case 1:
                if (slist.readFile()) { // if file exist
                    cout << "<max heap>\n";
                    slist.buildHeap();
                    
                }
                break;
            case 2:
                if (slist.readFile()){
                    cout << "<DEAP>\n";
                    slist.buildDeap();
                }
                break;
            default:
                cout << "\nCommand does not exist!\n";
        } // end switch case
    } // end while
} // end main()