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

template<typename T>
class phenotype {
private:
  T _genotype;
  inline static std::function<T()> _init_f = nullptr;
  inline static std::function<double(const T&)> _fitness_f = nullptr;
  inline static std::function<T(const T&, const T&)> _crossover_f = nullptr;
  inline static std::function<T(const T&)> _mutate_f = nullptr;
  inline static std::function<void(phenotype<T> *)> _deconstructor = nullptr;
  inline static std::function<void(phenotype<T> *, const T&)> _genotype_copy_constructor = nullptr;
  inline static std::function<void(phenotype<T> *, T&&)> _genotype_move_constructor = nullptr;
  inline static std::function<std::ostream&(std::ostream&, const phenotype<T>&)> _print_f = nullptr;
  inline static bool core = false;

public:
  phenotype() {
    if(!phenotype::core){
      std::cerr << "You must specify next methods for your phenotype:\n";
      std::cerr << "\tT init() : initialization function\n";
      std::cerr << "\tdouble fitnless(const T&) : fitness function\n";
      std::cerr << "\tT crossover(const T&, const T&) : crossover function\n";
      std::cerr << "\tT fitness(const T&) : mutate function\n";
      std::cerr << "where T is your genotype structure\n\n";
      std::cerr << "Check the source code more methods you can define.\n";
      std::cerr << std::flush;
      exit(EXIT_FAILURE);
    }
    _genotype = phenotype::_init_f();
  }

 ~phenotype(){ if(_deconstructor) _deconstructor(this); }

  phenotype(const T& t) {
    if(_genotype_copy_constructor) _genotype_copy_constructor(this, t);
    else _genotype = t;
  }

  phenotype(T&& t) {
    if(_genotype_move_constructor) _genotype_copy_constructor(this, t);
    else std::swap(t, _genotype);
  }

  T genotype(){
    return _genotype;
  }

  double fitness() const { return _fitness_f(_genotype); }

  phenotype<T> mutate() const {
    return phenotype<T>(_mutate_f(_genotype));
  }

  phenotype<T> operator + (const phenotype<T> & p) const {
    return phenotype<T>(_crossover_f(_genotype, p._genotype));
  }

  friend std::ostream& operator << (std::ostream & os, const phenotype<T>& p){
    if(_print_f) return phenotype::_print_f(os, p);
    else{
      for(auto & a: p._genotype){ os << a; }
      return os;
    }
  }

  static void install_core(
    std::function<T()> init_f,
    std::function<double(const T&)> fitness_f,
    std::function<T(const T&, const T&)> crossover_f,
    std::function<T(const T&)> mutate_f
  ) {
    phenotype::_init_f = init_f;
    phenotype::_fitness_f = fitness_f;
    phenotype::_crossover_f = crossover_f;
    phenotype::_mutate_f = mutate_f;
    phenotype::core = true;
  }

  static void install_print(
      std::function<std::ostream&(std::ostream&, const phenotype<T>&)> f
  ){
    phenotype::_print_f = f;
  }

  static void install_destructor(
      std::function<void(phenotype<T> *&)> f
  ){
    phenotype::_deconstructor = f;
  }

  static void install_genotype_move_constructor(
    std::function<void(phenotype<T> *&, T&&)> f
  ){
    phenotype::_genotype_move_constructor = f;
  }

  static void install_genotype_copy_constructor(
    std::function<void(phenotype<T> *&, const T&)> f
  ){
    phenotype::_genotype_copy_constructor = f;
  }
};

template<class T>
class evolution {
private:
  inline static const std::size_t _cores = std::thread::hardware_concurrency();
  std::vector<T> _population;
  std::size_t _p_size;
  T _winner;

  /* termiation */
  inline static std::size_t _no_progress = 600;
  inline static std::size_t _max_iterations = 6000;
  inline static std::function<bool(const std::vector<T>&, const T&, std::size_t)> _termination_f = nullptr;

  /* offspring */
  inline static std::function<std::vector<T>(const std::vector<T>&)> _offspring_f = nullptr;

  std::vector<T> _default_offspring() const {
    float sum = std::accumulate(_population.cbegin(), _population.cend(), 0, [](auto acc, auto& p){ return acc + p.fitness(); });
    std::vector<T> new_population;
    new_population.reserve(_p_size);
    for(std::size_t i = 0; i<_p_size; i++){
      T t = evolution::_roulette_pick(sum, _population) + evolution::_roulette_pick(sum, _population);
      new_population.push_back(t.mutate());
    }
    return new_population;
  }

  static T _roulette_pick(float sum, const std::vector<T>& population) {
    float arrow = 0;
    float worra = (rand()/(float)RAND_MAX) * sum;
    for(auto& p: population){
      arrow += p.fitness();
      if(arrow >= worra){
        return p;
      }
    }
    return population[0]; // make compiler happy
  }

public:
  evolution(std::size_t p_size) : _p_size(p_size) {
    std::cout << "evolution running on " << _cores << " cores\n";
    _population.reserve(p_size);
    for(std::size_t i=0; i<p_size; i++){
      _population.emplace_back();
    }
    _winner = *std::max_element(_population.cbegin(), _population.cend(), [](const T& t1, const T& t2){ return t1.fitness() < t2.fitness(); });
  }

  friend std::ostream& operator << (std::ostream & os, const evolution<T> & e){
    os << "population:\n";
    for (auto& p: e._population){
      os << '\t' << p << ' ' << p.fitness() << '\n';
    }
    return os;
  }

  void install_termination(std::size_t no_progress, std::size_t max_iterations){
    _no_progress = no_progress;
    _max_iterations = max_iterations;
  }

  void install_termination(
      std::function<bool(const std::vector<T>&, const T&, std::size_t)> f
  ){
    _termination_f = f;
  }

  void install_offspring(
    std::function<std::vector<T>(const std::vector<T>&)> f
  ){
    _offspring_f = f;
  }

  void evolve(){
    std::size_t current_iteration = 0;
    std::size_t no_progress = 0;
    while(current_iteration < _max_iterations &&
          no_progress < _no_progress
    ){
      //TODO add <, >, == to phenotype class
      if(_winner.genotype() == "Genetic algorithm demonstration") break;
      _population = _default_offspring();
      auto i = std::max_element(_population.cbegin(), _population.cend(), [](const T& t1, const T& t2){ return t1.fitness() < t2.fitness(); });
      if((*i).fitness() > _winner.fitness()){
        _winner = *i;
        no_progress = 0;
        std::cout << '\t' << *i << '\n';
      }
      else{
        no_progress++;
      }
    }
  }

  T winner(){ return _winner; }


};


} /* namespace gen */

#endif /* ifndef GEN_HPP */
