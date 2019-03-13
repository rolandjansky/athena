/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiLorentzAngleCondData.cxx
 * @author Susumu.Oda@cern.ch
**/

#include "SiLorentzAngleTool/SiLorentzAngleCondData.h"

#include <limits>

const double SiLorentzAngleCondData::s_invalidValue{std::numeric_limits<double>::quiet_NaN()};

SiLorentzAngleCondData::SiLorentzAngleCondData():
  m_lorentzShift{},
  m_lorentzShiftEta{},
  m_tanLorentzAngle{},
  m_tanLorentzAngleEta{},
  m_monitorBiasVoltage{},
  m_monitorTemperature{},
  m_monitorDepletionVoltage{},
  m_correctionFactor{1.}
{
}

void SiLorentzAngleCondData::resize(const unsigned int size)
{
  m_lorentzShift.resize(size, s_invalidValue);
  m_lorentzShiftEta.resize(size, s_invalidValue);
  m_tanLorentzAngle.resize(size, s_invalidValue);
  m_tanLorentzAngleEta.resize(size, s_invalidValue);
  m_monitorBiasVoltage.resize(size, s_invalidValue);
  m_monitorTemperature.resize(size, s_invalidValue);
  m_monitorDepletionVoltage.resize(size, s_invalidValue);
}

double SiLorentzAngleCondData::getLorentzShift(const IdentifierHash& elementHash) const
{
  if (m_lorentzShift.size()<elementHash) return s_invalidValue;
  return m_lorentzShift[elementHash];
}

double SiLorentzAngleCondData::getLorentzShiftEta(const IdentifierHash& elementHash) const
{
  if (m_lorentzShiftEta.size()<elementHash) return s_invalidValue;
  return m_lorentzShiftEta[elementHash];
}

double SiLorentzAngleCondData::getTanLorentzAngle(const IdentifierHash& elementHash) const
{
  if (m_tanLorentzAngle.size()<elementHash) return s_invalidValue;
  return m_tanLorentzAngle[elementHash];
}

double SiLorentzAngleCondData::getTanLorentzAngleEta(const IdentifierHash& elementHash) const
{
  if (m_tanLorentzAngleEta.size()<elementHash) return s_invalidValue;
  return m_tanLorentzAngleEta[elementHash];
}  

double SiLorentzAngleCondData::getBiasVoltage(const IdentifierHash& elementHash) const
{
  if (m_monitorBiasVoltage.size()<elementHash) return s_invalidValue;
  return m_monitorBiasVoltage[elementHash];
}

double SiLorentzAngleCondData::getTemperature(const IdentifierHash& elementHash) const
{
  if (m_monitorTemperature.size()<elementHash) return s_invalidValue;
  return m_monitorTemperature[elementHash];
}

double SiLorentzAngleCondData::getDepletionVoltage(const IdentifierHash& elementHash) const
{
  if (m_monitorDepletionVoltage.size()<elementHash) return s_invalidValue;
  return m_monitorDepletionVoltage[elementHash];
}

double SiLorentzAngleCondData::getCorrectionFactor() const
{
  return m_correctionFactor;
}

void SiLorentzAngleCondData::setLorentzShift(const IdentifierHash& elementHash, const double lorentzShift)
{
  if (m_lorentzShift.size()<elementHash) m_lorentzShift.resize(elementHash+1, s_invalidValue);
  m_lorentzShift[elementHash] = lorentzShift;
}

void SiLorentzAngleCondData::setLorentzShiftEta(const IdentifierHash& elementHash, const double lorentzShiftEta)
{
  if (m_lorentzShiftEta.size()<elementHash) m_lorentzShiftEta.resize(elementHash+1, s_invalidValue);
  m_lorentzShiftEta[elementHash] = lorentzShiftEta;
}

void SiLorentzAngleCondData::setTanLorentzAngle(const IdentifierHash& elementHash, const double tanLorentzAngle)
{
  if (m_tanLorentzAngle.size()<elementHash) m_tanLorentzAngle.resize(elementHash+1, s_invalidValue);
  m_tanLorentzAngle[elementHash] = tanLorentzAngle;
}

void SiLorentzAngleCondData::setTanLorentzAngleEta(const IdentifierHash& elementHash, const double tanLorentzAngleEta)
{
  if (m_tanLorentzAngleEta.size()<elementHash) m_tanLorentzAngleEta.resize(elementHash+1, s_invalidValue);
  m_tanLorentzAngleEta[elementHash] = tanLorentzAngleEta;
}

void SiLorentzAngleCondData::setBiasVoltage(const IdentifierHash& elementHash, const double biasVoltage)
{
  if (m_monitorBiasVoltage.size()<elementHash) m_monitorBiasVoltage.resize(elementHash+1, s_invalidValue);
  m_monitorBiasVoltage[elementHash] = biasVoltage;
}

void SiLorentzAngleCondData::setTemperature(const IdentifierHash& elementHash, const double temperature)
{
  if (m_monitorTemperature.size()<elementHash) m_monitorTemperature.resize(elementHash+1, s_invalidValue);
  m_monitorTemperature[elementHash] = temperature;
}

void SiLorentzAngleCondData::setDepletionVoltage(const IdentifierHash& elementHash, const double depletionVoltage)
{
  if (m_monitorDepletionVoltage.size()<elementHash) m_monitorDepletionVoltage.resize(elementHash+1, s_invalidValue);
  m_monitorDepletionVoltage[elementHash] = depletionVoltage;
}

void SiLorentzAngleCondData::setCorrectionFactor(const double correctionFactor)
{
  m_correctionFactor = correctionFactor;
}
