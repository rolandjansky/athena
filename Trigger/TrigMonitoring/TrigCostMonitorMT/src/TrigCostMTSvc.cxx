/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCostMonitorMT/TrigCostMTSvc.h"

/////////////////////////////////////////////////////////////////////////////

TrigCostMTSvc::TrigCostMTSvc(const std::string& name, ISvcLocator* pSvcLocator) :
AthService(name, pSvcLocator), m_eventMonitored(), m_algStartTimes(), m_algStopTimes()
{
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
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::finalize() {
  ATH_MSG_DEBUG("TrigCostMTSvc finalize()");
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::startEvent(const EventContext& context, const bool enableMonitoring) {
  m_eventMonitored.grow_to_at_least( context.slot() + 1, 0);
  const bool monitoredEvent = (enableMonitoring || m_monitorAll);
  m_eventMonitored.at( context.slot() ) = monitoredEvent; 
  if (monitoredEvent) {
    // Empty transient thread-safe stores in preparation for recording this event's cost data
    ATH_CHECK(m_algStartTimes.clear(context, msg()));
    ATH_CHECK(m_algStopTimes.clear(context, msg()));
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::processAlg(const EventContext& context, const std::string& caller, const AuditType type) {
  if (!isMonitoredEvent(context)) return StatusCode::SUCCESS;

  if (type == AuditType::Before) {

    ATH_CHECK( m_algStartTimes.insert(context, caller, msg(), TrigTimeStamp()) );
    ATH_MSG_DEBUG("Caller '" << caller << "', '" << context.getExtension<Atlas::ExtendedEventContext>()->proxy()->name() << "' began");

  } else if (type == AuditType::After) {

    ATH_CHECK( m_algStopTimes.insert(context, caller, msg(), TrigTimeStamp()) );
    ATH_MSG_DEBUG("Caller '" << caller << "', '" << context.getExtension<Atlas::ExtendedEventContext>()->proxy()->name() << "' ended");

  }

  return StatusCode::SUCCESS;
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTSvc::endEvent(const EventContext& context, SG::WriteHandle<xAOD::TrigCompositeContainer>& outputHandle) { 
  if (!isMonitoredEvent(context)) return StatusCode::SUCCESS;

  // Reset eventMonitored flag
  m_eventMonitored.at( context.slot() ) = 0;

  // Read payloads. Write to persistent format
  TrigCostDataStore::AITimerMapTBB::const_iterator beginIt;
  TrigCostDataStore::AITimerMapTBB::const_iterator endIt;
  ATH_CHECK(m_algStartTimes.getIterators(context, msg(), beginIt, endIt));

  for (TrigCostDataStore::AITimerMapTBB::const_iterator it = beginIt; it != endIt; ++it) {
    const TrigCostDataStore::AlgorithmIdentifier& ai = it->first;
    const TrigTimeStamp& startTime = it->second;
    const int viewID = ai.m_isView ? 0 : -1; // TODO get view ID (needs new interface)
    // Can we find the end time for this alg? Skip if not
    TrigTimeStamp stopTime;
    if (m_algStopTimes.retrieve(context, ai, msg(), stopTime).isFailure()) {
      ATH_MSG_DEBUG("No end time for '" << ai.m_caller << "', '" << ai.m_store << "'");
      continue;
    }

    // Make a new TrigComposite to persist monitoring payload for this alg
    xAOD::TrigComposite* tc = new xAOD::TrigComposite();
    (*outputHandle).push_back( tc ); 
    // tc is now owned by storegate and, and has an aux store provided by the TrigCompositeCollection

    tc->setName(ai.m_caller);
    bool result = true;
    
    result &= tc->setDetail("store", ai.m_store);
    result &= tc->setDetail("view", viewID);
    result &= tc->setDetail("CPUTime", static_cast<float>(startTime.millisecondsDifference(stopTime))); // Reduce from double precision

    if (!result) ATH_MSG_WARNING("Failed to append one or more details to trigger cost TC");
  }

  if (m_printTimes && msg().level() <= MSG::INFO) {
    ATH_MSG_INFO("--- Trig Cost Event Summary ---");
    for ( const xAOD::TrigComposite* tc : *outputHandle ) {
      ATH_MSG_INFO("Algorithm:'" << tc->name());
      ATH_MSG_INFO("  Store:'" << tc->getDetail<std::string>("store") << "'");
      ATH_MSG_INFO("  View ID:" << tc->getDetail<int>("view"));
      ATH_MSG_INFO("  CPU Time:" << tc->getDetail<float>("CPUTime") << " ms");
    }
  }
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool TrigCostMTSvc::isMonitoredEvent(const EventContext& context) {
  m_eventMonitored.grow_to_at_least( context.slot() + 1, 0);
  return m_eventMonitored.at( context.slot() );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
