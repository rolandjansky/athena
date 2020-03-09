/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file rGen.h
 * @brief interface to a random number generator, used to make sleep time
 * of Algorithms slightly variable
 */

#ifndef RGEN_H
#define RGEN_H 1

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

  unsigned int igen(int t) {
    return (unsigned int)( m_dst(m_gen) * float(t) );
  }

private:
  void init();
  static std::once_flag m_f;

  static std::default_random_engine m_gen;
  static std::uniform_real_distribution<float> m_dst;

};

  
#endif
