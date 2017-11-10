/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch <Elmar.Ritsch@cern.ch>
 * @date October 2016
 * @brief Core Athena algorithm for the Integrated Simulation Framework
 */

#include "SimKernelMT.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"


#include "AtlasDetDescr/AtlasRegionHelper.h"

// STL
#include <queue>


ISF::SimKernelMT::SimKernelMT( const std::string& name, ISvcLocator* pSvcLocator ) :
    ::AthAlgorithm( name, pSvcLocator ),
    m_inputEvgenKey(),
    m_outputTruthKey(),
    m_inputConverter("", name),
    m_simSelectors(),
    m_particleKillerSimulationSvc("", name)
{
    declareProperty("InputEvgenCollection",
                    m_inputEvgenKey,
                    "Input EVGEN collection.");
    declareProperty("OutputTruthCollection",
                    m_outputTruthKey,
                    "Output Truth collection.");
    declareProperty("InputConverter",
                    m_inputConverter,
                    "Input McEventCollection->ISFParticleContainer conversion service.");

    // routing tools
    declareProperty("ForwardBeamPipeSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasForward] );
    declareProperty("InDetSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasID] );
    declareProperty("CaloSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasCalo] );
    declareProperty("MSSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasMS] );
    declareProperty("CavernSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasCavern] );

    declareProperty("ParticleKillerSimulationSvc",
                    m_particleKillerSimulationSvc,
                    "Simulation Service to use when particle is not selected by any SimulationSelector in the routing chain.");
}


ISF::SimKernelMT::~SimKernelMT() {
}


StatusCode ISF::SimKernelMT::initialize() {

  // retrieve simulation selectors (i.e. the "routing chain")
  for ( short geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions ; ++geoID) {
    ATH_CHECK( m_simSelectors[geoID].retrieve() );
  }

  // info screen output
  ATH_MSG_INFO("The following routing chains are defined:");
  for ( short geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions ; ++geoID) {
    ATH_MSG_INFO( AtlasDetDescr::AtlasRegionHelper::getName(geoID)
                  << " (GeoID=" << geoID << "): \t" << m_simSelectors[geoID]);
  }

  ATH_CHECK( m_inputEvgenKey.initialize() );
  ATH_CHECK( m_outputTruthKey.initialize() );

  ATH_CHECK( m_particleKillerSimulationSvc.retrieve() );
  ATH_CHECK( m_inputConverter.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode ISF::SimKernelMT::execute() {

  SG::ReadHandle<McEventCollection> inputEvgen(m_inputEvgenKey);
  if (!inputEvgen.isValid()) {
    ATH_MSG_FATAL("Unable to read input GenEvent collection '" << inputEvgen.key() << "'");
    return StatusCode::FAILURE;
  }

  // copy input Evgen collection to output Truth collection
  SG::WriteHandle<McEventCollection> outputTruth(m_outputTruthKey);
  outputTruth = CxxUtils::make_unique<McEventCollection>(*inputEvgen);

  // read and convert input
  ISFParticleContainer simParticles; // particles for ISF simulation
  bool isPileup = false;
  ATH_CHECK( m_inputConverter->convert(*outputTruth, simParticles, isPileup) );
  // loop until there are no more particles to simulate
  while ( simParticles.size() ) {
    ISFParticle* curParticle = simParticles.back();
    simParticles.pop_back();
    auto& simSvc = identifySimulator(*curParticle);

    ATH_CHECK( simSvc.simulate( std::move(*curParticle) ).isSuccess() );
    // TODO: this is work in progress and newSecondaries should actually be
    // returned by the simulate() call above
    ISFParticleContainer newSecondaries;

    simParticles.splice( end(simParticles), std::move(newSecondaries) );
  }
  return StatusCode::SUCCESS;
}


StatusCode ISF::SimKernelMT::finalize() {
  return StatusCode::SUCCESS;
}


/// Returns the simulator to use for the given particle
ISF::ISimulationSvc& ISF::SimKernelMT::identifySimulator(const ISF::ISFParticle& particle) const {
  AtlasDetDescr::AtlasRegion geoID = particle.nextGeoID();
  auto& localSelectors = m_simSelectors[geoID];
  for (const auto& selector: localSelectors) {
    bool selected = selector->selfSelect(particle);
    if (selected) {
      return **selector->simulator();
    }
  }

  ATH_MSG_WARNING("No simulator found for particle (" << particle << ")."
      << " Will send it to " << m_particleKillerSimulationSvc);
  return *m_particleKillerSimulationSvc;
}
