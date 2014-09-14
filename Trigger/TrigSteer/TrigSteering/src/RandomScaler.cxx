/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : RandomScaler
 *
 * @brief Random prescaler
 * $Id: RandomScaler.cxx,v 1.5 2008-05-06 09:19:19 tbold Exp $
 **********************************************************************************/

#include "TrigSteering/RandomScaler.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/Ranlux64Engine.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventIncident.h"
#include "CxxUtils/hashtable.h"

const short DEFAULT_LUXURY(2);

HLT::RandomScaler::RandomScaler( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  :  ReusableScaler( type, name, parent ),
     m_engine(0), 
     m_seed(0)
{
  m_seedInput[0] = 0;
  m_seedInput[1] = 0;
  m_seedInput[2] = 0;
  
  declareProperty( "useNodeIDSeed", m_nodeIDSeed = false, "Use node ID as seed");
  declareProperty( "useEventSeed", m_eventSeed = false, "Use seed derived from event");  
}

StatusCode HLT::RandomScaler::initialize()
{
  StatusCode sc = ReusableScaler::initialize();
  if (sc.isFailure()) return sc;

  ServiceHandle<IAtRndmGenSvc> rngSvc("AtRanluxGenSvc",name());

  if ( rngSvc.retrieve().isFailure() ) {
    msg() << MSG::ERROR << "Could not initialize Random Number Service" << endreq;
    return StatusCode::FAILURE;
  }

  m_engine = dynamic_cast<CLHEP::Ranlux64Engine*>(rngSvc->GetEngine(name()));
  if (m_engine==0) {
    msg() << MSG::ERROR << "Random number engine not of type Ranlux64Engine" << endreq;
    return StatusCode::FAILURE;
  }
      
  // Fixed input to seed calculation
  m_seed = m_engine->getSeed();

  // Set 'seed' from nodeID if available
  if (m_nodeIDSeed) setSeedFromDataflow();
  
  m_seedInput[SEED_FIXED] = m_seed;

  if (m_eventSeed) {
    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
    if (incSvc.retrieve().isFailure()) {
      msg() << MSG::ERROR << "Cannot retrieve IncidentSvc" << endreq;
      return StatusCode::FAILURE;
    }
    incSvc->addListener(this, IncidentType::BeginEvent);
    msg() << MSG::INFO << "Re-seeding RNG on every event from time stamp and fixed seed "
          << m_seed << " with luxury level " << m_engine->getLuxury() << endreq;
  }
  else {
    msg() << MSG::INFO << "Using random seed = " << m_seed << endreq;
  }
  
  return StatusCode::SUCCESS;
}


void HLT::RandomScaler::handle(const Incident& inc)
{
  if (inc.type()==IncidentType::BeginEvent) {
    const EventInfo* event(0);
    const EventIncident* eventInc  = dynamic_cast<const EventIncident*>(&inc);
    if (eventInc) event = &eventInc->eventInfo();
    else {
      msg() << MSG::ERROR << "Cannot retrieve EventInfo object from BeginEvent incident." << endreq;
      return;
    }

    m_seedInput[SEED_SEC]  = event->event_ID()->time_stamp();
    m_seedInput[SEED_NSEC] = event->event_ID()->time_stamp_ns_offset();

    /* Generate hash-based seed from event quantities.
     * Ranlux64 only supports signed 32bit seeds ('long' on i686)
     */
    int32_t seed = static_cast<int32_t>(SG::Fnv_hash<4>::hash((char*)m_seedInput, sizeof(long)*N_SEED_INPUTS));

    if (msgLvl(MSG::VERBOSE)) {
      msg() << MSG::VERBOSE << "Setting random number seed = " << seed << endreq;
    }
    m_engine->setSeed(seed, /*seed lux*/ DEFAULT_LUXURY);  // seed luxury level is separate from RNG luxury level
  }
}


void HLT::RandomScaler::setSeedFromDataflow()
{
  // Check if application-specific random seed is provided via DataFlowConfig
  ServiceHandle<IJobOptionsSvc> jobOptSvc("JobOptionsSvc", name());
  if ( jobOptSvc.retrieve().isSuccess() ) {
    const Property* p = Gaudi::Utils::getProperty( jobOptSvc->getProperties("DataFlowConfig"), "DF_RandomSeed");
    if ( p!=0 ) {   // Partition running
      IntegerProperty seed;
      if ( seed.assign(*p) ) {
        m_seed = seed;
        msg() << MSG::INFO << "Using application-specific random seed = " << m_seed << endreq;
      }
      else msg() << MSG::WARNING << "Could not set Property 'seed' from DataFlowConfig.DF_RandomSeed" << endreq;
    }
  }
  else msg() << MSG::WARNING << "Could not retrieve JobOptionsSvc to read random seed. Using default value." << endreq;
}


bool HLT::RandomScaler::decision(float factor)
{
  if (m_engine) {
    double rand = CLHEP::RandFlat::shoot(m_engine);
    return (factor*rand < 1);
  }
  else {
    msg() << MSG::ERROR << "Engine not initialized! Returning false" << endreq;
    return false;
  }
}
