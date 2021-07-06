/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISiliconConditionsTool.h
 * @author Susumu.Oda@cern.ch
**/
#ifndef ISiliconConditionsTool_h
#define ISiliconConditionsTool_h
//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
// STL includes
#include <string>

//forward declarations
class Identifier;
class IdentifierHash;
/**
 * @class ISiliconConditionsTool
 * Interface class for service providing basic silicon parameters
**/
class ISiliconConditionsTool: virtual public IAlgTool {
public:
  virtual ~ISiliconConditionsTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISiliconConditionsTool, 1, 0);

  //@name methods taking the detector identifier
  //@{
  virtual float temperature(const Identifier& detectorElement, const EventContext& ctx) const = 0;
  virtual float biasVoltage(const Identifier& detectorElement, const EventContext& ctx) const = 0;
  virtual float depletionVoltage(const Identifier& detectorElement, const EventContext& ctx) const = 0;
  //@}
  //@name methods taking the detector hash identifier
  //@{
  virtual float temperature(const IdentifierHash& detectorElement, const EventContext& ctx) const = 0;
  virtual float biasVoltage(const IdentifierHash& detectorElement, const EventContext& ctx) const = 0;
  virtual float depletionVoltage(const IdentifierHash& detectorElement, const EventContext& ctx) const = 0;
  //@}

};

#endif // ISiliconConditionsTool_h
