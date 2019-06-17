// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockData/LBDurationCondData.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Hold luminosity block duration data produced from /TRIGGER/LUMI/LBLB.
 */


#ifndef LUMIBLOCKDATA_LBDURATIONCONDDATA_H
#define LUMIBLOCKDATA_LBDURATIONCONDDATA_H


#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"


/**
 * @brief Hold luminosity block duration data produced from /TRIGGER/LUMI/LBLB.
 */
class LBDurationCondData
{
public:
  /** 
   * @brief Constructor.
   * @apram duration Duration of the LB.
   */
  LBDurationCondData (double duration);


  // Luminosity block time (in seconds) from /TRIGGER/LUMI/LBLB
  // Multiply by lbAverageLuminosity to get integrated luminosity in the current lumi block
  double lbDuration() const; // Luminosity block time (in seconds)


private:
  // Lumi block length from /TRIGGER/LUMI/LBLB, in seconds.
  double m_LBDuration;
};


CLASS_DEF (LBDurationCondData, 184676788, 0)
CONDCONT_DEF (LBDurationCondData, 62600156);


#endif // not LUMIBLOCKDATA_LBDURATIONCONDDATA_H
