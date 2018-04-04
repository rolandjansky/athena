/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ByteStreamErrorsSvc.h
 * interface file for service that keeps track of errors in the bytestream.
 * @author nbarlow@cern.ch
 **/

#ifndef ISCT_ByteStreamErrorsSvc_h
#define ISCT_ByteStreamErrorsSvc_h

#include <set>

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "SCT_ConditionsData/SCT_ByteStreamErrors.h"

class Identifier;
class IdentifierHash;

/**
 * @class SCT_ByteStreamErrorsSvc
 * Service that keeps track of modules that give rise to errors in the bytestram.
 **/

class ISCT_ByteStreamErrorsSvc: virtual public ISCT_ConditionsSvc {

 public:
  //@name Service methods
  //@{

  virtual ~ISCT_ByteStreamErrorsSvc() = default;

  static const InterfaceID& interfaceID();

  //@}
  
  virtual const std::set<IdentifierHash>* getErrorSet(int errorType)=0;

  virtual bool isRODSimulatedData() const =0;

  virtual bool isCondensedReadout() const =0;
  virtual bool HVisOn() const =0;

  /** Temporary status of chips for a particular module (packed as 1st 12 bits of unsigned int) */
  virtual unsigned int tempMaskedChips(const Identifier& moduleId)=0;
  /** Status ABCD errors of chips for a particular module (packed as 1st 12 bits of unsigned int) */
  virtual unsigned int abcdErrorChips(const Identifier& moduleId)=0;

 private:

};

inline const InterfaceID& ISCT_ByteStreamErrorsSvc::interfaceID() {
  static const InterfaceID IID_SCT_ByteStreamErrorsSvc{"SCT_ByteStreamErrorsSvc", 1, 0};
  return IID_SCT_ByteStreamErrorsSvc;
}

#endif // ISCT_ByteStreamErrorsSvc_h
