#include "gen.hpp"
#include "my.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <utility>
#include <cstdint>


int main(int argc, char *argv[]) {
  srand(time(NULL));

  gen::evolution<net>::_offspring_f = offspring_f;
  gen::evolution<net>::_fitness_f = fitness_f;
  gen::evolution<net>::_winner_f = winner_f;
  gen::evolution<net>::_new_winner_f = new_winner_f;
  gen::evolution<net> ev(100);
  ev.install_termination(100, 1000);
  ev.evolve();
  auto winner = ev.winner();
  winner.save("winner");
  
  return 0;
}
