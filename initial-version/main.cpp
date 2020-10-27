#include "gen.hpp"

#include <iostream>
#include <cstdlib>
#include <string>
/* #include <utility> */
/* #include <cstdint> */

#include "my.hpp"

int main(int argc, char *argv[]) {
  gen::phenotype<std::string>::install_core(
      my::init_f,
      my::fitness_f,
      my::crossover_f,
      my::mutate_f
  );
  gen::evolution<gen::phenotype<std::string>> ev(500);
  ev.evolve();

  
  return 0;
}
