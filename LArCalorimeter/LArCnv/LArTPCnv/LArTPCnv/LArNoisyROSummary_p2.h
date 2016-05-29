// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARNOISYROSUMMARY_P2_H
#define LARNOISYROSUMMARY_P2_H

#include <vector>
#include <utility>
#include <stdint.h>

class LArNoisyROSummary_p2
{
 public:
  friend class LArNoisyROSummaryCnv_p2;

  LArNoisyROSummary_p2(): m_noisy_febs(), m_noisy_preamps(), 
    m_BadFEBFlaggedPartitions(0), m_SatMediumFlaggedPartitions(0),
    m_SatTightFlaggedPartitions(0) {};

  //private:
  std::vector<unsigned int> m_noisy_febs;
  std::vector< std::pair<unsigned int, std::vector<int> > > m_noisy_preamps;
  uint8_t m_BadFEBFlaggedPartitions;
  uint8_t m_SatMediumFlaggedPartitions;
  uint8_t m_SatTightFlaggedPartitions;
  
};

#endif
