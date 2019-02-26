/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_LinkMaskingTool.h
 * 
 * @author gwilliam@mail.cern.ch
**/

#ifndef SCT_LinkMaskingTool_h
#define SCT_LinkMaskingTool_h

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsData/SCT_ModuleVetoCondData.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

// Gaudi includes
#include "GaudiKernel/EventContext.h"

// Forward declarations
class SCT_ID;

/**
 * @class SCT_LinkMaskingTool
 * 
 * 
**/

class SCT_LinkMaskingTool: public extends<AthAlgTool, ISCT_ConditionsTool> {
public:

  //@name Tool methods
  //@{
  SCT_LinkMaskingTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~SCT_LinkMaskingTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}
  
  /**Can the service report about the given component? (chip, module...)*/
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;
  
  /**Is the detector element good?*/
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  
  /**Is it good?, using wafer hash*/
  virtual bool isGood(const IdentifierHash& hashId) const override;
  virtual bool isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;

private:
  const SCT_ID* m_sctHelper; //!< ID helper for SCT

  // ReadCondHandleKey
  SG::ReadCondHandleKey<SCT_ModuleVetoCondData> m_condKey{this, "CondKey", "SCT_LinkMaskingCondData", "SCT Front End olinks to be masked"};
  // Provides SCT_ModuleVetoCondData pointer
  const SCT_ModuleVetoCondData* getCondData(const EventContext& ctx) const;
  
};
#endif // SCT_LinkMaskingTool_h
