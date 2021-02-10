#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <queue> 
#include <stack>
#include <vector>

using namespace std;


int ROW, COL;   	//  number of rows, number of columns
int *init_row;		//  initial row in input text file, each element tells how many miners, in total, should be put in columns
int *init_column;	//  initial column in input text file, each element tells how many miners, in total, should be put in rows

char **init_map;	//	initial golden map(state) with no miner

int M = 0;			//  total number of mining sites( will be evaluated in "locate_the_mining_sites()" function )
int **Mining_Sites;   //  Mx2 matrix that shows the location of each mining site in the map.
					  //  Whenever we want to add a new miner next to the i'th mining site, this array directly provides us the location(x,y).


int visited = 0;	//  # of visited nodes(i.e the ones fetched from Queue/Stack)
clock_t t1, t2;


class Node_or_State {
  public:
	char **Gold_Map;				//  NxN map for current node/state, each cell contains ('.'), ('s') or ('m')  -->   ('.'): empty ,  ('s'): mining site , ('m'): miner
	int level;						//  starting from 0. This implies how far a node is away from the first node(map with no miner). # of miners currently in the map
	int *row;						//  each element in the "row" indicates how many remaining miners to be placed in each column from current node on(at the end, row array should contain zeroes in its all cells)
	int *column;					//  each element in the "column" indicates how many remaining miners to be placed in each row from current node on(at the end, column array should contain zeroes in its all cells)
		
  public:
	Node_or_State();									//  Default constructor
	Node_or_State(char**, int*, int*);		//  Constructor only for the first node/state
	Node_or_State(const Node_or_State&);				//  Copy Constructor for copying a parent node to its children
	void copy_map(char**);								//  copies the map in the node taken as parameter to "Gold_Map" variable
	void copy_row(int*);								//  copies the row in the node taken as parameter to "row" variable
	void copy_column(int*);								//  copies the column in the node taken as parameter to "column" variable
	bool columns_and_rows_are_all_positive_or_zero();	//  returns "true" if column or row numbers don't have any single negative number, and "false" otherwise
	bool columns_and_rows_are_all_zero();				//  returns "true" if column or row numbers are all zero
	bool miners_are_correctly_placed();					//  returns "true" if the limitations about placing miner to the map are not violated in current node/state
	bool is_the_same(Node_or_State*);					//  returns false if these two objects are uniquely different, and true if they are the same
	void print_data();									//  prints the data if necessary
	~Node_or_State();									//  Destructor
};


Node_or_State::Node_or_State(){ //  Default constructor to build an empty/rubbish Node
	Gold_Map = NULL;
	level = -1;
	row = NULL;
	column = NULL;
}


Node_or_State::Node_or_State(char **currentMap, int *currentRow, int *currentColumn){
	copy_map(currentMap);
	level = 0;
	copy_row(currentRow);
	copy_column(currentColumn);
}


Node_or_State::Node_or_State(const Node_or_State& input){ // Copy Constructor for copying a parent node to its children
	copy_map(input.Gold_Map);
	level = input.level;
	copy_row(input.row);
	copy_column(input.column);
}


void  Node_or_State::copy_map(char** sec){ // copies the second map to the first one
	Gold_Map = new char* [ROW];
	for(int r = 0; r < ROW; r++){
		Gold_Map[r] = new char [COL];
	}
	
	for(int r = 0; r < ROW; r++){
		for(int c = 0; c < COL; c++){
			Gold_Map[r][c] = sec[r][c];
		}
	}
}


void  Node_or_State::copy_row(int* sec){
	row = new int [COL];
	
	for(int i = 0; i < COL; i++){
		row[i] = sec[i];
	}
}


void  Node_or_State::copy_column(int* sec){
	column = new int [ROW];
	
	for(int i = 0; i < ROW; i++){
		column[i] = sec[i];
	}
}


bool  Node_or_State::columns_and_rows_are_all_positive_or_zero(){ // returns "true" column or row numbers don't have any single negative number, and "false" otherwise
	for(int i = 0; i < COL; i++){
		if(row[i] < 0 ){
			return false;
		}
	}
	
	for(int j = 0; j < ROW; j++){
		if(column[j] < 0 ){
			return false;
		}
	}
	return true;
}


bool  Node_or_State::columns_and_rows_are_all_zero(){
	for(int i = 0; i < COL; i++){
		if(row[i] != 0){
			return false;
		}
	}
	
	for(int j = 0; j < ROW; j++){
		if(column[j] != 0){
			return false;
		}
	}
	return true;
}



bool  Node_or_State::miners_are_correctly_placed(){
	bool check1;	// will be interpreted as "there is NO mining site ABOVE the miner"
	bool check2;	// will be interpreted as "there is NO mining site ON THE LEFT OF the miner"
	bool check3;	// will be interpreted as "there is NO mining site BENEATH the miner"
	bool check4;	// will be interpreted as "there is NO mining site ON THE RIGHT OF  the miner"
	
	if(level > M){								//  The number of miners cannot exceed the number of sites.
		cout << "\nlevel > M" << endl << endl;	//  If it does, surely there should be a mistake and the target map can never be obtained
		return false;
	}
	
	for(int r = 0; r < ROW; r++){
		for(int c = 0; c < COL; c++){
			if(Gold_Map[r][c] == 'm'){
				if(r == 0){
					check1 = true;		//  no mining site if no space above the miner
				}
				else{
					check1 = (Gold_Map[r-1][c] != 's');
				}
				
				
				if(c == 0){
					check2 = true;		//  no mining site if no space on the left of the miner
				}
				else{
					check2 = (Gold_Map[r][c-1] != 's' );
				}
				
				
				if(r == ROW-1){
					check3 = true;		//  no mining site if no space beneath the miner
				}
				else{
					check3 = (Gold_Map[r+1][c] != 's' );
				}
				
				
				if(c == COL-1){		//  no mining site if no space on the right of the miner
					check4 = true;
				}
				else{
					check4 = (Gold_Map[r][c+1] != 's' );
				}
				
				
				if( check1 && check2 && check3 && check4  ){	//	if no mining site around the miner(top, left, bottom, right)
					return false;
				}
				
				for(int i = r-1; i <= r+1; i++){
					for(int j = c-1; j <= c+1; j++){
						if((i == r) && (j == c)){			//  ignore the current miner itself, look for other contiguous miners
							continue;
						}
						if( ( i < 0 )  ||  ( j < 0 )  ||  ( i >= ROW )  ||  ( j >= COL ) ){		//  ignore the outsider cells
							continue;
						}
						if( Gold_Map[i][j] == 'm' ){			//  if any adjacent miner to the current miner, then return false
							return false;
						}
					}
				}	
			}
		}
	}
	return true;		//  if the node, after all of this, doesn't exit the function up to now, then return true
}



bool  Node_or_State::is_the_same(Node_or_State* in){		//  returns false if these two objects are uniquely different, and true if they are the same
	for(int r = 0; r < ROW; r++){
		for(int c = 0; c < COL; c++){
			if( Gold_Map[r][c] != in->Gold_Map[r][c] ){
				return false;
			}
		}
	}
	
	for(int i = 0; i < COL; i++){
		if(row[i] != in->row[i]){
			return false;
		}
	}
	
	
	for(int j = 0; j < ROW; j++){
		if(column[j] != in->column[j]){
			return false;
		}
	}
	
	return true;
}


void  Node_or_State::print_data(){
	cout << "----------------------------------------------------\n";
	for(int u = 0; u < COL; u++){
		cout << "\t" << row[u];
	}
	cout << "\n";
	for(int j = 0; j < ROW; j++){
		cout << column[j];
		
		for(int k = 0; k < COL; k++){
			cout << "\t" << Gold_Map[j][k];
		}
		cout << "\n";
	}
	cout << "----------------------------------------------------\n";
	cout << "\n\nLevel: " << level << endl << endl << endl;

	for(int m = 0; m < M; m++){
		cout << "[" << m+1 << "]:       row: " << Mining_Sites[m][0] << ",	column: " << Mining_Sites[m][1] << endl;
	}
}


Node_or_State::~Node_or_State(){}



int max_num_of_nodes = 0;					// maximum # of nodes kept in the memory
vector<Node_or_State>  discovered_Nodes;	//  Discovered Nodes' List for Queue or Stack



//-----------------------------------

class  MINING_GAME_BY_BFS {  // tries to find the solution with Breadth-First Search
  public:
	queue<Node_or_State>  myQueue;
	
  public:
	void  Find_The_Solution(char*);
	void  add_node_to_queue(Node_or_State*);
	Node_or_State*  retrieve_node_from_queue();
	bool  check_if_solution_is_found(Node_or_State*);
	bool  node_is_expandable(Node_or_State*);
	void  add_all_its_children_to_queue(Node_or_State*);
	void  add_node_to_discovered_list(Node_or_State*);
	bool  node_is_already_discovered(Node_or_State*);
	void  print_algorithm_summary(clock_t);
	void  write_solution_to_output_file(Node_or_State*, char*);
};


void  MINING_GAME_BY_BFS::Find_The_Solution(char* output){
	
	t1 = clock();

	Node_or_State  *first_node;
	first_node = new Node_or_State (init_map, init_row, init_column);
	
	add_node_to_queue(first_node);
	max_num_of_nodes = 1;
	
	while(!myQueue.empty()){
		Node_or_State* temporary;
		temporary = retrieve_node_from_queue();
		
		visited++;
		
		if(check_if_solution_is_found(temporary)){	//	IF THE SOLUTION IS FOUND, THEN THE GAME IS COMPLETED...
			t2 = clock();
			cout << "\nSOLUTION\n---------------\n";
			write_solution_to_output_file(temporary, output);
			print_algorithm_summary(t2-t1);
			return;
		}
		
		if( node_is_expandable(temporary) ){		//  if not found, check the expandability of the current node(i.e convenience of adding a new miner)
			add_all_its_children_to_queue(temporary);
			
			if( myQueue.size() > max_num_of_nodes){		//	after adding new nodes to the Queue, if size of queue exceeds the max # of nodes kept in the memory, update the "max_num_of_nodes"
				max_num_of_nodes = myQueue.size();
			}
		}
	}
}



void  MINING_GAME_BY_BFS::add_node_to_queue(Node_or_State* in){
	myQueue.push(*in);
}


Node_or_State*  MINING_GAME_BY_BFS::retrieve_node_from_queue(){
	Node_or_State* temp = new Node_or_State [1];
	*temp = myQueue.front();
	
	myQueue.pop();
	return temp;
}


bool  MINING_GAME_BY_BFS::check_if_solution_is_found(Node_or_State* in){
	bool z = in->columns_and_rows_are_all_zero();
	bool cp = in->miners_are_correctly_placed();
	
	if(z && cp){
		return true;
	}
	return false;
}



bool  MINING_GAME_BY_BFS::node_is_expandable(Node_or_State* in){
	bool pz = in->columns_and_rows_are_all_positive_or_zero();
	bool cp = in->miners_are_correctly_placed();
	
	if(pz && cp){
		return true;
	}
	return false;
}



void  MINING_GAME_BY_BFS::add_all_its_children_to_queue(Node_or_State* in){
	
	Node_or_State* temp;
	Node_or_State* temp2;
	
	for(int r = 0; r < ROW; r++){
		for(int c = 0; c < COL; c++){
			if(in->Gold_Map[r][c] == '.'){
				temp = new Node_or_State (*in);
				
				temp->Gold_Map[r][c] = 'm';
				temp->row[c]--;
				temp->column[r]--;
				temp->level++;
				if(node_is_already_discovered(temp)){
					continue;
				}
				else{
					temp2 = new Node_or_State (*temp);
					add_node_to_queue(temp);
					add_node_to_discovered_list(temp2);
				}
			}
		}
	}
}



void  MINING_GAME_BY_BFS::add_node_to_discovered_list(Node_or_State* in){
	discovered_Nodes.push_back(*in);
}



bool  MINING_GAME_BY_BFS::node_is_already_discovered(Node_or_State* in){
	
	vector<Node_or_State>::iterator Q_it;
	
	for ( Q_it = discovered_Nodes.begin() ; Q_it != discovered_Nodes.end(); Q_it++){
		if(in->is_the_same(&(*Q_it))){
			return true;
		}
	}
	return false;
}


void  MINING_GAME_BY_BFS::print_algorithm_summary(clock_t elapsed){
	cout << "Algorithm:  BFS" << endl;
	cout << "Number of the visited nodes: " << visited << endl;
	cout << "Maximum number of nodes kept in the memory: " << max_num_of_nodes << endl;
	cout << "Running time: " << float(elapsed) / CLOCKS_PER_SEC << " seconds" << endl;
	cout << "Solution is written to the file..." << endl;
}



void  MINING_GAME_BY_BFS::write_solution_to_output_file(Node_or_State* in, char* output){
	FILE* fptr;
	
	fptr = fopen(output, "w");
	if(!fptr){
		cout << endl << "ERROR :  Output file couldn't open!!" << endl << endl;
		return;
	}
	
	fprintf(fptr, "%d%c%d%c", COL, '\t', ROW, '\n');
	
	for(int j = 0; j < COL; j++){
		fprintf(fptr, "%c%d", '\t', init_row[j]);
	}
	
	fprintf(fptr, "%c", '\n');
	
	for(int r = 0; r < ROW; r++){
		fprintf(fptr, "%d", init_column[r]);
		
		for(int c = 0; c < COL; c++){
			fprintf(fptr, "%c%c", '\t', in->Gold_Map[r][c]);
		}
		fprintf(fptr, "%s", "\n");
	}
	
	fclose(fptr);
}



//-------------------------------------------------------------------------

class  MINING_GAME_BY_DFS {  // tries to find the solution with Depth-First Search
  public:
	stack<Node_or_State>  myStack;
	
  public:
	void  Find_The_Solution(char*);
	void  add_node_to_stack(Node_or_State*);
	Node_or_State*  retrieve_node_from_stack();
	bool  check_if_solution_is_found(Node_or_State*);
	bool  node_is_expandable(Node_or_State*);
	void  add_all_its_children_to_stack(Node_or_State*);
	void  add_node_to_discovered_list(Node_or_State*);
	bool  node_is_already_discovered(Node_or_State*);
	void  print_algorithm_summary(clock_t);
	void  write_solution_to_output_file(Node_or_State*, char*);
};

void  MINING_GAME_BY_DFS::Find_The_Solution(char* output){
	
	t1 = clock();
	
	Node_or_State  *first_node;
	first_node = new Node_or_State (init_map, init_row, init_column);
	
	
	add_node_to_stack(first_node);
	max_num_of_nodes = 1;
	
	while(!myStack.empty()){
		Node_or_State* temporary;
		temporary = retrieve_node_from_stack();
		
		visited++;
		
		if(check_if_solution_is_found(temporary)){	//	IF THE SOLUTION IS FOUND, THEN THE GAME IS COMPLETED...
			t2 = clock();
			cout << "\nSOLUTION\n---------------\n";
			write_solution_to_output_file(temporary, output);
			print_algorithm_summary(t2-t1);
			return;
		}
		
		if( node_is_expandable(temporary) ){		//  if not found, check the expandability of the current node(i.e convenience of adding a new miner)
			add_all_its_children_to_stack(temporary);
			
			if( myStack.size() > max_num_of_nodes){		//	after adding new nodes to the Stack, if size of stack exceeds the max # of nodes kept in the memory, update the "max_num_of_nodes"
				max_num_of_nodes = myStack.size();
			}
		}
	}
}




void  MINING_GAME_BY_DFS::add_node_to_stack(Node_or_State* in){
	myStack.push(*in);
}


Node_or_State*  MINING_GAME_BY_DFS::retrieve_node_from_stack(){
	Node_or_State* temp = new Node_or_State [1];
	*temp = myStack.top();
	
	myStack.pop();
	
	return temp;
}


bool  MINING_GAME_BY_DFS::check_if_solution_is_found(Node_or_State* in){
	bool z = in->columns_and_rows_are_all_zero();
	bool cp = in->miners_are_correctly_placed();
	
	if(z && cp){
		return true;
	}
	return false;
}


bool  MINING_GAME_BY_DFS::node_is_expandable(Node_or_State* in){
	bool pz = in->columns_and_rows_are_all_positive_or_zero();
	bool cp = in->miners_are_correctly_placed();
	
	if(pz && cp){
		return true;
	}
	return false;
}


void  MINING_GAME_BY_DFS::add_all_its_children_to_stack(Node_or_State* in){
	
	Node_or_State* temp;
	Node_or_State* temp2;
	
	for(int r = 0; r < ROW; r++){
		for(int c = 0; c < COL; c++){
			if( in->Gold_Map[r][c] == '.' ){
				temp = new Node_or_State (*in);
				
				temp->Gold_Map[r][c] = 'm';
				temp->row[c]--;
				temp->column[r]--;
				temp->level++;
				if(node_is_already_discovered(temp)){
					continue;
				}
				else{
					temp2 = new Node_or_State (*temp);
					add_node_to_stack(temp);
					add_node_to_discovered_list(temp2);
				}
			}
		}
	}
}


void  MINING_GAME_BY_DFS::add_node_to_discovered_list(Node_or_State* in){
	discovered_Nodes.push_back(*in);
}


bool  MINING_GAME_BY_DFS::node_is_already_discovered(Node_or_State* in){
	
	vector<Node_or_State>::iterator S_it;
	
	for ( S_it = discovered_Nodes.begin() ; S_it != discovered_Nodes.end(); S_it++){
		if(in->is_the_same(&(*S_it))){
			return true;
		}
	}
	return false;
}


void  MINING_GAME_BY_DFS::print_algorithm_summary(clock_t elapsed){
	cout << "Algorithm:  DFS" << endl;
	cout << "Number of the visited nodes: " << visited << endl;
	cout << "Maximum number of nodes kept in the memory: " << max_num_of_nodes << endl;
	cout << "Running time: " << float(elapsed) / CLOCKS_PER_SEC << " seconds" << endl;
	cout << "Solution is written to the file..." << endl;
}


void  MINING_GAME_BY_DFS::write_solution_to_output_file(Node_or_State* in, char* output){
	FILE* fptr;
	
	fptr = fopen(output, "w");
	if(!fptr){
		cout << endl << "ERROR :  Output file couldn't open..!!!" << endl << endl;
		return;
	}
	
	fprintf(fptr, "%d%c%d%c", COL, '\t', ROW, '\n');
	
	for(int j = 0; j < COL; j++){
		fprintf(fptr, "%c%d", '\t', init_row[j]);
	}
	
	fprintf(fptr, "%c", '\n');
	
	for(int r = 0; r < ROW; r++){
		fprintf(fptr, "%d", init_column[r]);
		
		for(int c = 0; c < COL; c++){
			fprintf(fptr, "%c%c", '\t', in->Gold_Map[r][c]);
		}
		fprintf(fptr, "%s", "\n");
	}
	
	fclose(fptr);
}


//--------------------------------------------------
void  read_the_input_file(const char *in){
	FILE* fptr;
	char token[4];

	fptr = fopen(in, "r");
	if(!fptr){
		cout << endl << "ERROR :  Input text file specified does not exist..!!!" << endl << endl;
		return;
	}

	fscanf (fptr, "%s", token);
	COL = atoi(token);
	init_row = new int [COL];

	fscanf (fptr, "%s", token);
	ROW = atoi(token);
	init_column = new int [ROW];
	
	
	init_map = new char* [ROW];
	for(int r = 0; r < ROW; r++){
		init_map[r] = new char [COL];
	}

	for(int i = 0; i < COL; i++){
		fscanf (fptr, "%s", token);
		init_row[i] = atoi(token);
	}

	for(int j = 0; j < ROW; j++){
		fscanf (fptr, "%s", token);
		init_column[j] = atoi(token);
		
		for(int k = 0; k < COL; k++){
			fscanf (fptr, "%s", token);
			init_map[j][k] = token[0];
		}
		
		if (feof(fptr)){
			break;
		}
		
	}

	fclose(fptr);
}



void  locate_the_mining_sites(){  // finds the location(row/column) of the mining sites and write them to 2-D array(Mining_Sites)  -->  Mx2
	for(int r = 0; r < ROW; r++){	// only for counting the sites
		for(int c = 0; c < COL; c++){
			if( init_map[r][c] == 's' ){
				M++;
			}
		}
	}
	
	Mining_Sites = new int* [M];		//  M mining sites
	for(int m = 0; m < M; m++){
		Mining_Sites[m] = new int [2];	//	for each mining site, reserve memory for its row and column  -->   [ row | column ]
	}
	
	int counter = 0;
	for(int r = 0; r < ROW; r++){	//	now, we can keep the row and column information of every mining site
		for(int c = 0; c < COL; c++){
			if( init_map[r][c] == 's' ){
				Mining_Sites[counter][0] = r;
				Mining_Sites[counter][1] = c;
				counter++;
			}
		}
	}
}



void  print_init_map(){
	for(int j = 0; j < ROW; j++){
		for(int k = 0; k < COL; k++){
			cout << "\t" << init_map[j][k];
		}
		cout << "\n";
	}
	cout << "\n\n\tROW: " << ROW << "  COL:" << COL << endl;
	cout << endl << endl << endl;
}


void  print_init_row(){
	cout << "[init_row]:  ";
	
	for(int j = 0; j < COL; j++){
		cout << init_row[j] << "\t";
	}
	cout << endl << endl << endl;
}


void  print_init_column(){
	cout << "[init_column]:\n";
	
	for(int j = 0; j < ROW; j++){
		cout << init_column[j] << "\n";
	}
	cout << endl << endl << endl;
}


void  print_mining_sites(){
	
	for(int m = 0; m < M; m++){
		cout << "[" << m+1 << "]:    row: " << Mining_Sites[m][0] << ",	column: " << Mining_Sites[m][1] << endl;
	}
	cout << endl << endl << endl;
}


//--------------------------------------------------

int main(int argc, char *argv[]){
	
	int param_num = argc;
	
	if ( param_num != 4 ){
		cout << endl << "INVALID PARAMETER NUMBER!!" << endl << endl;
		return 0;
	}
	
	char  algorithm[4];  
	char  input_file[30] = "InputOutput/";
	char  output_file[30] = "InputOutput/";
	
	strcpy(algorithm,  argv[1]);   //  "dfs" or "bfs"
	strcat(input_file,  argv[2]);   //  e.g  "input.txt"
	strcat(output_file,  argv[3]);   //  e.g  "output.txt"

	if (strcmp(algorithm, "dfs") != 0  &&  strcmp(algorithm, "bfs") != 0  &&  strcmp(algorithm, "DFS") != 0  &&  strcmp(algorithm, "BFS") != 0){
		cout << endl << "INVALID ENTRY:  Parameters provided are wrong!!" << endl;
		return 0;
	}
	
	read_the_input_file(input_file);
	locate_the_mining_sites();

	if( strcmp(algorithm, "bfs") == 0  ||  strcmp(algorithm, "BFS") == 0 ){
		MINING_GAME_BY_BFS Game;
		Game.Find_The_Solution(output_file);
	}
	
	else {
		MINING_GAME_BY_DFS Game;
		Game.Find_The_Solution(output_file);
	}
	
	getchar();
	return 0;
}