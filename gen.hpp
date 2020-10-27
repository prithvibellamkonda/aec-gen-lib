#ifndef GEN_HPP
#define GEN_HPP 0

#include <iostream>
#include <functional>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>

#include <ranges>
#include <utility>
#include <numeric>

namespace gen {

template<class T>
class evolution {
private:
  inline static const std::size_t _cores = std::thread::hardware_concurrency();
  std::vector<T> _population;
  std::vector<float> _scores;
  std::size_t _p_size;
  std::pair<T, float> _winner;

public:
  /* termiation */
  inline static std::size_t _no_progress = 200;
  inline static std::size_t _max_iterations = 6000;
  inline static std::function<bool(const std::vector<T>&, const T&, std::size_t)> _termination_f = nullptr;
  /* offspring */
  inline static std::function<std::vector<T>(const std::vector<T>&, const std::vector<float>&)> _offspring_f = nullptr;
  inline static std::function<std::pair<T, float>(const std::pair<T, float>&, const std::vector<T>&, const std::vector<float>&)> _new_winner_f = nullptr;
  inline static std::function<std::pair<T, float>(const std::vector<T>&, const std::vector<float>&)> _winner_f = nullptr;
  inline static std::function<std::vector<float>(const std::vector<T>&, std::size_t)> _fitness_f = nullptr;

  evolution(std::size_t p_size) : _p_size(p_size) {
    std::cout << "evolution running on " << _cores << " cores\n";
    _population = std::vector<T>(p_size);
    _scores = _fitness_f(_population,_cores);
    _winner = _winner_f(_population, _scores);
  }

  friend std::ostream& operator << (std::ostream & os, const evolution<T> & e){
    os << "population:\n";
    for (auto& p: e._population){
      os << p << '\n';
    }
    return os;
  }

  void install_termination(std::size_t no_progress, std::size_t max_iterations){
    _no_progress = no_progress;
    _max_iterations = max_iterations;
  }

  void evolve(){
    std::size_t current_iteration = 0;
    std::size_t no_progress = 0;
    while(current_iteration < _max_iterations &&
          no_progress < _no_progress
    ){
      _population = _offspring_f(_population, _scores);
      _scores = _fitness_f(_population, _cores);
      auto old_score = _winner.second;
      _winner = _new_winner_f(_winner, _population, _scores);
      current_iteration++;
      if(old_score == _winner.second) no_progress++;
      else no_progress = 0;

      std::cout << "iteration: " << current_iteration << " score: " << _winner.second << '\n';
    }
  }

  T winner(){ return _winner.first; }
};

} /* namespace gen */

#endif /* ifndef GEN_HPP */
