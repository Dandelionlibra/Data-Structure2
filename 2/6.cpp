// 11127138 林雨臻
#include <cstdlib>  // atoi, system
#include <fstream>  // open, is_open
#include <iomanip>  // setw, setprecision
#include <iostream> // cout, endl, fixed
#include <string>   // string
#include <vector>   // vector
#include <algorithm> // find
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
enum color{
    RED,
    BLACK
};

typedef struct AVLNode {
    schoolType data;
    struct AVLNode *parent ;
    struct AVLNode *lchild ;
    struct AVLNode *rchild ;
}AVLN;

struct Node { // 2-3 tree
    vector<int> keys; // store nstud value
    vector<vector<schoolType>> data; // the data have same key
    vector<Node*> children; //
    struct Node *parent;
    bool isLeaf; // leaf or not

    Node() : parent(nullptr), isLeaf(true) {
        children.assign(4, nullptr); // Temporary space for splitting
    }
    /*void clearUp(){
        for(int i = 0 ; i < 4 ; i++){
            delete children.at(i);
            children.at(i) = nullptr;
        }
        //delete parent;
        parent = nullptr;
        children.clear();
        keys.clear();
        data.clear();
    }
    ~Node(){clearUp();}*/
};

class Two_Three_Tree{
    Node *root; // point to the root of data
    Node *newinsert; // point to the new add data place
    int size; // number of items

    void pop(Node *r){
        if(r==nullptr)
            return ;
        else{
            for(int i = r->children.size()-1 ; i >= 0 ; i--)
            pop(r->children.at(i));
        }
        clear23tree(r);
    }
    void clear23tree(Node *r){
        for(int i = 0 ; i < 4 ; i++){
            delete r->children.at(i);
            r->children.at(i) = nullptr;
        }
        //delete parent;
        r->parent = nullptr;
        r->children.clear();
        r->keys.clear();
        r->data.clear();
    }
public:
    Two_Three_Tree() : size(0), root(nullptr), newinsert(nullptr){} // constructor of an empty tree
    ~Two_Three_Tree() { clearUp(); } // destructor
    void clearUp() { // cut off the entire tree
        pop(root);
        size = 0;
        delete newinsert;
        newinsert = nullptr;
        //newinsert->clearUp(); // point to the new add data place
    } // end clearUp

    bool IsEmpty(){
        return (root==nullptr);
    }

    void SortInOrder(Node *r){
        if( r->keys.size() == 2 && r->keys.at(0) > r->keys.at(1) ){
            int tmpkey = r->keys.at(0);
            r->keys.at(0) = r->keys.at(1);
            r->keys.at(1) = tmpkey;

            vector<schoolType> tmpdata = r->data.at(0);
            r->data.at(0) = r->data.at(1);
            r->data.at(1) = tmpdata;
        }
        else if(r->keys.size() == 3){
            int index_s = 0;
            int index_m = 0;
            int index_b = 0;
            for(int t = r->keys.size()-1 ; t>=0 ; t--){
                if(r->keys.at(1) < r->keys.at(index_s))  index_s = 1;
                else if(r->keys.at(2) < r->keys.at(index_s))  index_s = 2;

                if(r->keys.at(1) > r->keys.at(index_b))  index_b = 1;
                else if(r->keys.at(2) > r->keys.at(index_b))  index_b = 2;
            }

            if((index_s==0 && index_b==1)||(index_s==1 && index_b==0))  index_m = 2;
            else if((index_s==0 && index_b==1)||(index_s==1 && index_b==0))  index_m = 0;
            else if((index_s==0 && index_b==2)||(index_s==2 && index_b==0))  index_m = 1;
            cout << "index_s: " << index_s << " index_m: " << index_m << " index_b: " << index_b << endl ;

            if( index_s != 0 && index_m != 1 && index_b != 2 ){
                vector<int> tmpkeys;
                tmpkeys.push_back(r->keys.at(index_s));
                tmpkeys.push_back(r->keys.at(index_m));
                tmpkeys.push_back(r->keys.at(index_b));

                vector<vector<schoolType>> tmpdata;
                tmpdata.push_back(r->data.at(index_s));
                tmpdata.push_back(r->data.at(index_m));
                tmpdata.push_back(r->data.at(index_b));

                r->keys.clear();
                r->data.clear();
                r->keys = tmpkeys;
                r->data = tmpdata;
            }

        }
    }

    Node* Insert(schoolType pSet, Node *r){
        //item = pSet; // put new item at the end of heap
        newinsert = r; // update the newInsert position
        if(r == nullptr){
            Node *newnode = new Node();
            newnode->keys.push_back(pSet.nstud);
            vector<schoolType> p;
            p.push_back(pSet);
            newnode->data.push_back(p);
            r = newnode;
            size++;
        }
        else{// check whether has same key first
            vector<int>::iterator it = find(r->keys.begin(), r->keys.end(), pSet.ngrad);
            if (it != r->keys.end()){ // have the same key
                int index = distance(r->keys.begin(), it);
                r->data.at(index).push_back(pSet);
            }
            else{
                if(r->isLeaf){ // is leaf, add new data
                    //int s = r->keys.size();
                    //if(s == 1 || s==2){
                    r->keys.push_back(pSet.ngrad);
                    vector<schoolType> p;
                    p.push_back(pSet);
                    r->data.push_back(p);
                    SortInOrder(r); //check whether swap the data
                    //}
                    size++;
                }
                else{ // not leaf, go down to find its correct place like a general binary tree
                    if(r->keys.size() == 1){
                        if(pSet.ngrad == r->keys.at(0))
                            r->data.at(0).push_back(pSet);
                        else if(pSet.ngrad < r->keys.at(0)) // small than key[0], check the left child
                            r->children.at(0) = Insert(pSet, r->children.at(0));
                        else                                // big than key[0], check the right child
                            r->children.at(1) = Insert(pSet, r->children.at(1));
                    }
                    else if(r->keys.size() == 2){
                        if(pSet.ngrad < r->keys.at(0)) // small than key[0], check the left child
                            r->children.at(0) = Insert(pSet, r->children.at(0));
                        else if(pSet.ngrad < r->keys.at(1)) // small than key[1], check the mid child
                            r->children.at(1) = Insert(pSet, r->children.at(1));
                        else                                // big than key[0] & key[1], check the right child
                            r->children.at(2) = Insert(pSet, r->children.at(2));
                    }
                }

            }
        }
        return r;
    }

    void outerInsert(schoolType pSet){
        cout << "\033[31m" << pSet.sname << "\033[0m" << "\t" << "\033[31m" << pSet.ngrad << "\033[1m" << endl;
        root = Insert( pSet, root );

        /*if(newinsert->keys.size()==3)// sort
            split();*/
    }
    
    void splitchild(Node* parent, Node* right){
        /*function*/

        // parent node, the mid key must combine with its parent node
        parent->keys.push_back(newinsert->keys.at(1)); // the middle value
        parent->data.push_back(newinsert->data.at(1));
        parent->isLeaf = false; // play the node which is grow up, not a leaf
        SortInOrder(parent); // rearrange the data if it is messy

        // Right node
        right->parent = parent; // setting parent node
        right->keys.push_back(newinsert->keys.at(2)); // the biggest value
        right->data.push_back(newinsert->data.at(2));
        
        // Ｌeft node
        newinsert->parent = parent;
        newinsert->keys.erase(newinsert->keys.begin()+1, newinsert->keys.end());
        newinsert->data.erase(newinsert->data.begin()+1, newinsert->data.end());
        
        if(!newinsert->isLeaf){ // if newinsert is innernode, update right and leftnode'child
            right->isLeaf = false;
            right->children.at(0) = newinsert->children.at(2);
            right->children.at(1) = newinsert->children.at(3);
            newinsert->children.at(2)->parent = right;
            newinsert->children.at(3)->parent = right;
            // delete the children in left which give to right
            newinsert->children.at(2) = nullptr;
            newinsert->children.at(3) = nullptr;
        }
        /*function*/
    }

    void split(){
        //vector<schoolType> temp;
        if(newinsert->parent==nullptr){ // root
            Node* parent = new Node();
            root = parent; // update new root
            Node* right = new Node();
            splitchild(parent, right);
            
            // connect child and parent
            parent->children.at(0) = newinsert;
            parent->children.at(1) = right;
        }
        else{ // leaf if(newinsert->isLeaf)
            Node* parent = newinsert->parent;
            Node* right = new Node();
            splitchild(parent, right);
            if(parent->keys.size()==2){
                if(newinsert == parent->children.at(0)){
                    // connect child and parent
                    parent->children.at(2) = parent->children.at(1);
                    parent->children.at(0) = newinsert;
                    parent->children.at(1) = right;
                }
                else{ // if(newinsert == parent->children.at(1))
                    // connect child and parent
                    parent->children.at(1) = newinsert;
                    parent->children.at(2) = right;
                }
            }
            else{ // (parent->keys.size()==3)
                if(newinsert == parent->children.at(0)){
                    // connect child and parent
                    parent->children.at(3) = parent->children.at(2);
                    parent->children.at(2) = parent->children.at(1);
                    parent->children.at(1) = right;
                    parent->children.at(0) = newinsert;
                }
                else if(newinsert == parent->children.at(1)){
                    // connect child and parent
                    parent->children.at(3) = parent->children.at(2);
                    parent->children.at(2) = right;
                    parent->children.at(1) = newinsert;
                }
                else{ // if(newinsert == parent->children.at(2))
                    // connect child and parent
                    parent->children.at(3) = right;
                    parent->children.at(2) = newinsert;
                }
            }
            newinsert = newinsert->parent;
            split();
        }
        
    }
    
    void show(schoolType pSet){
        //cout << left << "\t#\t" << setw(32) << "Name" << "Type 1\t\t" << "HP" << "\t" << "Attack" << "\t" << "Defense\n";
        cout << "\033[32m name:" << pSet.sname << "\t ngrad:" << pSet.ngrad << "\033[1m" << endl;
    }

    void showAll(Node *r){
        for( Node *r = root ; i < item.size() ; i++ ){
            show(r->data.at(i).at(j));
        }        
        
    }


    void showheight(){
        //int tmp = item.size() ;
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
        //cout << "[" << item.at(idx).no << "] " << item.at(idx).nstud << "\n" ;
    }
/*
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
        cout << "[" << item.at(idx).list.at(0).no << "] " << item.at(idx).nstud << "\n" ;
    }*/






}; // end Two_Three_Tree

class AVL_Tree{
    //vector<schoolType> item;
    AVLNode *root; // point to the root of data
    //schoolType rootItem;
    int size; // number of heap items

    void clearHeap(){
        size = 0;
        //records.clear();
    }
public:
    AVL_Tree() : size(0) {} // constructor of an empty tree

    bool IsEmpty(){
        return (root==nullptr);
    }

    /*void Insert( schoolType pSet ){
        //item = pSet; // put new item at the end of heap
        AVLNode t;
        t.list.push_back(pSet);
        t.amount = 1;



        item.push_back(t);
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
    */

    void clearUp() { // cut off the entire tree
        clearHeap();
    } // end clearUp

    ~AVL_Tree() { clearUp(); } // destructor
}; // end AVL_Tree




class SchoolList{
    vector<schoolType> pSet; // set of data records
    string fileID; // number to form a file name
    bool fileexist; // already enter a file or not
    bool TwoThree_Tree_exist;
    bool AVL_Tree_exist;

    Two_Three_Tree Two_Three;
    AVL_Tree AVL;
    
    void show(int i, vector<schoolType> p){ // display one record on screen 
        cout << right << "[" << setw(2) <<i+1<< "]\t" << left << p.at(i).no << "\t" << p[i].nstud << endl;
    }

    public:
    SchoolList(): fileID(""),fileexist(false),TwoThree_Tree_exist(false),AVL_Tree_exist(false){ clearUp(); } // constructor: do nothing
    bool isEmpty(){ // check if there is nothing
        if (pSet.size() == 0)
            return true;
        return false;
    }
    bool fileExist(){
        return fileexist;
    }
    bool Tree23Exist(){
        return TwoThree_Tree_exist;
    }
    bool AVLExist(){
        return AVL_Tree_exist;
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




    void build_23(){
        TwoThree_Tree_exist = true;
        for(int i = 0 ; i < pSet.size() ; i++){
            Two_Three.outerInsert(pSet.at(i));
            //Two_Three.sortInsert();
        }
        //Two_Three.showroot();
        //Two_Three.showbottom();
        //Two_Three.showleftmost();
    }


    void build_AVL(){
        AVL_Tree_exist = true;
        /*for(int i = 0 ; i < pSet.size() ; i++)
            AVL.Insert(pSet[i]);
            
        AVL.showbottom();
        AVL.showleftmost();*/

    }



    void clearUp(){
        fileexist = false;
        TwoThree_Tree_exist = false;
        AVL_Tree_exist = false;
        fileID.clear();
        pSet.clear();
        Two_Three.clearUp();
        AVL.clearUp();

    } // end clearUp()

    ~SchoolList(){ clearUp(); } // desturctor: destory the object
}; // end SchoolList


int main() {
    SchoolList slist;
    int command = -1;
    bool isEnd = false;
    string fileName, input;
    while (!isEnd) {
        cout << "\n**** Balanced Search Trees ****";
        cout << "\n* 0. QUIT                     *";
        cout << "\n* 1. Build 23 tree            *";
        cout << "\n* 2. Build AVL tree           *";
        //cout << "\n* 3. Top-K maximums from DEAP *";
        cout << "\n*******************************";
        cout << "\nInput a choice(0, 1, 2): ";
        //cout << "\nInput a choice(0, 1, 2, 3): ";
        cin >> command;
        switch (command) {
            case 0:
                isEnd = true;
                break;
            case 1:
                if (slist.readFile()) { // if file exist
                    cout << "<23 tree>\n";
                    slist.build_23();
                }
                break;
            case 2:
                if (slist.readFile()){
                    cout << "<AVL tree>\n";
                    slist.build_AVL();
                }
                break;
            /*case 3:
                if (slist.DeapExist())
                    slist.getMaxK();
                /*else
                    cout << "\nPlease choose command 2 first!\n";
                break;*/
            
            default:
                cout << "\nCommand does not exist!\n";
        } // end switch case
    } // end while
} // end main()