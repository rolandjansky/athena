/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledSvc.h
 * interface file for service that keeps track of Tdaq enabling/disabling of SCT Rods.
 * @author shaun.roe@cern.ch
**/

#ifndef SCT_TdaqEnabledSvc_h
#define SCT_TdaqEnabledSvc_h

#include <set>
#include <list>
#include <map>
#include <mutex>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

#include "SCT_ConditionsData/SCT_TdaqEnabledCondData.h"

// Read Handle Key
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

// Event Info
#include "EventInfo/EventInfo.h"

class Identifier;
class IdentifierHash;
class StatusCode;
class SCT_ID;

/**
 * @class SCT_ConfigurationConditionsSvc
 * Service that keeps track of Tdaq enabling/disabling of SCT Rods.
**/

class SCT_TdaqEnabledSvc: virtual public ISCT_ConditionsSvc, public AthService {
 friend class SvcFactory<SCT_TdaqEnabledSvc>;
public:
  //@name Service methods
  //@{
  // destructor
  SCT_TdaqEnabledSvc(const std::string& name, ISvcLocator* svc);
   virtual ~SCT_TdaqEnabledSvc(){}
   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
   static const InterfaceID& interfaceID();
  //@}
  
   ///Can the service report about the given component? (TdaqEnabledSvc can answer questions about a module or module side)
   virtual bool canReportAbout(InDetConditions::Hierarchy h);

   ///Is the detector element good?
   virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);

   ///is it good?, using wafer hash
   virtual bool isGood(const IdentifierHash& hashId);

   ///Manually get the data in the structure before proceeding
   virtual StatusCode fillData() { return StatusCode::FAILURE; };
   
   ///Overload 'fillData' to provide callback to data folder
   virtual StatusCode fillData(int& /*i*/ , std::list<std::string>& /*l*/) { return StatusCode::FAILURE; };

   ///Are the data available?
   virtual bool filled() const;

   ///Can the data be filled during the initialize phase?
   virtual bool canFillDuringInitialize();
  
private:
   // Mutex to protect the contents.
   mutable std::mutex m_mutex;
   // Cache to store events for slots
   mutable std::vector<EventContext::ContextEvt_t> m_cache;
   // Pointer of SCT_TdaqEnabledCondData
   mutable Gaudi::Hive::ContextSpecificPtr<const SCT_TdaqEnabledCondData> m_condData;
   
  const SCT_ID* m_pHelper;
   bool m_useDatabase;
   ServiceHandle<StoreGateSvc> m_detStore; //!< Handle on the detector store

   SG::ReadHandleKey<EventInfo> m_eventInfoKey;
   SG::ReadCondHandleKey<SCT_TdaqEnabledCondData> m_condKey;

   const SCT_TdaqEnabledCondData* getCondData(const EventContext& ctx) const;
};

inline const InterfaceID& SCT_TdaqEnabledSvc::interfaceID() {
  static const InterfaceID IID_SCT_TdaqEnabledSvc("SCT_TdaqEnabledSvc", 1, 0);
  return IID_SCT_TdaqEnabledSvc;
}

#endif // SCT_TdaqEnabledSvc_h
