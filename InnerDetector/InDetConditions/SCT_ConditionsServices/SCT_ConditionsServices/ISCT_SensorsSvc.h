/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_SensorsSvc.h
 * header file for baseclass of SCT conditions services
 * @author shaun.roe@cern.ch
 **/

#ifndef ISCT_SensorsSvc_h
#define ISCT_SensorsSvc_h

//STL includes
#include <list>
#include <string>
#include <vector>

//Gaudi Includes
#include "GaudiKernel/IInterface.h"

//fwd declarations

class SCT_SensorCondData;

/**
 * @class ISCT_SensorsSvc
 * Base class to give a Vdep, crystal orientation and Mfr from conditions for each sensor in a module
 **/
class ISCT_SensorsSvc: virtual public IInterface {
 public:
  virtual ~ISCT_SensorsSvc() {}
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface
  virtual StatusCode fillSensorsData(int& i, std::list<std::string>& keys)=0;

  ///Fill vector with sensors info
  virtual void getSensorsData(std::vector<std::string>& userVector)=0;
  virtual const SCT_SensorCondData* getSensorsData(const unsigned int truncatedSerialNumber) const=0;
  virtual std::string getManufacturer(unsigned int truncatedSerialNumber)=0;
  virtual void printManufacturers()=0;
};


inline const InterfaceID& ISCT_SensorsSvc::interfaceID() {
  static const InterfaceID IID{"ISCT_SensorsSvc", 1, 0};
  return IID;
}

#endif // ISCT_SensorsSvc_h
