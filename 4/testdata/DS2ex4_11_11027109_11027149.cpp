# include <iostream> // cin,cout,endl
# include <cstdlib> // atoi, system
# include <algorithm>
# include <stdio.h>
# include <iomanip>
# include <fstream>
# include <vector>
# include <string>
# include <string.h>
# include <map>
# include <queue>
# include <algorithm>
#include <random>
#include <chrono>
#include <set>
#include <unordered_set>

// 資訊二甲_第 11組
// 11027109_吳昀蔚
// 11027149_游婕歆

using namespace std ;

struct Data{
	public:
	char putID[12] ; // 發訊
	char getID[12] ; // 收訊
	float weight ;  // 權重
};

struct Node{
	public:
	int bfsNum = 0 ;
	vector<string> travel ;
	Node * next = NULL ;
};

typedef Node * Ptr ;

class Readfile{
	public:
	vector<Data> readbin( Data data, string &filename ) {
		// 讀 .bin 檔
		fstream file ;
		int count ;
		vector<Data> d ;
		Data temp ;
		string input = "pairs" + filename + ".bin" ;
		file.open( input.c_str(), fstream::in | fstream::binary ) ; // 開檔案
		if( file.is_open() == true ) {
			file.seekg( 0, file.end ) ;  //取得文件中第 0個到最後一個
			count = file.tellg() / sizeof( data ) ;  // 來計算檔案大小
			file.seekg( 0, file.beg ) ; // 找檔案的開頭

			for( int i = 0 ; i < count ; i++ ) {
				file.read( (char*)&data, sizeof(data) ) ;
				// 從 file 中讀取 data 所指向的記憶體位置開始的 sizeof(data) 個位元組，並將其存儲在 data 中
				strcpy( temp.putID, data.putID ) ;
                strcpy( temp.getID, data.getID ) ;
				temp.weight = data.weight ;
				d.push_back( temp ) ;
			}  // for

		}  // if
		file.close() ; // 關檔案
		return d ;
	}  // readbin

	bool binisexist( vector<Data> &d, string &filename ) {
		fstream file ;
		string input = "\0" ;
		Data data ;
		bool success = true ;
		cout << "\n" << "Input a file number ([0] Quit): " ;
	    cin >> filename ;
	    if ( filename.size() == 1 && filename[0] == '0' )  return false ; // [0] Quit
	    else {
			input = "pairs" + filename + ".bin" ;
			file.open( input.c_str(), fstream::in | fstream::binary ) ; // 以二進制模式打開檔案
		}  // else

		if( file.is_open() != true ) {
			// 開 .bin 檔失敗
			cout << "###" << input << "does not exist! ###\n\n";
			success = false ;
		}  // if
		else  d = readbin( data, filename ) ;
		file.close() ;

		return success ;
	}  // binisexist

};

class Adjacencylist{
	public:
	void createlist( vector<Data> data, map< string, vector<Data> > &mainlist, string filename ) {
		for( int i = 0 ; i < data.size() ; i++ ) {
			mainlist[string(data[i].putID)].push_back( data[i] );
		}  // for
		print( mainlist, filename );
	}  // createlist

	void runall( vector<Data> &data, string filename, map< string, vector<Data> > &mainlist ) {  // 計算總共有多少ID

		vector<Data> empty;
		for( int i = 0 ; i < data.size() ; i++ ) {
			auto find = mainlist.find( string(data[i].putID) );  // 自動轉換型別
			if( find != mainlist.end() )  continue;
			else  mainlist[string(data[i].putID)] = vector<Data>();  // 不存在，所以放進mainlist

			find = mainlist.find( string(data[i].getID) );
			if( find != mainlist.end() )  continue;
			else  mainlist[string(data[i].getID)] = vector<Data>();  // 不存在，所以放進mainlist
		}  // for

		createlist( data, mainlist, filename );
	}  // runall

	void merge( vector<Data> &data, int front, int mid, int end ) {
		int first1 = front;
		int last1 = mid;
		int first2 = mid+1;
		int last2 = end;
		vector<Data> temp;
		temp = data;
		int index = front;
		for( index = front ; first1 <= last1 && first2 <= last2 ; index++ ) {
			if ( string( data[first1].getID ) < string( data[first2].getID ) ) {
				temp[index] = data[first2];
				first2++;
			}  // if
			else {
				temp[index] = data[first1];
				first1++;
			}  // else
		}  // for
		for ( ; first1 <= last1 ; first1++, index++ ) {
			temp[index] = data[first1];
		}  // for
		for ( ; first2 <= last2 ; first2++, index++ ) {
			temp[index] = data[first2];
		}  // for
		for ( index = front ; index <= end ; index++ ) {
			data[index] = temp[index];
		}  // for
	}  // merge()

	void mergesort( vector<Data> &data, int front, int end ) {

		if( front < end ) {
			int mid = ( front+end )/2;
			mergesort( data, front, mid);  // 前半段
			mergesort( data, mid+1, end );  // 後半段
			merge( data, front, mid, end );  // 把前半段和後半段比較再合併
		}  // if
	}  // mergesort()

	void print( map< string, vector<Data> > &mainlist, string filename ) {
		string name = "pairs" + filename + ".adj";
		fstream file;
		file.open( name.c_str(), ios::out );
		file <<  "<<< There are " << mainlist.size() << " IDs in total. >>>\n";
		cout << endl <<  "<<< There are " << mainlist.size() << " IDs in total. >>>\n";
		int n = 1;
		int node = 0;

		for ( auto it = mainlist.begin(); it != mainlist.end(); ++it ) {
		    file << "[ " << setw(2) << n << "] " << it->first << ": \n";
		    mergesort( it->second, 0, it->second.size()-1 );
		    reverse( it->second.begin() , it->second.end() );
		    for( int i = 0 ; i < it->second.size() ; i++ ) {
		    	node++;
		    	file << "\t(" << setw(2) << i+1 << ") " << it->second[i].getID << ",  " << setw(4) << it->second[i].weight ;
		    	if( (i+1)%10 == 0 && i != 0 )  file << "\n";
		    	if( i == it->second.size()-1 )  file << "\n";
			}  // for
			n++;
		}  // for

		file << "\n<<< There are " << node << " nodes in total. >>>";
		cout << endl << "<<< There are " << node << " nodes in total. >>>\n";
		file.close();
	}  // print
};

class ConnectionCount{
	public:
	void count( map< string, vector<Data> > mainlist, string filename ) {
		Ptr head = NULL ;
		Ptr ctc = new Node ;
		int n = 0 ;
		int c = 0 ;
		queue<string> q ;
		map< string, string > tra ;
		for ( auto it = mainlist.begin(); it != mainlist.end(); ++it ) {
		    auto find = it ;
		    ctc->travel.push_back( string( find->first ) ) ;
		    tra[find->first] = find->first ;
		    do {
			    if ( ! q.empty() ) q.pop() ;
			    for( int i = 0 ; i < find->second.size() ; i++ ) {
			    	auto f = tra.find( find->second[i].getID ) ;
			    	if( f != tra.end() )  continue;
			        else  {
			        	tra[find->second[i].getID] = find->second[i].getID ;
			        	q.push( string(find->second[i].getID) ) ;
					    ctc->travel.push_back( string(find->second[i].getID) ) ;
					}
			    }
                if ( ! q.empty() ) find = mainlist.find( q.front() );  // 自動轉換型別
		    } while ( ! q.empty() ) ;
		    tra.clear() ;
		    ctc->bfsNum = ctc->travel.size()-1 ;
		    oderInsert( head, ctc, n ) ;
		    ctc = new Node ;
	    }
	    print( n, head, filename ) ;
	} // count()


	void print( int m , Ptr head, string filename ) {
		// 寫檔
		string name = "pairs" + filename + ".cnt" ;
		fstream file ;
		file.open( name.c_str(), ios::out ) ;
	    file <<  "<<< There are " << m << " IDs in total. >>>" ;
		cout << endl <<  "<<< There are " << m << " IDs in total. >>>" << endl ;
	    Ptr ctc = head ;
	    int n = 1 ;
	    while ( ctc != NULL ) {
	    	file << endl << "[" ;
	    	if ( n < 10 ) file << "  " ;
	    	else if ( n < 100 ) file << " " ;
	    	file << n << "] " << ctc->travel[0] << "(" << ctc->bfsNum << ")" << ": " << endl ;
	    	sort( ctc->travel.begin()+1, ctc->travel.end() ) ;
	    	for ( int i = 1 ; i < ctc->travel.size() ; i++ ) {
		        file << "	" << "(" ;
		        if ( i < 10 ) file << " " ;
			    file << i << ")" << " " << ctc->travel[i] ;
                if( (i)%10 == 0 )  file << endl ;
		    }  // for
	    	ctc = ctc->next ;
			n++ ;
		}

		file << endl ;
	    file.close() ;
	}  // print

	void oderInsert( Ptr &head, Ptr &temp, int &n ) {
		// 排序 + insert
		if ( n < 3 ) {
			if ( head == NULL ) {
			    head = temp ;
			    n++ ;
			    return ;
		    }  // if
		    else if ( head->next == NULL ) {
			    if ( head->bfsNum > temp->bfsNum ) head->next = temp ;
			    else if ( head->bfsNum == temp->bfsNum ) head->next = temp ;
			    else {
				    temp->next = head ;
			    	head = temp ;
			    }  // else
			    n++ ;
			    return ;
		    }  // else if
		}  // if

		if ( head->bfsNum < temp->bfsNum ) {
			temp->next = head ;
			head = temp ;
			n++ ;
	        return ;
		}  // if

		Ptr cur = head ;
		while ( cur->next != NULL ) {
			if ( cur->next->bfsNum < temp->bfsNum ) {
				temp->next = cur->next ;
				cur->next = temp ;
				n++ ;
				return ;
			}  // if
			else if ( cur->bfsNum == temp->bfsNum ) {
				while ( cur->next != NULL && cur->next->bfsNum == temp->bfsNum ) cur = cur->next ;
				temp->next = cur->next ;
				cur->next = temp ;
				n++ ;
				return ;
			}  // else if
			cur = cur->next ;
		}  // while
		n++ ;
		cur->next = temp ;
	}  // oderInser
};

class Proinfluence{
	public:

	void count( map< string, vector<Data> > mainlist, string filename ) {
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        mt19937 gen(seed);
        uniform_real_distribution<float> dis(0.0f, 1.0f);

		Ptr head = NULL ;
		Ptr ctc = new Node ;
		int n = 0 ;
		vector<string> my_vector ;
		map< string, string > tra ;
		for ( auto it = mainlist.begin(); it != mainlist.end(); ++it ) {
		    auto find = it ;
		    ctc->travel.push_back( string( find->first ) ) ;
		    tra[find->first] = find->first ;
		    int c = 0 ;
		    float real = dis(gen);
            while (real == 0.0f)
                real = dis(gen);
		    do {
			    if ( ! my_vector.empty() ) my_vector.pop_back() ;
			    for( int i = 0 ; i < find->second.size() ; i++ ) {
			    	auto f = tra.find( find->second[i].getID ) ;
			    	if( f != tra.end() )  continue ;
			        else if ( find->second[i].weight >= real )  {
			        	tra[find->second[i].getID] = find->second[i].getID ;
			        	my_vector.push_back( string(find->second[i].getID) ) ;
					    ctc->travel.push_back( string(find->second[i].getID) ) ;
					    c++ ;

					}
			    }
                if ( ! my_vector.empty() ) find = mainlist.find( my_vector[my_vector.size()-1] );  // 自動轉換型別
		    } while ( ! my_vector.empty() ) ;

		    tra.clear() ;

		    if ( c != 0 ) {
		    	ctc->bfsNum = ctc->travel.size()-1 ;
		    	n++;
		    	orderInsert( head, ctc, n ) ;
		    	ctc = new Node ;
			}
			else {
				delete ctc ;
				ctc = new Node ;
			}

	    }
	    print( n, head, filename ) ;
	} // count()

    void print(int m, Ptr head, string filename) {
        // 寫檔
        string name = "pairs" + filename + ".pro";
        fstream file(name.c_str(), ios::out );
        file << "<<< There are " << m << " IDs in total. >>>" << endl;
        cout << endl << "<<< There are " << m << " IDs in total. >>>" << endl;

        Ptr ctc = head;
        int n = 1;
        while (ctc != nullptr) {
            if (ctc->bfsNum != 0) {
                set<string> unique;
                file << endl << "[";
                if (n < 10) file << "  ";
                else if (n < 100) file << " ";
                file << n << "] " << ctc->travel[0] << "(" << ctc->bfsNum << ")" << ": " << endl;

                sort(ctc->travel.begin() + 1, ctc->travel.end());
                int j = 1;
                for (int i = 1; i < ctc->travel.size(); i++) {
                    if (unique.count(ctc->travel[i]) == 0 && ctc->travel[i] != ctc->travel[0]) {
                        file << "    (" << setw(2) << j << ") " << ctc->travel[i];
                        if ((i % 12) == 0)
                            file << endl;
                        j++;
                        unique.insert(ctc->travel[i]);
                    }  // if
                }  // for
                file << endl;
                ctc = ctc->next;
                n++;
            }  // if
        }  // while
        file << endl;
    }  // print

	void orderInsert(Ptr& head, Ptr& temp, int& n) {
	    // 排序 + insert
	    if (n < 3) {
	        if (head == nullptr) {
	            head = temp;

	            return;
	        }  // if
	        else if (head->next == nullptr) {
	            if (head->bfsNum > temp->bfsNum) head->next = temp;
	            else if (head->bfsNum == temp->bfsNum) head->next = temp;
	            else {
	                temp->next = head;
	                head = temp;
	            }  // else

	            return;
	        }  // else if
	    }  // if

	    if (head->bfsNum < temp->bfsNum) {
	        temp->next = head;
	        head = temp;

	        return;
	    }  // if

	    Ptr cur = head;
	    while (cur->next != nullptr) {
	        if (cur->next->bfsNum < temp->bfsNum) {
	            temp->next = cur->next;
	            cur->next = temp;

	            return;
	        }  // if
	        else if (cur->bfsNum == temp->bfsNum) {
	            while (cur->next != nullptr && cur->next->bfsNum == temp->bfsNum) cur = cur->next;
	            temp->next = cur->next;
	            cur->next = temp;
	            return;
	        }  // else if
	        cur = cur->next;
	    }  // while

	    cur->next = temp;
	}  // orderInsert

};

class Mission_tree{
	public:
	void count( map< string, vector<Data> > mainlist, string filename, float real ) {
		Ptr head = NULL ;
		Ptr ctc = new Node ;
		int n = 0 ;
		vector<string> my_vector ;
		map< string, string > tra ;
		for ( auto it = mainlist.begin(); it != mainlist.end(); ++it ) {
		    auto find = it ;
		    ctc->travel.push_back( string( find->first ) ) ;
		    tra[find->first] = find->first ;
		    int c = 0 ;
		    do {
			    if ( ! my_vector.empty() ) my_vector.pop_back() ;
			    for( int i = 0 ; i < find->second.size() ; i++ ) {
			    	auto f = tra.find( find->second[i].getID ) ;
			    	if( f != tra.end() )  continue ;
			        else if ( find->second[i].weight >= real )  {
			        	tra[find->second[i].getID] = find->second[i].getID ;
			        	my_vector.push_back( string(find->second[i].getID) ) ;
					    ctc->travel.push_back( string(find->second[i].getID) ) ;
					    c++ ;
					}
			    }
                if ( ! my_vector.empty() ) find = mainlist.find( my_vector[my_vector.size()-1] );  // 自動轉換型別
		    } while ( ! my_vector.empty() ) ;

		    tra.clear() ;

		    if ( c != 0 ) {
		    	ctc->bfsNum = ctc->travel.size()-1 ;
		    	oderInsert( head, ctc, n ) ;
		    	ctc = new Node ;
			}
			else {
				delete ctc ;
				ctc = new Node ;
			}

	    }
	    print( n, head, filename ) ;
	} // count()


	void print( int m , Ptr head, string filename ) {
		// 寫檔
		string name = "pairs" + filename + ".inf" ;
		fstream file ;
		file.open( name.c_str(), ios::out ) ;
	    file <<  "<<< There are " << m << " IDs in total. >>>" ;
		cout <<  "<<< There are " << m << " IDs in total. >>>" << endl ;
	    Ptr ctc = head ;
	    int n = 1 ;
	    while ( ctc != NULL ) {
	    	file << endl << "[" ;
	    	if ( n < 10 ) file << "  " ;
	    	else if ( n < 100 ) file << " " ;
	    	file << n << "] " << ctc->travel[0] << "(" << ctc->bfsNum << ")" << ": " << endl ;
	    	sort( ctc->travel.begin()+1, ctc->travel.end() ) ;
	    	for ( int i = 1 ; i < ctc->travel.size() ; i++ ) {
		        file << "	" << "(" ;
		        if ( i < 10 ) file << " " ;
			    file << i << ")" << " " << ctc->travel[i] ;
                if( (i)%12 == 0 )  file << endl ;
		    }  // for
	    	ctc = ctc->next ;
			n++ ;
		}

		file << endl ;
	    file.close() ;
	}  // print

	void oderInsert( Ptr &head, Ptr &temp, int &n ) {
		// 排序 + insert
		if ( head == NULL ) {
			head = temp ;
			n++ ;
			return ;
		}
		else if ( head->next == NULL ) {
			if ( head->bfsNum > temp->bfsNum ) head->next = temp ;
			else if ( head->bfsNum == temp->bfsNum ) head->next = temp ;
			else {
				temp->next = head ;
			    head = temp ;
			}
			n++ ;
			return ;
		}

		if ( head->bfsNum < temp->bfsNum ) {
			temp->next = head ;
			head = temp ;
			n++ ;
	        return ;
		}

		Ptr cur = head ;
		while ( cur->next != NULL ) {
			if ( cur->next->bfsNum < temp->bfsNum ) {
				temp->next = cur->next ;
				cur->next = temp ;
				n++ ;
				return ;
			}
			else if ( cur->bfsNum == temp->bfsNum ) {
				while ( cur->next != NULL && cur->next->bfsNum == temp->bfsNum ) cur = cur->next ;
				temp->next = cur->next ;
				cur->next = temp ;
				n++ ;
				return ;
			}
			cur = cur->next ;
		}
		n++ ;
		cur->next = temp ;
	}

	int StoN( string str ) {
	  // 字串轉整數
	  int num = 0, number = 0, i = 1 ;
	  for ( int j = str.size()-1 ; j >= 0 ; j -- ) {
	  	if ( str[j] != '0' && str[j] != '1' && str[j] != '2' && str[j] != '3' && str[j] != '4' && str[j] != '5' &&
		     str[j] != '6' && str[j] != '7' && str[j] != '8' && str[j] != '9' ) return -1 ;
		number = str[j] - '0' ;
		num = number * i + num ;
		i = i * 10 ;
	  }
	  return num ;
    } // StoN
};

int main() {
	int run = 0;
	int size = 0 ;
    int command = 0 ;
    float num = 0.0 ;
    vector<Data> data ;
    string filename ;
    Readfile r ;
	Adjacencylist a;
	ConnectionCount c;
	Proinfluence p ;
	Mission_tree m ;
	map< string, vector<Data> > list;
    while ( run == 0 ) {
    	bool functionisnum = true;
		string function;
		int command = 0;
    	cout << endl << "**** Graph data manipulation ****" ;
        cout << endl << "* 0. QUIT                       *" ;
        cout << endl << "* 1. Build adjacency lists      *" ;
        cout << endl << "* 2. Compute connection counts  *" ;
        cout << endl << "* 3. Estimate influence values  *" ;
        cout << endl << "* 4. Probability-based influence*" ;
        cout << endl << "*********************************" ;
        cout << endl << "Input a choice(0, 1, 2, 3, 4): " ;
	    cin >> function ;
		for ( int f = 0; f < function.size(); f++ ) {  // 判斷讀入的字串是否為數字
        	int temp = (int)function[f] ;
        	if (temp >= 48 && temp <= 57) {
        		continue ;
        	}  // if
        	else  functionisnum = false ;
    	}  // for

    	if ( functionisnum )  command = atoi(function.c_str()) ;  // 字串轉int
    	if ( functionisnum && command == 0 )  run++ ;

    	else if ( functionisnum && command == 1 ) {
    		data.clear() ;
    		list.clear() ;
    		filename = "" ;
	    	if ( r.binisexist( data, filename ) ) {
	    		 a.runall( data, filename, list ) ;
			}  // if

	    }  // else if
		else if ( functionisnum && command == 2 ) {
			if ( data.size() != 0 ) {
				if( filename == "410" || filename == "409" )  cout << filename << "要等不要太快打叉\n";
			    c.count( list, filename ) ;
	        }  // if
	        else  cout << "### There is no graph and choose 1 first. ###" << endl ;
		}  // else if
		else if ( functionisnum && command == 3 ) {
			float n = 0.0 ;
			if ( data.size() != 0 ) {
			    while ( n <= 0 || n > 1 ) {
			    	cout << "Input a real number in (0,1]: " ;
			        cin >> n ;
			        cout << endl ;
				}
			    m.count( list, filename, n ) ;
	        }  // if
	        else  cout << "### There is no graph and choose 1 first. ###" << endl ;
		}  // else if
		else if ( functionisnum && command == 4 ) {
			if ( data.size() != 0 ) {
			    p.count( list, filename ) ;
	        }  // if
	        else  cout << "### There is no graph and choose 1 first. ###" << endl ;
		}  // else if

		else  cout << "\nthis command does not exist";
	}  // while

	system( "pause" ) ;
  	return 0 ;
}  // main
