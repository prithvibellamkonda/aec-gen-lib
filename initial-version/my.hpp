#ifndef MY
#define MY 0

#include "gen.hpp"

#include <string>
#include <cstdlib>

namespace my {

static const std::string str = " ;:!@#$\%^&*()_+-=<>?/|\\`~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const std::size_t range = str.size();
static const std::string solution = "Genetic algorithm demonstration";
static const std::string blank    = "_______________________________";

auto init_f = [](){
  std::string g =  blank;
  std::size_t len = g.size();
  for(std::size_t i=0; i<len; i++){
    g[i] = str[rand() % range];
  }
  return g;
};

auto fitness_f = [](const std::string& s) {
  uint32_t f = 0;
  std::size_t len = solution.size();
  for(std::size_t i=0; i<len; i++){
    f += (s[i] == solution[i]) ? 1 : 0;
  }
  return f*f;
};

auto crossover_f = [](const std::string& s, const std::string& s1){
  std::string tmp = s;
  std::size_t size = tmp.size();
  for(std::size_t i=0; i<size; i++){
    if(rand() % 2) tmp[i] =s1[i];
  }
  return tmp;
};

auto mutate_f = [](const std::string& s){
  std::string tmp = s;
  if((rand()% 10) < 3){
    tmp[rand() % tmp.size()] = str[rand() % range];
    return tmp;
  }
  return tmp;
};

} /* namespace my */

#endif /* ifndef MY */
