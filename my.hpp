#ifndef MY
#define MY 0

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <array>
#include <numeric>
#include <utility>
#include <fstream>
#include <future>

static const uint_fast8_t kernels[8][3] = {{8, 4, 0}, {6, 4, 2}, {8, 7, 6}, {5, 4, 3}, {2, 1, 0}, {8, 5, 2}, {7, 4, 1}, {6, 3, 0}};
static const uint_fast16_t wins[8] = {0b100010001, 0b001010100, 0b111000000, 0b000111000, 0b000000111, 0b100100100, 0b010010010, 0b001001001};
static const uint_fast16_t squares[9] = {0b100000000, 0b010000000, 0b001000000, 0b000100000, 0b000010000, 0b000001000, 0b000000100, 0b000000010, 0b000000001};

struct net{
  /* weights: 2*3 + 2 per kernel*/
  float weights1[8][8];
  float weights2[8];
  float biases[8];

  net(){
    for(auto i=0; i<8; i++){
      weights2[i] = 2 * (rand() / float(RAND_MAX)) - 1;
      biases[i] = 4 * (rand() / float(RAND_MAX)) - 2;
      for(auto k=0; k<8; k++)
        weights1[i][k] = 2 * (rand() / float(RAND_MAX)) - 1;
    }
  }

  net(const net& n){
    for(auto i=0; i<8; i++){
      weights2[i] = n.weights2[i];
      biases[i] = n.biases[i];
      for(auto k=0; k<8; k++)
        weights1[i][k] = n.weights1[i][k];
    }
  }

  /* net operator=(const net& n) const { */
  /*   return net(n); */
  /* } */

  void save(const std::string& name){
    std::ofstream ofs;
    ofs.open(name, std::ios::out | std::ios::trunc);
    ofs << *this;
    ofs.close();
  }

  void load(const char* name){
    FILE* is = fopen(name, "r");
    for(auto i=0; i<8; i++){
      for(auto k=0; k<8; k++){
        fscanf(is, "%f", &weights1[i][k]);
      }
    }
    for(auto i=0; i<8; i++)
      fscanf(is, "%f", &biases[i]);
    for(auto i=0; i<8; i++)
      fscanf(is, "%f", &weights2[i]);
    fclose(is);
  }

  float forward(const std::array<std::array<bool, 8>, 2>& table) const {
    float layer1[8];
    for(auto i=0; i<8; i++){
      float tmp[2] = {0,0};
      for(auto k=0; k<3; k++){
        tmp[0] += table[0][kernels[i][k]] * weights1[i][k];
        tmp[1] += table[1][kernels[i][k]] * weights1[i][3+k];
      }
      layer1[i] = tmp[0] * weights1[i][6] +
                  tmp[1] * weights1[i][7];
      layer1[i] = layer1[i] > 0 ? layer1[i] : 0; //ReLU
      layer1[i] += biases[i];
    }
    float res = 0;
    for(auto i=0; i<8; i++){
      res += layer1[i] * weights2[i];
    }
    return tanh(res);
  }

  friend std::ostream& operator<< (std::ostream& os, const net& n){
    for(auto i=0; i<8; i++){
      for(auto k=0; k<8; k++)
        os << n.weights1[i][k] << ' ';
      os << '\n';
    }
    os << '\n';
    for(auto i=0; i<8; i++){
      os << n.biases[i] << ' ';
    }
    os << '\n';
    for(auto i=0; i<8; i++){
      os << n.weights2[i] << ' ';
    }
    os << '\n';
    return os;
  }

  net operator+ (const net& n2) const { //crossover
    net n = *this;
    for(auto i=0; i<8; i++){
      if(rand() % 2) n.weights2[i] = n2.weights2[i];
      if(rand() % 2) n.biases[i] = n2.biases[i];
      for(auto k=0; k<8; k++){
        if(rand() % 2) n.weights1[i][k] = n2.weights1[i][k];
      }
    }
    return n;
  }

  net operator~ () const { //mutate
    net n = *this;
    for(auto i=0; i<8; i++){
      if(rand() % 50 < 2) n.weights2[i] = 2 * (rand() / float(RAND_MAX)) - 1;
      if(rand() % 50 < 2) n.biases[i] = 4 * (rand() / float(RAND_MAX)) - 2;
      for(auto k=0; k<8; k++){
        if(rand() % 50 < 1) n.weights1[i][k] = 2 * (rand() / float(RAND_MAX)) - 1;
      }
    }
    return n;
  }
};


class TTToe{
private:
  uint_fast16_t _xo[2];
  int _side; // 1: x, -1: o
public:
  TTToe() {
    _side = 1;
    _xo[0] = 0;
    _xo[1] = 0;
  }

  int result() const {
    auto c = _xo[(_side + 1)/2];
    for (auto w: wins){
      if((w & c) == w) return -_side;
    }
    return 0;
  }

  bool over() const {
    return (result() != 0) || ((_xo[0] | _xo[1]) == 0b111111111);
  }

  void playIndex(uint_fast8_t i){
    _side *= -1;
    _xo[(_side + 1)/2] |= 1 << i;
  }

  void playMove(uint_fast16_t i){
    _side *= -1;
    _xo[(_side + 1)/2] |= i;
  }

  std::vector<uint_fast16_t> genMoves() const {
    std::vector<uint_fast16_t> moves; moves.reserve(9);
    const auto empty_squares = ~(_xo[0] | _xo[1]);
    for(auto i=0; i<9; i++){
      const uint_fast16_t tmp = 1 << i;
      if(tmp & empty_squares) moves.emplace_back(tmp);
    }
    return moves;
  }

  std::array<std::array<bool, 8>, 2> table() const {
    std::array<std::array<bool, 8>, 2> m;
    for(auto i=0; i<8; i++){
      m[0][i] = _xo[0] & (1 << i);
      m[1][i] = _xo[1] & (1 << i);
    }
    return m;
  }

  int playOut(const net& xn, const net& on){
    while(!over()){
      std::vector<uint_fast16_t> sons = genMoves();
      const auto n = _side == 1 ? xn : on;
      const auto move = *std::max_element(sons.cbegin(), sons.cend(),
      [&n, this](const auto a, const auto b){
        auto tmpa = *this; tmpa.playMove(a);
        auto tmpb = *this; tmpb.playMove(b);
        return (_side * n.forward(tmpa.table())) < (_side * n.forward(tmpb.table()));
      });
      playMove(move);
    }
    return result();
  }

  int humanPlay(const net& n, int side){
    system("clear");
    std::cout << *this;
    while(!over()){
      if(_side == side){
        unsigned index;
        std::cin >> index;
        playIndex(index);
      }
      else{
        const auto moves = genMoves();
        const auto move = *std::max_element(moves.cbegin(), moves.cend(),
        [&n, this](const auto a, const auto b){
          auto tmpa = *this; tmpa.playMove(a);
          auto tmpb = *this; tmpb.playMove(b);
          return (_side * n.forward(tmpa.table())) < (_side * n.forward(tmpb.table()));
        });
        playMove(move);
      }
      system("clear");
      std::cout << *this;
    }
    return result();
  }

  void randomize(){
    const auto k = rand() % 8;
    for(auto i=0; (i<k) && (!over()); i++){
      const auto moves = genMoves();
      const auto size = moves.size();
      playMove(moves[rand() % size]);
    }
  }

  friend std::ostream& operator<< (std::ostream& os, const TTToe& t){
    for(auto s: squares){
      if(s == squares[3] || s == squares[6]) os << '\n';
      auto m = ((s & (t._xo[0] | t._xo[1])) == 0) ? "- " : ((s & t._xo[0]) == s) ? "x " : "o ";
      os << m;
    }
    os << '\n';
    return os;
  }
};

std::size_t roulette_pick(float sum, std::vector<float> scores) {
  float arrow = 0;
  float worra = (rand()/(float)RAND_MAX) * sum;
  for(std::size_t i=0; i<scores.size(); i++){
    arrow += scores[i];
    if(arrow >= worra){
      return i;
    }
  }
  return 0; // make compiler happy
}

std::pair<net, float> winner_f(const std::vector<net>& population, const std::vector<float>& scores){
  const auto i = std::max_element(scores.cbegin(), scores.cend());
  return std::make_pair(population[std::distance(scores.cbegin(), i)], *i);
};

static std::mutex scores_lock;

std::vector<float> fitness_f(const std::vector<net>& population, std::size_t cores){
  const auto size = population.size();
  std::vector<float> scores(size, 0.0);
  for(std::size_t i=0; i<size-1; i++){
    for(std::size_t k=i+1; k<size; k++){
      std::vector<std::future<void>> futs; futs.reserve(cores);
      for(std::size_t s=0; s<cores; s++){
        futs.push_back(std::async(std::launch::async, [i, k, &scores, &population]{
          for(auto p=0; p<25; p++){
            TTToe tmp1; tmp1.randomize();
            TTToe tmp2 = tmp1;
            const int r1 = tmp1.playOut(population[i], population[k]);
            const int r2 = tmp2.playOut(population[k], population[i]);
            std::lock_guard<std::mutex> lock(scores_lock);
            scores[i] += (2+r1-r2);
            scores[k] += (2+r2-r1);
          }
        }));
      }
    }
  }
  return scores;
};

std::vector<net> offspring_f(const std::vector<net>& population, const std::vector<float>& scores){
  const auto size = population.size();
  const auto sum = std::accumulate(scores.cbegin(), scores.cend(), 0);
  std::vector<net> new_population; new_population.reserve(size);

  for(std::size_t i = 0; i<size; i++){
    const auto t1 = population[roulette_pick(sum, scores)];
    const auto t2 = population[roulette_pick(sum, scores)];
    const auto t = t1 + t2;
    new_population.push_back(~t);
  }

  return new_population;
};


std::pair<net, float> new_winner_f(const std::pair<net, float>& winner, const std::vector<net>& population, const std::vector<float>& scores){
  const auto p_winner = winner_f(population, scores);
  TTToe tmp1;
  TTToe tmp2;
  const int r1 = tmp1.playOut(winner.first, p_winner.first);
  const int r2 = tmp2.playOut(p_winner.first, winner.first);

  if(r1 == r2)
    return p_winner.second > winner.second ? p_winner: winner;
  else
    return r2-r1 ? p_winner : winner;
};

#endif //MY
