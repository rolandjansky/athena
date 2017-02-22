#ifndef RGEN_H
#define RGEN_H 1

#include <thread>
#include <random>
#include <mutex>

class rGen {
public:

  rGen() {
    std::call_once( m_f, &rGen::init, this );    
  }

  float gen() {
    return m_dst(m_gen);
  }

  int igen(int t) {
    return int( m_dst(m_gen) * float(t) );
  }

private:
  void init();
  static std::once_flag m_f;

  static std::default_random_engine m_gen;
  static std::uniform_real_distribution<float> m_dst;

};

  
#endif
