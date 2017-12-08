/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_LinkMaskingSvc.h
 * 
 * @author gwilliam@mail.cern.ch
**/

#ifndef SCT_LinkMaskingSvc_h
#define SCT_LinkMaskingSvc_h

// STL includes
#include <string>
#include <set>
#include <vector>
#include <mutex>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "Identifier/Identifier.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsData/SCT_ModuleVetoCondData.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

// Forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class SCT_ID;
class StatusCode;

/**
 * @class SCT_LinkMaskingSvc
 * 
 * 
**/

class SCT_LinkMaskingSvc: virtual public ISCT_ConditionsSvc, public AthService {
  friend class SvcFactory<SCT_LinkMaskingSvc>;
public:

  //@name Service methods
  //@{
  SCT_LinkMaskingSvc(const std::string& name, ISvcLocator* svc);
  virtual ~SCT_LinkMaskingSvc() {}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  static const InterfaceID& interfaceID();
  //@}
  
  /**Can the service report about the given component? (chip, module...)*/
  virtual bool                          canReportAbout(InDetConditions::Hierarchy h);
  
  /**Is the detector element good?*/
  virtual bool                          isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  
  /**Is it good?, using wafer hash*/
  virtual bool                          isGood(const IdentifierHash& hashId);

  /**Manually get the data in the structure before proceding*/
  virtual StatusCode                    fillData() { return StatusCode::FAILURE; }
  
  /**Fill data from an IOVDbSvc callback*/
  virtual StatusCode                    fillData(int& /*i*/, std::list<std::string>& /*l*/) { return StatusCode::SUCCESS; };
  
  /**Are the data available?*/
  virtual bool                          filled() const;
  
  /**Can the data be filled during the initialize phase?*/
  virtual bool                          canFillDuringInitialize() { return false; }
  
private:
  SCT_ModuleVetoCondData                   m_data;      //!< Set of masked link identifiers
  ServiceHandle<StoreGateSvc>              m_detStore;  //!< Handle on the detector store
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

inline const InterfaceID& SCT_LinkMaskingSvc::interfaceID() {
  return ISCT_ConditionsSvc::interfaceID(); 
}

#endif // SCT_LinkMaskingSvc_h
