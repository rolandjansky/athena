/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./FatrasSimTool.h"

ISF::FatrasSimTool::FatrasSimTool(const std::string& type, const std::string& name,  const IInterface* parent)
  : BaseSimulatorTool(type, name, parent)
{
  declareProperty("UseSimulationTool",  m_useExtrapolator);
}

ISF::FatrasSimTool::~FatrasSimTool() {

}

StatusCode ISF::FatrasSimTool::initialize() {

  ATH_CHECK( BaseSimulatorTool::initialize() );

  // retrieve simulation tool
  ATH_CHECK( m_IDsimulationTool.retrieve() );

  // retrieve simulation tool
  if ( m_useExtrapolator ) {
    ATH_CHECK( m_simulationTool.retrieve() );
  }
  else {
    m_simulationTool.disable();
    ATH_MSG_INFO ( "Using only Extrapolation Engine Tools...");
  }

  // retrieve particle filter
  if ( !m_particleFilter.empty() ) {
    ATH_CHECK( m_particleFilter.retrieve() );
  }
  return StatusCode::SUCCESS;
}

StatusCode ISF::FatrasSimTool::simulate( const ISFParticle& isp, ISFParticleContainer& secondaries, McEventCollection* ) const {

  // give a screen output that you entered FatrasSimSvc
  ATH_MSG_VERBOSE( "Particle " << isp << " received for simulation." );

  // now decide what to do with the particle
  if (!m_particleFilter.empty() && !m_particleFilter->passFilter(isp)){
      ATH_MSG_VERBOSE( "ISFParticle " << isp << " does not pass selection. Ignore.");
      return StatusCode::SUCCESS;
  }
  /** Process Particle from particle broker */
  ISF::ISFParticle* newIsp = (isp.nextGeoID()==AtlasDetDescr::fAtlasID || isp.nextGeoID()==AtlasDetDescr::fAtlasForward) ? m_IDsimulationTool->process(isp) : m_simulationTool->process(isp);
  ATH_MSG_VERBOSE( "Simulation created : " << ( newIsp ? "" : "no") << " new particle");

  if (newIsp) {
    // new particle into the stack
    secondaries.push_back(newIsp);
  }

  // Fatras call done
  return StatusCode::SUCCESS;
}
