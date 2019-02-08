/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_SensorsTool.h
 * header file for baseclass of SCT conditions tool
 * @author shaun.roe@cern.ch
 **/

#ifndef ISCT_SensorsTool_h
#define ISCT_SensorsTool_h

//STL includes
#include <list>
#include <string>
#include <vector>

//Gaudi Includes
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"

//fwd declarations

class SCT_SensorCondData;

/**
 * @class ISCT_SensorsTool
 * Base class to give a Vdep, crystal orientation and Mfr from conditions for each sensor in a module
 **/
class ISCT_SensorsTool: virtual public IAlgTool {
 public:
  virtual ~ISCT_SensorsTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_SensorsTool, 1, 0);

  ///Fill vector with sensors info
  virtual void getSensorsData(std::vector<std::string>& userVector, const EventContext& ctx) const =0;
  virtual void getSensorsData(std::vector<std::string>& userVector) const =0;
  virtual const SCT_SensorCondData* getSensorsData(const unsigned int truncatedSerialNumber, const EventContext& ctx) const =0;
  virtual const SCT_SensorCondData* getSensorsData(const unsigned int truncatedSerialNumber) const =0;
  virtual std::string getManufacturer(unsigned int truncatedSerialNumber, const EventContext& ctx) const =0;
  virtual std::string getManufacturer(unsigned int truncatedSerialNumber) const =0;
  virtual void printManufacturers(const EventContext& ctx) const =0;
  virtual void printManufacturers() const =0;
};

#endif // ISCT_SensorsTool_h
