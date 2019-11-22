/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigHLTJetHypoTests_RandUInt_h
#define TrigHLTJetHypoTests_RandUInt_h

#include <random>
class RandUint {
 public:
 RandUint():m_dist(0, 1){}
 RandUint(std::size_t low, std::size_t high):m_dist(low, high){}
  std::size_t operator ()() {return m_dist(m_re);}
 private:
  std::default_random_engine m_re;
  std::uniform_int_distribution<std::size_t> m_dist;
};
#endif
