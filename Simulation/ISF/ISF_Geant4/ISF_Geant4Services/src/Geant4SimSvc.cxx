/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "Geant4SimSvc.h"

/** Constructor **/
iGeant4::Geant4SimSvc::Geant4SimSvc(const std::string& name,ISvcLocator* svc) :
  BaseSimulationSvc(name, svc)
{
}

iGeant4::Geant4SimSvc::~Geant4SimSvc()
{}

/** framework methods */
StatusCode iGeant4::Geant4SimSvc::initialize()
{
  ATH_CHECK (m_simulatorTool.retrieve());
  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode iGeant4::Geant4SimSvc::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode iGeant4::Geant4SimSvc::setupEvent()
{
  return m_simulatorTool->setupEventST();
}

StatusCode iGeant4::Geant4SimSvc::releaseEvent()
{
  return m_simulatorTool->releaseEventST();
}

/** Simulation Call */
StatusCode iGeant4::Geant4SimSvc::simulate(const ISF::ISFParticle& isp, McEventCollection* mcEventCollection)
{
  ISF::ISFParticleContainer secondaries; // filled, but not used
  ATH_CHECK(m_simulatorTool->simulate(isp, secondaries, mcEventCollection));
  return StatusCode::SUCCESS;
}

/** Simulation Call */
StatusCode iGeant4::Geant4SimSvc::simulateVector(const ISF::ConstISFParticleVector& particles, McEventCollection* mcEventCollection)
{
  ISF::ISFParticleContainer secondaries; // filled, but not used
  ATH_CHECK (m_simulatorTool->simulateVector(particles,secondaries, mcEventCollection));
  return StatusCode::SUCCESS;
}

