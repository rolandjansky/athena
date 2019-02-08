/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file SCT_FlaggedConditionTool.h
 * header file for service allowing one to flag modules as 'bad' with a reason
 * @author gwilliam@mail.cern.ch
 */

#ifndef SCT_FlaggedConditionTool_h
#define SCT_FlaggedConditionTool_h
 
// Gaudi

// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_FlaggedConditionTool.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "StoreGate/ReadHandleKey.h"

// Forward declarations
class SCT_ID;

/*
 * @class SCT_FlaggedConditionTool
 * Service allowing one to flag detector elements as 'bad' with a reason
 */

class SCT_FlaggedConditionTool: public extends<AthAlgTool, ISCT_FlaggedConditionTool> {

public:
  //@name Tool methods
  //@{
  SCT_FlaggedConditionTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_FlaggedConditionTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}

  /**Can the tool report about the given component? (chip, module...)*/
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;
  
  /**Is the detector element good?*/
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const IdentifierHash& hashId) const override;
  virtual bool isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;

  /**Get the reason why the wafer is bad (by Identifier)*/ 
  virtual const std::string& details(const Identifier& id) const override;
  virtual const std::string& details(const Identifier& id, const EventContext& ctx) const override;
  /**Get the reason why the wafer is bad (by IdentifierHash)*/ 
  virtual const std::string& details(const IdentifierHash& id) const override;
  virtual const std::string& details(const IdentifierHash& id, const EventContext& ctx) const override;

  /**Get number flagged as bad (per event)*/
  virtual int numBadIds() const override;
  virtual int numBadIds(const EventContext& ctx) const override;

  /**Get IdentifierHashs ofwafers flagged as bad + reason (per event)*/
  virtual const SCT_FlaggedCondData* getBadIds() const override;
  virtual const SCT_FlaggedCondData* getBadIds(const EventContext& ctx) const override;

 private:
  SG::ReadHandleKey<SCT_FlaggedCondData> m_badIds;

  const SCT_ID* m_sctID; //!< ID helper for SCT

  const SCT_FlaggedCondData* getCondData(const EventContext& ctx) const;
};

#endif // SCT_FlaggedConditionTool_h
