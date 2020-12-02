#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// node classes
enum class State {kEmpty, kObstacle, kClosed, kPath};
// directional deltas
const int delta[4][2]={{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

vector<State> parse_line(string line) {
    istringstream my_line(line);
    int n;
    char c;
    vector<State> row;
    while (my_line >> n >> c && c == ',') {
      if (n == 0) {
        row.push_back(State::kEmpty);
      } else {
        row.push_back(State::kObstacle);
      }
    }
    return row;
}

vector<vector<State>> read_board_file(string path) {
  ifstream my_file (path);
  vector<vector<State>> board{};
  if (my_file) {
    string line;
    while (getline(my_file, line)) {
      vector<State> row = parse_line(line);
      board.push_back(row);
    }
  }
  return board;
}

//Compare the F values of two cells
bool compare(const vector<int> a, const vector<int> b) {
  int f1 = a[2] + a[3]; // f1 = g1 + h1
  int f2 = b[2] + b[3]; // f2 = g2 + h2
  return f1 > f2; 
}

//Sort the 2D vector in descending order
void cell_sort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), compare);
}

// Calculate the manhattan distance
int heuristic(int x1, int y1, int x2, int y2) {
  return abs(x2 - x1) + abs(y2 - y1);
}


//Check that a cell is valid: on the grid, not an obstacle, and clear
bool check_valid_cell(int x, int y, vector<vector<State>> &board) {
  bool valid_x = (x >= 0 && x < board.size());
  bool valid_y = (y >= 0 && y < board[0].size());
  if (valid_x && valid_y)
    return board[x][y] == State::kEmpty;
  return false;
}

//Add a node to the open list and mark it as open
void add_to_open(int x, int y, int g, int h, vector<vector<int>> &open, vector<vector<State>> &board) {
  open.push_back(vector<int>{x, y, g, h});
  board[x][y] = State::kClosed;
}

//Expand current nodes's neighbors and add them to the open list.
void expand_neighbors(const vector<int> &current, int goal[2], vector<vector<int>> &open, vector<vector<State>> &board) {
	int x1=current[0];
	int y1=current[1];
	int g1=current[2];
  
	for (int i=0;i<4;i++){
    	int x2=x1+delta[i][0];
        int y2=y1+delta[i][1];
      if(check_valid_cell(x2,y2,board)){
      	int g2=g1+1;
        int h2=heuristic(x2,y2,goal[0],goal[1]); 
        add_to_open(x2,y2,g2,h2,open,board);
      }
    }
}


// Implementation of A* search algorithm
vector<vector<State>> search(vector<vector<State>> board, int init[2], int goal[2]) {
  // Create the vector of open nodes.
  vector<vector<int>> open {};
  
  // Initialize the starting node.
  int x = init[0];
  int y = init[1];
  int g = 0;
  int h = heuristic(x, y, goal[0],goal[1]);
  add_to_open(x, y, g, h, open, board);

  while (open.size() > 0) {
    // Get the next node
    cell_sort(&open);
    auto current = open.back();
    open.pop_back();
    x = current[0];
    y = current[1];
    board[x][y] = State::kPath;
    // Check if goal reached
    if (x == goal[0] && y == goal[1]) {
      return board;
    }
    // expand to neighboring node
    expand_neighbors(current, goal, open, board);
  }
  //no path found
  cout << "No path found!" << endl;
  return std::vector<vector<State>>{};
}


string cell_string(State cell) {
  switch(cell) {
    case State::kObstacle: return "⛰️   ";
    case State::kPath: return "🚗   ";
    default: return "0   "; 
  }
}


void print_board(const vector<vector<State>> board) {
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      cout << cell_string(board[i][j]);
    }
    cout << "\n";
  }
}


int main() {
  int init[2]{0, 0};
  int goal[2]{4, 5};
  auto board = read_board_file("1.board");
  cout<<"initial board"<<endl;
  print_board(board);
  cout<<"start: "<<init[0]<<" "<<init[1]<<endl;
  cout<<"goal: "<<goal[0]<<" "<<goal[1]<<endl;
  cout<<"now finding solution..."<<endl;
  auto solution = search(board, init, goal);
  print_board(solution);
}