/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <utility>

#include "PixelConditionsData/PixelModuleData.h"


// Digitization parameters
void PixelModuleData::setBunchSpace(double bunchSpace) { m_bunchSpace = bunchSpace; }
double PixelModuleData::getBunchSpace() const { return m_bunchSpace; }

void PixelModuleData::setBarrelNumberOfBCID(const std::vector<int> &barrelNumberOfBCID) { m_barrelNumberOfBCID = barrelNumberOfBCID; }
void PixelModuleData::setEndcapNumberOfBCID(const std::vector<int> &endcapNumberOfBCID) { m_endcapNumberOfBCID = endcapNumberOfBCID; }
void PixelModuleData::setDBMNumberOfBCID(const std::vector<int> &DBMNumberOfBCID) { m_DBMNumberOfBCID = DBMNumberOfBCID; }

int PixelModuleData::getNumberOfBCID(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelNumberOfBCID.size())
    return m_barrelNumberOfBCID[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapNumberOfBCID.size())
    return m_endcapNumberOfBCID[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_DBMNumberOfBCID.size())
    return m_DBMNumberOfBCID[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getNumberOfBCID(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setBarrelTimeOffset(const std::vector<double> &barrelTimeOffset) { m_barrelTimeOffset = barrelTimeOffset; }
void PixelModuleData::setEndcapTimeOffset(const std::vector<double> &endcapTimeOffset) { m_endcapTimeOffset = endcapTimeOffset; }
void PixelModuleData::setDBMTimeOffset(const std::vector<double> &DBMTimeOffset) { m_DBMTimeOffset = DBMTimeOffset; }

double PixelModuleData::getTimeOffset(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelTimeOffset.size())
    return m_barrelTimeOffset[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapTimeOffset.size())
    return m_endcapTimeOffset[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_DBMTimeOffset.size())
    return m_DBMTimeOffset[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getTimeOffset(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setBarrelTimeJitter(const std::vector<double> &barrelTimeJitter) { m_barrelTimeJitter = barrelTimeJitter; }
void PixelModuleData::setEndcapTimeJitter(const std::vector<double> &endcapTimeJitter) { m_endcapTimeJitter = endcapTimeJitter; }
void PixelModuleData::setDBMTimeJitter(const std::vector<double> &DBMTimeJitter) { m_DBMTimeJitter = DBMTimeJitter; }

double PixelModuleData::getTimeJitter(int barrel_ec, int layer) const
{
  // FEI3 only
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelTimeJitter.size())
    return m_barrelTimeJitter[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapTimeJitter.size())
    return m_endcapTimeJitter[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_DBMTimeJitter.size())
    return m_DBMTimeJitter[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getTimeJitter(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setDefaultBarrelAnalogThreshold(const std::vector<int> &barrelAnalogThreshold) { m_defaultBarrelAnalogThreshold = barrelAnalogThreshold; }
void PixelModuleData::setDefaultEndcapAnalogThreshold(const std::vector<int> &endcapAnalogThreshold) { m_defaultEndcapAnalogThreshold = endcapAnalogThreshold; }
void PixelModuleData::setDefaultDBMAnalogThreshold(const std::vector<int> &DBMAnalogThreshold) { m_defaultDBMAnalogThreshold = DBMAnalogThreshold; }

int PixelModuleData::getDefaultAnalogThreshold(int barrel_ec, int layer) const
{
  // only for charge calibration cond algs
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_defaultBarrelAnalogThreshold.size())
    return m_defaultBarrelAnalogThreshold[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_defaultEndcapAnalogThreshold.size())
    return m_defaultEndcapAnalogThreshold[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_defaultDBMAnalogThreshold.size())
    return m_defaultDBMAnalogThreshold[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getDefaultAnalogThreshold(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setDefaultBarrelAnalogThresholdSigma(const std::vector<int> &barrelAnalogThresholdSigma) { m_defaultBarrelAnalogThresholdSigma = barrelAnalogThresholdSigma; }
void PixelModuleData::setDefaultEndcapAnalogThresholdSigma(const std::vector<int> &endcapAnalogThresholdSigma) { m_defaultEndcapAnalogThresholdSigma = endcapAnalogThresholdSigma; }
void PixelModuleData::setDefaultDBMAnalogThresholdSigma(const std::vector<int> &DBMAnalogThresholdSigma) { m_defaultDBMAnalogThresholdSigma = DBMAnalogThresholdSigma; }

int PixelModuleData::getDefaultAnalogThresholdSigma(int barrel_ec, int layer) const
{
  // only for charge calibration cond algs
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_defaultBarrelAnalogThresholdSigma.size())
    return m_defaultBarrelAnalogThresholdSigma[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_defaultEndcapAnalogThresholdSigma.size())
    return m_defaultEndcapAnalogThresholdSigma[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_defaultDBMAnalogThresholdSigma.size())
    return m_defaultDBMAnalogThresholdSigma[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getDefaultAnalogThresholdSigma(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setDefaultBarrelAnalogThresholdNoise(const std::vector<int> &barrelAnalogThresholdNoise) { m_defaultBarrelAnalogThresholdNoise = barrelAnalogThresholdNoise; }
void PixelModuleData::setDefaultEndcapAnalogThresholdNoise(const std::vector<int> &endcapAnalogThresholdNoise) { m_defaultEndcapAnalogThresholdNoise = endcapAnalogThresholdNoise; }
void PixelModuleData::setDefaultDBMAnalogThresholdNoise(const std::vector<int> &DBMAnalogThresholdNoise) { m_defaultDBMAnalogThresholdNoise = DBMAnalogThresholdNoise; }

int PixelModuleData::getDefaultAnalogThresholdNoise(int barrel_ec, int layer) const
{
  // only for charge calibration cond algs
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_defaultBarrelAnalogThresholdNoise.size())
    return m_defaultBarrelAnalogThresholdNoise[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_defaultEndcapAnalogThresholdNoise.size())
    return m_defaultEndcapAnalogThresholdNoise[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_defaultDBMAnalogThresholdNoise.size())
    return m_defaultDBMAnalogThresholdNoise[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getDefaultAnalogThresholdNoise(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setDefaultBarrelInTimeThreshold(const std::vector<int> &barrelInTimeThreshold) { m_defaultBarrelInTimeThreshold = barrelInTimeThreshold; }
void PixelModuleData::setDefaultEndcapInTimeThreshold(const std::vector<int> &endcapInTimeThreshold) { m_defaultEndcapInTimeThreshold = endcapInTimeThreshold; }
void PixelModuleData::setDefaultDBMInTimeThreshold(const std::vector<int> &DBMInTimeThreshold) { m_defaultDBMInTimeThreshold = DBMInTimeThreshold; }

int PixelModuleData::getDefaultInTimeThreshold(int barrel_ec, int layer) const
{
  // only for charge calibration cond algs
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_defaultBarrelInTimeThreshold.size())
    return m_defaultBarrelInTimeThreshold[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_defaultEndcapInTimeThreshold.size())
    return m_defaultEndcapInTimeThreshold[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_defaultDBMInTimeThreshold.size())
    return m_defaultDBMInTimeThreshold[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getDefaultInTimeThreshold(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setBarrelToTThreshold(const std::vector<int> &barrelToTThreshold) { m_barrelToTThreshold = barrelToTThreshold; }
void PixelModuleData::setEndcapToTThreshold(const std::vector<int> &endcapToTThreshold) { m_endcapToTThreshold = endcapToTThreshold; }
void PixelModuleData::setDBMToTThreshold(const std::vector<int> &DBMToTThreshold) { m_DBMToTThreshold = DBMToTThreshold; }

int PixelModuleData::getToTThreshold(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelToTThreshold.size())
    return m_barrelToTThreshold[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapToTThreshold.size())
    return m_endcapToTThreshold[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_DBMToTThreshold.size())
    return m_DBMToTThreshold[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getToTThreshold(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setBarrelCrossTalk(const std::vector<double> &barrelCrossTalk) { m_barrelCrossTalk = barrelCrossTalk; }
void PixelModuleData::setEndcapCrossTalk(const std::vector<double> &endcapCrossTalk) { m_endcapCrossTalk = endcapCrossTalk; }
void PixelModuleData::setDBMCrossTalk(const std::vector<double> &DBMCrossTalk) { m_DBMCrossTalk = DBMCrossTalk; }

double PixelModuleData::getCrossTalk(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelCrossTalk.size())
    return m_barrelCrossTalk[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapCrossTalk.size())
    return m_endcapCrossTalk[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_DBMCrossTalk.size())
    return m_DBMCrossTalk[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getCrossTalk(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setBarrelThermalNoise(const std::vector<double> &barrelThermalNoise) { m_barrelThermalNoise = barrelThermalNoise; }
void PixelModuleData::setEndcapThermalNoise(const std::vector<double> &endcapThermalNoise) { m_endcapThermalNoise = endcapThermalNoise; }
void PixelModuleData::setDBMThermalNoise(const std::vector<double> &DBMThermalNoise) { m_DBMThermalNoise = DBMThermalNoise; }

double PixelModuleData::getThermalNoise(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelThermalNoise.size())
    return m_barrelThermalNoise[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapThermalNoise.size())
    return m_endcapThermalNoise[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_DBMThermalNoise.size())
    return m_DBMThermalNoise[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getThermalNoise(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setBarrelNoiseOccupancy(const std::vector<double> &barrelNoiseOccupancy) { m_barrelNoiseOccupancy = barrelNoiseOccupancy; }
void PixelModuleData::setEndcapNoiseOccupancy(const std::vector<double> &endcapNoiseOccupancy) { m_endcapNoiseOccupancy = endcapNoiseOccupancy; }
void PixelModuleData::setDBMNoiseOccupancy(const std::vector<double> &DBMNoiseOccupancy) { m_DBMNoiseOccupancy = DBMNoiseOccupancy; }

double PixelModuleData::getNoiseOccupancy(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelNoiseOccupancy.size())
    return m_barrelNoiseOccupancy[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapNoiseOccupancy.size())
    return m_endcapNoiseOccupancy[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_DBMNoiseOccupancy.size())
    return m_DBMNoiseOccupancy[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getNoiseOccupancy(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setBarrelDisableProbability(const std::vector<double> &barrelDisableProbability) { m_barrelDisableProbability = barrelDisableProbability; }
void PixelModuleData::setEndcapDisableProbability(const std::vector<double> &endcapDisableProbability) { m_endcapDisableProbability = endcapDisableProbability; }
void PixelModuleData::setDBMDisableProbability(const std::vector<double> &DBMDisableProbability) { m_DBMDisableProbability = DBMDisableProbability; }

double PixelModuleData::getDisableProbability(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelDisableProbability.size())
    return m_barrelDisableProbability[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapDisableProbability.size())
    return m_endcapDisableProbability[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_DBMDisableProbability.size())
    return m_DBMDisableProbability[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getDisableProbability(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setBarrelNoiseShape(const std::vector<std::vector<float>> &barrelNoiseShape) { m_barrelNoiseShape = barrelNoiseShape; }
void PixelModuleData::setEndcapNoiseShape(const std::vector<std::vector<float>> &endcapNoiseShape) { m_endcapNoiseShape = endcapNoiseShape; }
void PixelModuleData::setDBMNoiseShape(const std::vector<std::vector<float>> &DBMNoiseShape) { m_DBMNoiseShape = DBMNoiseShape; }

const std::vector<float> &PixelModuleData::getNoiseShape(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelNoiseShape.size())
    return m_barrelNoiseShape[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapNoiseShape.size())
    return m_endcapNoiseShape[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_DBMNoiseShape.size())
    return m_DBMNoiseShape[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getNoiseShape(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setFEI3BarrelLatency(const std::vector<int> &FEI3BarrelLatency) { m_FEI3BarrelLatency = FEI3BarrelLatency; }
void PixelModuleData::setFEI3EndcapLatency(const std::vector<int> &FEI3EndcapLatency) { m_FEI3EndcapLatency = FEI3EndcapLatency; }

int PixelModuleData::getFEI3Latency(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_FEI3BarrelLatency.size())
    return m_FEI3BarrelLatency[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_FEI3EndcapLatency.size())
    return m_FEI3EndcapLatency[layerIndex];

  throw std::range_error("PixelModuleData::getFEI3Latency array out of bounds");
}

void PixelModuleData::setFEI3BarrelHitDuplication(const std::vector<bool> &FEI3BarrelHitDuplication) { m_FEI3BarrelHitDuplication = FEI3BarrelHitDuplication; }
void PixelModuleData::setFEI3EndcapHitDuplication(const std::vector<bool> &FEI3EndcapHitDuplication) { m_FEI3EndcapHitDuplication = FEI3EndcapHitDuplication; }

bool PixelModuleData::getFEI3HitDuplication(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_FEI3BarrelHitDuplication.size())
    return m_FEI3BarrelHitDuplication[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_FEI3EndcapHitDuplication.size())
    return m_FEI3EndcapHitDuplication[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getFEI3HitDuplication(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setFEI3BarrelSmallHitToT(const std::vector<int> &FEI3BarrelSmallHitToT) { m_FEI3BarrelSmallHitToT = FEI3BarrelSmallHitToT; }
void PixelModuleData::setFEI3EndcapSmallHitToT(const std::vector<int> &FEI3EndcapSmallHitToT) { m_FEI3EndcapSmallHitToT = FEI3EndcapSmallHitToT; }

int PixelModuleData::getFEI3SmallHitToT(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_FEI3BarrelSmallHitToT.size())
    return m_FEI3BarrelSmallHitToT[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_FEI3EndcapSmallHitToT.size())
    return m_FEI3EndcapSmallHitToT[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getFEI3SmallHitToT(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setFEI3BarrelTimingSimTune(const std::vector<int> &FEI3BarrelTimingSimTune) { m_FEI3BarrelTimingSimTune = FEI3BarrelTimingSimTune; }
void PixelModuleData::setFEI3EndcapTimingSimTune(const std::vector<int> &FEI3EndcapTimingSimTune) { m_FEI3EndcapTimingSimTune = FEI3EndcapTimingSimTune; }

int PixelModuleData::getFEI3TimingSimTune(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_FEI3BarrelTimingSimTune.size())
    return m_FEI3BarrelTimingSimTune[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_FEI3EndcapTimingSimTune.size())
    return m_FEI3EndcapTimingSimTune[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getFEI3TimingSimTune(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setFEI4BarrelHitDiscConfig(const std::vector<int> &FEI4BarrelHitDiscConfig) { m_FEI4BarrelHitDiscConfig = FEI4BarrelHitDiscConfig; }
void PixelModuleData::setFEI4EndcapHitDiscConfig(const std::vector<int> &FEI4EndcapHitDiscConfig) { m_FEI4EndcapHitDiscConfig = FEI4EndcapHitDiscConfig; }

int PixelModuleData::getFEI4HitDiscConfig(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_FEI4BarrelHitDiscConfig.size())
    return m_FEI4BarrelHitDiscConfig[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_FEI4EndcapHitDiscConfig.size())
    return m_FEI4EndcapHitDiscConfig[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_FEI4EndcapHitDiscConfig.size())
    return m_FEI4EndcapHitDiscConfig[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getFEI4HitDiscConfig(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

int PixelModuleData::getFEI4OverflowToT(int barrel_ec, int layer) const
{
  // also used in reconstruction
  static const int overflow[] = {14,15,16,14,16};
  unsigned int idx = getFEI4HitDiscConfig(barrel_ec, layer) < 4 ? getFEI4HitDiscConfig(barrel_ec, layer) : 4;
  return overflow[idx];
}

void PixelModuleData::setFEI4ChargScaling(float scaleFEI4) { m_scaleFEI4 = scaleFEI4; }
void PixelModuleData::setUseFEI4SpecialScalingFunction(bool useFEI4SpecialScalingFunction) { m_useFEI4SpecialScalingFunction = useFEI4SpecialScalingFunction; }
float PixelModuleData::getFEI4ChargScaling() const { return m_scaleFEI4; }
bool PixelModuleData::getUseFEI4SpecialScalingFunction() const { return m_useFEI4SpecialScalingFunction; }

void PixelModuleData::setFEI4ToTSigma(const std::vector<double> &FEI4ToTSigma) { m_FEI4ToTSigma = FEI4ToTSigma; }
double PixelModuleData::getFEI4ToTSigma(int tot) const
{
  size_t totIndex = static_cast<size_t>(tot);
  if (totIndex < m_FEI4ToTSigma.size())
    return m_FEI4ToTSigma[totIndex];

  std::stringstream error;
  error << "PixelModuleData::getFEI4ToTSigma(" << tot << "): array out of bounds";
  throw std::range_error(error.str());
}

// Charge calibration parameters
void PixelModuleData::setDefaultQ2TotA(float paramA) { m_paramA = paramA; }
void PixelModuleData::setDefaultQ2TotE(float paramE) { m_paramE = paramE; }
void PixelModuleData::setDefaultQ2TotC(float paramC) { m_paramC = paramC; }
float PixelModuleData::getDefaultQ2TotA() const { return m_paramA; }
float PixelModuleData::getDefaultQ2TotE() const { return m_paramE; }
float PixelModuleData::getDefaultQ2TotC() const { return m_paramC; }

// Lorentz angle correction
void PixelModuleData::setBarrelLorentzAngleCorr(const std::vector<double> &barrelLorentzAngleCorr) { m_barrelLorentzAngleCorr = barrelLorentzAngleCorr; }
void PixelModuleData::setEndcapLorentzAngleCorr(const std::vector<double> &endcapLorentzAngleCorr) { m_endcapLorentzAngleCorr = endcapLorentzAngleCorr; }

double PixelModuleData::getLorentzAngleCorr(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelLorentzAngleCorr.size())
    return m_barrelLorentzAngleCorr[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapLorentzAngleCorr.size())
    return m_endcapLorentzAngleCorr[layerIndex];

  if (std::abs(barrel_ec) == 4) // DBM has no correction applied
    return 1.0;

  std::stringstream error;
  error << "PixelModuleData::getLorentzAngleCorr(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

// DCS parameters
void PixelModuleData::setDefaultBiasVoltage(float biasVoltage) { m_biasVoltage = biasVoltage; }
float PixelModuleData::getDefaultBiasVoltage() const { return m_biasVoltage; }

void PixelModuleData::setDefaultBarrelBiasVoltage(const std::vector<float> &barrelBiasVoltage) { m_barrelBiasVoltage = barrelBiasVoltage; }
void PixelModuleData::setDefaultEndcapBiasVoltage(const std::vector<float> &endcapBiasVoltage) { m_endcapBiasVoltage = endcapBiasVoltage; }
void PixelModuleData::setDefaultDBMBiasVoltage(const std::vector<float> &DBMBiasVoltage) { m_DBMBiasVoltage = DBMBiasVoltage; }

float PixelModuleData::getDefaultBiasVoltage(int barrel_ec, int layer) const
{
  size_t layerIndex = static_cast<size_t>(layer);

  if (barrel_ec == 0 && layerIndex < m_barrelBiasVoltage.size())
    return m_barrelBiasVoltage[layerIndex];

  if (std::abs(barrel_ec) == 2 && layerIndex < m_endcapBiasVoltage.size())
    return m_endcapBiasVoltage[layerIndex];

  if (std::abs(barrel_ec) == 4 && layerIndex < m_DBMBiasVoltage.size())
    return m_DBMBiasVoltage[layerIndex];

  std::stringstream error;
  error << "PixelModuleData::getDefaultBiasVoltage(" << barrel_ec << ", " << layer << "): array out of bounds";
  throw std::range_error(error.str());
}

void PixelModuleData::setDefaultTemperature(float temperature) { m_temperature = temperature; }
float PixelModuleData::getDefaultTemperature() const { return m_temperature; }

// Radiation damage fluence maps
void PixelModuleData::setFluenceLayer(const std::vector<double> &fluenceLayer) { m_fluenceLayer = fluenceLayer; }
const std::vector<double> &PixelModuleData::getFluenceLayer() const { return m_fluenceLayer; }

void PixelModuleData::setRadSimFluenceMapList(const std::vector<std::string> &radSimFluenceMapList) { m_radSimFluenceMapList = radSimFluenceMapList; }
const std::vector<std::string> &PixelModuleData::getRadSimFluenceMapList() const { return m_radSimFluenceMapList; }

void PixelModuleData::setFluenceLayer3D(const std::vector<double> &fluenceLayer) { m_fluenceLayer3D = fluenceLayer; }
const std::vector<double> &PixelModuleData::getFluenceLayer3D() const { return m_fluenceLayer3D; }

void PixelModuleData::setRadSimFluenceMapList3D(const std::vector<std::string> &radSimFluenceMapList3D) { m_radSimFluenceMapList3D = radSimFluenceMapList3D; }
const std::vector<std::string> &PixelModuleData::getRadSimFluenceMapList3D() const { return m_radSimFluenceMapList3D; }

// Cabling parameters
void PixelModuleData::setCablingMapToFile(bool cablingMapToFile) { m_cablingMapToFile = cablingMapToFile; }
bool PixelModuleData::getCablingMapToFile() const { return m_cablingMapToFile; }

void PixelModuleData::setCablingMapFileName(const std::string &cablingMapFileName) { m_cablingMapFileName = cablingMapFileName; }
const std::string &PixelModuleData::getCablingMapFileName() const { return m_cablingMapFileName; }

// Distortion parameters
void PixelModuleData::setDistortionInputSource(int distortionInputSource) { m_distortionInputSource = distortionInputSource; }
int PixelModuleData::getDistortionInputSource() const { return m_distortionInputSource; }

void PixelModuleData::setDistortionVersion(int distortionVersion) { m_distortionVersion = distortionVersion; }
int PixelModuleData::getDistortionVersion() const { return m_distortionVersion; }

void PixelModuleData::setDistortionR1(double distortionR1) { m_distortionR1 = distortionR1; }
double PixelModuleData::getDistortionR1() const { return m_distortionR1; }

void PixelModuleData::setDistortionR2(double distortionR2) { m_distortionR2 = distortionR2; }
double PixelModuleData::getDistortionR2() const { return m_distortionR2; }

void PixelModuleData::setDistortionTwist(double distortionTwist) { m_distortionTwist = distortionTwist; }
double PixelModuleData::getDistortionTwist() const { return m_distortionTwist; }

void PixelModuleData::setDistortionMeanR(double distortionMeanR) { m_distortionMeanR = distortionMeanR; }
double PixelModuleData::getDistortionMeanR() const { return m_distortionMeanR; }

void PixelModuleData::setDistortionRMSR(double distortionRMSR) { m_distortionRMSR = distortionRMSR; }
double PixelModuleData::getDistortionRMSR() const { return m_distortionRMSR; }

void PixelModuleData::setDistortionMeanTwist(double distortionMeanTwist) { m_distortionMeanTwist = distortionMeanTwist; }
double PixelModuleData::getDistortionMeanTwist() const { return m_distortionMeanTwist; }

void PixelModuleData::setDistortionRMSTwist(double distortionRMSTwist) { m_distortionRMSTwist = distortionRMSTwist; }
double PixelModuleData::getDistortionRMSTwist() const { return m_distortionRMSTwist; }

void PixelModuleData::setDistortionWriteToFile(bool distortionWriteToFile) { m_distortionWriteToFile = distortionWriteToFile; }
bool PixelModuleData::getDistortionWriteToFile() const { return m_distortionWriteToFile; }

void PixelModuleData::setDistortionFileName(const std::string &distortionFileName) { m_distortionFileName = distortionFileName; }
const std::string &PixelModuleData::getDistortionFileName() const { return m_distortionFileName; }
