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
#include <cmath>
#include <chrono>
#include <random>

using namespace std;


struct Node {
    string ID;
    float weight;
    // bool visited; // !!!
    Node *next;

    Node() : ID("\0"), weight(0), next(nullptr) {}// outID("\0"), ID_pos(nullptr),, visited(false)
};


struct Graph {
    char putID[12] = {'\0'}; // Outdegree student id
    char getID[12] = {'\0'}; // Indegree student id
    float weight = 0.0; // (0,1] the weight of the edge
};

class adjacencyList{
private:
    map<string, Node*> adjacencyNode;
    vector<pair<string, set<string>>> connection; // BFS

    // map<string, map<string, Node>> DFS;
    vector<pair<string, set<string>>> DFS;

    int numVertices;
    int numEdges;
    float effective;
    bool random_effective;

    struct CompareNodes {
        bool operator()(string a, string b) const {
            return a > b; // Sorting by ID in ascending order
        }
        bool operator()(pair<string, vector<string>> a, pair<string, vector<string>> b) const {
            return a.second.size() > b.second.size();
        }
        bool operator()(pair<string, unordered_set<string>> a, pair<string, unordered_set<string>> b) const {
            return a.second.size() > b.second.size();
        }
        bool operator()(pair<string, set<string>> a, pair<string, set<string>> b) const {
            return a.second.size() > b.second.size();
        }
    };


    pair<string, set<string>> BFS(string startID) { // Function to perform breadth-first search
        set<string> visitedNodes; // store the path
        unordered_set<string> visited; // store visited nodes
        queue<string> Queue;

        Queue.push(startID);
        visited.insert(startID);
        
        while(!Queue.empty()){
            string currentID = Queue.front();
            Node* currentnode = adjacencyNode[currentID];
            Queue.pop();

            bool found = visitedNodes.count(currentID);
            if(!found && currentID != startID)
                visitedNodes.insert(currentID);

            while(currentnode != nullptr){
                if(visited.count(currentnode->ID) == 0){ // if the node is not visited
                    visited.insert(currentnode->ID);
                    Queue.push(currentnode->ID);
                }
                currentnode = currentnode->next;

            }
            
            currentnode = adjacencyNode[currentID];
        }

        return make_pair(startID, visitedNodes);
    }

    pair<string, set<string>> iterativeDFS(string startID) {
        set<string> visitedNodes; // store the path
        unordered_set<string> visited; // store visited nodes
        stack<string> Stack;

        Stack.push(startID);
        visited.insert(startID);

        while(!Stack.empty()){
            string currentID = Stack.top();
            Node* currentnode = adjacencyNode[currentID];

            while( currentnode != nullptr ) {
                if ((currentnode->weight) >= effective && visited.count(currentnode->ID) == 0)
                    break;
                else
                    currentnode = currentnode->next;
            } // while(currentnode != nullptr)

            // generate random effective value
            if(random_effective)
                effective = random_eff();

            if ( currentnode != nullptr && (currentnode->weight) >= effective && visited.count(currentnode->ID) == 0) {
                Stack.push(currentnode->ID);
                visited.insert(currentnode->ID);
                // cout << "\033[1;32minsert: " << currentnode->ID << "\033[0m" << endl;
            } // if
            else {
                bool found = visitedNodes.count(currentID);
                if(!found && currentID != startID){
                    visitedNodes.insert(currentID);
                }// if

                Stack.pop();
            } // else

        } // while(!Stack.empty())

        return make_pair(startID, visitedNodes);
    } // DFS

public:
    adjacencyList() : numVertices(0), numEdges(0), effective(0), random_effective(false){}

    void addEdge(Graph gSet){
        numVertices++;
        if (adjacencyNode.size()==0 || adjacencyNode.find(gSet.putID) == adjacencyNode.end()) {
            Node* tmp = new Node();
            tmp->ID = gSet.getID;
            tmp->weight = gSet.weight;
            // tmp->visited = false;
            adjacencyNode.insert(pair<string, Node*>(gSet.putID, tmp));
            // if the receiver don't exist in main_list, it's also need be added into main_list
            if(adjacencyNode.find(gSet.getID) == adjacencyNode.end()){
                Node* tmp = nullptr;
                adjacencyNode.insert(pair<string, Node*>(gSet.getID, tmp));
            }
        }
        // if the key of putID exist, connect the new value to the end of list
        else {
            // if the receiver don't exist in main_list, it's also need be added into main_list
            if(adjacencyNode.find(gSet.getID) == adjacencyNode.end()){
                Node* tmp = nullptr;
                adjacencyNode.insert(pair<string, Node*>(gSet.getID, tmp));
            }

            // if the sender exist in main_list, but it's value is nullptr
            if(adjacencyNode.find(gSet.putID) != adjacencyNode.end() && adjacencyNode[gSet.putID] == nullptr){
                Node* tmp = new Node();
                tmp->ID = gSet.getID;
                tmp->weight = gSet.weight;
                // tmp->visited = false;
                adjacencyNode[gSet.putID] = tmp;
            }
            // if the sender exist in main_list, and it's value isn't nullptr
            else{
                // new data need to be insert into the list by the order of ID
                Node* current = adjacencyNode.at(gSet.putID);
                if ( gSet.getID < current->ID ) {
                    // cout << "NO\n";
                    Node* tmp = new Node();
                    tmp->ID = gSet.getID;
                    tmp->weight = gSet.weight;
                    // tmp->visited = false;
                    tmp->next = current;
                    adjacencyNode[gSet.putID] = tmp;
                } // if
                else {
                    // find the place that the new data should be inserted
                    for ( ; current->next != nullptr && gSet.getID > (current->next->ID) ; current = current->next ) {
                        // int cnID = stoi(current->next->ID);
                        // gID = stoi(gSet.getID);
                        // if ( gID < cnID )
                           // break;
                    } // for

                    if ( current->next == nullptr ) {
                        // cout << "Yes\n";
                        current->next = new Node();
                        current->next->ID = gSet.getID;
                        current->next->weight = gSet.weight;
                        // current->next->visited = false;
                    } // if
                    else if ( gSet.getID < (current->next->ID) ) {
                        Node* temp = new Node();
                        temp->ID = gSet.getID;
                        temp->weight = gSet.weight;
                        // temp->visited = false;
                        temp->next = current->next;
                        current->next = temp;
                    } // else if
                    
                } // else
            }

        }
    }

    void count_BFS(){
        connection.clear();
        
        pair<string, set<string>> bfs;
        for(auto& pair : adjacencyNode){
            bfs = BFS(pair.first);
            // Compare the size of bfs.second with each pair in connection
            connection.insert(upper_bound(connection.begin(), connection.end(), bfs, CompareNodes()), bfs);
        }

    }

    void random_or_not(bool r_eff){
        random_effective = r_eff;
    }

    float random_eff(){ //float &effective
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dis(0.8, 1.0);

        // effective = dis(gen);
        // cout << "\033[1;34mEffective value: " << effective << "\033[0m" << endl;
        return dis(gen);
    }

    void count_DFS(int eff){
        DFS.clear();
        if(eff == 0)
            effective = random_eff(); // generate random effective value
        else
            effective = eff;

        pair<string, set<string>> dfs;
        for(auto& pair : adjacencyNode){
            set<string> visitedPath;
            unordered_set<string> visited;
            // cout << "\033[1;31mStart ID: " << pair.first << "\033[0m" << endl;
            dfs = iterativeDFS(pair.first);
            if(dfs.second.size() != 0)
                DFS.insert(upper_bound(DFS.begin(), DFS.end(), dfs, CompareNodes()), dfs);
        }

    }

    void print_DFS(){
        int topK = 0 ;
        string inputStr;
        do{
            cout << "\nInput an integer to show top-K in [1,"<< DFS.size() << "]: " ;
            cin >> inputStr;
            try {
                topK = stoi(inputStr);
                if(topK >= 1 && topK <= DFS.size())
                    break;
                else if (topK < 1 || topK > DFS.size()){
                    cout << "\n### " << topK << " is NOT in [1," << DFS.size() << "] ###\n" ;
                    continue;
                }

            } catch (exception& e) {
                topK = 0;
            }
        } while(true);

        int i = 1;
        for(auto& pair : DFS){
            if(i > topK)
                break;

            cout << "<" << i << "> " << pair.first << ": " << pair.second.size() << endl;
            i++;
        }

    }

    void write_adj_file(string fileID){
        string fileName = "pairs" + fileID + ".adj";
        fstream adjFile(fileName.c_str(), ios::out);
        if (adjFile.is_open()) {
            adjFile <<  "<<< There are " << adjacencyNode.size() << " IDs in total. >>>\n";
            cout <<  "<<< There are " << adjacencyNode.size() << " IDs in total. >>>\n";

            int count = 0;
            for (const auto& pair : adjacencyNode) {
                count++;
                if(count<10)
                    adjFile << "[  " << count << "] " << pair.first << ": \n";
                else if(count<100)
                    adjFile << "[ " << count << "] " << pair.first << ": \n";
                else
                    adjFile << "[" << count << "] " << pair.first << ": \n";

                // adjFile << "[ " << setw(2) << count << "] " << pair.first << ": \n";
                // cout << "[ " << setw(2) << count << "] " << pair.first << ": \n";

                int vertex = 0;
                Node* current = pair.second;
                while (current != nullptr) {
                    vertex++;
                    adjFile << "\t(" << setw(2) << vertex << ") " << current->ID << ",  " << setw(5) << current->weight ;
                    // cout << "\t(" << setw(2) << vertex << ") " << current->ID << ",  " << setw(4) << current->weight ;
                    current = current->next;

                    if(vertex%12 == 0){
                        adjFile << endl;
                        // cout << endl;
                    }

                }
                adjFile << endl;
                // cout << endl;
            }
            adjFile << "<<< There are " << numVertices << " nodes in total. >>>\n";
            cout << "\n<<< There are " << numVertices << " nodes in total. >>>\n";

            adjFile.close();
        }
        else
            cout << "### " << fileName << " does not exist! ###\n" << endl;

    }

    void write_cnt_file(string fileID){
        string fileName = "pairs" + fileID + ".cnt";
        fstream cntFile(fileName.c_str(), ios::out);
        if (cntFile.is_open()) {
            cntFile <<  "<<< There are " << connection.size() << " IDs in total. >>>\n";
            cout <<  "\n<<< There are " << connection.size() << " IDs in total. >>>\n";

            int count = 0;
            for (const auto& pair : connection) {
                count++;
                if(count<10)
                    cntFile << "[  " << count << "] " << pair.first << "(" << pair.second.size() << ")" << ": \n";
                else if(count<100)
                    cntFile << "[ " << count << "] " << pair.first << "(" << pair.second.size() << ")" << ": \n";
                else
                    cntFile << "[" << count << "] " << pair.first << "(" << pair.second.size() << ")" << ": \n";
                // cout << "[ " << setw(2) << count << "] " << pair.first << ": \n";

                set<string>::iterator it = pair.second.begin();
                for(int vertex = 0 ; vertex < pair.second.size() ; vertex++, it++){
                    cntFile << "\t(" << setw(2) << vertex+1 << ") " << *it ;
                    // cout << "\t(" << setw(2) << vertex+1 << ") " << pair.second[vertex] << ", ";
                    if((vertex+1)%12 == 0){
                        cntFile << endl;
                        // cout << endl;
                    }
                }
                cntFile << endl;
                // cout << endl;
            }


            cntFile.close();
        }
        else
            cout << "### " << fileName << " does not exist! ###\n" << endl;

    }

    void write_inf_file(string fileID){
        string fileName = "pairs" + fileID + ".inf";
        fstream infFile(fileName.c_str(), ios::out);
        if (infFile.is_open()) {
            infFile <<  "<<< There are " << DFS.size() << " IDs in total. >>>\n";
            cout <<  "\n<<< There are " << DFS.size() << " IDs in total. >>>\n";

            int count = 0;
            for (const auto& pair : DFS) {
                count++;
                if(count<10)
                    infFile << "[  " << count << "] " << pair.first << "(" << pair.second.size() << ")" << ": \n";
                else if(count<100)
                    infFile << "[ " << count << "] " << pair.first << "(" << pair.second.size() << ")" << ": \n";
                else
                    infFile << "[" << count << "] " << pair.first << "(" << pair.second.size() << ")" << ": \n";
                // cout << "[ " << setw(2) << count << "] " << pair.first << ": \n";

                set<string>::iterator it = pair.second.begin();
                for(int vertex = 0 ; vertex < pair.second.size() ; vertex++, it++){
                    infFile << "\t(" << setw(2) << vertex+1 << ") " << *it ;
                    // cout << "\t(" << setw(2) << vertex+1 << ") " << pair.second[vertex] << ", ";
                    if((vertex+1)%12 == 0){
                        infFile << endl;
                        // cout << endl;
                    }
                }
                infFile << endl;
                // cout << endl;
            }
        }
    }

    void clearUp(){
        for (auto& pair : adjacencyNode) {
            Node* current = pair.second;
            while (current != nullptr) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        adjacencyNode.clear();
        connection.clear();
        DFS.clear();

        numVertices = 0;
        numEdges = 0;
        effective = 0;
        random_effective = false;
    }

    ~adjacencyList(){
        clearUp();
    }
};

class GraphList{
private:
    vector<Graph> gSet;
    string fileID; // number to form a file name
    bool fileexist; // already enter a file or not
    adjacencyList adjList;

public:
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
                readBinary("pairs" + fileID + ".bin");
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

    void showGraph(){
        cout << "\033[1;31mshowGraph: \n\033[0m";
        for (int i = 0; i < gSet.size(); i++){
            cout << gSet[i].putID << " " << gSet[i].getID << " " << gSet[i].weight << endl;
        }
    }

    void createAdjacency(){
        for(int i = 0 ; i < gSet.size() ; i++)
            adjList.addEdge(gSet.at(i));
        adjList.write_adj_file(fileID);
    }

    void compute_connection(){
        adjList.count_BFS();
        adjList.write_cnt_file(fileID);
    }


    void influence(){
        // get the useful_weight from the user
        string inputStr;
        float effective = 0.0;
        do {
            cout << "\nInput a real number in [0.5,1]: ";
            cin >> inputStr;
            try {
                effective = stof(inputStr);
                if(effective < 0.5 || effective > 1)
                    cout << "\n### It is NOT in [0.5,1] ###\n" ;
            } catch (exception& e) {
                effective = 0;
                // cout << "### Invalid input! ###\n";
            }
            
        } while (effective < 0.5 || effective > 1);

        adjList.random_or_not(false);
        adjList.count_DFS(effective);
        adjList.write_inf_file(fileID);
    }

    void random_influence(){
        auto start = chrono::high_resolution_clock::now(); // start the timer
        adjList.random_or_not(true);
        adjList.count_DFS(0);
        auto end = chrono::high_resolution_clock::now(); // end the timer
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start); // calculate the elapsed time
        cout << "\n[Elapsed time] " << duration.count() << " ms" << endl;

        adjList.write_inf_file(fileID);

        adjList.print_DFS();
    }

    /*void compute_connection(){
        auto start = std::chrono::high_resolution_clock::now();
        adjList.count_BFS();
        adjList.write_cnt_file(fileID);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;
    }*/

    void clearUp(){
        fileexist = false;
        fileID.clear();
        gSet.clear();
        adjList.clearUp(); // clear the adjacency list
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
        cout << endl << "* 2. Compute connection counts   *" ;
        cout << endl << "* 3. Estimate influence values   *" ;
        cout << endl << "* 4. Probability-based influence *" ;
        cout << endl << "**********************************" ;
        cout << endl << "Input a choice(0, 1, 2, 3, 4): " ;
        cin >> input;

        if (input == "0")
            command = 0;
        else if (input == "1")
            command = 1;
        else if (input == "2")
            command = 2;
        else if (input == "3")
            command = 3;
        else if (input == "4")
            command = 4;
        else
            command = -1;

        switch (command) {
            case 0:
                isEnd = true;
                break;
            case 1:
                if (glist.readFile()) { // if file exist
                    glist.createAdjacency();
                }
                break;
            case 2:
                if (glist.fileExist()) {
                    glist.compute_connection();
                }
                else
                    cout << "### There is no graph and choose 1 first. ###" << endl;
                break;
            case 3:
                if (glist.fileExist()) {
                    glist.influence();
                }
                else
                    cout << "### There is no graph and choose 1 first. ###" << endl;
                break;
            case 4:
                if (glist.fileExist()) {
                    // cout << "probability point\n";
                    glist.random_influence();
                }
                else
                    cout << "### There is no graph and choose 1 first. ###" << endl;
                break;
            default:
                cout << "\nCommand does not exist!" << endl;
                break;
        } // end switch case
    } // end while

    return 0;
}
