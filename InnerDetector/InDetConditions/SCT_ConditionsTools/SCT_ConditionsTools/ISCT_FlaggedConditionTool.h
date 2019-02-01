/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file ISCT_FlaggedConditionTool.h
 * interface file for service allowing one to flag modules as 'bad' with a reason
 * @author gwilliam@mail.cern.ch
 */

#ifndef ISCT_FlaggedConditionTool_h
#define ISCT_FlaggedConditionTool_h
 
// Athena 
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
 
// Local 
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"
#include "SCT_ConditionsData/SCT_FlaggedCondData.h"

// Forward declarations
class Identifier;

/*
 * @class SCT_FlaggedConditionTool
 * Tool allowing one to flag detector elements as 'bad' with a reason
 */

class ISCT_FlaggedConditionTool: virtual public ISCT_ConditionsTool {

 public:
  //@name Service methods
  //@{
  virtual ~ISCT_FlaggedConditionTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_FlaggedConditionTool, 1, 0);
  //@}

  /**Get the reason why the wafer is bad (by Identifier)*/ 
  virtual const std::string& details(const Identifier& id) const =0;
  virtual const std::string& details(const Identifier& id, const EventContext& ctx) const =0;
  /**Get the reason why the wafer is bad (by IdentifierHash)*/ 
  virtual const std::string& details(const IdentifierHash& id) const =0;
  virtual const std::string& details(const IdentifierHash& id, const EventContext& ctx) const =0;

  /**Get number flagged as bad (per event)*/
  virtual int numBadIds() const =0;
  virtual int numBadIds(const EventContext& ctx) const =0;

  /**Get IdentifierHashs ofwafers flagged as bad + reason (per event)*/
  virtual const SCT_FlaggedCondData* getBadIds() const =0;
  virtual const SCT_FlaggedCondData* getBadIds(const EventContext& ctx) const =0;

 private:
};

#endif // ISCT_FlaggedConditionTool_h
