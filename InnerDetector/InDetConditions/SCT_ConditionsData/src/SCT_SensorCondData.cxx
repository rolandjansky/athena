/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for the data object class for SCT_SensorsConditionsSvc
//----------------------------------------------------------------------

#include "SCT_ConditionsData/SCT_SensorCondData.h"

//----------------------------------------------------------------------
// Constructor
SCT_SensorCondData::SCT_SensorCondData():
  m_truncatedSerialNumber{0},
  m_manufacturer{""},
  m_depletionVoltage{0., 0., 0., 0.},
  m_crystalOrientation{0, 0, 0, 0}
{}

//----------------------------------------------------------------------
// Destructor
SCT_SensorCondData::~SCT_SensorCondData()
{}

//----------------------------------------------------------------------
// Set truncated serial number
bool SCT_SensorCondData::setTruncatedSerialNumber(const CondAttrListCollection::ChanNum truncatedSerialNumber)
{
  m_truncatedSerialNumber = truncatedSerialNumber;
  return true;
}

//----------------------------------------------------------------------
// Get truncated serial number
CondAttrListCollection::ChanNum SCT_SensorCondData::getTruncatedSerialNumber() const
{
  return m_truncatedSerialNumber;
}

//----------------------------------------------------------------------
// Set manufacturer
bool SCT_SensorCondData::setManufacturer(const std::string& manufacturer)
{
  m_manufacturer = manufacturer;
  return true;
}

//----------------------------------------------------------------------
// Get manufacturer
std::string SCT_SensorCondData::getManufacturer() const
{
  return m_manufacturer;
}

//----------------------------------------------------------------------
// Set depletion voltage
bool SCT_SensorCondData::setDepletionVoltage(const unsigned int sensor, const float depletionVoltage)
{
  switch (sensor) {
  case SENSOR1:
    m_depletionVoltage[SENSOR1-OFFSET] = depletionVoltage;
    return true;
  case SENSOR2:
    m_depletionVoltage[SENSOR2-OFFSET] = depletionVoltage;
    return true;
  case SENSOR3:
    m_depletionVoltage[SENSOR3-OFFSET] = depletionVoltage;
    return true;
  case SENSOR4:
    m_depletionVoltage[SENSOR4-OFFSET] = depletionVoltage;
    return true;
  default:
    return false;
  }
}

//----------------------------------------------------------------------
// Get depletion voltage
float SCT_SensorCondData::getDepletionVoltage(const unsigned int sensor) const
{
  switch (sensor) {
  case SENSOR1:
    return m_depletionVoltage[SENSOR1-OFFSET];
  case SENSOR2:
    return m_depletionVoltage[SENSOR2-OFFSET];
  case SENSOR3:
    return m_depletionVoltage[SENSOR3-OFFSET];
  case SENSOR4:
    return m_depletionVoltage[SENSOR4-OFFSET];
  default:
    return -999.;
  }
}

//----------------------------------------------------------------------
// Set crystal Orientation
bool SCT_SensorCondData::setCrystalOrientation(const unsigned int sensor, const int crystalOrientation)
{
  switch (sensor) {
  case SENSOR1:
    m_crystalOrientation[SENSOR1-OFFSET] = crystalOrientation;
    return true;
  case SENSOR2:
    m_crystalOrientation[SENSOR2-OFFSET] = crystalOrientation;
    return true;
  case SENSOR3:
    m_crystalOrientation[SENSOR3-OFFSET] = crystalOrientation;
    return true;
  case SENSOR4:
    m_crystalOrientation[SENSOR4-OFFSET] = crystalOrientation;
    return true;
  default:
    return false;
  }
}

//----------------------------------------------------------------------
// Get crystal Orientation
int SCT_SensorCondData::getCrystalOrientation(const unsigned int sensor) const
{
  switch (sensor) {
  case SENSOR1:
    return m_crystalOrientation[SENSOR1-OFFSET];
  case SENSOR2:
    return m_crystalOrientation[SENSOR2-OFFSET];
  case SENSOR3:
    return m_crystalOrientation[SENSOR3-OFFSET];
  case SENSOR4:
    return m_crystalOrientation[SENSOR4-OFFSET];
  default:
    return -999;
  }
}
