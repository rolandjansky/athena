/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARNOISYROSUMMARY_P1_H
#define LARNOISYROSUMMARY_P1_H

#include <vector>
#include <utility>

class LArNoisyROSummary_p1
{
 public:
  friend class LArNoisyROSummaryCnv_p1;

  LArNoisyROSummary_p1(): m_noisy_febs(), m_noisy_preamps() {};

  //private:
  std::vector<unsigned int> m_noisy_febs;
  std::vector< std::pair<unsigned int, std::vector<int> > > m_noisy_preamps;
  
};

#endif
