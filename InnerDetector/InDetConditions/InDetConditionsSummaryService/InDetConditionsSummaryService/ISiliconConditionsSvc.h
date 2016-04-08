/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISiliconConditionsSvc.h
 * @author shaun.roe@cern.ch
**/
#ifndef ISiliconConditionsSvc_h
#define ISiliconConditionsSvc_h
//Gaudi Includes
#include "GaudiKernel/IInterface.h"
// STL includes
#include <list>
#include <string>

//forward declarations
class Identifier;
class IdentifierHash;
/**
 * @class ISiliconConditionsSvc
 * Interface class for service providing basic silicon parameters
**/
class ISiliconConditionsSvc: virtual public IInterface{
public:
  virtual ~ISiliconConditionsSvc(){}
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface
  //@name methods taking the detector identifier
  //@{
  virtual  float temperature(const Identifier & detectorElement)=0;
  virtual  float biasVoltage(const Identifier & detectorElement)=0;
  virtual  float depletionVoltage(const Identifier & detectorElement)=0;
  //@}
  //@name methods taking the detector hash identifier
  //@{
  virtual  float temperature(const IdentifierHash & detectorElement)=0;
  virtual  float biasVoltage(const IdentifierHash & detectorElement)=0;
  virtual  float depletionVoltage(const IdentifierHash & detectorElement)=0;
  //@}

  /// IOV CallBack
  virtual StatusCode callBack(int&, std::list<std::string>&) = 0;
  /// Query whether a CallBack has been registered.
  virtual bool hasCallBack() = 0;

};

inline const InterfaceID & ISiliconConditionsSvc::interfaceID(){
  static const InterfaceID IID_ISiliconConditionsSvc("ISiliconConditionsSvc",1,0);
  return IID_ISiliconConditionsSvc;
}
#endif
