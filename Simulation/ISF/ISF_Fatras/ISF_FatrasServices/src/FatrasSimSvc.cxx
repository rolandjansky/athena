/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportSimSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ISF_FatrasServices/FatrasSimSvc.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"
// Remaining ISF include
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Interfaces/IParticleFilter.h"
#include "ISF_Event/ISFParticle.h"
#include "ISF_FatrasInterfaces/ITransportTool.h"

/** Constructor **/
iFatras::FatrasSimSvc::FatrasSimSvc(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_simulationTool(""),
  m_particleFilter("")
{
  // retrieve the simulation tool and the transport tool
  declareProperty("SimulationTool",   m_simulationTool);
  declareProperty("ParticleFilter",   m_particleFilter); 
}

iFatras::FatrasSimSvc::~FatrasSimSvc() 
{}

/** framework methods */
StatusCode iFatras::FatrasSimSvc::initialize()
{
   ATH_MSG_INFO ( m_screenOutputPrefix << "Initializing ...");
   // retrieve simulation tool
   if ( retrieveTool<iFatras::ITransportTool>(m_simulationTool).isFailure() ) 
       return StatusCode::FAILURE;
   // retrieve particle filter
   if ( !m_particleFilter.empty() && retrieveTool<ISF::IParticleFilter>(m_particleFilter).isFailure())
       return StatusCode::FAILURE;

   return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode iFatras::FatrasSimSvc::finalize()
{
    ATH_MSG_INFO ( m_screenOutputPrefix << "Finalizing ...");
    return StatusCode::SUCCESS;
}

StatusCode iFatras::FatrasSimSvc::setupEvent()
{ 
  ATH_MSG_DEBUG ( m_screenOutputPrefix << "setup Event");
  return StatusCode::SUCCESS;
}  

StatusCode iFatras::FatrasSimSvc::releaseEvent() 
{ 
  ATH_MSG_DEBUG ( m_screenOutputPrefix << "release Event");  
  return StatusCode::SUCCESS; 
}

/** Simulation Call */
StatusCode iFatras::FatrasSimSvc::simulate(const ISF::ISFParticle& isp)
{

  // give a screen output that you entered FatrasSimSvc
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Particle " << isp << " received for simulation." );

  // now decide what to do with the particle
  if (!m_particleFilter.empty() && !m_particleFilter->passFilter(isp)){
      ATH_MSG_VERBOSE( m_screenOutputPrefix << "ISFParticle " << isp << " does not pass selection. Ignore.");
      return StatusCode::SUCCESS;
  }
  /** Process Particle from particle broker */
  ISF::ISFParticle* newIsp = m_simulationTool->process(isp);
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Simulation created : " << ( newIsp ? "" : "no") << " new particle");
  
  if (newIsp) {
    // new particle into the stack
    m_particleBroker->push(newIsp, &isp);
  }

  // Fatras call done
  return StatusCode::SUCCESS;
}


