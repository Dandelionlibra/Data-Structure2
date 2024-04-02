// 11127138 林雨臻
// 11127156 王佩諭
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

struct AVLNode {
    int key;
    vector<int> no;
    struct AVLNode *lchild ;
    struct AVLNode *rchild ;
    int height;
};

struct Node { // 2-3 tree
    vector<int> keys; // store nstud value
    vector<vector<int>> no; // the no have same key
    vector<Node*> children;
    struct Node *parent;
    bool isLeaf; // leaf or not

    Node() : parent(nullptr), isLeaf(true) {
        children.assign(4, nullptr); // Temporary space for splitting
    }
};

class Two_Three_Tree{
    Node *root; // point to the root of data
    Node *newinsert; // point to the new add data place
    int size; // number of items

    void pop(Node *&r){
        if(r==nullptr)
            return ;
        else{
            for(int i = r->children.size()-1 ; i >= 0 ; i--)
            pop(r->children.at(i));
        }
        delete r;
        r = nullptr;
    }
public:
    Two_Three_Tree() : size(0), root(nullptr), newinsert(nullptr){clearUp();} // constructor of an empty tree
    ~Two_Three_Tree() { clearUp(); } // destructor
    void clearUp() { // cut off the entire tree
        pop(root);
        size = 0;
        root = nullptr;
        newinsert = nullptr;
    } // end clearUp

    bool IsEmpty(){
        return (root==nullptr);
    }

    void SortInOrder(Node *r){
        if( r->keys.size() == 2 && r->keys.at(0) > r->keys.at(1) ){
            int tmpkey = r->keys.at(0);
            r->keys.at(0) = r->keys.at(1);
            r->keys.at(1) = tmpkey;
            r->no.push_back(r->no.at(1));
            r->no.push_back(r->no.at(0));
            r->no.erase(r->no.begin(), r->no.begin()+2);
        }
        else if(r->keys.size() == 3){
            int index_s = 0;
            int index_m = 0;
            int index_b = 0;
            for(int t = 1 ; t < 3 ; t++){
                if(r->keys.at(t) < r->keys.at(index_s))  index_s = t;
                if(r->keys.at(t) > r->keys.at(index_b))  index_b = t;
            }

            if((index_s==0 && index_b==1)||(index_s==1 && index_b==0))  index_m = 2;
            else if((index_s==2 && index_b==1)||(index_s==1 && index_b==2))  index_m = 0;
            else if((index_s==0 && index_b==2)||(index_s==2 && index_b==0))  index_m = 1;

            if( index_s != 0 || index_m != 1 || index_b != 2 ){
                r->keys.push_back(r->keys.at(index_s));
                r->keys.push_back(r->keys.at(index_m));
                r->keys.push_back(r->keys.at(index_b));
                r->keys.erase(r->keys.begin(), r->keys.begin()+3);

                r->no.push_back(r->no.at(index_s));
                r->no.push_back(r->no.at(index_m));
                r->no.push_back(r->no.at(index_b));
                r->no.erase(r->no.begin(), r->no.begin()+3);

            }
        }
    }

    Node* Insert(schoolType pSet, Node *r){
        if(r == nullptr){
            Node *newnode = new Node();
            newnode->keys.push_back(pSet.ngrad);
            vector<int> integer;
            integer.push_back(pSet.no);
            newnode->no.push_back(integer);
            newinsert = newnode; // update the newInsert position
            return newnode;
        }
        else{
            // check whether has same key first
            vector<int>::iterator it = find(r->keys.begin(), r->keys.end(), pSet.ngrad);
            if (it != r->keys.end()){ // have the same key
                int index = distance(r->keys.begin(), it);
                r->no.at(index).push_back(pSet.no);
                // size--;
            }
            else{
                if(r->isLeaf){ // is leaf, add new data
                    newinsert = r; // update the newInsert position
                    r->keys.push_back(pSet.ngrad);
                    vector<int> integer;
                    integer.push_back(pSet.no);
                    r->no.push_back(integer);
                    SortInOrder(r); //check whether swap the data
                }
                else{ // not leaf, go down to find its correct place like a general binary tree
                    if(r->keys.size() == 1){ // 2 node
                        if(pSet.ngrad < r->keys.at(0)) // small than key[0], check the left child
                            r->children.at(0) = Insert(pSet, r->children.at(0));
                        else                                // big than key[0], check the right child
                            r->children.at(1) = Insert(pSet, r->children.at(1));
                    }
                    else if(r->keys.size() == 2){ // 3 node
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
        size++;
        root = Insert( pSet, root );

        if(newinsert->keys.size()==3)// sort
            split();
    }

    void splitchild(Node* &parent, Node* &right){
        // parent node
        parent->keys.push_back(newinsert->keys.at(1)); // the middle value
        parent->no.push_back(newinsert->no.at(1));
        parent->isLeaf = false; // play the node which is grow up, not a leaf
        SortInOrder(parent); // rearrange the data if it is messy

        // Right node
        right->parent = parent; // setting parent node
        right->keys.push_back(newinsert->keys.at(2)); // the biggest value
        right->no.push_back(newinsert->no.at(2));

        // Ｌeft node
        newinsert->parent = parent;
        newinsert->keys.erase(newinsert->keys.begin()+1, newinsert->keys.end());
        newinsert->no.erase(newinsert->no.begin()+1, newinsert->no.end());

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
    }

    void split(){
        if(newinsert->keys.size() < 3)
            return;

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

    void showAll(Node *r){
        if(r!=nullptr){
            for(int i = 0 ; i < r->keys.size() ; i++){
                for(int j = 0 ; j < r->no.at(i).size() ; j++)
                    cout << "no:" << r->no.at(i).at(j) << endl;
                    //show(r->no.at(i).at(j));
            }

            cout << "\n" << endl;
            for(int i = 0 ; i < r->children.size() ; i++)
                showAll(r->children.at(i));
        }

    }
    /*************************************************************************************/
    void find_max(Node* r, int k, vector<int>& result) {
        if(r == nullptr)
            return;

        // for (int i = r->children.size()-1; i >= 0; i--) {
        if(r->keys.size() == 1){
            find_max(r->children.at(1), k, result);
            for(int j = 0 ; j < r->no.at(0).size() ; j++){
                if(k == result.size())
                    return;
                result.push_back(r->no.at(0).at(j));
            }
            find_max(r->children.at(0), k, result);
        }
        else if(r->keys.size() == 2){
            find_max(r->children.at(2), k, result);
            for(int j = 0 ; j < r->no.at(1).size() ; j++){
                if(k == result.size())
                    return;
                result.push_back(r->no.at(1).at(j));
            }
            find_max(r->children.at(1), k, result);
            for(int j = 0 ; j < r->no.at(0).size() ; j++){
                if(k == result.size())
                    return;
                result.push_back(r->no.at(0).at(j));
            }
            find_max(r->children.at(0), k, result);
        }

        for (int i = r->keys.size()-1 ; i >= 0 ; i--) {
            for(int j = 0 ; j < r->no.at(i).size() ; j++){
                if(k == result.size())
                    return;
                vector<int>::iterator it = find(result.begin(), result.end(), r->no.at(i).at(j));
                if (it == r->keys.end()){ // dont have the same key
                    //cout << "\n\033[1;32mpush_back: " << r->no.at(i).at(j) << ": " << r->keys.at(i) <<" \033[0m" << endl; // green
                    result.push_back(r->no.at(i).at(j));
                }

            }
        }

    }

    int get_size(){
        return size;
    }

    vector<int> find_max_outer(int k){
        vector<int> result;
        find_max(root, k, result);
        // find_max(root, k, result);
        return result;
    }


    /*************************************************************************************/

    void Nodenum(Node *r){
        if(r==nullptr)
            return;
        size++;
        for(int i = 0 ; i < r->keys.size()+1 ; i++)
            Nodenum(r->children.at(i));

    }
    void showheight(){
        Node *t = root;
        int height = 0;
        size = 0;
        Nodenum(root);
        while( t!=nullptr && t->children.size()>0 ){
            t = t->children.at(0);
            height++;
        }
        cout << "Tree height = " << height << "\n";
        cout << "Number of nodes = " << size << "\n";
    }
    vector<vector<int>> showroot(){
        return root->no;
    }

}; // end Two_Three_Tree

class AVL_Tree{
    AVLNode *root; // point to the root of data
    int size;
    vector<int> delno;

    int getH(AVLNode *r) const { // calculate the tree by recursion
        if (r == nullptr)
            return 0;
        else {
            int a = getH(r->lchild);
            int b = getH(r->rchild);
            if (a > b) // which is higher
                return a + 1;
            else
                return b + 1;
        }
    } // end getH

    int height(AVLNode *N) {
        if (N == nullptr)
            return 0;
        return N->height;
    }
    int max(int a, int b) { //  get max
        return (a > b) ? a : b;
    }
    AVLNode* insertNode(AVLNode *node, schoolType value){ // insert a data by recursion
        if(node == nullptr){
            size++;
            node = new AVLNode();
            node->key = value.nstud;
            node->no.push_back(value.no);
            node->lchild = nullptr;
            node->rchild = nullptr;
            node->height = 1;
            return node;
        }
        else{
            if(value.nstud == node->key){ // if key equal to value
                node->no.push_back(value.no);
                return node;
            }
            else if (value.nstud < node->key) // if value smaller than key
                node->lchild = insertNode(node->lchild, value);
            else                              // if value bigger than key
                node->rchild = insertNode(node->rchild, value);
        }
        node->height = 1 + max(height(node->lchild), height(node->rchild)); // update_height

        // return confirm_balance(node, value.nstud);
        return checkBalance(node);

    } // end insertNode

    AVLNode* rightRotate(AVLNode *x){ // right rotate subtree rooted with x
        AVLNode *y = x->lchild;
        AVLNode *z = y->rchild;

        //rotation
        y->rchild = x;
        x->lchild = z;

        //Update height
        x->height = max(height(x->lchild), height(x->rchild)) + 1;
        y->height = max(height(y->lchild), height(y->rchild)) + 1;

        return y;
    }

    AVLNode* leftRotate(AVLNode *x){ // left rotate subtree rooted with x
        AVLNode *y = x->rchild;
        AVLNode *z = y->lchild;

        //rotation
        y->lchild = x;
        x->rchild = z;

        //Update height
        x->height = max(height(x->lchild), height(x->rchild)) + 1;
        y->height = max(height(y->lchild), height(y->rchild)) + 1;

        return y;
    }

    AVLNode* deleteMaxNode(AVLNode *r){
        if(r == nullptr)
            return r;
        if(r->rchild == nullptr){ // find the max node
            for(int i = 0 ; i < r->no.size() ; i++) // add the no to delno
                delno.push_back(r->no.at(i));

            if(r->lchild == nullptr){ // don't have left child
                r->no.clear();
                delete r;
                r = nullptr;
                size--;
                return r;
            }
            else{ // have left child
                AVLNode *tmp = r->lchild;
                r->no.clear();
                r->lchild = nullptr;
                delete r;
                r = nullptr;
                size--;
                return tmp;
            }

        }
        r->rchild = deleteMaxNode(r->rchild);
        r->height = 1 + max(height(r->lchild), height(r->rchild));
        // r->height = r->height - 1;

        return checkBalance(r);
    }

    AVLNode* checkBalance(AVLNode *r){
        int balance;
        if (r == nullptr)
            balance = 0;
        else
            balance = height(r->lchild) - height(r->rchild);

        if (balance > 1 && height(r->lchild->lchild) >= height(r->lchild->rchild)) //LL
            return rightRotate(r);

        else if (balance < -1 && height(r->rchild->rchild) >= height(r->rchild->lchild)) //RR
            return leftRotate(r);

        else if (balance > 1 && height(r->lchild->lchild) < height(r->lchild->rchild)){ //LR
            r->lchild = leftRotate(r->lchild);
            return rightRotate(r);
        }
        else if (balance < -1 && height(r->rchild->rchild) < height(r->rchild->lchild)){ //RL
            r->rchild = rightRotate(r->rchild);
            return leftRotate(r);
        }
        return r;
    }


    void clearAVL(AVLNode *&del) { // clear up the entire tree by recursion
        if(del == nullptr){
            delete del;
            del = nullptr;
        }
        else{
            clearAVL(del->lchild);
            clearAVL(del->rchild);
            del->no.clear();
            delete del;
            del = nullptr;
        }
    } // end clearBST
public:
    AVL_Tree() : root(nullptr), size(0) {} // constructor of an empty tree

    bool IsEmpty(){
        return root==nullptr;
    }
    void add(schoolType value) { // insert a node into BST on any type
        root = insertNode(root, value);
    } //  end add

    void Del(){
        root = deleteMaxNode(root);
        // cout << "\033[1;31minordertravel. \033[0m" << endl; // red
        // travel();
    }

    int get_size(){
        return size;
    }

    vector<int> find_max_outer(int k){
        vector<int> result;
        find_max(root, k, result);
        return result;
    }

    void find_max(AVLNode *r, int k, vector<int>& result){
        if(r == nullptr)
            return;

        find_max(r->rchild, k, result);
        for(int i = 0 ; i < r->no.size() ; i++){
            if(k == result.size())
                return;
            // cout << "\n\033[1;32mpush_back: " << r->no.at(i) <<" \033[0m" << endl; // green
            result.push_back(r->no.at(i));
        }

        find_max(r->lchild, k, result);
    }

    void showheight(){
       cout << "Tree height = " << getH(root) << "\n";
       cout << "Number of nodes = " << size << "\n";
    }
    vector<int> showroot(){
        return root->no;
    }

    vector<int> showdel(){
        return delno;
    }

    void clear_delno(){
        delno.clear();
    }

    void inordertravel(AVLNode *r){
        if(r!=nullptr){
            inordertravel(r->lchild);
            cout << r->key << " ";
            inordertravel(r->rchild);
        }
        // cout <<" null " ;
    }

    void travel(){
        inordertravel(root);
        cout << "\n";
    }

    void clearUp() { // cut off the entire tree
        clearAVL(root);
        delno.clear();
        size = 0;
        root = nullptr; // have to initail!!!!
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
        cout << right << "[" << setw(2) <<i+1<< "]\t" << left << p.at(i).no << "\t" << p[i].ngrad<< endl;
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

        if (!pSet.size()) {
            fileexist = false;
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
        for(int i = 0 ; i < pSet.size() ; i++)
            Two_Three.outerInsert(pSet.at(i));

        if(!Two_Three.IsEmpty()){
            Two_Three.showheight();
            vector<vector<int>> no = Two_Three.showroot();
            for(int i = 0, out = 1 ; i < no.size() ; i++){
                for(int j = 0 ; j < no.at(i).size() ; j++){
                    for(int index = 0 ; index < pSet.size() ; index++){
                        if(no.at(i).at(j) == pSet.at(index).no){
                            cout << out << ": [" << no.at(i).at(j) << "] " << pSet.at(index).sname << ", "
                                << pSet.at(index).dname << ", " << pSet.at(index).type << ", " << pSet.at(index).level
                                << ", " << pSet.at(index).nstud << ", " << pSet.at(index).ngrad<< "\n" ;
                            out++;
                            break;
                        }
                    }
                }
            }
            cout<<endl;

        }
    }


    void build_AVL(){
        if(AVL_Tree_exist){
            cout << "### AVL tree has been built. ###" << endl;
        }
        else{
            AVL_Tree_exist = true;
            for (int i = 0; i < pSet.size(); i++)
                AVL.add(pSet.at(i));
        }

        if(!AVL.IsEmpty()){
            AVL.showheight();
            vector<int> no = AVL.showroot();
            int pos=0;
            for(int i = 0, out = 1 ; i < no.size() ; i++){
                for( ; pos < pSet.size() ; pos++){
                    if(no.at(i) == pSet.at(pos).no){
                        cout << out << ": [" << no.at(i) << "] " << pSet.at(pos).sname << ", "
                             << pSet.at(pos).dname << ", " << pSet.at(pos).type << ", " << pSet.at(pos).level
                             << ", " << pSet.at(pos).nstud << ", " << pSet.at(pos).ngrad<< "\n" ;
                        out++;
                        break;
                    }
                }
            }
            cout<<endl;
        }

        // AVL.travel();

    }
    void get23MaxK(){
        int k = 0;
        int s = pSet.size();
        if( s <= 0 ) return;

        cout << "\nEnter K in [1," << s << "]: ";
        cin >> k;
        if( k<1 || k>s ) return;

        cout << "K: " << k << endl;
        vector<int> no = Two_Three.find_max_outer(k);

        int index = 1;
        for(int i = 0 ; i < no.size() ; i++){
            for(int j = 0 ; j < pSet.size() ; j++){
                if(no.at(i) == pSet.at(j).no){
                    cout << index << ": ";
                    cout << "[" << no.at(i) << "] " << pSet.at(j).sname << ", "
                        << pSet.at(j).dname << ", " << pSet.at(j).type << ", " << pSet.at(j).level
                        << ", " << pSet.at(j).nstud << ", " << pSet.at(j).ngrad<< "\n" ;
                    index++;
                    break;
                }
            }
        }
    }
    void getAVLMaxK(){
        int k = 0;
        int s = pSet.size();
        if( s <= 0 ) return;

        cout << "\nEnter K in [1," << s << "]: ";
        cin >> k;
        if( k<1 || k>s ) return;

        cout << "K: " << k << endl;
        vector<int> no = AVL.find_max_outer(k);

        int index = 1;
        for(int i = 0 ; i < no.size() ; i++){
            for(int j = 0 ; j < pSet.size() ; j++){
                if(no.at(i) == pSet.at(j).no){
                    cout << index << ": ";
                    cout << "[" << no.at(i) << "] " << pSet.at(j).sname << ", "
                        << pSet.at(j).dname << ", " << pSet.at(j).type << ", " << pSet.at(j).level
                        << ", " << pSet.at(j).nstud << ", " << pSet.at(j).ngrad<< "\n" ;
                    index++;
                    break;
                }
            }
        }

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
        cout << "\n* 3. Top-K search on 23 tree  *";
        cout << "\n* 4. Top-K search on AVL tree *";


        cout << "\n*******************************";
        cout << "\nInput a choice(0, 1, 2, 3, 4): ";
        // cout << "\nInput a choice(0, 1, 2, 3): ";
        cin >> input;
        if(input == "0")
            command = 0;
        else if(input == "1")
            command = 1;
        else if(input == "2")
            command = 2;
        else if(input == "3")
            command = 3;
        else if(input == "4")
            command = 4;
        else
            command = -1;

        switch (command) {
            case 0:
                isEnd = true;
                break;
            case 1:
                if (slist.readFile()) { // if file exist
                    //cout << "<23 tree>\n";
                    slist.build_23();
                }
                break;
            case 2:
                if (slist.fileExist()){
                    slist.build_AVL();
                }
                else
                    cout << "### Choose 1 first. ###" << endl;
                break;
            case 3:
                if (slist.Tree23Exist())
                    slist.get23MaxK();
                else
                    cout << "\nPlease choose command 1 first!\n";
                break;
            case 4:
                if (slist.fileExist()){
                    if (slist.AVLExist())
                        slist.getAVLMaxK();
                    else
                        cout << "### Choose 2 first. ###\n";
                }
                else
                    cout << "### Choose 1 first. ###\n";
                break;

            default:
                cout << "\nCommand does not exist!\n";
        } // end switch case
    } // end while
} // end main()
