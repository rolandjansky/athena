/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/OnlineLumiCalibrationCondData.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2018
 * @brief Luminosity calibration data, produced by OnlineLumiCalibrationCondAlg.
 */


#include "CoolLumiUtilities/OnlineLumiCalibrationCondData.h"


/**
 * @brief Store a calibration data object,
 * @param channel Calibration channel to write.
 * @param lc Calibration object to write.
 */
void OnlineLumiCalibrationCondData::set (unsigned int channel,
                                         OnlineLumiCalibrator&& lc)
{
  m_cali[channel] = std::move(lc);
}


/**
 * @brief Return muToLumi parameter.
 * @param channel Calibration channel to retrieve.
 * Returns -1 if channel doesn't exist
 */
float OnlineLumiCalibrationCondData::getMuToLumi (unsigned int channel) const
{
  if (const OnlineLumiCalibrator* lc = findLC (channel)) {
    return lc->getMuToLumi();
  }
  return -1;
}


/**
 * @brief Luminosity calibration.
 * @param channel Calibration channel.
 * @param raw Uncalibrated luminosity.
 * @parma lumi[out] Calibrated luminosity.
 *
 * Returns false on error.
 */
bool OnlineLumiCalibrationCondData::calibrateLumi (unsigned int channel,
                                                   float raw,
                                                   float& lumi) const
{
  if (const OnlineLumiCalibrator* lc = findLC (channel)) {
    return lc->calibrateLumi (raw, lumi);
  }
  return false;
}


/**
 * @brief Mu calibration.
 * @param channel Calibration channel.
 * @param raw Uncalibrated mu.
 * @parma mu[out] Calibrated mu.
 *
 * Returns false on error.
 */
bool OnlineLumiCalibrationCondData::calibrateMu (unsigned int channel,
                                                 float raw,
                                                 float& mu) const
{
  if (const OnlineLumiCalibrator* lc = findLC (channel)) {
    return lc->calibrateMu (raw, mu);
  }
  return false;
}


/**
 * @brief Look up calibration for a specific channel.
 * @param channel Calibration channel.
 * Returns nullptr if the channel was not found.
 */
const OnlineLumiCalibrator*
OnlineLumiCalibrationCondData::findLC (unsigned int channel) const
{
  auto it = m_cali.find (channel);
  if (it != m_cali.end()) {
    return &it->second;
  }
  return nullptr;
}
