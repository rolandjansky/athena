/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockData/src/TrigLiveFractionCondData.cxx
 * @author scott snyder <snyder@bnl.gov>, from existing TrigLivefractionTool.
 * @date Jun, 2019
 * @brief Hold live fraction data produced by TrigLiveFractionCondAlg.
 */


#include "LumiBlockData/TrigLiveFractionCondData.h"


/**
 * @brief Constructor.
 * @param turnCounter Number of turns in this lumi block./
 * @param liveFractionLo Per-BCID live fraction, low-priority triggers.
 * @param liveFractionHi Per-BCID live fraction, high-priority triggers.
 * @param lumiLiveFractionLo Luminosity-averaged live fraction, low-priority triggers.
 * @param lumiLiveFractionHi Luminosity-averaged live fraction, high-priority triggers.
 */
TrigLiveFractionCondData::TrigLiveFractionCondData (unsigned int turnCounter,
                                                    std::vector<float>&& liveFractionLo,
                                                    std::vector<float>&& liveFractionHi,
                                                    float lumiLiveFractionLo,
                                                    float lumiLiveFractionHi)
  : m_turnCounter (turnCounter),
    m_liveFractionLo (std::move (liveFractionLo)),
    m_liveFractionHi (std::move (liveFractionHi)),
    m_lumiLiveFractionLo (lumiLiveFractionLo),
    m_lumiLiveFractionHi (lumiLiveFractionHi)
{
}


/**
 * @brief Return vector with all BCIDs indexed by BCID number.
 * @param highPriority High- or low-priority triggers.
 */
const std::vector<float>&
TrigLiveFractionCondData::l1LiveFractionVector (bool highPriority /*= true*/) const
{
  if (highPriority) {
    return m_liveFractionHi;
  }
  return m_liveFractionLo;
}



/**
 * @brief Number of turns in this lumi block (can be used to compute errors I guess).
 */
unsigned int TrigLiveFractionCondData::lhcTurnCounter() const
{
  return m_turnCounter;
}


/** 
 * @brief Luminosity-averaged live fraction over all physics BCIDs.
 * @param highPriority High- or low-priority triggers.
 */
float
TrigLiveFractionCondData::lbAverageLiveFraction (bool highPriority /*=true*/) const
{
  if (highPriority) {
    return m_lumiLiveFractionHi;
  }
  return m_lumiLiveFractionLo;
}
