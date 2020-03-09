/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigHLTJetHypoTests_RandDouble_h
#define TrigHLTJetHypoTests_RandDouble_h

#include <random>
class RandDouble {
 public:
 RandDouble(): m_dist(0., 1.){}
 RandDouble(double low, double high):m_dist(low, high){
    //  note the generation is not symmetric: [low, high)...
  }
  double operator ()() {return m_dist(m_re);}
 private:
  std::default_random_engine m_re;
  std::uniform_real_distribution<double> m_dist;
};
#endif
