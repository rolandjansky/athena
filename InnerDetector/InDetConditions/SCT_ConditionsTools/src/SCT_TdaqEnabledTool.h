/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledTool.h
 * interface file for tool that keeps track of Tdaq enabling/disabling of SCT Rods.
 * @author shaun.roe@cern.ch
**/

#ifndef SCT_TdaqEnabledTool_h
#define SCT_TdaqEnabledTool_h

#include <list>
#include <mutex>

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "SCT_ConditionsData/SCT_TdaqEnabledCondData.h"

#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

// Read Handle Key
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

// Event Info
#include "EventInfo/EventInfo.h"

class SCT_ID;

/**
 * @class SCT_TdaqEnabledTool
 * Tool that keeps track of Tdaq enabling/disabling of SCT Rods.
**/

class SCT_TdaqEnabledTool: public extends<AthAlgTool, ISCT_ConditionsTool> {
public:
  //@name Tool methods
  //@{
  SCT_TdaqEnabledTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_TdaqEnabledTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}
  
  ///Can the service report about the given component? (TdaqEnabledSvc can answer questions about a module or module side)
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;

  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;

  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash& hashId) const override;
  virtual bool isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;

 private:
  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cache;
  // Pointer of SCT_TdaqEnabledCondData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_TdaqEnabledCondData> m_condData;
   
  const SCT_ID* m_pHelper;
  bool m_useDatabase;

  SG::ReadHandleKey<EventInfo> m_eventInfoKey;
  SG::ReadCondHandleKey<SCT_TdaqEnabledCondData> m_condKey{this, "CondKey", "SCT_TdaqEnabledCondData", "Active SCT RODs"};

  const SCT_TdaqEnabledCondData* getCondData(const EventContext& ctx) const;
};

#endif // SCT_TdaqEnabledTool_h
