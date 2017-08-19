// Porting the maze.py program to C++ using SDL for drawing
// Max Malacari 24/02/2017
// To compile: g++ -framework SDL2 maze.cc

#include <iostream>
#include <vector>
#include <cmath>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int COLS = 50;
const int ROWS = 50;

const int w = WINDOW_WIDTH/COLS;
const int h = WINDOW_HEIGHT/ROWS;



void SetDrawColour(SDL_Renderer *, string);

class Cell{ // Holds the properties of each cell
private:
  vector<bool> walls;
  vector<bool> visitedNeighbours;
public:
  int i;
  int j;
  bool visited;

  void InitCell(int i, int j) // constructor
  {
    this->i = i;
    this->j = j;
    this->visited = 0;
    for (int k=0; k<4; k++){ // Fill walls and neighbours
      this->walls.push_back(1);
      this->visitedNeighbours.push_back(0);
    }

    // set visited neighbours to true for cells at the edge
    if (this->j == 0) this->visitedNeighbours[0] = 1;
    if (this->j == ROWS - 1) this->visitedNeighbours[2] = 1;
    if (this->i == 0) this->visitedNeighbours[3] = 1;
    if (this->i == COLS - 1) this->visitedNeighbours[1] = 1;
  }

  void ShowCellBoundary(SDL_Renderer * renderer){ //top,right,bottom,left
    if (this->walls[0] == 1) SDL_RenderDrawLine(renderer, this->i*w, this->j*h, (this->i+1)*w, this->j*h);
    if (this->walls[1] == 1) SDL_RenderDrawLine(renderer, (this->i+1)*w, this->j*h, (this->i+1)*w, (this->j+1)*h);
    if (this->walls[2] == 1) SDL_RenderDrawLine(renderer, this->i*w, (this->j+1)*h, (this->i+1)*w, (this->j+1)*h);
    if (this->walls[3] == 1) SDL_RenderDrawLine(renderer, this->i*w, this->j*h, this->i*w, (this->j+1)*h);
  }

  void CalculateUnvisitedNeighbours(Cell grid[COLS][ROWS]){
    if (this->j > 0){ // top neighbour
      if (grid[this->i][this->j-1].visited == 1) this->visitedNeighbours[0] = 1;
    }
    if (this->i < COLS - 1){ // right neighbour
      if (grid[this->i+1][this->j].visited == 1) this->visitedNeighbours[1] = 1;
    }
    if (this->j < ROWS - 1){ // bottom neighbour
      if (grid[this->i][this->j+1].visited == 1) this->visitedNeighbours[2] = 1;
    }
    if (this->i > 0){ // left neighbour
      if (grid[this->i-1][this->j].visited == 1) this->visitedNeighbours[3] = 1;
    }
  }

  int CountAvailableNeighbours(){ // Count available neighbours to go to next
    int counter = 0;
    for (int k=0; k<4; k++){
      if (this->visitedNeighbours[k] == 0) counter++;
    }
    return counter;
  }

  Cell * ChooseRandomNeighbour(Cell grid[COLS][ROWS]){
    vector<int> cellIndex;
    for (int k=0; k<4; k++){
      if (this->visitedNeighbours[k] == 0) cellIndex.push_back(k);
    }
    int p = floor(rand() % cellIndex.size()); // random index
    Cell * neighbourCell;
    if (cellIndex[p] == 0) neighbourCell = &grid[this->i][this->j-1]; // top
    if (cellIndex[p] == 1) neighbourCell = &grid[this->i+1][this->j]; // right
    if (cellIndex[p] == 2) neighbourCell = &grid[this->i][this->j+1]; // bottom
    if (cellIndex[p] == 3) neighbourCell = &grid[this->i-1][this->j]; // left
    return neighbourCell;
  }

  void DeleteWalls(Cell * cell){ // delete walls between this cell and "cell"
    if (this->i == cell->i && this->j == cell->j+1) this->walls[0] = 0;
    if (this->i == cell->i-1 && this->j == cell->j) this->walls[1] = 0;
    if (this->i == cell->i && this->j == cell->j-1) this->walls[2] = 0;
    if (this->i == cell->i+1 && this->j == cell->j) this->walls[3] = 0;
  }
};

int main(int argc, char *argv[]){

  srand(time(NULL)); // init random seed

  // Define size of a cell within the borders (2 pixels wide) -----
  SDL_Rect cellObj;
  cellObj.w = w-1;
  cellObj.h = h-1;
  // -----------

  // Initialize cells -----
  Cell grid[COLS][ROWS];
  for (int i=0; i<COLS; i++){
    for (int j=0; j<ROWS; j++){
      grid[i][j].InitCell(i,j);
    }
  }
  // -------------
  
  // SDL setup ------
  SDL_Event event;
  SDL_Renderer *renderer;
  SDL_Window *window;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WINDOW_WIDTH+1, WINDOW_WIDTH+1, 0, &window, &renderer);
  // -----------------


  // Algorithm starts here
  Cell * startCell = &grid[0][0]; // Set address of start cell
  bool finished = 0;
  vector<Cell*> stack; // vector of addresses to cells in the stack
  Cell * currentCell = startCell;

  while(!finished){

    // Draw the current state ----
    SetDrawColour(renderer,"black");
    SDL_RenderClear(renderer);
    SetDrawColour(renderer, "white");
    for (int i=0; i<COLS; i++){
      for (int j=0; j<ROWS; j++){
  	grid[i][j].ShowCellBoundary(renderer);
      }
    }
    cellObj.x = currentCell->i*w + 1;
    cellObj.y = currentCell->j*h + 1;
    SDL_RenderFillRect(renderer, &cellObj);
    SDL_RenderPresent(renderer);
    // ----------------

    //cout << currentCell->i << " " << currentCell->j << endl;

    currentCell->visited = 1; // Set cell to visited
    currentCell->CalculateUnvisitedNeighbours(grid); // Calculate unvisted neighbouring cells
    if ( currentCell->CountAvailableNeighbours() > 0){
      stack.push_back(currentCell);
      Cell * nextCell = currentCell->ChooseRandomNeighbour(grid);
      currentCell->DeleteWalls(nextCell); // delete walls of both cells that border each other
      nextCell->DeleteWalls(currentCell);
      currentCell = nextCell;
    }
    else if (stack.size() > 0){
      currentCell = stack[stack.size()-1];
      stack.pop_back();
    }
    if (stack.size() == 0) finished = 1;

    //SDL_Delay(10);
  }


  // Wait for a quit event before closing the window
  while (1) {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
      break;
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
  
}

// Set the renderer draw colour M.M
void SetDrawColour(SDL_Renderer *renderer, string colour){
  int r,g,b;

  if (colour == "red"){ r=255; g=0; b=0;}
  if (colour == "black"){ r=0; g=0; b=0;}
  if (colour == "white"){ r=255; g=255; b=255;}

  SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  return;
}
