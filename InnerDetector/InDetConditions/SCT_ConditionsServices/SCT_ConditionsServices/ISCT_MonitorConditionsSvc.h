/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_MonitorConditionsSvc.h
 * interface file for service that keeps track of errors caught by the monitoring.
 * @author gwilliam@mail.cern.ch
 **/

#ifndef ISCT_MonitorConditionsSvc_h
#define ISCT_MonitorConditionsSvc_h

#include <set>
#include <string>

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

class Identifier;

/**
 * @class ISCT_MonitorConditionsSvc
 * Service that keeps track of errors caught by the monitoring
 **/

class ISCT_MonitorConditionsSvc: virtual public ISCT_ConditionsSvc {

 public:
  //@name Service methods
  //@{

  virtual ~ISCT_MonitorConditionsSvc() {}
  static const InterfaceID & interfaceID();

  //@}
  
  /// Return a list of bad stip Identifiers
  virtual void badStrips(std::set<Identifier>& strips)=0;
  /// Return a list of bad stip Identifiers for a given module
  virtual void badStrips(const Identifier& moduleId, std::set<Identifier>& strips)=0;
  /// Return a (space-separated) string of bad stip numbers (including hyphanated ranges) as they are written to the DB 
  virtual std::string badStripsAsString(const Identifier& moduleId)=0;

 private:

};

inline const InterfaceID& ISCT_MonitorConditionsSvc::interfaceID() {
  static const InterfaceID IID_SCT_MonitorConditionsSvc{"SCT_MonitorConditionsSvc", 1, 0};
  return IID_SCT_MonitorConditionsSvc;
}

#endif // ISCT_MonitorConditionsSvc_h
