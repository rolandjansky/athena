// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockData/TrigLiveFractionCondData.h
 * @author scott snyder <snyder@bnl.gov>, from existing TrigLivefractionTool.
 * @date Jun, 2019
 * @brief Hold live fraction data produced by TrigLiveFractionCondAlg.
 */


#ifndef LUMIBLOCKDATA_TRIGLIVEFRACTIONCONDDATA_H
#define LUMIBLOCKDATA_TRIGLIVEFRACTIONCONDDATA_H


#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>


/**
 * @brief Hold live fraction data produced by TrigLiveFractionCondAlg.
 *
 * L1 livefraction information from /TRIGGER/LUMI/PerBcidDeadtime
 * Use highPriority = false to get low priority triggers
 */
class TrigLiveFractionCondData
{
public:
  /**
   * @brief Constructor.
   * @param turnCounter Number of turns in this lumi block./
   * @param liveFractionLo Per-BCID live fraction, low-priority triggers.
   * @param liveFractionHi Per-BCID live fraction, high-priority triggers.
   * @param lumiLiveFractionLo Luminosity-averaged live fraction, low-priority triggers.
   * @param lumiLiveFractionHi Luminosity-averaged live fraction, high-priority triggers.
   */
  TrigLiveFractionCondData (unsigned int turnCounter,
                            std::vector<float>&& liveFractionLo,
                            std::vector<float>&& liveFractionHi,
                            float lumiLiveFractionLo,
                            float lumiLiveFractionHi);

  // L1 livefraction information from /TRIGGER/LUMI/PerBcidDeadtime
  // Use highPriority = false to get low priority triggers

  // Return vector with all BCIDs indexed by BCID number
  const std::vector<float>& l1LiveFractionVector (bool highPriority = true) const;

  
  // Number of turns in this lumi block (can be used to compute errors I guess)
  unsigned int lhcTurnCounter() const;

  
  // Luminosity-averaged live fraction over all physics BCIDs (high or low priority triggers)
  // Depends on luminosity from LuminosityCondAlg.
  float lbAverageLiveFraction (bool highPriority=true) const;


private:
  // Number of turns in this lumi block
  unsigned int m_turnCounter;

  // Live fraction per BCID
  std::vector<float> m_liveFractionLo;
  std::vector<float> m_liveFractionHi;

  // Luminosity-weighted live fractions
  float m_lumiLiveFractionLo;
  float m_lumiLiveFractionHi;
};


CLASS_DEF (TrigLiveFractionCondData, 143256074, 0)
CONDCONT_DEF (TrigLiveFractionCondData, 241111590);


#endif // not LUMIBLOCKDATA_TRIGLIVEFRACTIONCONDDATA_H
