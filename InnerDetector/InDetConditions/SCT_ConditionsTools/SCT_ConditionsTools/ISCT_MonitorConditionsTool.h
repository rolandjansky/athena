/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_MonitorConditionsTool.h
 * interface file for service that keeps track of errors caught by the monitoring.
 * @author gwilliam@mail.cern.ch
 **/

#ifndef ISCT_MonitorConditionsTool_h
#define ISCT_MonitorConditionsTool_h

#include <set>
#include <string>

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

class Identifier;

/**
 * @class ISCT_MonitorConditionsTool
 Tool that keeps track of errors caught by the monitoring
 **/

class ISCT_MonitorConditionsTool: virtual public ISCT_ConditionsTool {

 public:
  //@name Service methods
  //@{

  virtual ~ISCT_MonitorConditionsTool() = default;

  //@}

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_MonitorConditionsTool, 1, 0);
  
  /// Return a list of bad stip Identifiers
  virtual void badStrips(std::set<Identifier>& strips) const =0;
  virtual void badStrips(std::set<Identifier>& strips, const EventContext& ctx) const =0;
  /// Return a list of bad stip Identifiers for a given module
  virtual void badStrips(const Identifier& moduleId, std::set<Identifier>& strips) const =0;
  virtual void badStrips(const Identifier& moduleId, std::set<Identifier>& strips, const EventContext& ctx) const =0;
  /// Return a (space-separated) string of bad stip numbers (including hyphanated ranges) as they are written to the DB 
  virtual std::string badStripsAsString(const Identifier& moduleId) const =0;
  virtual std::string badStripsAsString(const Identifier& moduleId, const EventContext& ctx) const =0;

 private:

};

#endif // ISCT_MonitorConditionsTool_h
