// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CoolLumiUtilities/OnlineLumiCalibrationCondData.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2018
 * @brief 
 */


#ifndef COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDDATA_H
#define COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDDATA_H


#include "CoolLumiUtilities/OnlineLumiCalibrator.h"
#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"


class OnlineLumiCalibrationCondData
{
public:
  void set (unsigned int channel, OnlineLumiCalibrator&& lc)
  {
    m_cali[channel] = std::move(lc);
  }


  // Returns -1 if channel doesn't exist
  float getMuToLumi(unsigned int channel) const
  {
    if (const OnlineLumiCalibrator* lc = findLC (channel)) {
      return lc->getMuToLumi();
    }
    return -1;
  }

  
  // Return false on error
  bool calibrateLumi(unsigned int channel, float raw, float& lumi) const
  {
    if (const OnlineLumiCalibrator* lc = findLC (channel)) {
      return lc->calibrateLumi (raw, lumi);
    }
    return false;
  }
  bool calibrateMu(unsigned int channel, float raw, float& mu) const
  {
    if (const OnlineLumiCalibrator* lc = findLC (channel)) {
      return lc->calibrateMu (raw, mu);
    }
    return false;
  }

  
private:
  const OnlineLumiCalibrator* findLC (unsigned int channel) const
  {
    auto it = m_cali.find (channel);
    if (it != m_cali.end()) {
      return &it->second;
    }
    return nullptr;
  }
  std::map<unsigned int, OnlineLumiCalibrator> m_cali;
};


CLASS_DEF (OnlineLumiCalibrationCondData, 139571659, 0)
CONDCONT_DEF (OnlineLumiCalibrationCondData, 263944367);


#endif // not COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDDATA_H

