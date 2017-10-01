/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file ISCT_FlaggedConditionSvc.h
 * interface file for service allowing one to flag modules as 'bad' with a reason
 * @author gwilliam@mail.cern.ch
 */

#ifndef ISCT_FlaggedConditionSvc_h
#define ISCT_FlaggedConditionSvc_h
 
// STL
#include <map>
#include <string>

// Athena 
#include "Identifier/IdentifierHash.h"
 
// Local 
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

// Forward declarations
class Identifier;
class IdentifierHash;

/*
 * @class SCT_FlaggedConditionSvc
 * Service allowing one to flag detector elements as 'bad' with a reason
 */

class ISCT_FlaggedConditionSvc: virtual public ISCT_ConditionsSvc {

 public:
  //@name Service methods
  //@{
  virtual ~ISCT_FlaggedConditionSvc() {}
  static const InterfaceID& interfaceID();
  //@}

  /**Flag a wafer as bad with a reason (by Identifier)*/
  virtual bool flagAsBad(const Identifier& id, const std::string& source) =0;
  /**Flag a wafer ID as bad with a reason (by IdentifierHash)*/
  virtual bool flagAsBad(const IdentifierHash& hashId, const std::string& source) =0;
  
  /**Get the reason why the wafer is bad (by Identifier)*/ 
  virtual const std::string& details(const Identifier& id) const =0;
  /**Get the reason why the wafer is bad (by IdentifierHash)*/ 
  virtual const std::string& details(const IdentifierHash& id) const =0;

  /**Get number flagged as bad (per event)*/
  virtual int numBadIds() const =0;

  /**Get IdentifierHashs ofwafers flagged as bad + reason (per event)*/
  virtual const std::map<IdentifierHash, std::string>* getBadIds() const =0;

  /**Reset between events*/
  virtual void resetBadIds()=0;

 private:
};

inline const InterfaceID& ISCT_FlaggedConditionSvc::interfaceID() {
  static const InterfaceID IID_SCT_FlaggedConditionSvc{"SCT_FlaggedConditionSvc", 1, 0};
  return IID_SCT_FlaggedConditionSvc;
}

#endif // ISCT_FlaggedConditionSvc_h
