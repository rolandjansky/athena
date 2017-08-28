/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_DCSConditionsSvc.h
 * interface file for service that keeps track of errors in the bytestream.
 * @author timothy.robert.andeen@cern.ch
 **/

#ifndef ISCT_DCSConditionsSvc_h
#define ISCT_DCSConditionsSvc_h

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

class Identifier;
class IdentifierHash;

/**
 * @class SCT_DCSConditionsSvc
 * Service that returns DCS info about a module
 **/

class ISCT_DCSConditionsSvc: virtual public ISCT_ConditionsSvc {

 public:
  //@name Service methods
  //@{

  virtual ~ISCT_DCSConditionsSvc() {}

  static const InterfaceID& interfaceID();

  //@}
  virtual float modHV(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT)=0;
  //Does the same for hashIds
  virtual float modHV(const IdentifierHash& hashId)=0;
  //Returns temp0 (0 if there is no information)
  virtual float hybridTemperature(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT)=0;
  //Does the same for hashIds
  virtual float hybridTemperature(const IdentifierHash& hashId)=0;
  //Returns temp0 + correction for Lorentz angle calculation (0 if there is no information)
  virtual float sensorTemperature(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT)=0;
  //Does the same for hashIds
  virtual float sensorTemperature(const IdentifierHash& hashId)=0;

 private:

};

inline const InterfaceID& ISCT_DCSConditionsSvc::interfaceID() {
  static const InterfaceID IID_SCT_DCSConditionsSvc{"SCT_DCSConditionsSvc", 1, 0};
  return IID_SCT_DCSConditionsSvc;
}

#endif // ISCT_DCSConditionsSvc_h
