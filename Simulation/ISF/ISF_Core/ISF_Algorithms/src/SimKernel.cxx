/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ISF_Algs includes
#include "SimKernel.h"
// ISF_Event includes
#include "ISF_Event/ISFParticle.h"
// ISF_Interfaces includes
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Interfaces/ISimulationSvc.h"
#include "ISF_Interfaces/IMonitoringTool.h"
#include "ISF_Interfaces/IEventFilterTool.h"
// FrameWork includes
#include "GaudiKernel/Property.h"
// Boost
#include <boost/lexical_cast.hpp>
// ATLAS cxx utils
#include "CxxUtils/make_unique.h"
#include "PmbCxxUtils/CustomBenchmark.h"
// ROOT includes
#include "TTree.h"
// DetectorDescription
#include "AtlasDetDescr/AtlasRegionHelper.h"
// GeneratorObjects
#include "GeneratorObjects/McEventCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ISF::SimKernel::SimKernel( const std::string& name, ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_inputHardScatterEvgen(),
  m_inputPileupEvgen(),
  m_outputHardScatterTruth(),
  m_outputPileupTruth(),
  m_inputConverter("",name),
  m_particleBroker("ISF_ParticleBroker", name),
  m_truthRecordSvc("ISF_TruthRecordSvc", name),
  m_qspatcher("", name),
  m_doMemMon(true),
  m_memMon("MemMonitoringTool"),
  m_memUsageEvts(1000),
  m_simSvcs(ISF::fMaxNumAtlasSimIDs),
  m_simSvcNames(ISF::fMaxNumAtlasSimIDs),
  m_numSimSvcs(ISF::fFirstAtlasSimID), // ==1 since UndefinedSimID is always there
  m_numISFEvents(0),
  m_doCPUMon(true),
  //m_benchPDGCode(0), TODO: implement this if feasible
  //m_benchGeoID(0), TODO: implement this if feasible
  m_benchSimID(0),
  m_numParticles(0),
  m_maxParticleVectorSize(10240)
{
    declareProperty("InputHardScatterCollection",
                    m_inputHardScatterEvgen,
                    "Input Hard Scatter EVGEN collection.");
    declareProperty("InputPileupCollection",
                     m_inputPileupEvgen,
                    "Input Pileup EVGEN collection.");
    declareProperty("OutputHardScatterTruthCollection",
                    m_outputHardScatterTruth,
                    "Output Hard Scatter Truth collection.");
    declareProperty("OutputPileupTruthCollection",
                     m_outputPileupTruth,
                    "Output Pileup Truth collection.");
    declareProperty("InputConverter",
                    m_inputConverter,
                    "Input McEventCollection->ISFParticleContainer conversion service.");

    // the general services and tools needed
    declareProperty("ParticleBroker"             , m_particleBroker                  );
    declareProperty("TruthRecordService"         , m_truthRecordSvc                  );
    declareProperty("DoCPUMonitoring"            , m_doCPUMon                        );
    declareProperty("DoMemoryMonitoring"         , m_doMemMon                        );
    declareProperty("MemoryMonitoringTool"       , m_memMon                          );
    declareProperty("SummarizeMemUsageEveryNEvts", m_memUsageEvts                    );
    // routing tool
    declareProperty("BeamPipeSimulationSelectors", m_simSelectors[AtlasDetDescr::fAtlasForward]  );
    declareProperty("IDSimulationSelectors"      , m_simSelectors[AtlasDetDescr::fAtlasID]       );
    declareProperty("CaloSimulationSelectors"    , m_simSelectors[AtlasDetDescr::fAtlasCalo]     );
    declareProperty("MSSimulationSelectors"      , m_simSelectors[AtlasDetDescr::fAtlasMS]       );
    declareProperty("CavernSimulationSelectors"  , m_simSelectors[AtlasDetDescr::fAtlasCavern]   );
    // Quasi-stable particle sim
    declareProperty("QuasiStablePatcher", m_qspatcher);
    // event filter
    declareProperty("EventFilterTools"           , m_eventFilters                      );
    // tuning parameters
    declareProperty("MaximumParticleVectorSize"  , m_maxParticleVectorSize             );
}

// Destructor
///////////////
ISF::SimKernel::~SimKernel()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ISF::SimKernel::initialize()
{

  ATH_MSG_VERBOSE ( "--------------------------------------------------------" );
  ATH_MSG_INFO( "Initializing the ISF KERNEL " );

  // setup memory monitoring Tool
  if ( m_doMemMon) {
    // memory monitoring tool given -> do memory monitoring
    ATH_CHECK( m_memMon.retrieve() );
    ATH_MSG_INFO( "- MemoryMonitoring  : " << m_memMon.typeAndName() );
    // record current memory usage
    m_memMon->recordCurrent("at beginning of SimKernel initialize()");
  }


  // setup CPU Benchmarks
  if (m_doCPUMon) {
    //if (!m_benchPDGCode)
    //  m_benchPDGCode = new PMonUtils::CustomBenchmark(ISF::fMaxBenchmarkPDGCode);
    //if (!m_benchGeoID)
    //  m_benchGeoID   = new PMonUtils::CustomBenchmark(AtlasDetDescr::fNumAtlasRegions     );
    if (!m_benchSimID)
      m_benchSimID   = new PMonUtils::CustomBenchmark(ISF::fMaxNumAtlasSimIDs  );
  }

  // retrieve the stack service
  ATH_CHECK ( m_particleBroker.retrieve() );
  ATH_MSG_INFO( "- ParticleBroker   : " << m_particleBroker.typeAndName() );

  // the truth service
  ATH_CHECK ( m_truthRecordSvc.retrieve() );
  ATH_MSG_INFO( "- TruthRecordSvc   : " << m_truthRecordSvc.typeAndName() );

  // initialize all SimulationServices
  //
  for ( short geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions ; ++geoID) {
    ATH_CHECK ( initSimSvcs(m_simSelectors[geoID]) );
  }

  if(!m_qspatcher.empty()) {
    ATH_CHECK(m_qspatcher.retrieve());
  }

  // initialize all the EventFilterTools
  ATH_CHECK ( m_eventFilters.retrieve() );
  ATH_MSG_INFO( "The following Event Filters are defined:");
  ATH_MSG_INFO( m_eventFilters );

  // free unused space
  m_simSvcs.resize( m_numSimSvcs);
  m_simSvcNames.resize( m_numSimSvcs);
  // some screen output
  ATH_MSG_INFO ( "The following SimulationSvc are registered to ISF:");
  for (SimSvcID id=ISF::fFirstAtlasSimID; id<m_numSimSvcs; id++)
    ATH_MSG_INFO ( "ID: " << id << "\t  Name: '" << m_simSvcNames[id]
                   << "'");

  // setup the simulation selectors
  //
  for ( short geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions ; ++geoID) {
    ATH_CHECK ( m_particleBroker->registerSimSelector( m_simSelectors[geoID], (AtlasDetDescr::AtlasRegion)geoID) );
  }
  // screen output
  ATH_MSG_INFO( "The following routing chains are defined:");
  for ( short geoID = 0; geoID<AtlasDetDescr::fNumAtlasRegions ; ++geoID) {
    ATH_MSG_INFO( AtlasDetDescr::AtlasRegionHelper::getName(geoID)
                  << " (GeoID=" << geoID << "): \t" << m_simSelectors[geoID]);
  }

  // record current memory usage
  if (m_doMemMon) m_memMon->recordCurrent("at end of ISF SimKernel initialize()");

  // intialziation successful
  return StatusCode::SUCCESS;
}


StatusCode ISF::SimKernel::finalize()
{
  ATH_MSG_INFO ( "Finalizing ..." );

  // record current memory usage
  if (m_doMemMon) m_memMon->recordCurrent("at beginning of ISF SimKernel finalize()");

  // statistics: number of particles handled
  ATH_MSG_INFO(" Number of particles handled by the ISF SimKernel: " << m_numParticles );

  ATH_MSG_INFO(" ========================= ISF Timing Stats  =========================");
  // Benchmarking: by SimID
  if (m_benchSimID) {
    ATH_MSG_INFO("Breakdown of simulation loop by SimulatorID:");
    //TODO: for (unsigned simID=0;simID<m_benchSimID->size();++simID) {
    for (unsigned simID=0;simID<ISF::fMaxNumAtlasSimIDs;++simID) {
      uint64_t count;
      double time_ms;
      m_benchSimID->getData(simID, count, time_ms);
      if (count>0)
       ATH_MSG_INFO("  "<<std::setprecision(4)
                        <<m_simSvcNames[simID]<<" (id="<<simID<<")"
                        <<"\t\tn="<<count<<"\t\tt=" <<time_ms<<" ms\t\tt/n="<<time_ms/count<<" ms"
                        <<std::setprecision(-1) );
    }

    delete m_benchSimID;
    m_benchSimID=0;
  }

  //TODO: implement this if feasible
  // Benchmarking: by GeoID
  //if (m_benchGeoID) {
  //  ATH_MSG_INFO("Breakdown of simulation loop by GeoID:");
  //  //TODO: for (unsigned geoID=0;geoID<m_benchGeoID->size();++simID) {
  //  for (unsigned geoID=AtlasDetDescr::fFirstAtlasRegion;geoID<AtlasDetDescr::fNumAtlasRegions;++geoID) {
  //    uint64_t count;
  //    double time_ms;
  //    m_benchGeoID->getData(geoID, count, time_ms);
  //    if (count>0)
  //     ATH_MSG_INFO("  "<<std::setprecision(4)
  //                      <<AtlasDetDescr::AtlasRegionHelper::getName(geoID)<<" (id="<<geoID<<")"
  //                      <<"\t\tn="<<count<<"\t\tt=" <<time_ms<<" ms\t\tt/n="<<time_ms/count<<" ms"
  //                      <<std::setprecision(-1) );
  //  }

  //  delete m_benchGeoID;
  //  m_benchGeoID=0;
  //}

  //TODO: implement this if feasible
  // Benchmarking: by PDGCode
  //if (m_benchPDGCode) {
  //  ATH_MSG_INFO("Breakdown of simulation loop by PDG Particle Code:");
  //  //TODO: for (unsigned geoID=0;geoID<m_benchGeoID->size();++simID) {
  //  for (int pdgCode=ISF::fUndefinedPDGCode;pdgCode<ISF::fMaxBenchmarkPDGCode;++pdgCode) {
  //    uint64_t count;
  //    double time_ms;
  //    m_benchPDGCode->getData(pdgCode, count, time_ms);
  //    if (count>0)
  //     ATH_MSG_INFO( std::setprecision(4)
  //                   << "  |PDGCode|="<<pdgCode<<", n="
  //                   <<count<<", t="<<time_ms<<" ms, t/n="<<time_ms/count<<" ms"
  //                   <<std::setprecision(-1) );
  //  }

  //  delete m_benchPDGCode;
  //  m_benchPDGCode=0;
  //}

  // call the memory monitoring tool to print some memory stats
  if (m_doMemMon) {
    ATH_MSG_INFO(" ====================== ISF Memory Usage Stats =======================");
    m_memMon->dumpSummary("end of ISF event");
  }

  ATH_MSG_INFO(" =====================================================================");

  return StatusCode::SUCCESS;
}


StatusCode ISF::SimKernel::initSimSvcs( SimSelectorToolArray &simSelectorTools)
{
  // (1.) retrieve all SimulationSelector tools in the array
  if ( simSelectorTools.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Could not retrieve SimulatorSelector Tool Array. Abort." );
      return StatusCode::FAILURE;
  }

  // (2.) loop over SimulationSelector tool array and retrieve simulators
  SimSelectorToolArray::iterator fSimSelectorIter    = simSelectorTools.begin();
  SimSelectorToolArray::iterator fSimSelectorIterEnd = simSelectorTools.end();
  for ( ; fSimSelectorIter != fSimSelectorIterEnd; ++fSimSelectorIter ) {

    // take the simulator from the current SimulationSelector
    ServiceHandle<ISimulationSvc> *curSimulator = (*fSimSelectorIter)->simulator();

    if ( (*curSimulator).retrieve().isFailure() ){
        ATH_MSG_FATAL( "Could not retrieve SimulatorSelector Tool. Abort." );
        return StatusCode::FAILURE;
    } else
        ATH_MSG_INFO( "- SimulationSelector   : " << fSimSelectorIter->typeAndName() );

    // hand over particle broker to simulator
    if ( (*curSimulator)->setParticleBroker( &*m_particleBroker).isFailure() ){
      ATH_MSG_FATAL( "Unable to register ParticleService to SimulationService "
                                          << *curSimulator );
      return StatusCode::FAILURE;
    }

    // get the unique ID assigned to the Simulation Service
    SimSvcID curID = (*curSimulator)->simSvcID();
    // if no ID assigned yet -> new simulator
    if ( curID == ISF::fUndefinedSimID) {
      // assign a new new ID to the simulator
      (*curSimulator)->assignSimSvcID( m_numSimSvcs);
      // register current simulator to the simulatorArray
      m_simSvcs[m_numSimSvcs]     = (&**curSimulator);
      m_simSvcNames[m_numSimSvcs] = (*curSimulator)->simSvcDescriptor();
      ATH_MSG_DEBUG( "Assigned SimSvcID=" << m_numSimSvcs
                    << " to simulator '" << m_simSvcNames[m_numSimSvcs]
                    << "'");
      // increment the total number of simulators registered (=used as IDs)
      ++m_numSimSvcs;
    }

  } // loop over simulation Selectors


  return StatusCode::SUCCESS;
}


StatusCode ISF::SimKernel::execute()
{

  ATH_MSG_DEBUG ("Executing ...");

  // dump and record current memory stats
  if ( m_doMemMon && (m_numISFEvents==0) ) {
    m_memMon->dumpCurrent( "before 1st event", false );
    m_memMon->recordCurrent("before 1st event");
  }

  // read and convert input
  //  a. hard-scatter
  ISFParticleContainer simParticles{}; // particles for ISF simulation
  ATH_CHECK( prepareInput(m_inputHardScatterEvgen, m_outputHardScatterTruth, simParticles) );
  //  b. pileup
  if (!m_inputPileupEvgen.key().empty()) {
    ATH_CHECK( prepareInput(m_inputPileupEvgen, m_outputPileupTruth, simParticles) );
  }

  // -----------------------------------------------------------------------------------------------
  // Step 1: Initialize the particle stack and the TruthManager, ABORT if failure
  StatusCode sc = m_particleBroker->initializeEvent( std::move(simParticles) );
  if ( sc.isFailure() ){
      ATH_MSG_FATAL( "Failed to initialize Particle Broker. Abort." );
      return StatusCode::FAILURE;
  }
  if ( (m_truthRecordSvc->initializeTruthCollection()).isFailure() ){
      ATH_MSG_FATAL( "Failed to initialize TruthService. Abort." );
      return StatusCode::FAILURE;
  }
  // -----------------------------------------------------------------------------------------------


  // -----------------------------------------------------------------------------------------------
  // Step 2: Initialize the Event
  {
    std::vector<ISimulationSvc*>::iterator fSimSvcIter     = m_simSvcs.begin();
    std::vector<ISimulationSvc*>::iterator fSimSvcIterEnd  = m_simSvcs.end();
    for ( ; fSimSvcIter != fSimSvcIterEnd; ++fSimSvcIter ){
      ISimulationSvc *curSimSvc = (*fSimSvcIter);
      // if simulation with current flavour is registered
      //  -> setupEvent
      if ( curSimSvc){
        if( curSimSvc->setupEvent().isFailure() ) {
          ATH_MSG_WARNING( "Event setup failed for "
                           << curSimSvc->simSvcDescriptor() );
        } else {
          ATH_MSG_DEBUG  ( "Event setup done for "
                           << curSimSvc->simSvcDescriptor() );
        }
      }
    }
  }
  // -----------------------------------------------------------------------------------------------



  // -----------------------------------------------------------------------------------------------
  // Step 3: ISimulation KERNEL : loop over particle stack, until empty
  ATH_MSG_DEBUG( "Starting simulation loop, initial particle stack size: " << m_particleBroker->numParticles());
  while ( true ) {
    // get next vector of particles for simulation
    const ISF::ConstISFParticleVector &particles = m_particleBroker->popVector(m_maxParticleVectorSize);
    int numParticles = particles.size();
    // particle vector empty -> end simulation
    if (numParticles==0) break;

    // for job statistics
    m_numParticles += numParticles;

    // retrieve the particle destination simulator (and geoID)
    const ISFParticle *firstP = particles.front();
    ISF::SimSvcID simID = firstP->nextSimID();
    //AtlasDetDescr::AtlasRegion geoID = firstP->nextGeoID();

    ATH_MSG_DEBUG  ( "Took " << numParticles << " particles from queue (remaining: " << m_particleBroker->numParticles() << ")" );
    ATH_MSG_VERBOSE( " -> All particles will be sent to '" << m_simSvcNames[simID] << "' simulator (SimSvcID=" << simID << ")"  );

    // block defines scope for Benchmarks
    //  -> benchmarks will be stared/stopped automatically via the CustomBenchmarkGuard
    //     constructor and destructor, respectively
    {
      // setup sim svc benchmarks
      //PMonUtils::CustomBenhmarkGuard benchPDG  ( m_benchPDGCode, pdgCode );
      //PMonUtils::CustomBenchmarkGuard benchGeoID( m_benchGeoID  , geoID , numParticles );
      PMonUtils::CustomBenchmarkGuard benchSimID( m_benchSimID  , simID , numParticles );

      // ===> simulate particle
      // NB Passing only the hard-scatter McEventCollection is not
      // correct if Geant4 simulation were to be used for pile-up Hits
      // in Fast Chain.
      if (m_simSvcs[simID]->simulateVector(particles, m_outputHardScatterTruth.ptr()).isFailure()) {
        ATH_MSG_WARNING( "Simulation of particles failed in Simulator: " << m_simSvcNames[simID]);
      }
    }

  }
  ATH_MSG_VERBOSE( "Ending simulation loop, no more particles in the stack" );
  // -----------------------------------------------------------------------------------------------



  // Step 4: Finalize the Event
  //  -> stack service
  if ( m_particleBroker->finalizeEvent().isFailure()) {
    ATH_MSG_WARNING( "ParticleBroker returned with an error in event finalization." );
  }
  //  -> simulator services
  {
    std::vector<ISimulationSvc*>::iterator fSimSvcIter     = m_simSvcs.begin();
    std::vector<ISimulationSvc*>::iterator fSimSvcIterEnd  = m_simSvcs.end();
    for ( ; fSimSvcIter != fSimSvcIterEnd; ++fSimSvcIter ){
      ISimulationSvc *curSimSvc = (*fSimSvcIter);
      // if simulation with current flavour is registered
      //  -> releaseEvent()
      if ( curSimSvc){
        if( curSimSvc->releaseEvent().isFailure() ) {
          ATH_MSG_WARNING( "Event setup failed for "
                           << curSimSvc->simSvcDescriptor() );
        } else {
          ATH_MSG_DEBUG  ( "Event setup done for "
                           << curSimSvc->simSvcDescriptor() );
        }
      }
    } // -> loop over SimSvcs
  }

  if ( m_truthRecordSvc->releaseEvent().isFailure() ){
      ATH_MSG_FATAL( "Event finalize failed for TruthService. Abort." );
      return StatusCode::FAILURE;
  }

  // Step 4a: Remove QS patch if required
  if(!m_qspatcher.empty()) {
    for (const auto& currentGenEvent : *m_outputHardScatterTruth ) {
      ATH_CHECK(m_qspatcher->removeWorkaround(*currentGenEvent));
    }
  }

  // Step 5: Check Any Filters
  ToolHandleArray<IEventFilterTool>::iterator eventFilter(m_eventFilters.begin());
  const ToolHandleArray<IEventFilterTool>::iterator endOfEventFilters(m_eventFilters.end());
  while (eventFilter != endOfEventFilters) {
    if (!((**eventFilter).eventPassesFilter())) {
      setFilterPassed(false);
      ATH_MSG_INFO("This event failed the " << (**eventFilter).name() << " Filter. Therefore it will not be recorded.");
      break;
    }
    ++eventFilter;
  }


  // -----------------------------------------------------------------------------------------------

  // dump current memory monitoring information
  if (m_doMemMon) {
    std::string evtStr = boost::lexical_cast<std::string>( m_numISFEvents );
    std::string descr("after event " + evtStr);
    m_memMon->dumpCurrent( descr.c_str(), true);

    // ISF internal event counting
    m_numISFEvents++;

    // memory monitoring records for the final summary
    if ( !(m_numISFEvents%m_memUsageEvts) ) m_memMon->recordCurrent( descr.c_str() );
    else if ( m_numISFEvents==1)   m_memMon->recordCurrent("after   1st event");
    else if ( m_numISFEvents==2)   m_memMon->recordCurrent("after   2nd event");
    else if ( m_numISFEvents==10)  m_memMon->recordCurrent("after  10th event");
    else if ( m_numISFEvents==100) m_memMon->recordCurrent("after 100th event");
  }

  return StatusCode::SUCCESS;
}


/** Convert input generator particles to ISFParticles and copy input
    generator truth collection into output simulation truth collection */
StatusCode ISF::SimKernel::prepareInput(SG::ReadHandle<McEventCollection>& inputTruth,
                                        SG::WriteHandle<McEventCollection>& outputTruth,
                                        ISFParticleContainer& simParticles) const {

  if (!inputTruth.isValid()) {
    ATH_MSG_FATAL("Unable to read input GenEvent collection '" << inputTruth.key() << "'");
    return StatusCode::FAILURE;
  }

  // create copy
  outputTruth = CxxUtils::make_unique<McEventCollection>(*inputTruth);

  // Apply QS patch if required
  if(!m_qspatcher.empty()) {
    for (const auto& currentGenEvent : *outputTruth ) {
      ATH_CHECK(m_qspatcher->applyWorkaround(*currentGenEvent));
    }
  }
  ATH_CHECK( m_inputConverter->convert(*outputTruth, simParticles, HepMcParticleLink::find_enumFromKey(outputTruth.name())) );

  return StatusCode::SUCCESS;
}
