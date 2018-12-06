/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "FastCaloSimSvc.h"
#include "ISF_Interfaces/IParticleBroker.h"

/** Constructor **/
ISF::FastCaloSimSvc::FastCaloSimSvc(const std::string& name,ISvcLocator* svc) :
  BaseSimulationSvc(name, svc)
{
}

ISF::FastCaloSimSvc::~FastCaloSimSvc()
{}

/** framework methods */
StatusCode ISF::FastCaloSimSvc::initialize()
{
  ATH_MSG_INFO ( m_screenOutputPrefix << "Initializing ...");

  ATH_CHECK ( m_simulatorTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvc::setupEvent()
{
  return m_simulatorTool->setupEventST();
}

StatusCode ISF::FastCaloSimSvc::releaseEvent()
{
  return m_simulatorTool->releaseEventST();
}

/** Simulation Call */
StatusCode ISF::FastCaloSimSvc::simulate(const ISF::ISFParticle& isfp)
{
  ISF::ISFParticleContainer secondaries;
  ATH_CHECK(m_simulatorTool->simulate(isfp, secondaries));
  if (not secondaries.empty()) {
    for (auto particle : secondaries) {
      m_particleBroker->push( particle, &isfp);
    }
  }
  return StatusCode::SUCCESS;
}
