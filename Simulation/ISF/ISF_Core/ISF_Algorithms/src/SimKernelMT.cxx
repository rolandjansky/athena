/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
    m_simulationTools(), // FIXME make private
    m_particleKillerTool(""), // FIXME make private
    m_geoIDSvc("", name),
    m_simSelectors() //FIXME make private
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
    declareProperty("BeamPipeSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasForward] );
    declareProperty("IDSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasID] );
    declareProperty("CaloSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasCalo] );
    declareProperty("MSSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasMS] );
    declareProperty("CavernSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasCavern] );

    declareProperty("SimulationTools", m_simulationTools );

    declareProperty("ParticleKillerTool",
                    m_particleKillerTool,
                    "Simulation Tool to use when particle is not selected by any SimulationSelector in the routing chain.");
    declareProperty("GeoIDSvc",
                    m_geoIDSvc,
                    "Since InputConverter doesn't set Geo ID yet, do it here");
}


ISF::SimKernelMT::~SimKernelMT() {
}


StatusCode ISF::SimKernelMT::initialize() {

  ATH_CHECK( m_simulationTools.retrieve() );
  for (auto& curSimTool: m_simulationTools) {
    if ( curSimTool ) {
      const auto flavor = curSimTool->simFlavor();
      auto itr = m_simToolMap.find(flavor);
      if (itr != m_simToolMap.end() )
        {
          ATH_MSG_FATAL("Two ISimulatorTool instances (" << itr->second->name() << "," << curSimTool->name() << ") with the same flavor in this job!\n Check your configuration!");
          return StatusCode::FAILURE;
        }
      // New flavour add it to the map.
      m_simToolMap[flavor] = &*curSimTool;
    }
  }
  ATH_CHECK( m_particleKillerTool.retrieve() );
  const ISF::SimulationFlavor pkFlavor = m_particleKillerTool->simFlavor();
  if ( m_simToolMap.find(pkFlavor) == m_simToolMap.end() )
    {
      m_simToolMap[pkFlavor] = &*m_particleKillerTool;
    }
  else
    {
      ATH_MSG_WARNING("Two ISimulatorTool instances (" << m_simToolMap.find(ISF::ParticleKiller)->second->name() << "," << m_particleKillerTool->name() << ") with the same flavor in this job!\n Check your configuration!");
    }

  ATH_MSG_INFO("The following Simulators will be used in this job: \t" << m_simulationTools << "\n" << m_particleKillerTool);
  // retrieve simulation selectors (i.e. the "routing chain")
  for ( auto& selectorsToolHandleArray: m_simSelectors ) {
    ATH_CHECK( selectorsToolHandleArray.retrieve() );
  }

  // info screen output
  ATH_MSG_INFO("The following routing chains are defined:");
  for ( short geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions ; ++geoID) {
    auto& localSelectors = m_simSelectors[geoID];
    ATH_MSG_INFO( AtlasDetDescr::AtlasRegionHelper::getName(geoID)
                  << " (GeoID=" << geoID << "): \t" << localSelectors);
    for (auto& selector: localSelectors)
      {
        const auto flavor = selector->simFlavor();
        auto itr = m_simToolMap.find(flavor);
        if (itr == m_simToolMap.end() )
          {
            ATH_MSG_WARNING("Map from SimulationFlavor to SimulatorTool:");
            for(auto& entry: m_simToolMap)
              {
                ATH_MSG_WARNING("SimulationFlavor: " << entry.first << ", SimulatorTool Name: "<< entry.second->name());
              }
            ATH_MSG_FATAL("No SimulationTool with flavor " << flavor << " expected by " << selector->name() << " found in this job!\n Check your configuration!");
            return StatusCode::FAILURE;
          }
      }
  }

  ATH_CHECK( m_inputEvgenKey.initialize() );
  ATH_CHECK( m_outputTruthKey.initialize() );

  ATH_CHECK( m_inputConverter.retrieve() );

  ATH_CHECK( m_geoIDSvc.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode ISF::SimKernelMT::execute() {

  // Release the event from all simulators (TODO: make the tools do this)
  for (auto& curSimTool: m_simulationTools) {
    if ( curSimTool ) {
      ATH_CHECK(curSimTool->setupEvent());
      ATH_MSG_DEBUG( "Event setup done for " << curSimTool->name() );
    }
  }

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
  ATH_CHECK( m_inputConverter->convert(*outputTruth, simParticles, HepMcParticleLink::find_enumFromKey(outputTruth.name())) );

  // loop until there are no more particles to simulate
  while (simParticles.size() ) {
    ISF::ConstISFParticleVector particles{};
    ISimulatorTool* lastSimulator{};
    while ( simParticles.size() ) {
      auto particlePtr =  simParticles.front();
      ISFParticle& curParticle(*particlePtr);
      simParticles.pop_front();

      // Get the geo ID for the particle (should happen in inputConverter really)
      /*AtlasDetDescr::AtlasRegion geoID =*/ m_geoIDSvc->identifyAndRegNextGeoID(curParticle);

      auto& simTool = identifySimulator(curParticle);
      if (&simTool==lastSimulator){ particles.push_back(particlePtr); }
      else {
        if (not particles.empty()) {
          ISFParticleContainer newSecondaries;
          //ATH_CHECK( simTool.simulate( std::move(curParticle), newSecondaries ) );
          ATH_CHECK( simTool.simulateVector( particles, newSecondaries, outputTruth.ptr() ) );
          // add any returned ISFParticles to the list of particles to be simulated
          simParticles.splice( end(simParticles), std::move(newSecondaries) );
          // delete simulated particles
          for (auto usedParticle: particles) {
            ISF::ISFParticle *curPart = const_cast<ISF::ISFParticle*>(usedParticle); //FIXME const_cast badness
            delete curPart;
          }
          particles.clear();
        }
        particles.push_back(particlePtr);
        lastSimulator=&simTool;
      }
    }
    //clean-up unsimulated particles
    if (not particles.empty()) {
      ISFParticleContainer newSecondaries;
      if(!lastSimulator) { ATH_MSG_FATAL("Particles with no assigned simulator. Bail!"); return StatusCode::FAILURE; }
      ATH_CHECK( lastSimulator->simulateVector( particles, newSecondaries, outputTruth.ptr() ) );
      // add any returned ISFParticles to the list of particles to be simulated
      simParticles.splice( end(simParticles), std::move(newSecondaries) );
      // delete simulated particles
      for (auto usedParticle: particles) {
        ISF::ISFParticle *curPart = const_cast<ISF::ISFParticle*>(usedParticle); //FIXME const_cast badness
        delete curPart;
      }
      particles.clear();
    }
  }
  // Release the event from all simulators (TODO: make the tools do this)
  for (auto& curSimTool: m_simulationTools) {
    if ( curSimTool ) {
      ATH_CHECK(curSimTool->releaseEvent());
      ATH_MSG_DEBUG( "releaseEvent() completed for " << curSimTool->name() );
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode ISF::SimKernelMT::finalize() {
  return StatusCode::SUCCESS;
}


/// Returns the simulator to use for the given particle
ISF::ISimulatorTool& ISF::SimKernelMT::identifySimulator(const ISF::ISFParticle& particle) const {
  AtlasDetDescr::AtlasRegion geoID = particle.nextGeoID();

  auto& localSelectors = m_simSelectors[geoID];
  for (auto& selector: localSelectors) {
    bool selected = selector->selfSelect(particle);
    if (selected) {
      return *m_simToolMap.at(selector->simFlavor());
    }
  }

  ATH_MSG_WARNING("No simulator found for particle (" << particle << ")."
                  << " Will send it to " << m_particleKillerTool->name());
  return *m_particleKillerTool;
}
