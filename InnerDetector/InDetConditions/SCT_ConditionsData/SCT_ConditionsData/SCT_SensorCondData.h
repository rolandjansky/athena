// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SensorCondData.h
 * @brief header file for data object for SCT_SensorsCondAlg and SCT_SensorsTool.
 * @author Susumu Oda
 * @date 2017-12-04
 **/

#ifndef SCT_SENSORCONDDATA_H
#define SCT_SENSORCONDDATA_H

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <string>

/**
 * @class SCT_SensorCondData
 * @brief Class for data object used in SCT_SensorsCondAlg and SCT_SensorsTool.
 **/
class SCT_SensorCondData {
public:
  /// Constructor
  SCT_SensorCondData();
  /// Destructor
  virtual ~SCT_SensorCondData() = default;

  /// Set truncated serial number
  bool setTruncatedSerialNumber(const CondAttrListCollection::ChanNum truncatedSerialNumber);
  /// Get truncated serial number
  CondAttrListCollection::ChanNum getTruncatedSerialNumber() const;

  /// Set manufacturer
  bool setManufacturer(const std::string& manufacturer);
  /// Get manufacturer
  std::string getManufacturer() const;

  /// Set depletion voltage
  bool setDepletionVoltage(const unsigned int sensor, const float depletionVoltage);
  /// Get depletion voltage
  float getDepletionVoltage(const unsigned int sensor) const;

  /// Set depletion voltage
  bool setCrystalOrientation(const unsigned int sensor, const int crystalOrientation);
  /// Get depletion voltage
  int getCrystalOrientation(const unsigned int sensor) const;

  enum {OFFSET=1, SENSOR1=1, SENSOR2=2, SENSOR3=3, SENSOR4=4, NUM_SENSORS=4};

private:
  CondAttrListCollection::ChanNum m_truncatedSerialNumber;
  std::string m_manufacturer;
  float m_depletionVoltage[NUM_SENSORS];
  int m_crystalOrientation[NUM_SENSORS];
};

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_SensorCondData , 144247573 , 1 )

#endif // SCT_SENSORDATA_H
