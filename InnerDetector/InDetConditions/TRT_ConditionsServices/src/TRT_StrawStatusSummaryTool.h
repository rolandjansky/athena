/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_STRAWSTATUSSUMMARYTOOL_H
#define TRT_STRAWSTATUSSUMMARYTOOL_H
/** @file TRT_StrawStatusSummaryTool.h
 * @brief interface to TRT straw status constants used in normal reconstruction
 * @author Peter Hansen <phansen@nbi.dk>
 */
//STL
#include <vector>
//Gaudi
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/ICondSvc.h"
//StoreGate
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/StoreGateSvc.h"
//Athena
#include "AthenaBaseComps/AthAlgTool.h"
//TRT
#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h" 
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"


class TRT_StrawStatusSummaryTool: public extends<AthAlgTool, ITRT_StrawStatusSummaryTool>
{  
 public:

  /// constructor
  TRT_StrawStatusSummaryTool ( const std::string& type, const std::string& name, const IInterface* parent);
  
  /// destructor
  virtual ~TRT_StrawStatusSummaryTool() = default;
  
  /// tool initialize
  virtual StatusCode initialize() override;

  /// tool finalize
  virtual StatusCode finalize() override;


 /// access to the status
  int getStatus(Identifier offlineId) const;
  int getStatusPermanent(Identifier offlineId) const;
  int getStatusHT(Identifier offlineId) const;
  bool get_status(Identifier offlineId) const;
  bool get_statusHT(Identifier offlineId) const;


  const StrawStatusContainer* getStrawStatusContainer() const;
  const StrawStatusContainer* getStrawStatusPermanentContainer() const;
  const StrawStatusContainer* getStrawStatusHTContainer() const;


 private:
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<IToolSvc> m_toolsvc; 
  std::string m_par_strawstatuscontainerkey;
  std::string m_par_strawstatuspermanentcontainerkey;
  std::string m_par_strawstatusHTcontainerkey;


  const TRT_ID* m_trtid;

  ServiceHandle<ICondSvc> m_condSvc;
  //  ReadHandle  keys
  SG::ReadCondHandleKey<StrawStatusContainer> m_statReadKey{this,"StatReadKeyName","in","StrawStatus in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_permReadKey{this,"PermReadKeyName","in","StrawStatusPermanent in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_statHTReadKey{this,"StatHTReadKeyName","in","StrawStatusHT in-key"};

  // Caches
  mutable std::vector<const StrawStatusContainer*> m_strawstatuscontainer;
  mutable std::vector<const StrawStatusContainer*> m_strawstatuspermanentcontainer;
  mutable std::vector<const StrawStatusContainer*> m_strawstatusHTcontainer;
  mutable std::mutex m_cacheMutex;
  mutable std::vector<EventContext::ContextEvt_t> m_evtstat;
  mutable std::vector<EventContext::ContextEvt_t> m_evtperm;
  mutable std::vector<EventContext::ContextEvt_t> m_evtstatHT;

  // Used in simulation (GEANT4) jobs
  bool m_isGEANT4;
  const DataHandle<StrawStatusContainer> m_strawstatusG4;
  const DataHandle<StrawStatusContainer> m_strawstatuspermanentG4;
  const DataHandle<StrawStatusContainer> m_strawstatusHTG4;


};



////////////////////////////////////////////////////////////////////////////////////////////
//  inline methods
////////////////////////////////////////////////////////////////////////////////////////////




inline const TRT_StrawStatusSummaryTool::StrawStatusContainer* TRT_StrawStatusSummaryTool::getStrawStatusContainer() const{
  if(m_isGEANT4) {
    return m_strawstatusG4.cptr();
  }
  const EventContext& event_context=Gaudi::Hive::currentContext();
  EventContext::ContextID_t slot=event_context.slot();
  EventContext::ContextEvt_t event_id=event_context.evt();
  std::lock_guard<std::mutex> lock(m_cacheMutex);
  if(slot>=m_evtstat.size()) {
     m_evtstat.resize(slot+1);
     m_strawstatuscontainer.resize(slot+1);
  }

  if(m_evtstat[slot]!=event_id) {
    SG::ReadCondHandle<StrawStatusContainer> rst(m_statReadKey,event_context);
    m_strawstatuscontainer[slot]=(*rst);
    m_evtstat[slot]=event_id;
  }

  return m_strawstatuscontainer[slot];

}
inline const TRT_StrawStatusSummaryTool::StrawStatusContainer* TRT_StrawStatusSummaryTool::getStrawStatusPermanentContainer() const{

  if(m_isGEANT4) {
    return m_strawstatuspermanentG4.cptr();
  }
  const EventContext& event_context=Gaudi::Hive::currentContext();
  EventContext::ContextID_t slot=event_context.slot();
  EventContext::ContextEvt_t event_id=event_context.evt();
  std::lock_guard<std::mutex> lock(m_cacheMutex);
  if(slot>=m_evtperm.size()) {
     m_evtperm.resize(slot+1);
     m_strawstatuspermanentcontainer.resize(slot+1);
  }

  if(m_evtperm[slot]!=event_id) {
    SG::ReadCondHandle<StrawStatusContainer> rp(m_permReadKey,event_context);
    m_strawstatuspermanentcontainer[slot]=(*rp);
    m_evtperm[slot]=event_id;
  }

  return m_strawstatuspermanentcontainer[slot];

}
inline const TRT_StrawStatusSummaryTool::StrawStatusContainer* TRT_StrawStatusSummaryTool::getStrawStatusHTContainer() const{

  if(m_isGEANT4) {
    return m_strawstatusHTG4.cptr();
  }
  const EventContext& event_context=Gaudi::Hive::currentContext();
  EventContext::ContextID_t slot=event_context.slot();
  EventContext::ContextEvt_t event_id=event_context.evt();
  std::lock_guard<std::mutex> lock(m_cacheMutex);
  if(slot>=m_evtstatHT.size()) {
    m_evtstatHT.resize(slot+1);
    m_strawstatusHTcontainer.resize(slot+1);
  }

  if(m_evtstatHT[slot]!=event_id) {
    SG::ReadCondHandle<StrawStatusContainer> rht(m_statHTReadKey,event_context);
    m_strawstatusHTcontainer[slot]=(*rht);
    m_evtstatHT[slot]=event_id;
  }

  return m_strawstatusHTcontainer[slot];
}
#endif //  TRT_STRAWSTATUSSUMMARYTOOL_H
