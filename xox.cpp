#include "my.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

void logMsg(int i, int side){
  if(i == side) std::cout << "congrats!\n";
  else if(i == -side) std::cout << "bummer!\n";
  else std::cout << "draw!\n";
}

int main(int argc, char *argv[]) {
  srand(time(NULL));

  TTToe t;
  net n;
  n.load("winner");

  int side; std::cin >> side;
  int i = t.humanPlay(n, side);

  logMsg(i, side);
  return 0;
}
