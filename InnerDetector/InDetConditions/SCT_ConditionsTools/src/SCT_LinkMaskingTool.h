/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_LinkMaskingTool.h
 * 
 * @author gwilliam@mail.cern.ch
**/

#ifndef SCT_LinkMaskingTool_h
#define SCT_LinkMaskingTool_h

// STL includes
#include <vector>
#include <mutex>

// Gaudi includes
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsData/SCT_ModuleVetoCondData.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

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
  virtual bool                          canReportAbout(InDetConditions::Hierarchy h) override;
  
  /**Is the detector element good?*/
  virtual bool                          isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) override;
  
  /**Is it good?, using wafer hash*/
  virtual bool                          isGood(const IdentifierHash& hashId) override;

private:
  SCT_ModuleVetoCondData                   m_data;      //!< Set of masked link identifiers
  const SCT_ID*                            m_sctHelper; //!< ID helper for SCT

  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cache;
  // Pointer of SCT_ModuleVetoCondData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_ModuleVetoCondData> m_condData;
  // ReadCondHandleKey
  SG::ReadCondHandleKey<SCT_ModuleVetoCondData> m_condKey;
  // Provides SCT_ModuleVetoCondData pointer
  const SCT_ModuleVetoCondData* getCondData(const EventContext& ctx) const;
  
};
#endif // SCT_LinkMaskingTool_h
