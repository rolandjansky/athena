/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

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

  if (type == AuditType::Before) {

    ATH_CHECK( m_algStartTimes.insert(caller, context, TrigTimeStamp(), msg()) );
    ATH_MSG_DEBUG("Caller '" << caller << "' began");

  } else if (type == AuditType::After) {

    ATH_CHECK( m_algStopTimes.insert(caller, context, TrigTimeStamp(), msg()) );
    ATH_MSG_DEBUG("Caller '" << caller << "' ended");

    if (m_printTimes) {
      double algTime;
      ATH_CHECK(getAlgTimeFromStore(caller, context, algTime));
      ATH_MSG_INFO("Caller '" << caller << "' time: " << algTime << " ms");
    }

  }

  return StatusCode::SUCCESS;
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::getAlgTimeFromStore(const std::string& caller, const EventContext& context, double& algTime) const {
  TrigTimeStamp start;
  TrigTimeStamp stop;
  ATH_CHECK(m_algStartTimes.retrieve(caller, context, msg(), start));
  ATH_CHECK(m_algStopTimes.retrieve(caller, context, msg(), stop));
  algTime = start.millisecondsDifference(stop);
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::endEvent(const EventContext& context) { 
  // TODO record all of the tings
  ATH_CHECK(m_algStartTimes.clear(context, msg()));
  ATH_CHECK(m_algStopTimes.clear(context, msg()));
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool TrigCostMTSvc::isMonitoredEvent(const EventContext& context) const {
  return (m_monitorAll || context.slot() % m_moitorFrequency == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
