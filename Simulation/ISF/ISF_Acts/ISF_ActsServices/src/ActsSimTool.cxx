/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./ActsSimTool.h"

ISF::ActsSimTool::ActsSimTool(const std::string& type, const std::string& name,
                              const IInterface* parent)
    : BaseSimulatorTool(type, name, parent) {
  declareProperty("UseSimulationTool", m_useExtrapolator);
}

ISF::ActsSimTool::~ActsSimTool() {}

StatusCode ISF::ActsSimTool::initialize() {
  ATH_CHECK(BaseSimulatorTool::initialize());

  // retrieve simulation tool
  ATH_CHECK(m_IDsimulationTool.retrieve());

  // retrieve simulation tool
  if (m_useExtrapolator) {
    ATH_CHECK(m_simulationTool.retrieve());
  } else {
    m_simulationTool.disable();
    ATH_MSG_INFO("Using only Extrapolation Engine Tools...");
  }

  // retrieve particle filter
  if (!m_particleFilter.empty()) {
    ATH_CHECK(m_particleFilter.retrieve());
  }
  return StatusCode::SUCCESS;
}

StatusCode ISF::ActsSimTool::simulate(const ISFParticle& isp,
                                      ISFParticleContainer& secondaries,
                                      McEventCollection*) const {
  // same step as Fatras sim tool
  // give a screen output that you entered ActsSimSvc
  ATH_MSG_VERBOSE("Particle " << isp << " received for simulation.");

  // now decide what to do with the particle
  if (!m_particleFilter.empty() && !m_particleFilter->passFilter(isp)) {
    ATH_MSG_VERBOSE("YIFEI ISFParticle "
                    << isp << " does not pass selection. Ignore.");
    return StatusCode::SUCCESS;
  }
  /** Process Particle from particle broker */
  ISF::ISFParticle* newIsp = (isp.nextGeoID() == AtlasDetDescr::fAtlasID ||
                              isp.nextGeoID() == AtlasDetDescr::fAtlasForward)
                                 ? m_IDsimulationTool->process(isp)
                                 : m_simulationTool->process(isp);
  ATH_MSG_VERBOSE("Simulation created : " << (newIsp ? "" : "no")
                                          << " new particle");

  if (newIsp) {
    // new particle into the stack
    secondaries.push_back(newIsp);
  }

  // Acts call done
  return StatusCode::SUCCESS;
}
