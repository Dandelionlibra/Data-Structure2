// 11127138 林雨臻
#include <cstdlib>  // atoi, system
#include <fstream>  // open, is_open
#include <iomanip>  // setw, setprecision
#include <iostream> // cout, endl, fixed
#include <string>   // stringu
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
            cout << "[" << item.at(i).no << "]\t" << " " << setw(16) << item.at(i).nstud << "\n" ;
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
        if(min){
            int gap = (tmp-pretmp)/2;
            correspond = index + gap;
        }
        else{
            int gap = (tmp-pretmp)/2;
            correspond = index - gap;
        }
        p_correspond = (correspond-1)/2 ;

        
        return min;
    }

    void Checkmin( int index ){
        if(index == 1) // avoid 0/0
            return;
        int father = (index-1)/2 ;
        if(father == 0) // avoid goto root which have no data
            return;
        
        if(item.at(index).nstud < item.at(father).nstud){
            schoolType t = item.at(index);
            item.at(index) = item.at(father);
            item.at(father) = t;

            Checkmin(father);
        }
    }

    void Checkmax( int index ){
        int father = (index-1)/2 ;
        if(father == 0) // avoid goto root which have no data
            return;

        if(item.at(index).nstud > item.at(father).nstud){
            schoolType t = item.at(index) ;
            item.at(index) = item.at(father);
            item.at(father) = t;

            Checkmax( father );
        }
    }

    void rearrangeDEAP(int index) {
        int correspond=0;
        int p_correspond=0; // father of correspond
        bool min = minormax(index, correspond, p_correspond); // min = false means max

        if(min){ // min, must dont have correspond
            if(p_correspond != 0 && item.at(index).nstud > item.at(p_correspond).nstud){
                schoolType t = item.at(index);
                item.at(index) = item.at(p_correspond);
                item.at(p_correspond) = t;
                Checkmax(p_correspond);
            }
            else
                Checkmin(index);
        }
        else{ // max, must have correspond
            if(item.at(index).nstud < item.at(correspond).nstud){
                schoolType t = item.at(index);
                item.at(index) = item.at(correspond);
                item.at(correspond) = t;
                Checkmin(correspond);
            }
            else
                Checkmax(index);
        }

    }
    
    void DelCheckmin(int index){
        int child = (index+1)*2 ;

        if(child-1 > item.size()-1){ // dont have child
            int correspond, p_correspond;
            minormax(index, correspond, p_correspond);
            if(correspond < item.size()){ // have correspond
                if(item.at(index).nstud > item.at(correspond).nstud){
                    schoolType tmp = item.at(index);
                    item.at(index) = item.at(correspond);
                    item.at(correspond) = tmp;
                    Checkmax(correspond);
                }
                return;
            }
            else{ // dont have correspond
                if(p_correspond == 0)
                    return;
                else if(item.at(index).nstud > item.at(p_correspond).nstud){
                    //cout << "\033[31;1mindex:" << index <<" p_correspond:" << p_correspond <<" size:" << item.size() << " \033[0m \n";

                    schoolType tmp = item.at(index);
                    item.at(index) = item.at(p_correspond);
                    item.at(p_correspond) = tmp;
                    Checkmax(p_correspond);
                }
                return;
            }
        }
        // only have one child or left<right
        else if(child == item.size() || item.at(child).nstud>=item.at(child-1).nstud)
            child--;
        //cout << "\033[31;1mindex:" << index <<" child:" << child <<" size:" << item.size() << " \033[0m \n";
        
        if(item.at(index).nstud > item.at(child).nstud){
            schoolType tmp = item.at(index);
            item.at(index) = item.at(child);
            item.at(child) = tmp;
            DelCheckmin(child);
        }
    }

    int seletionMax(int correspond, int childR, int childL){
        int max = correspond;
        if(childR < item.size()){
            if(item.at(childR).nstud > item.at(childL).nstud && item.at(childR).nstud > item.at(correspond).nstud)
                max = childR;
            if(item.at(childL).nstud >= item.at(childR).nstud && item.at(childL).nstud >= item.at(correspond).nstud)
                max = childL;
        }
        else if(childL < item.size()){
            if(item.at(childL).nstud > item.at(correspond).nstud)
                max = childL;
        }
        return max;
    }

    void DelCheckmax( int index ){
        int child = (index+1)*2 ;
        if(child-1 >= item.size()){ // dont have child
            int correspond, p_correspond;
            minormax(index, correspond, p_correspond);
            if(correspond < item.size()){ // have correspond
                int childR = (correspond+1)*2;
                int childL = childR-1;
                int max = seletionMax(correspond, childR, childL);
                if(item.at(index).nstud < item.at(max).nstud){
                    schoolType tmp = item.at(index);
                    item.at(index) = item.at(max);
                    item.at(max) = tmp;
                    Checkmin(max);
                }
                return;

            }
            else{ // dont have correspond; This will not happen with maximum heap
                if(p_correspond == 0)
                    return;
                if(item[index].nstud < item[p_correspond].nstud){
                    schoolType tmp = item[index];
                    item[index] = item[p_correspond];
                    item[p_correspond] = tmp;
                    Checkmin(p_correspond);
                }
                return;
            }
        }
        // only have one child or left>right
        else if(child == item.size() || item.at(child).nstud<=item.at(child-1).nstud)
            child--;
        
        if(item.at(index).nstud < item.at(child).nstud){
            schoolType tmp = item.at(index) ;
            item.at(index) = item.at(child);
            item.at(child) = tmp;
            DelCheckmax(child);
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
    int deap_size(){
        return item.size();
    }
    void deapInsertAll( vector<schoolType> pSet ){
        item = pSet;
        //size = item.size();
    }

    void deapInsert( schoolType pSet ){ // Insert new
        if(item.size()==0){
            schoolType p;
            p.no=0;
            p.nstud=0;
            item.push_back(p);
        }

        item.push_back(pSet); // put new item at the end of heap
        rearrangeDEAP(item.size()-1);
    }

    void del(int index){
        //cout << "The Remove data is: " ;
        int n = item.at(index).no ;
        cout << "[" << n << "] " << item.at(index).sname << item.at(index).dname << ", " << item.at(index).type << ", " << item.at(index).level << ", " << item.at(index).nstud << "\n" ;
            
        if(item.size()==2){
            item.clear();
            return;
        }
        else if(index==2 && item.size()==3){
            item.erase(item.end() - 1);
            return;
        }
        else
            item.at(index) = item.at(item.size()-1); // change the last element to item[index]
        
        
        int correspond, p_correspond;
        bool min = minormax(index, correspond, p_correspond);
        item.erase(item.end() - 1); // delete the last element

        if(min)
            DelCheckmin(index);
        
        else
            DelCheckmax(index);
        
        return;
    }

    void showdeap(){
        if(item.size()==1){
            //cout << "### heap is empty ###\n" ;
            return;
        }
        for( int i = 1, j = pow(2,i), c = 1 ; c < item.size() ; i++, j = pow(2,i) ){
            for( int z = j ; c < item.size()  && z > 0 ; z--, c++ )
                cout << item.at(c).sname << item.at(c).nstud << " " ;
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
    bool deapexist;

    Heap aHeap;
    Deap aDeap;
    
    void show(int i, vector<schoolType> p){ // display one record on screen 
        cout << right << "[" << setw(2) <<i+1<< "]\t" << left << p.at(i).no << "\t" << p[i].nstud << endl;
    }

    public:
    SchoolList(): fileID(""),fileexist(false),heapexist(false),deapexist(false){ clearUp(); } // constructor: do nothing
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
        for(int i = 0 ; i < pSet.size() ; i++)
            show(i, pSet) ;
    }

    void buildHeap(){
        for(int i = 0 ; i < pSet.size() ; i++){
            aHeap.heapInsert(pSet[i]);
            aHeap.sortInsert();
        }
        aHeap.showroot();
        aHeap.showbottom();
        aHeap.showleftmost();
    }

    bool DeapExist(){
        return deapexist;
    }
    void buildDeap(){
        deapexist = true;
        for(int i = 0 ; i < pSet.size() ; i++)
            aDeap.deapInsert(pSet[i]);
            
        aDeap.showbottom();
        aDeap.showleftmost();

    }
    void getMaxK(){
        int k = 0;
        int s = aDeap.deap_size()-1;
        if( s<=0 ) return;

        cout << "\nEnter the value of K in [1," << s << "]: ";
        cin >> k;
        if( k<1 || k>s ) return;

        for(int i=1 ; i<=k ; i++){
            if(aDeap.deapIsEmpty()){
                cout << "The Dataset is empty" << endl;
                return;
            }
            

            cout << "Top "  << setw(3) << i << ": ";
            delBDeap(2);
        }
    }

    void delSDeap(int index){ // delete smallest
        if(aDeap.deapIsEmpty()){
            cout << "The Dataset is empty" << endl;
            return;
        }

        aDeap.del(index);

        aDeap.showdeap();
        aDeap.showbottom();
        aDeap.showleftmost();
    }

    void delBDeap(int index){ // delete biggest
        int s = aDeap.deap_size();
        if(s<=index)
            aDeap.del(s-1);
        else
            aDeap.del(index);
        //aDeap.showdeap();
    }

    void clearUp(){
        fileexist = false;
        heapexist = false;
        deapexist = false;
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
        cout << "\n**** Heap Construction ********";
        cout << "\n* 0. QUIT                     *";
        cout << "\n* 1. Build a max heap         *";
        cout << "\n* 2. Build a DEAP             *";
        cout << "\n* 3. Top-K maximums from DEAP *";
        cout << "\n*******************************";
        cout << "\nInput a choice(0, 1, 2, 3): ";
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
            case 3:
                if (slist.DeapExist())
                    slist.getMaxK();
                /*else
                    cout << "\nPlease choose command 2 first!\n";*/
                break;
            
            default:
                cout << "\nCommand does not exist!\n";
        } // end switch case
    } // end while
} // end main()