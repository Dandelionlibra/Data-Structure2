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
#include <unordered_map> // unordered_map
#include <map> // map
#include <queue> // queue
#include <cmath>
using namespace std;


struct Node {
    string ID;
    float weight;
    bool visited; // !!!
    Node *next;

    Node() : ID("\0"), weight(0), visited(false), next(nullptr) {}// outID("\0"), ID_pos(nullptr),
};


struct Graph {
    char putID[12] = {'\0'}; // Outdegree student id
    char getID[12] = {'\0'}; // Indegree student id
    float weight = 0.0; // (0,1] the weight of the edge
};

class adjacencyList{
private:
    map<string, Node*> adjacencyNode;
    vector<pair<string, vector<string>>> connection;

    int numVertices;
    int numEdges;

    struct CompareNodes {
        bool operator()(string a, string b) const {
            return a > b; // Sorting by ID in ascending order
        }
        bool operator()(pair<string, vector<string>> a, pair<string, vector<string>> b) const {
            return a.second.size() > b.second.size();
        }
    };


    pair<string, vector<string>> BFS(string startID) { // Function to perform breadth-first search
        vector<string> visitedNodes;// greater, less
        queue<string> Queue;

        Queue.push(adjacencyNode[startID]->ID);
        adjacencyNode[startID]->visited = true;
        Node* currentnode = adjacencyNode[startID];
        
        while(!Queue.empty()){
            string currentID = Queue.front();
            Queue.pop();

            bool found = binary_search(visitedNodes.begin(), visitedNodes.end(), currentID);

            if(!found && currentID != startID)
                visitedNodes.insert(lower_bound(visitedNodes.begin(), visitedNodes.end(), currentID), currentID);

            while(currentnode != nullptr){
                if(currentnode->visited == false){
                    currentnode->visited = true;
                    Queue.push(currentnode->ID);
                }
                currentnode = currentnode->next;

            }
            
            currentnode = adjacencyNode[currentID];
        }

        return make_pair(startID, visitedNodes);
    }


public:
    adjacencyList() : numVertices(0), numEdges(0){}

    void addEdge(Graph gSet){
        numVertices++;
        if (adjacencyNode.size()==0 || adjacencyNode.find(gSet.putID) == adjacencyNode.end()) {
            Node* tmp = new Node();
            tmp->ID = gSet.getID;
            tmp->weight = gSet.weight;
            tmp->visited = false;
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
                tmp->visited = false;
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
                    tmp->visited = false;
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
                        current->next->visited = false;
                    } // if
                    else if ( gSet.getID < (current->next->ID) ) {
                        Node* temp = new Node();
                        temp->ID = gSet.getID;
                        temp->weight = gSet.weight;
                        temp->visited = false;
                        temp->next = current->next;
                        current->next = temp;
                    } // else if
                    
                } // else
            }

        }
    }

    void count_BFS(){
        connection.clear();
        
        pair<string, vector<string>> bfs;
        for(auto& pair : adjacencyNode){
            bfs = BFS(pair.first);
            // Compare the size of bfs.second with each pair in connection
            connection.insert(upper_bound(connection.begin(), connection.end(), bfs, CompareNodes()), bfs);

            // reset visited flag to false for visited nodes
            bfs.second.push_back(pair.first); // !!! because the startID is not in the visitedNodes
            for(auto& visit : bfs.second){
                Node* current = adjacencyNode[visit];
                while(current != nullptr){
                    current->visited = false;
                    current = current->next;
                }
            }
        }

    }

    void print_connection(){

        for(auto& pair : connection){
            cout << "\nOutdegree ID: " << pair.first << endl;
            cout << "Indegree ID: ";
            int n = 1;
            for(auto& visit : pair.second){
                cout << "[" <<setw(2) << n << "] " << visit << ", ";
                n++;
            }

            cout << "\t";
        }
    }

    void printAdjacencyMap() {
        for (const auto& pair : adjacencyNode) {
            cout << "\nOutdegree ID: " << pair.first << "\t";
            // cout << "Edges: \n";
            Node* current = pair.second;
            while (current != nullptr) {
                cout << "Indegree ID: " << current->ID << ", Weight: " << current->weight << "\t";
                current = current->next;
            }
            cout << endl;
        }
    }

    void printSender(){
        for (const auto& pair : adjacencyNode) {
            cout << pair.first << endl;
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
                    adjFile << "\t(" << setw(2) << vertex << ") " << current->ID << ",  " << setw(4) << current->weight ;
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
            adjFile << "\n<<< There are " << numVertices << " nodes in total. >>>";
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


                for(int vertex = 0 ; vertex < pair.second.size() ; vertex++){
                    cntFile << "\t(" << setw(2) << vertex+1 << ") " << pair.second[vertex] ;
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

        numVertices = 0;
        numEdges = 0;
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
        cout << endl << "**********************************" ;
        cout << endl << "Input a choice(0, 1, 2): " ;
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
            default:
                cout << "\nCommand does not exist!" << endl;
                break;
        } // end switch case
    } // end while

    return 0;
}
