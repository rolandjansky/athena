/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelChargeCalibCondData.h"
#include <cfloat>

#include <cfloat>

PixelChargeCalibCondData::PixelChargeCalibCondData()
{
  const std::array<InDetDD::PixelDiodeType, 4> types
    = {InDetDD::PixelDiodeType::NORMAL, InDetDD::PixelDiodeType::LONG, InDetDD::PixelDiodeType::GANGED, InDetDD::PixelDiodeType::LARGE};

  for (InDetDD::PixelDiodeType type : types) {
    m_analogThreshold.emplace(type, chipThreshold());
    m_analogThresholdSigma.emplace(type, chipThreshold());
    m_analogThresholdNoise.emplace(type, chipThreshold());
    m_inTimeThreshold.emplace(type, chipThreshold());

    m_totA.emplace(type, chipCharge());
    m_totE.emplace(type, chipCharge());
    m_totC.emplace(type, chipCharge());
  }
}

void PixelChargeCalibCondData::setAnalogThreshold(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<int> value)
{
  m_analogThreshold.at(type)[moduleHash] = std::move(value);
}

void PixelChargeCalibCondData::setAnalogThresholdSigma(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<int> value)
{
  m_analogThresholdSigma.at(type)[moduleHash] = std::move(value);
}

void PixelChargeCalibCondData::setAnalogThresholdNoise(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<int> value)
{
  m_analogThresholdNoise.at(type)[moduleHash] = std::move(value);
}

void PixelChargeCalibCondData::setInTimeThreshold(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<int> value)
{
  m_inTimeThreshold.at(type)[moduleHash] = std::move(value);
}

void PixelChargeCalibCondData::setQ2TotA(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<float> value)
{
  m_totA.at(type)[moduleHash] = std::move(value);
}

void PixelChargeCalibCondData::setQ2TotE(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<float> value)
{
  m_totE.at(type)[moduleHash] = std::move(value);
}

void PixelChargeCalibCondData::setQ2TotC(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<float> value)
{
  m_totC.at(type)[moduleHash] = std::move(value);
}

void PixelChargeCalibCondData::setTotRes1(unsigned int moduleHash, std::vector<float> value) {
  m_totRes1[moduleHash] = std::move(value);
}

void PixelChargeCalibCondData::setTotRes2(unsigned int moduleHash, std::vector<float> value) {
  m_totRes2[moduleHash] = std::move(value);
}

int PixelChargeCalibCondData::getAnalogThreshold(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const
{
  const chipThreshold &typeMap = m_analogThreshold.at(type);
  auto it = typeMap.find(moduleHash);
  if (it != typeMap.end() && FE < it->second.size()) {
    return it->second[FE];
  }

  std::stringstream error;
  error << "PixelChargeCalibCondData::getAnalogThreshold(" << static_cast<int>(type) << ", " << moduleHash << ", " << FE << "): array out of bounds";
  throw std::range_error(error.str());
}

int PixelChargeCalibCondData::getAnalogThresholdSigma(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const
{
  const chipThreshold &typeMap = m_analogThresholdSigma.at(type);
  auto it = typeMap.find(moduleHash);
  if (it != typeMap.end() && FE < it->second.size()) {
    return it->second[FE];
  }

  std::stringstream error;
  error << "PixelChargeCalibCondData::getAnalogThresholdSigma(" << static_cast<int>(type) << ", " << moduleHash << ", " << FE << "): array out of bounds";
  throw std::range_error(error.str());
}

int PixelChargeCalibCondData::getAnalogThresholdNoise(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const
{
  const chipThreshold &typeMap = m_analogThresholdNoise.at(type);
  auto it = typeMap.find(moduleHash);
  if (it != typeMap.end() && FE < it->second.size()) {
    return it->second[FE];
  }

  std::stringstream error;
  error << "PixelChargeCalibCondData::getAnalogThresholdNoise(" << static_cast<int>(type) << ", " << moduleHash << ", " << FE << "): array out of bounds";
  throw std::range_error(error.str());
}

int PixelChargeCalibCondData::getInTimeThreshold(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const
{
  const chipThreshold &typeMap = m_inTimeThreshold.at(type);
  auto it = typeMap.find(moduleHash);
  if (it != typeMap.end() && FE < it->second.size()) {
    return it->second[FE];
  }

  std::stringstream error;
  error << "PixelChargeCalibCondData::getInTimeThreshold(" << static_cast<int>(type) << ", " << moduleHash << ", " << FE << "): array out of bounds";
  throw std::range_error(error.str());
}

float PixelChargeCalibCondData::getQ2TotA(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const
{
  const chipCharge &typeMap = m_totA.at(type);
  auto it = typeMap.find(moduleHash);
  if (it != typeMap.end() && FE < it->second.size()) {
    return it->second[FE];
  }

  std::stringstream error;
  error << "PixelChargeCalibCondData::getQ2TotA(" << static_cast<int>(type) << ", " << moduleHash << ", " << FE << "): array out of bounds";
  throw std::range_error(error.str());
}

float PixelChargeCalibCondData::getQ2TotE(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const
{
  const chipCharge &typeMap = m_totE.at(type);
  auto it = typeMap.find(moduleHash);
  if (it != typeMap.end() && FE < it->second.size()) {
    return it->second[FE];
  }

  std::stringstream error;
  error << "PixelChargeCalibCondData::getQ2TotE(" << static_cast<int>(type) << ", " << moduleHash << ", " << FE << "): array out of bounds";
  throw std::range_error(error.str());
}

float PixelChargeCalibCondData::getQ2TotC(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const
{
  const chipCharge &typeMap = m_totC.at(type);
  auto it = typeMap.find(moduleHash);
  if (it != typeMap.end() && FE < it->second.size()) {
    return it->second[FE];
  }

  std::stringstream error;
  error << "PixelChargeCalibCondData::getQ2TotC(" << static_cast<int>(type) << ", " << moduleHash << ", " << FE << "): array out of bounds";
  throw std::range_error(error.str());
}

float PixelChargeCalibCondData::getTotRes(unsigned int moduleHash, unsigned int FE, float Q) const
{
  float res1{};
  auto it = m_totRes1.find(moduleHash);
  if (it != m_totRes1.end() && FE < it->second.size()) {
    res1 = it->second[FE];
  } else {
    std::stringstream error;
    error << "PixelChargeCalibCondData::getTotRes(" << moduleHash << ", " << FE << "): res1 array out of bounds";
    throw std::range_error(error.str());
  }

  float res2{};
  it = m_totRes2.find(moduleHash);
  if (it != m_totRes2.end() && FE < it->second.size()) {
    res2 = it->second.at(FE);
  } else {
    std::stringstream error;
    error << "PixelChargeCalibCondData::getTotRes(" << moduleHash << ", " << FE << "): res2 array out of bounds";
    throw std::range_error(error.str());
  }

  return res1 + res2 * Q;
}

float PixelChargeCalibCondData::getToT(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE, float Q) const
{
  if (getCalibrationStrategy(moduleHash) == CalibrationStrategy::LUTFEI4) {
    return getToTLUTFEI4(moduleHash, FE, Q);
  }

  float paramA = getQ2TotA(type, moduleHash, FE);
  float paramE = getQ2TotE(type, moduleHash, FE);
  float paramC = getQ2TotC(type, moduleHash, FE);
  float tot = 0.0;
  if (paramC + Q != 0.0) {
    tot = paramA * (paramE + Q) / (paramC + Q);
  }
  return tot;
}

float PixelChargeCalibCondData::getCharge(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE, float ToT) const
{
  if (getCalibrationStrategy(moduleHash) == CalibrationStrategy::LUTFEI4) {
    return getChargeLUTFEI4(moduleHash, FE, ToT);
  }

  float paramA = getQ2TotA(type, moduleHash, FE);
  float paramE = getQ2TotE(type, moduleHash, FE);
  float paramC = getQ2TotC(type, moduleHash, FE);
  float charge = 0.0;
  if (std::fabs(paramA) > 0.0 && std::fabs(ToT / paramA - 1.0) > 0.0) {
    charge = (paramC * ToT / paramA - paramE) / (1.0 - ToT / paramA);
  }
  return charge;
}

void PixelChargeCalibCondData::setCalibrationStrategy(unsigned int moduleHash, CalibrationStrategy strategy)
{
  m_calibrationStrategy[moduleHash] = strategy;
}

PixelChargeCalibCondData::CalibrationStrategy PixelChargeCalibCondData::getCalibrationStrategy(unsigned int moduleHash) const
{
  auto itr = m_calibrationStrategy.find(moduleHash);
  if (itr != m_calibrationStrategy.end()) {
    return itr->second;
  }
  return CalibrationStrategy::RUN1PIX;
}

void PixelChargeCalibCondData::setTot2Charges(unsigned int moduleHash, IBLModule charges)
{
  m_tot2Charges[moduleHash] = std::move(charges);
}

const PixelChargeCalibCondData::IBLCalibration &PixelChargeCalibCondData::getTot2Charges(unsigned int moduleHash, unsigned int FE) const
{
  auto it = m_tot2Charges.find(moduleHash);
  if (it != m_tot2Charges.end() && FE < it->second.size()) {
    return it->second.at(FE);
  }

  std::stringstream error;
  error << "PixelChargeCalibCondData::getTot2Charges(" << moduleHash << ", " << FE << "): array out of bounds";
  throw std::range_error(error.str());
}

float PixelChargeCalibCondData::getChargeLUTFEI4(unsigned int moduleHash, unsigned int FE, unsigned int ToT) const
{
  if (ToT < 1 || ToT > IBLCalibrationSize) {
    std::stringstream error;
    error << "PixelChargeCalibCondData::getChargeLUTFEI4(" << moduleHash << ", " << FE << ", " << ToT << "): array out of bounds";
    throw std::runtime_error(error.str());
  }

  const IBLCalibration &charges = getTot2Charges(moduleHash,FE);
  return charges[ToT - 1];
}

float PixelChargeCalibCondData::getToTLUTFEI4(unsigned int moduleHash, unsigned int FE, float Q) const
{
  int tot = -1;
  float minDiff = FLT_MAX;
  for (size_t t = 0; t < IBLCalibrationSize; t++) {
    float charge = getChargeLUTFEI4(moduleHash, FE, t + 1);
    float diff = std::fabs(charge - Q);
    if (diff < minDiff) {
      minDiff = diff;
      tot = t + 1;
    }
  }
  return tot;
}
