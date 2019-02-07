/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_DCSConditionsTool.h
 * interface file for service that keeps track of errors in the bytestream.
 * @author timothy.robert.andeen@cern.ch
 **/

#ifndef ISCT_DCSConditionsTool_h
#define ISCT_DCSConditionsTool_h

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

class Identifier;
class IdentifierHash;

/**
 * @class SCT_DCSConditionsTool
 * Tool that returns DCS info about a module
 **/

class ISCT_DCSConditionsTool: virtual public ISCT_ConditionsTool {

 public:
  //@name Tool methods
  //@{

  virtual ~ISCT_DCSConditionsTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_DCSConditionsTool, 1, 0);

  //@}
  virtual float modHV(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const =0;
  virtual float modHV(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const =0;
  //Does the same for hashIds
  virtual float modHV(const IdentifierHash& hashId, const EventContext& ctx) const =0;
  virtual float modHV(const IdentifierHash& hashId) const =0;
  //Returns temp0 (0 if there is no information)
  virtual float hybridTemperature(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const =0;
  virtual float hybridTemperature(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const =0;
  //Does the same for hashIds
  virtual float hybridTemperature(const IdentifierHash& hashId, const EventContext& ctx) const =0;
  virtual float hybridTemperature(const IdentifierHash& hashId) const =0;
  //Returns temp0 + correction for Lorentz angle calculation (0 if there is no information)
  virtual float sensorTemperature(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const =0;
  virtual float sensorTemperature(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const =0;
  //Does the same for hashIds
  virtual float sensorTemperature(const IdentifierHash& hashId, const EventContext& ctx) const =0;
  virtual float sensorTemperature(const IdentifierHash& hashId) const =0;

 private:

};

#endif // ISCT_DCSConditionsTool_h
