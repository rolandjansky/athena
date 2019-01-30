/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ByteStreamErrorsTool.h
 * interface file for tool that keeps track of errors in the bytestream.
 * @author Susumu.Oda@cern.ch
 **/

#ifndef ISCT_ByteStreamErrorsTool_h
#define ISCT_ByteStreamErrorsTool_h

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"
#include "SCT_ConditionsData/SCT_ByteStreamErrors.h"

#include "GaudiKernel/EventContext.h"

#include <set>

class Identifier;
class IdentifierHash;

/**
 * @class SCT_ByteStreamErrorsTool
 * Tool that keeps track of modules that give rise to errors in the bytestram.
 **/

class ISCT_ByteStreamErrorsTool: virtual public ISCT_ConditionsTool {

 public:
  //@name Tool methods
  //@{

  virtual ~ISCT_ByteStreamErrorsTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_ByteStreamErrorsTool, 1, 0);
  //@}
  
  virtual const std::set<IdentifierHash>* getErrorSet(int errorType) const =0;
  virtual const std::set<IdentifierHash>* getErrorSet(int errorType, const EventContext& ctx) const =0;

  virtual bool isRODSimulatedData() const =0;
  virtual bool isRODSimulatedData(const EventContext& ctx) const =0;
  virtual bool isRODSimulatedData(const IdentifierHash& elementIdHash) const =0;
  virtual bool isRODSimulatedData(const IdentifierHash& elementIdHash, const EventContext& ctx) const =0;

  virtual bool isCondensedReadout() const =0;
  virtual bool isCondensedReadout(const EventContext& ctx) const =0;
  virtual bool isHVOn() const =0;
  virtual bool isHVOn(const EventContext& ctx) const =0;

  /** Temporary status of chips for a particular module (packed as 1st 12 bits of unsigned int) */
  virtual unsigned int tempMaskedChips(const Identifier& moduleId) const =0;
  virtual unsigned int tempMaskedChips(const Identifier& moduleId, const EventContext& ctx) const =0;
  /** Status ABCD errors of chips for a particular module (packed as 1st 12 bits of unsigned int) */
  virtual unsigned int abcdErrorChips(const Identifier& moduleId) const =0;
  virtual unsigned int abcdErrorChips(const Identifier& moduleId, const EventContext& ctx) const =0;

 private:

};

#endif // ISCT_ByteStreamErrorsTool_h
