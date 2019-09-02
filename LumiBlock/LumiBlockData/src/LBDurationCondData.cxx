/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockData/src/LBDurationCondData.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Hold luminosity block duration data produced from /TRIGGER/LUMI/LBLB.
 */


#include "LumiBlockData/LBDurationCondData.h"


/** 
 * @brief Constructor.
 * @apram duration Duration of the LB.
 */
LBDurationCondData::LBDurationCondData (double duration)
  : m_LBDuration (duration)
{
}


// Luminosity block time (in seconds) from /TRIGGER/LUMI/LBLB
// Multiply by lbAverageLuminosity to get integrated luminosity in the current lumi block
double LBDurationCondData::lbDuration() const
{
  return m_LBDuration;
}
