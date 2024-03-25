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

struct AVLNode {
    int key;
    vector<int> no;
    struct AVLNode *lchild ;
    struct AVLNode *rchild ;
    int height;
};

struct Node { // 2-3 tree
    vector<int> keys; // store nstud value
    //vector<vector<schoolType>> data; // the data have same key
    vector<vector<int>> no; // the no have same key
    vector<Node*> children; //
    struct Node *parent;
    bool isLeaf; // leaf or not

    Node() : parent(nullptr), isLeaf(true) {
        children.assign(4, nullptr); // Temporary space for splitting
    }
};

class Two_Three_Tree{
    Node *root; // point to the root of data
    Node *newinsert; // point to the new add data place
    vector<int> delno;
    int size; // number of items

    void SortInOrder(Node *r){
        cout << "\033[1;32m SortInOrder. \033[0m" << endl; // green
        for(int i = 0 ; i < r->keys.size() ; i++)
            cout << r->keys.at(i) <<" ";
        cout << endl;

        if( r->keys.size() == 2 && r->keys.at(0) > r->keys.at(1) ){
            int tmpkey = r->keys.at(0);
            r->keys.at(0) = r->keys.at(1);
            r->keys.at(1) = tmpkey;

            /**/
            // r->data.push_back(r->data.at(1));
            // r->data.push_back(r->data.at(0));
            // r->data.erase(r->data.begin(), r->data.begin()+2);

            r->no.push_back(r->no.at(1));
            r->no.push_back(r->no.at(0));
            r->no.erase(r->no.begin(), r->no.begin()+2);
            /**/

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
            // cout << "index_s: " << index_s << " index_m: " << index_m << " index_b: " << index_b << endl ;

            if( index_s != 0 || index_m != 1 || index_b != 2 ){
                //vector<int> tmpkeys;
                r->keys.push_back(r->keys.at(index_s));
                r->keys.push_back(r->keys.at(index_m));
                r->keys.push_back(r->keys.at(index_b));
                r->keys.erase(r->keys.begin(), r->keys.begin()+3);

                /**/
                //vector<vector<schoolType>> tmpdata;
                // r->data.push_back(r->data.at(index_s));
                // r->data.push_back(r->data.at(index_m));
                // r->data.push_back(r->data.at(index_b));
                // r->data.erase(r->data.begin(), r->data.begin()+3);

                r->no.push_back(r->no.at(index_s));
                r->no.push_back(r->no.at(index_m));
                r->no.push_back(r->no.at(index_b));
                r->no.erase(r->no.begin(), r->no.begin()+3);
                /**/
            
            }

        }
        for(int i = 0 ; i < r->keys.size() ; i++)
            cout << r->keys.at(i) <<" ";
        cout << "\n\033[1;32m End SortInOrder. \033[0m" << endl; // green

    }

    
    // Node* Insert(schoolType pSet, Node *r){
    Node* Insert(int key, vector<int> no, Node *r){ //(pSet.ngrad, no, root)
        if(r == nullptr){
            Node *newnode = new Node();
            newnode->keys.push_back(key);
            /**/
            // vector<schoolType> p;
            // p.push_back(pSet);
            // newnode->data.push_back(p);

            newnode->no.push_back(no);
            /**/

            newinsert = newnode; // update the newInsert position
            //size++;
            //cout << "\033[1;33m This node is null, add new data. \033[0m" << endl; // yellow
            return newnode;
        }
        else{
            // check whether has same key first
            vector<int>::iterator it = find(r->keys.begin(), r->keys.end(), key);
            if (it != r->keys.end()){ // have the same key
                int index = distance(r->keys.begin(), it);
                /**/
                // r->data.at(index).push_back(pSet);
                r->no.at(index).push_back(no.at(0));
                /**/
                //cout << "\033[1;35m Add to exist data. \033[0m" << endl; // purple

                //size++;
            }
            else{
                if(r->isLeaf){ // is leaf, add new data
                    newinsert = r; // update the newInsert position
                    r->keys.push_back(key);
                    /**/
                    // vector<schoolType> p;
                    // p.push_back(pSet);
                    // r->data.push_back(p);

                    r->no.push_back(no);
                    /**/
                    SortInOrder(r); //check whether swap the data

                    //size++;
                    //cout << "\033[1;33m This node is leaf, add new no. \033[0m" << endl; // yellow

                }
                else{ // not leaf, go down to find its correct place like a general binary tree
                    if(r->keys.size() == 1){ // 2 node
                        //cout << "\033[1;31m keys.size() == 1 \033[0m" << endl; // red
                        if(key < r->keys.at(0)) // small than key[0], check the left child
                            r->children.at(0) = Insert(key, no, r->children.at(0));
                        else                                // big than key[0], check the right child
                            r->children.at(1) = Insert(key, no, r->children.at(1));
                    }
                    else if(r->keys.size() == 2){ // 3 node
                        //cout << "\033[1;31m keys.size() == 2 \033[0m" << endl; // red
                        if(key < r->keys.at(0)) // small than key[0], check the left child
                            r->children.at(0) = Insert(key, no, r->children.at(0));
                        else if(key < r->keys.at(1)) // small than key[1], check the mid child
                            r->children.at(1) = Insert(key, no, r->children.at(1));
                        else                                // big than key[0] & key[1], check the right child
                            r->children.at(2) = Insert(key, no, r->children.at(2));
                    }
                }

            }
        }
        return r;
    }

    void splitchild(Node* &parent, Node* &right){
        // parent node, the mid key must combine with its parent node
        parent->keys.push_back(newinsert->keys.at(1)); // the middle value

        /**/
        // parent->data.push_back(newinsert->data.at(1));
        parent->no.push_back(newinsert->no.at(1));
        /**/

        parent->isLeaf = false; // play the node which is grow up, not a leaf
        SortInOrder(parent); // rearrange the data if it is messy

        // Right node
        right->parent = parent; // setting parent node
        right->keys.push_back(newinsert->keys.at(2)); // the biggest value

        /**/
        // right->data.push_back(newinsert->data.at(2));
        right->no.push_back(newinsert->no.at(2));
        /**/
        
        // Ｌeft node
        newinsert->parent = parent;
        newinsert->keys.erase(newinsert->keys.begin()+1, newinsert->keys.end());
        /**/
        // newinsert->data.erase(newinsert->data.begin()+1, newinsert->data.end());
        newinsert->no.erase(newinsert->no.begin()+1, newinsert->no.end());
        /**/
        
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
        //vector<schoolType> temp;
        // cout << "\033[1;34m split. \033[0m" << endl; // blue
        if(newinsert->keys.size() < 3){
            // cout << "\033[1;34m End split. \033[0m\n" << endl; // blue
            return;
        }

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
        // cout << "\033[1;34m End split. \033[0m\n" << endl; // blue
        
    }

    void Delsplit(){
        // Ｌeft node
        Node * left = new Node();
        left->parent = newinsert;
        left->keys.push_back(newinsert->keys.at(0));
        left->no.push_back(newinsert->no.at(0));

        // Right node
        Node * right = new Node();
        right->parent = newinsert; // setting parent node
        right->keys.push_back(newinsert->keys.at(2)); // the biggest value
        right->no.push_back(newinsert->no.at(2));
        
        newinsert->isLeaf = false;
        newinsert->children.at(0) = left;
        newinsert->children.at(1) = right;
        newinsert->keys.erase(newinsert->keys.begin(), newinsert->keys.begin()+1);
        newinsert->keys.erase(newinsert->keys.begin()+1, newinsert->keys.end());
        newinsert->no.erase(newinsert->no.begin(), newinsert->no.begin()+1);
        newinsert->no.erase(newinsert->no.begin()+1, newinsert->no.end());

    }
    void checkcase(Node *&r, int index, int key){
        Node *tmp = r->parent;
        delno = r->no.at(index); // store delete no
        if(r->isLeaf){
            cout << "\033[1;32mCurrent size is " << r->keys.size() <<" \033[0m" << endl; // green
            if(r->keys.size() == 2){ // delete the value, and done!
                for(int i = 0 ; i < r->keys.size() ; i++)
                    cout << r->keys.at(i) << " ";
                cout << "\n\033[1;32m1.Erase " << key << " vs " << r->keys.at(index) <<" \033[0m" << endl; // green

                // erase data
                r->keys.erase(r->keys.begin()+index, r->keys.begin()+index+1);
                r->no.erase(r->no.begin()+index, r->no.begin()+index+1);

                for(int i = 0 ; i < r->keys.size() ; i++)
                    cout << r->keys.at(i) << " ";
                cout << "\n\033[1;32m1.End Erase\033[0m" << endl; // green
                return;
            }
            else{ // r->keys.size() == 1
                if(tmp == nullptr){ // only remain root and keys==1, directly delete root
                    // delete r;
                    // r = nullptr;
                    pop(r);
                    return;
                }
                if(tmp->keys.size() == 1){
                    // setting the childnode which need to insert tmp
                    int tmpchild;
                    if(r == tmp->children.at(0))  tmpchild = 1;
                    else    tmpchild = 0;

                    // setting the parentchild node, the location where need to insert tmp
                    int parentchild;
                    if(tmp == tmp->parent->children.at(0))  parentchild = 0;
                    else if(tmp == tmp->parent->children.at(1))  parentchild = 1;
                    else    parentchild = 2;

                    //cout << "\033[0;32;42mtmp.parent.childssize:  \033[0m" << tmp->parent->children.at(parentchild)->keys.at(0) << endl; // green
                    // rotate
                    newinsert = tmp->children.at(tmpchild);
                    r->keys.at(0) = tmp->keys.at(0);
                    r->no.at(0) = tmp->no.at(0);
                    tmp->keys.at(0) = newinsert->keys.at(0);
                    tmp->no.at(0) = newinsert->no.at(0);

                    vector<int> k;
                    k.push_back(newinsert->keys.at(1));
                    newinsert->keys = k;

                    vector<vector<int>> n;
                    n.push_back(newinsert->no.at(1));
                    newinsert->no = n;
                    

                    cout << "\033[0;32;42m2.Start showAll!!! \033[0m" << endl; // green
                    showAll(root);
                    cout << "\033[0;32;42m2.End showAll!!! \033[0m" << endl; // green

                    return;
                }
                else{ //if(tmp->keys.size() == 2){
                    
                }


            }
        }
        else{

        }
    }

    Node* Delete(Node *r, int key){
        if(r == nullptr){
            //newinsert = r; // update the delete position
            cout << "\033[1;33m This node is null. \033[0m" << endl; // yellow
            return nullptr;
        }

        //try to find the same key first
        vector<int>::iterator it = find(r->keys.begin(), r->keys.end(), key);
        if (it != r->keys.end()){
            cout << "\033[1;35m Find delete position. \033[0m" << endl; // purple
            int index = distance(r->keys.begin(), it);
            
            checkcase(r, index, key);

            cout << "\033[1;35m End delete data. \033[0m" << endl; // purple

            //size++;
        }

        //cannot find same key
        else if(r->keys.size()==1){ // 2 node
            cout << "\033[1;31m Delete: keys.size() == 1 \033[0m" << endl; // red
            if(key < r->keys.at(0))
                r->children.at(0) = Delete(r->children.at(0), key);
            else
                r->children.at(1) = Delete(r->children.at(1), key);
        }
        else{ // if(r->keys.size() == 2) // 3 node
            cout << "\033[1;31m Delete: keys.size() == 2 \033[0m" << endl; // red
            if(key < r->keys.at(0)) // small than key[0], check the left child
                r->children.at(0) = Delete(r->children.at(0), key);
            else if(key < r->keys.at(1)) // small than key[1], check the mid child
                r->children.at(1) = Delete(r->children.at(1), key);
            else                                // big than key[0] & key[1], check the right child
                r->children.at(2) = Delete(r->children.at(2), key);
        }

        return r;
    }

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
        size = 0;\
        root = nullptr;
        newinsert = nullptr;
        delno.clear();
    } // end clearUp

    bool IsEmpty(){
        return (root==nullptr);
    }

    void outerInsert(schoolType pSet){
        //cout << "currently add  " << "name:" << pSet.sname << "\t ngrad:" << pSet.ngrad <<  endl;
        //cout << "\n\n\033[1;31mAll:\033[0m" << endl;
        //showAll(root);
        vector<int> no;
        no.push_back(pSet.no);
        root = Insert(pSet.ngrad, no, root);
        //root = Insert( pSet, root );
        if(newinsert->keys.size()==3)// sort
            split();
    }

    void outerDelete(int value){
        delno.clear(); // initial delno
        if(root != nullptr)
            root = Delete(root, value);
        else
            cout << "\033[1;31m ROOT is null. \033[0m" << endl; // red
        
        // showAll(root);
    }

    vector<int> showDelete(){
        return delno;
    }
    
    /*void show(int no){
        //cout << "name:" << pSet.sname << "\t ngrad:" << pSet.ngrad << endl;
        cout << "no:" << no << endl;
    }*/

    void showAll(Node *r){
        // all
        if(r!=nullptr){
            for(int i = 0 ; i < r->keys.size() ; i++){
                for(int j = 0 ; j < r->no.at(i).size() ; j++)
                    cout << "no:" << r->no.at(i).at(j) << ", key:" << r->keys.at(i) << endl;
                
            }

            cout << "\n" << endl;
            for(int i = 0 ; i < r->children.size() ; i++)
                showAll(r->children.at(i));
        }

    }

    void s(){
        showAll(root);
    }

    void Nodenum(Node *r){
        //int nodenum = 0;
        if(r==nullptr)
            return;
        size++;
        for(int i = 0 ; i < r->keys.size()+1 ; i++){
            cout << "key:" << r->keys.at(0) << "," <<endl;
            Nodenum(r->children.at(i));
        }
        //return size;
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

    int height(AVLNode *N) {
        if (N == nullptr)
            return 0;
        return N->height;
    }
    AVLNode* insertNode(AVLNode *node, schoolType value){ // insert a node by recursion
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

        //update_height(node, value);
        node->height = 1 + max(height(node->lchild), height(node->rchild)); // update_height
        return confirm_balance(node, value.nstud);

    } // end insertNode

    AVLNode* confirm_balance(AVLNode *r, int value){
        int balance;
        if (r == nullptr)
            balance = 0;
        else
            balance = height(r->lchild) - height(r->rchild);

        // LL
        if (balance > 1 && value < r->lchild->key)
            return rightRotate(r);
        // RR
        if (balance < -1 && value > r->rchild->key)
            return leftRotate(r);
        // LR
        if (balance > 1 && value > r->lchild->key) {
            r->lchild = leftRotate(r->lchild);
            return rightRotate(r);
        }
        // RL
        if (balance < -1 && value < r->rchild->key) {
            r->rchild = rightRotate(r->rchild);
            return leftRotate(r);
        }
        return r;
    }
    
    int max(int a, int b) { // Function to get maximum of two integers
        return (a > b) ? a : b;
    }
    
    AVLNode* rightRotate(AVLNode *r){ // Function to right rotate subtree rooted with y
        AVLNode *tmp1 = r->lchild;
        AVLNode *tmp2 = tmp1->rchild;

        // Perform rotation
        tmp1->rchild = r;
        r->lchild = tmp2;

        // Update heights
        r->height = max(height(r->lchild), height(r->rchild)) + 1;
        tmp1->height = max(height(tmp1->lchild), height(tmp1->rchild)) + 1;

        // Return new root
        return tmp1;
    }

    AVLNode* leftRotate(AVLNode *r){ // Function to left rotate subtree rooted with x
        AVLNode *tmp1 = r->rchild;
        AVLNode *tmp2 = tmp1->lchild;

        // Perform rotation
        tmp1->lchild = r;
        r->rchild = tmp2;

        // Update heights
        r->height = max(height(r->lchild), height(r->rchild)) + 1;
        tmp1->height = max(height(tmp1->lchild), height(tmp1->rchild)) + 1;

        // Return new root
        return tmp1;
    }

    void clearAVL(AVLNode *&del) { // clear up the entire tree by recursion
        if (del == nullptr) {
            delete del;
            del = nullptr;
        } else {
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
    void outerInsert(schoolType value) { // insert a node into BST on any type
        root = insertNode(root, value);
        
    } //  end add

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

    /*int getNode(AVLNode *r) const { // calculate the tree by recursion
        if (r == nullptr)
            return 0;
        else {
            int a = getNode(r->lchild)+1;
            int b = getNode(r->rchild)+1;
            return a + b;
        }
    } // end getH*/

    void showheight(){
       cout << "Tree height = " << getH(root) << "\n";
       cout << "Number of nodes = " << size << "\n";
    }
    vector<int> showroot(){
        return root->no;
    }

    void clearUp() { // cut off the entire tree
        clearAVL(root);
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
            //cout << "\n### Get nothing from the file " << fileName << "!###\n";
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
        for(int i = 0 ; i < pSet.size() ; i++){
            Two_Three.outerInsert(pSet.at(i));
            //Two_Three.sortInsert();
        }
        Two_Three.s();

        if(!Two_Three.IsEmpty()){
            Two_Three.showheight();
            vector<vector<int>> no = Two_Three.showroot();
            //vector<int> no = Two_Three.showroot();
            //vector<schoolType> tmp = pSet;
            //int pos=0;
            for(int i = 0, out = 1 ; i < no.size() ; i++){
                for(int j = 0 ; j < no.at(i).size() ; j++){
                    for(int index = 0 ; index < pSet.size() ; index++){
                        if(no.at(i).at(j) == pSet.at(index).no){
                            cout << out << ": [" << no.at(i).at(j) << "] " << pSet.at(index).sname << ", "
                                << pSet.at(index).dname << ", " << pSet.at(index).type << ", " << pSet.at(index).level
                                << ", " << pSet.at(index).nstud << ", " << pSet.at(index).ngrad<< "\n" ;
                            out++;
                            //tmp.erase(tmp.begin(), tmp.begin()+index);
                            break;
                        }
                    }
                }
            }
            cout << "\n";
        }
    }

    void Del_23(){
        if(pSet.size()==0){
            cout << "Pset == null!" << endl;
            return;
        }
        int input;
        cout << "please enter an number:" ;
        cin >> input ;
        Two_Three.outerDelete(input);

        if(!Two_Three.IsEmpty())
            Two_Three.showheight();

        vector<int> no = Two_Three.showDelete();
        int pos=0;
        for(int i = 0, out = 1 ; i < no.size() ; i++){
            for( ; pos < pSet.size() ; pos++){
                if(no.at(i) == pSet.at(pos).no){
                    cout << out << ": [" << no.at(i) << "] " << pSet.at(pos).sname << ", "
                            << pSet.at(pos).dname << ", " << pSet.at(pos).type << ", " << pSet.at(pos).level
                            << ", " << pSet.at(pos).nstud << ", " << pSet.at(pos).ngrad<< "\n" ;
                    out++;
                    pSet.erase(pSet.begin()+pos, pSet.begin()+pos+1);
                    showAll();
                    pos--;
                    break;
                }
            }
        }
        cout << "\n";

        if(Two_Three.IsEmpty())
            TwoThree_Tree_exist = false;
    }

    void build_AVL(){
        if(AVL_Tree_exist){
            cout << "### AVL tree has been built. ###" << endl;
        }
        else{
            AVL_Tree_exist = true;
            for (int i = 0; i < pSet.size(); i++) 
                AVL.outerInsert(pSet.at(i));
        }

        if(!AVL.IsEmpty()){
            AVL.showheight();
            vector<int> no = AVL.showroot();
            //vector<schoolType> tmp = pSet;
            int pos=0;
            for(int i = 0, out = 1 ; i < no.size() ; i++){
                for( ; pos < pSet.size() ; pos++){
                    if(no.at(i) == pSet.at(pos).no){
                        cout << out << ": [" << no.at(i) << "] " << pSet.at(pos).sname << ", "
                             << pSet.at(pos).dname << ", " << pSet.at(pos).type << ", " << pSet.at(pos).level
                             << ", " << pSet.at(pos).nstud << ", " << pSet.at(pos).ngrad<< "\n" ;
                        out++;
                        //tmp.erase(tmp.begin(), tmp.begin()+index);
                        break;
                    }
                }
            }
            cout << "\n";
        }

    }

    void Del_AVL(){
        int input;
        cout << "please enter an number:" ;
        cin >> input ;
        //AVL.outerDelete(input);

        // vector<int> no = AVL.showDelete();
        // int pos=0;
        // for(int i = 0, out = 1 ; i < no.size() ; i++){
        //     for( ; pos < pSet.size() ; pos++){
        //         if(no.at(i) == pSet.at(pos).no){
        //             cout << out << ": [" << no.at(i) << "] " << pSet.at(pos).sname << ", "
        //                     << pSet.at(pos).dname << ", " << pSet.at(pos).type << ", " << pSet.at(pos).level
        //                     << ", " << pSet.at(pos).nstud << ", " << pSet.at(pos).ngrad<< "\n" ;
        //             out++;
        //             pSet.erase(pSet.begin()+pos, pSet.begin()+pos+1);
        //             pos--;
        //             break;
        //         }
        //     }
        // }
        // cout << "\n";

        if(AVL.IsEmpty())
            AVL_Tree_exist = false;
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
        cout << "\n* 3. Delete 23 tree           *";
        cout << "\n*******************************";
        //cout << "\nInput a choice(0, 1, 2): ";
        cout << "\nInput a choice(0, 1, 2, 3): ";
        cin >> command;
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
                if (slist.fileExist()){
                    if (slist.Tree23Exist())
                        slist.Del_23();
                    else
                        cout << "### Choose 1 first. ###" << endl;
                }
                else
                    cout << "### Choose 1 first. ###" << endl;
                
                break;
            /*case 4:
                if (slist.fileExist()){
                    if (slist.AVLExist())
                        slist.Del_AVL();
                    else
                        cout << "### Choose 2 first. ###" << endl;
                }
                else
                    cout << "### Choose 1 first. ###" << endl;
                
                break;*/
            
            default:
                cout << "\nCommand does not exist!\n";
        } // end switch case
    } // end while
} // end main()