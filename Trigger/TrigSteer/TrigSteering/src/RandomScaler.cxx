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

namespace {


struct Fnv_hash4
{
  static std::uint32_t
  hash(const char* first, std::size_t length)
  {
    std::uint32_t result = static_cast<std::uint32_t>(2166136261UL);
    for (; length > 0; --length)
    {
      result ^= (std::uint32_t)*first++;
      result *= 16777619UL;
    }
    return result;
  }
};

} //anonymous namespace

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
    msg() << MSG::ERROR << "Could not initialize Random Number Service" << endmsg;
    return StatusCode::FAILURE;
  }

  m_engine = dynamic_cast<CLHEP::Ranlux64Engine*>(rngSvc->GetEngine(name()));
  if (m_engine==0) {
    msg() << MSG::ERROR << "Random number engine not of type Ranlux64Engine" << endmsg;
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
      msg() << MSG::ERROR << "Cannot retrieve IncidentSvc" << endmsg;
      return StatusCode::FAILURE;
    }
    incSvc->addListener(this, IncidentType::BeginEvent);
    msg() << MSG::INFO << "Re-seeding RNG on every event from time stamp and fixed seed "
          << m_seed << " with luxury level " << m_engine->getLuxury() << endmsg;
  }
  else {
    msg() << MSG::INFO << "Using random seed = " << m_seed << endmsg;
  }
  
  return StatusCode::SUCCESS;
}


void HLT::RandomScaler::handle(const Incident& inc)
{
  if (inc.type()==IncidentType::BeginEvent) {
    const EventIncident* eventInc  = dynamic_cast<const EventIncident*>(&inc);
    if (!eventInc) {
      msg() << MSG::ERROR << "Cannot cast BeginEvent incident to EventIncident." << endmsg;
      return;
    }
    EventContext context = inc.context();
    m_seedInput[SEED_SEC]  = context.eventID().time_stamp();
    m_seedInput[SEED_NSEC] = context.eventID().time_stamp_ns_offset();

    /* Generate hash-based seed from event quantities.
     * Ranlux64 only supports signed 32bit seeds ('long' on i686)
     */
    int32_t seed = static_cast<int32_t>(Fnv_hash4::hash((char*)m_seedInput, sizeof(long)*N_SEED_INPUTS));

    if (msgLvl(MSG::VERBOSE)) {
      msg() << MSG::VERBOSE << "Setting random number seed = " << seed << endmsg;
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
        msg() << MSG::INFO << "Using application-specific random seed = " << m_seed << endmsg;
      }
      else msg() << MSG::WARNING << "Could not set Property 'seed' from DataFlowConfig.DF_RandomSeed" << endmsg;
    }
  }
  else msg() << MSG::WARNING << "Could not retrieve JobOptionsSvc to read random seed. Using default value." << endmsg;
}


bool HLT::RandomScaler::decision(float factor)
{
  if (m_engine) {
    double rand = CLHEP::RandFlat::shoot(m_engine);
    return (factor*rand < 1);
  }
  else {
    msg() << MSG::ERROR << "Engine not initialized! Returning false" << endmsg;
    return false;
  }
}
