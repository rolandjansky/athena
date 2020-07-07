/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorModules/GenModule.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/Incident.h"
#include "CLHEP/Random/Ranlux64Engine.h"
#include <fstream>


GenModule::GenModule(const std::string& name, ISvcLocator* pSvcLocator)
  : GenBase(name, pSvcLocator),
    m_rndmGenSvc("AtRndmGenSvc", name),
    m_incidentSvc("IncidentSvc", name)
{
  m_mkMcEvent = true;
  //declareProperty("MakeMcEvent", m_mkMcEvent=true, "Create a new MC event collection if it doesn't exist");
  declareProperty("RandomSeed", m_randomSeed=1234567, "Random seed for the built-in random engine");
  declareProperty("IsAfterburner", m_isAfterburner=false, "Set true if generator modifies existing events rather than creating new ones");
  declareProperty("AtRndmGenSvc", m_rndmGenSvc);
  declareProperty("IncidentSvc", m_incidentSvc);
}


StatusCode GenModule::initialize() {
  // Base class initializations
  CHECK(GenBase::initialize());
  // Get the random number service
  if (m_rndmGenSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not initialize ATLAS Random Generator Service");
    return StatusCode::FAILURE;
  }
  // Get the incident service
  if (m_incidentSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not initialize Incident Service");
    return StatusCode::FAILURE;
  }
  CHECK(genInitialize());
  CHECK(genuserInitialize());
  return StatusCode::SUCCESS;
}


CLHEP::HepRandomEngine& GenModule::randomEngine() {
  if (m_pRandomEngine.get() == 0) {
    m_pRandomEngine.reset( new CLHEP::Ranlux64Engine(m_randomSeed) );
  }
  return *m_pRandomEngine;
}


StatusCode GenModule::execute() {
  // Call the code that generates an event
  CHECK(this->callGenerator());

  // Create the MC event and send the GeneratorEvent stored in it to fillEvt
  HepMC::GenEvent* evt = HepMC::newGenEvent(1,1);
  CHECK(this->fillEvt(evt));

  // Add the event to the MC event collection
   if (events()) {
    // If this is an "afterburner" generator, replace the last event rather than add a new one
    /// @todo Remove hard-coded alg name checking (already incomplete)
        if (m_isAfterburner || name() == "Tauola" || name() == "Photos") {
         events()->pop_back();
     }
    // Add the event to the end of the collection
    events()->push_back(evt);
    ATH_MSG_DEBUG("MC event added to McEventCollection");

   // remove the empty event in case of ParticleDecayer
    if (name() == "ParticleDecayer")
        events()->pop_back();
      }

  // Call the incident service to notify that an event has been made
  m_incidentSvc->fireIncident( Incident(name(), "McEventGenerated") );
  return StatusCode::SUCCESS;
}
