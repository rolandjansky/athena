// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARNOISYROSUMMARY_p5_H
#define LARNOISYROSUMMARY_p5_H

#include <vector>
#include <utility>
#include <stdint.h>

class LArNoisyROSummary_p5
{
 public:
  friend class LArNoisyROSummaryCnv_p5;

  LArNoisyROSummary_p5():
    m_noisy_febs(),
    m_MNBTight_febs(),
    m_MNBLoose_febs(),
    m_noisy_preamps(), 
    m_BadFEBFlaggedPartitions(0),
    m_BadFEB_WFlaggedPartitions(0),
    m_SatMediumFlaggedPartitions(0),
    m_SatTightFlaggedPartitions(0),
    m_MNBLooseFlaggedPartitions(0),
    m_MNBTightFlaggedPartitions(0),
    m_MNBTightFlaggedPartitions_PsVeto(0)
  {};

 private:
  std::vector<unsigned int> m_noisy_febs;
  std::vector<unsigned int> m_MNBTight_febs;
  std::vector<unsigned int> m_MNBLoose_febs;
  std::vector< std::pair<unsigned int, std::vector<int> > > m_noisy_preamps;
  uint8_t m_BadFEBFlaggedPartitions;
  uint8_t m_BadFEB_WFlaggedPartitions;
  uint8_t m_SatMediumFlaggedPartitions;
  uint8_t m_SatTightFlaggedPartitions;
  uint8_t m_MNBLooseFlaggedPartitions;
  uint8_t m_MNBTightFlaggedPartitions;
  uint8_t m_MNBTightFlaggedPartitions_PsVeto;
  
};

#endif
