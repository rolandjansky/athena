/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "AthViews/View.h"

#include "TrigCostMonitorMT/TrigCostMTSvc.h"

/////////////////////////////////////////////////////////////////////////////

TrigCostMTSvc::TrigCostMTSvc(const std::string& name, ISvcLocator* pSvcLocator) :
AthService(name, pSvcLocator) {
  ATH_MSG_DEBUG("TrigCostMTSvc regular constructor");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TrigCostMTSvc::~TrigCostMTSvc() {
  ATH_MSG_DEBUG("TrigCostMTSvc destructor()");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( ITrigCostMTSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ITrigCostMTSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::initialize(){
  ATH_MSG_DEBUG("TrigCostMTSvc initialize()");
  if (m_moitorFrequency <= 0) {
    ATH_MSG_FATAL("MonitorFrequency must be > 0");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::finalize() {
  ATH_MSG_DEBUG("TrigCostMTSvc finalize()");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::processAlg(const std::string& caller, const EventContext& context, const AuditType type) {
  if (!isMonitoredEvent(context)) return StatusCode::SUCCESS;

  // This minimises string operations
  std::string callerWithView;
  std::string const* const nameToUse = (addViewToCaller(caller, context, callerWithView) ? &callerWithView : &caller);

  if (type == AuditType::Before) {
    
    // if (m_tempStore.count(*nameToUse) > 0) ATH_MSG_WARNING("Already have an entry for '" + *nameToUse + "'");
    ATH_CHECK( m_algStartTimes.insert(*nameToUse, context, TrigTimeStamp()) );
    ATH_MSG_DEBUG("Caller '" << *nameToUse << "' began");

  } else if (type == AuditType::After) {

    ATH_CHECK( m_algStopTimes.insert(*nameToUse, context, TrigTimeStamp()) );
    ATH_MSG_DEBUG("Caller '" << *nameToUse << "' ended");

    if (m_printTimes) {
      double algTime;
      ATH_CHECK(getAlgTimeFromStore(caller, context, algTime));
      ATH_MSG_INFO("Caller '" << *nameToUse << "' time: " << algTime << " ms");
    }

  }

  return StatusCode::SUCCESS;
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::getAlgTimeFromStore(const std::string& caller, const EventContext& context, double& algTime) const {
  TrigTimeStamp start;
  TrigTimeStamp stop;
  ATH_CHECK(m_algStartTimes.retrieve(caller, context, start));
  ATH_CHECK(m_algStopTimes.retrieve(caller, context, stop));
  algTime = start.millisecondsDifference(stop);
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::endEvent(const EventContext& context) { 
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool TrigCostMTSvc::addViewToCaller(const std::string& caller,  const EventContext& context, std::string& output) const {
  const IProxyDict* proxy = context.getExtension<Atlas::ExtendedEventContext>()->proxy();
  ATH_MSG_INFO("Proxy name:'" << proxy->name() << "'");
  const SG::View* view = dynamic_cast<const SG::View*>(proxy); // This will fail if we are in the global context
  if (view == nullptr) return false;
  output = view->name() + "_" + caller;
  return true;
}

bool TrigCostMTSvc::isMonitoredEvent(const EventContext& context) const {
  return (m_monitorAll || context.slot() % m_moitorFrequency == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
