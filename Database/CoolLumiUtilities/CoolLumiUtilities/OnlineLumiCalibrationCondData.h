// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/OnlineLumiCalibrationCondData.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2018
 * @brief Luminosity calibration data, produced by OnlineLumiCalibrationCondAlg.
 */


#ifndef COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDDATA_H
#define COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDDATA_H


#include "CoolLumiUtilities/OnlineLumiCalibrator.h"
#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"


/**
 * @brief Luminosity calibration data, produced by OnlineLumiCalibrationCondAlg.
 */
class OnlineLumiCalibrationCondData
{
public:
  /**
   * @brief Store a calibration data object,
   * @param channel Calibration channel to write.
   * @param lc Calibration object to write.
   */
  void set (unsigned int channel, OnlineLumiCalibrator&& lc);


  /**
   * @brief Return muToLumi parameter.
   * @param channel Calibration channel to retrieve.
   * Returns -1 if channel doesn't exist
   */
  float getMuToLumi (unsigned int channel) const;


  /**
   * @brief Luminosity calibration.
   * @param channel Calibration channel.
   * @param raw Uncalibrated luminosity.
   * @parma lumi[out] Calibrated luminosity.
   *
   * Returns false on error.
   */
  bool calibrateLumi(unsigned int channel, float raw, float& lumi) const;


  /**
   * @brief Mu calibration.
   * @param channel Calibration channel.
   * @param raw Uncalibrated mu.
   * @parma mu[out] Calibrated mu.
   *
   * Returns false on error.
   */
  bool calibrateMu(unsigned int channel, float raw, float& mu) const;

  
private:
  /**
   * @brief Look up calibration for a specific channel.
   * @param channel Calibration channel.
   * Returns nullptr if the channel was not found.
   */
  const OnlineLumiCalibrator* findLC (unsigned int channel) const;


  /// Calibration storage by channel.
  std::map<unsigned int, OnlineLumiCalibrator> m_cali;
};


CLASS_DEF (OnlineLumiCalibrationCondData, 139571659, 0)
CONDCONT_DEF (OnlineLumiCalibrationCondData, 263944367);


#endif // not COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDDATA_H

