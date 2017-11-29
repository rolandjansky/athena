/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleBrokerDynamicOnReadIn.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ParticleBrokerDynamicOnReadIn.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleContainer.h"
#include "ISF_Event/ISFBenchmarkHelper.h"

#include "ISF_Interfaces/IEntryLayerTool.h"
#include "ISF_Interfaces/IGeoIDSvc.h"
#include "ISF_Interfaces/SimulationFlavor.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegionHelper.h"

// Benchmarking
#include "PmbCxxUtils/CustomBenchmark.h"

// Framework includes
#include "GaudiKernel/ITHistSvc.h"

// ROOT includes
#include "TTree.h"

// C include
#include <assert.h>

/** Constructor **/
ISF::ParticleBrokerDynamicOnReadIn::ParticleBrokerDynamicOnReadIn(const std::string& name,ISvcLocator* svc) :
  AthService(name,svc),
  m_entryLayerTool("iGeant4::EntryLayerTool/ISF_EntryLayerTool"),
  m_orderingTool(""),
  m_hasOrderingTool(false),
  m_geoIDSvc("", name),
  m_geoIDSvcQuick(0),
  m_forceGeoIDSvc(false),
  m_validateGeoID(false),
  m_particles(),
  m_popParticles(),
  m_simSelector(),
  m_simSelectorSet(),
  m_screenOutputPrefix("isf >> "),
  m_barcodeSvc("", name),
  m_doSelectorCPUMon(false),
  m_benchPDGCode(0),
  m_benchGeoID(0),
  m_validationOutput(false),
  m_thistSvc("THistSvc",name),
  m_validationStream("ParticleBroker"),
  m_t_pushPosition(0),
  m_t_entryLayerPos(),
  m_val_pdg(0),
  m_val_p(0.),
  m_val_x(0.),
  m_val_y(0.),
  m_val_z(0.)
{
  // the entry layer tool to write TrackRecordCollections
  declareProperty("EntryLayerTool"             , m_entryLayerTool       );
  // particle ing tool
  declareProperty("ParticleOrderingTool"       , m_orderingTool         );
  // the geo service and selectors needed
  declareProperty("GeoIDSvc"                   , m_geoIDSvc             );
  declareProperty("AlwaysUseGeoIDSvc"          , m_forceGeoIDSvc        );
  declareProperty("ValidateGeoIDs"             , m_validateGeoID        );
  // collect and print cpu monitoring information
  declareProperty("SimSelectorCPUMonitoring"   , m_doSelectorCPUMon     );
  // The Particle/Vertex BarcodeService used in ISF to store barcode info
  declareProperty("BarcodeService"             , m_barcodeSvc,
                  "The Particle/Vertex BarcodeService used in ISF" );

  // write out validation info
  declareProperty( "ValidationOutput",
                   m_validationOutput = false,
                   "If turned on, write out a ROOT tree.");
  declareProperty("ValidationStreamName",
                  m_validationStream = "ParticleBroker",
                  "Name of the output stream" );
  declareProperty("THistService",
                  m_thistSvc,
                  "The THistSvc" );
}


ISF::ParticleBrokerDynamicOnReadIn::~ParticleBrokerDynamicOnReadIn()
{}


/** framework methods */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::initialize()
{
  ATH_MSG_INFO("initialize() ...");

  // retrieve the entry layer tool
  if ( m_entryLayerTool.retrieve().isFailure() ){
    ATH_MSG_FATAL("Could not retrieve EntryLayer Tool. Abort.");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "- EntryLayerTool   : " << m_entryLayerTool.typeAndName() );
  }

  // retrieve the particle ing tool if given
  if ( ! m_orderingTool.empty()) {
    if ( m_orderingTool.retrieve().isFailure() ){
      ATH_MSG_FATAL("Could not retrieve ParticleOrderingTool. Abort.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( "- Particel OrderingTool   : " << m_orderingTool.typeAndName() );
      m_hasOrderingTool = true;
    }
  }

  // retrieve the geo identification decision tool
  if ( m_geoIDSvc.retrieve().isFailure()){
    ATH_MSG_FATAL("Could not retrieve GeometryIdentifier Service. Abort.");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "- GeoIDSvc         : "
                  << (m_geoIDSvc.empty() ? "<not configured>" : m_geoIDSvc.typeAndName()) );
    // store a quick-access-pointer (removes GaudiOverhead)
    m_geoIDSvcQuick = &(*m_geoIDSvc);
  }

  // retrieve the barcode service
  if ( m_barcodeSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( m_barcodeSvc.propertyName() << ": Failed to retrieve service " << m_barcodeSvc.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( m_barcodeSvc.propertyName()  << ": Retrieved service " << m_barcodeSvc.type());
  }

  // setup CPU Benchmarks
  if (m_doSelectorCPUMon) {
    if (!m_benchPDGCode)
      m_benchPDGCode = new PMonUtils::CustomBenchmark(ISF::fMaxBenchmarkPDGCode );
    if (!m_benchGeoID)
      m_benchGeoID   = new PMonUtils::CustomBenchmark(AtlasDetDescr::fNumAtlasRegions      );
  }

  // setup for validation mode
  if ( m_validationOutput) {

    // retrieve the histogram service
    if ( m_thistSvc.retrieve().isSuccess() ) {
      ATH_CHECK( registerPosValTree( "push_position",
                                     "push() particle positions",
                                     m_t_pushPosition) );
      ATH_CHECK( registerPosValTree( "caloEntry_pos",
                                     "CaloEntryLayer positions",
                                     m_t_entryLayerPos[ISF::fAtlasCaloEntry] ) );
      ATH_CHECK( registerPosValTree( "muonEntry_pos",
                                     "MuonEntryLayer positions",
                                     m_t_entryLayerPos[ISF::fAtlasMuonEntry] ) );
      ATH_CHECK( registerPosValTree( "muonExit_pos",
                                     "MuonExitLayer positions",
                                     m_t_entryLayerPos[ISF::fAtlasMuonExit]  ) );
    }

    // error when trying to retrieve the THistSvc
    else {
      // -> turn off validation output
      ATH_MSG_ERROR("Validation mode turned on but unable to retrieve THistService. Will not write out ROOT histograms/Trees.");
      m_validationOutput = false;
    }

  }

  // initialization was successful
  return StatusCode::SUCCESS;
}


/** framework methods */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::finalize()
{
  ATH_MSG_INFO("finalize() ...");
  return StatusCode::SUCCESS;
}


/** Register an array of SimulationSelectors */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::registerSimSelector( SimSelectorToolArray &simSelectorTools, AtlasDetDescr::AtlasRegion geoID)
{
  // (1.) retrieve all SimulationSelector tools in the array
  if ( simSelectorTools.retrieve().isFailure() ) {
    ATH_MSG_FATAL( m_screenOutputPrefix <<  "Could not retrieve SimulatorSelector Tool Array. Abort.");
    return StatusCode::FAILURE;
  }

  // reserve memory in STL vector
  m_simSelector[geoID].reserve ( simSelectorTools.size());

  // (2.) loop over SimulationSelector tool array and store
  //      its contents in m_simSelector[]
  SimSelectorToolArray::iterator fSimSelIter    = simSelectorTools.begin();
  SimSelectorToolArray::iterator fSimSelIterEnd = simSelectorTools.end();
  for ( ; fSimSelIter != fSimSelIterEnd; ++fSimSelIter ) {
    ISimulationSelector *curSelector = &**fSimSelIter;
    // initialize the SimulationSelector
    curSelector->initializeSelector();
    // register current SimulationSelector to the m_simSelector vector
    m_simSelector[geoID].push_back( curSelector);
  }

  return StatusCode::SUCCESS;
}


/** setup the ROOT TTrees for THistSvc in validation mode */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::registerPosValTree( const char *treeName,
                                                                   const char *treeDescr,
                                                                   TTree *&tree ) {
  // Create the prefix of histogram names for the THistSvc
  std::string prefix = "/" + m_validationStream + "/";

  tree = new TTree( treeName, treeDescr );
  tree->Branch("x"  , &m_val_x  , "x/F");
  tree->Branch("y"  , &m_val_y  , "y/F");
  tree->Branch("z"  , &m_val_z  , "z/F");
  tree->Branch("p"  , &m_val_p  , "p/F");
  tree->Branch("pdg", &m_val_pdg, "pdg/I");

  // register the Tree to the THistSvc and return it's StatusCode
  return (m_thistSvc->regTree( prefix+treeName, tree) );
}


/** fill the TTree with the given (x,y,z) values */
void ISF::ParticleBrokerDynamicOnReadIn::fillPosValTree( TTree *tree,
                                                         const ISFParticle &p ) {
  // the particle position
  const Amg::Vector3D &pos = p.position();
  // fill the member variables
  m_val_x = pos.x();
  m_val_y = pos.y();
  m_val_z = pos.z();
  m_val_p   = p.momentum().mag();
  m_val_pdg = p.pdgCode();
  // fill the ROOT TTree
  tree->Fill();

  return;
}


/** update all SimulationSelectors in the routing chain with the given particle */
void ISF::ParticleBrokerDynamicOnReadIn::updateAllSelectors(const ISFParticle &particle) {
  for ( const auto& simSelector : m_simSelectorSet ) {
    simSelector->update(particle);
  }
}


/** go through the chain of SimulationSelectors and let the first one which selects the particle
    decide which simulation the particle will be assigned to */
void ISF::ParticleBrokerDynamicOnReadIn::selectAndStore( ISF::ISFParticle* p)
{
  // consult the routing chain to find the simulator ID corresponding to this
  // particle
  SimSvcID selectedSimID = identifySimID( p);

  // in case a simulator was determined for this particle
  //   -> register this simulator to the particle
  //   -> push particle onto container
  if ( selectedSimID != ISF::fUndefinedSimID) {
    ATH_MSG_VERBOSE( m_screenOutputPrefix << "Assigning " << *p
                     << " to simulator with ID=" << selectedSimID << ".");
    // register the SimulatorID to the particle
    p->setNextSimID( selectedSimID);

    if (m_hasOrderingTool) m_orderingTool->setOrder(*p);

    // store particle locally
    m_particles.push(p);

    // no simulator could be found
    //   -> drop particle
  } else {
    AtlasDetDescr::AtlasRegion geoID = p->nextGeoID();
    // different error message for empty simulation Selector chain:
    if ( !m_simSelector[geoID].size()) {
      ATH_MSG_INFO( m_screenOutputPrefix << "No SimulationSelectors registered for GeoID="
                    << AtlasDetDescr::AtlasRegionHelper::getName(geoID) << ". Will not assign this particle to any simulator, dropping it.");
    }
    else {
      ATH_MSG_INFO( m_screenOutputPrefix << "Current particle not selected by any SimulationSelector with GeoID="
                    << AtlasDetDescr::AtlasRegionHelper::getName(geoID) << ". Will not assign this particle to any simulator, dropping it.");
    }
    delete p;
  }
}

/** go through the chain of SimulationSelectors and return the SimulatoID of the first
    SimulationSelector that selects the particle */
ISF::SimSvcID ISF::ParticleBrokerDynamicOnReadIn::identifySimID( const ISF::ISFParticle* p) {
  // implicit assumption that the geoID is set properly at this point (it is a private method)!
  AtlasDetDescr::AtlasRegion geoID = p->nextGeoID();

  // iterators used to loop over all simulation Selectors
  SimSelectorArray::iterator selectorIt     = m_simSelector[geoID].begin();
  SimSelectorArray::iterator selectorItEnd  = m_simSelector[geoID].end();

  // will store whether the particle was selected by a simulation filter
  bool selected = false;

  // block defines scope for Benchmarks
  //  -> benchmarks will be stared/stopped automatically via the CustomBenchmarkGuard
  //     constructor and destructor, respectively
  {
    // prepare the pdgCode for the benchmark
    unsigned int pdgCode = ISFBenchmarkHelper::getBenchReadyPdgCode( *p);

    // setup sim svc benchmarks
    PMonUtils::CustomBenchmarkGuard benchPDG  ( m_benchPDGCode, pdgCode );
    PMonUtils::CustomBenchmarkGuard benchGeoID( m_benchGeoID  , geoID   );

    // go through the chain of selectors and find the first
    // selector which selects out the particle
    for ( ; !selected && (selectorIt != selectorItEnd) ; ++selectorIt) {
      // check if the current selector selects it
      selected = (*selectorIt)->selfSelect( *p);
    }

  } // stop benchmarks

  // determine the simulator ID
  ISF::SimSvcID simID = selected ? (*--selectorIt)->simSvcID() : ISF::SimSvcID(ISF::fUndefinedSimID);

  return simID;
}


/** Initialize the stackSvc and the truthSvc */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::initializeEvent(ISFParticleContainer&& simParticles)
{
  ATH_MSG_DEBUG( m_screenOutputPrefix << "Initializing particle stack");

  // fill set of simulation selectors once per job
  //  -> the std::set allows us to address each selector only once
  //     (the same selector might appear in multiple geoIDs)
  if ( !m_simSelectorSet.size() )  {
    for ( int curGeoID = AtlasDetDescr::fFirstAtlasRegion; curGeoID < AtlasDetDescr::fNumAtlasRegions; ++curGeoID) {
      // fill the set with the selectors from the current geoID
      m_simSelectorSet.insert( m_simSelector[curGeoID].begin(), m_simSelector[curGeoID].end() );
    }

    ATH_MSG_DEBUG("Number of unique SimulationSelctors registered: "
                  << m_simSelectorSet.size() );
  }

  // call beginEvent() for all selectors registerd to the ParticleBroker:
  for ( const auto& simSelector : m_simSelectorSet ) {
    simSelector->beginEvent();
  }

  // update the routing chain selectors with the particles in the initial stack
  for ( auto& particlePointer : simParticles ) {
    auto& particle = *particlePointer;

    // identify the geoID of the particle
    m_geoIDSvcQuick->identifyAndRegNextGeoID(particle);
    // the geoID at this point better makes sense :)
    assertAtlasRegion( particle.nextGeoID() );

    // update all registered selectors (in all geoIDs) with this particle
    updateAllSelectors(particle);

    m_entryLayerTool->registerParticle(particle);
  }

  size_t order = simParticles.size(); // FIXME: ugly hack to keep bit-wise identical output with prior FullG4 implementation :(

  for ( auto& particlePtr: simParticles ) {
    // FIXME: ugly hack to keep bit-wise identical output with prior FullG4 implementation :(
    if (!m_hasOrderingTool) particlePtr->setOrder(order--); 

    selectAndStore(particlePtr);
  }

  return StatusCode::SUCCESS;
}


/** Finalize the event in the stackSvc */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::finalizeEvent() {

  // go throught all SimSelectors and call the endEvent() on them
  SimSelectorSet::iterator fSimSelIter    = m_simSelectorSet.begin();
  SimSelectorSet::iterator fSimSelIterEnd = m_simSelectorSet.end();
  for ( ; fSimSelIter != fSimSelIterEnd; ++fSimSelIter )
    (*fSimSelIter)->endEvent();

  return StatusCode::SUCCESS;
}


/** add a new particle to the stack and link it to its parent */
void ISF::ParticleBrokerDynamicOnReadIn::push( ISFParticle *particlePtr, const ISFParticle *parentPtr) {
  // this call does not make much sense with no given particle
  assert(particlePtr);

  ISFParticle &particle = *particlePtr;

  if (parentPtr) {
    int bcid = parentPtr->getBCID();
    particle.setBCID(bcid);

    //Let's make sure that the new ISFParticle has a valid TruthBinding and HepMcParticleLink
    //(could happen that the new particles are not saved by the TruthSvc for instance)
    //or attach pointers to the parent otherwise
    if (!particlePtr->getTruthBinding()) {
        ATH_MSG_WARNING("The provided new ISFParticle had no TruthBinding ! Copying over the one from the parent ISFParticle.");
        particle.setTruthBinding(new TruthBinding(*parentPtr->getTruthBinding()));
    }
  }

  // get the particle's next geoID
  AtlasDetDescr::AtlasRegion geoID = particle.nextGeoID();

  // if GeoID not set (e.g. ISF::fUndefinedGeoID) or a flag is set to always use the GeoIDSvc
  // -> let the geoIDSvc find the next geoID
  if ( m_forceGeoIDSvc || !validAtlasRegion(geoID) ) {
    geoID = m_geoIDSvcQuick->identifyAndRegNextGeoID(particle);
  }
  // inform the entry layer tool about this particle
  ISF::EntryLayer layer = m_entryLayerTool->registerParticle( particle );

  // ---> if validation mode: fill the corresponding entry layer ROOT tree
  if ( m_validationOutput ) {
    // fill the push() position TTree
    fillPosValTree( m_t_pushPosition, particle);
    // in case particle was added to an entry layer, add it to the corresponding TTree
    if ( validEntryLayer(layer) ) {
      fillPosValTree( m_t_entryLayerPos[layer], particle);
    }
  }
  // <--- end validation output

  // validation mode: check whether the particle position corresponds to the GeoID given
  // by the particle itself
  if ( m_validateGeoID) {
    AtlasDetDescr::AtlasRegion identifiedGeoID = m_geoIDSvcQuick->identifyNextGeoID(particle);
    if ( (geoID!=AtlasDetDescr::fUndefinedAtlasRegion) && (geoID!=identifiedGeoID) ) {
      ATH_MSG_WARNING("Validating GeoID: GeoIDSvc resolves a particle's position to a different GeoID than stored in the particle:");
      ATH_MSG_WARNING("     assigned=" << geoID << "  GeoIDSvc=" << identifiedGeoID);
      ATH_MSG_WARNING("     Particle: " << particle);
    }
  }

  // only process particles with well defined geoID
  if ( !validAtlasRegion( geoID) ) {
    ATH_MSG_ERROR( m_screenOutputPrefix << "Trying to push particle onto the stack with unknown geoID=" << geoID
                   << ". Dropping this particle.");
    delete particlePtr;
    return;
  }

  // (*) let the Selectors select the particle
  //       - if a Selector selects a particle -> it is pushed onto the active stack
  //       - if it is not selected -> particle is dropped (deleted)
  selectAndStore( particlePtr );
}

/** Get vectors of ISF particles from the broker */
const ISF::ConstISFParticleVector& ISF::ParticleBrokerDynamicOnReadIn::popVector(size_t maxVectorSize)
{
  // delete particles from previous return vector and empty the vector
  ISF::ConstISFParticleVector::const_iterator partIt  = m_popParticles.begin();
  ISF::ConstISFParticleVector::const_iterator partEnd = m_popParticles.end();
  for ( ; partIt != partEnd; ++partIt) {
    ISF::ISFParticle *curPart = const_cast<ISF::ISFParticle*>(*partIt);
    delete curPart;
  }
  m_popParticles.clear();

  // if there are particles in the queue
  if ( m_particles.size()) {

    ParticleOrder returnOrder = m_particles.top()->getOrder();

    // loop as long as we have particles in the m_particles queue
    do {
      // get the next particle from the ordered queue
      const ISFParticle *curParticle = m_particles.top();
      ParticleOrder      curOrder    = curParticle->getOrder();

      // if this particle has a different order or the maximum size of the return vector is reached
      //   -> don't add any more particles to the m_popParticles std::vector
      if ( m_hasOrderingTool && ((curOrder != returnOrder) || (m_popParticles.size()>=maxVectorSize) ) ) break;

      // add this particle to the, later returned, m_popParticles std::vector
      m_popParticles.push_back( curParticle);
      // remove this particle from the ordered queue
      m_particles.pop();
    } while ( m_particles.size() ) ;

  }
  // return the popParticles vector
  return m_popParticles;
}


/* Dump the stack to the screen */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::dump() const
{
  ATH_MSG_INFO( m_screenOutputPrefix << "==== ISF Particle Stack Dump ====");
  ATH_MSG_INFO( m_screenOutputPrefix << " 'active' particles -> ready to be simulated");
  ATH_MSG_INFO( m_screenOutputPrefix << " 'onHold' particles -> no sim Selector decision yet, waiting in routing chain");
  ATH_MSG_INFO( m_screenOutputPrefix << " --- ");
  ATH_MSG_INFO( m_screenOutputPrefix << " Number of 'active' particles: " <<  m_particles.size());
  for ( int geoID = AtlasDetDescr::fFirstAtlasRegion; geoID < AtlasDetDescr::fNumAtlasRegions; ++geoID) {
    ATH_MSG_INFO( m_screenOutputPrefix << " --- SimGeoID=" << geoID
                  << " (" << AtlasDetDescr::AtlasRegionHelper::getName(geoID) << ") ---");
    ATH_MSG_INFO( m_screenOutputPrefix << "   Routing Chain has length "
                  << m_simSelector[geoID].size() );
  }
  ATH_MSG_INFO( m_screenOutputPrefix << "=================================");

  return StatusCode::SUCCESS;
}


/** Query the interfaces. */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::queryInterface(const InterfaceID& riid, void** ppvInterface) {

  if ( IID_IParticleBroker == riid )
    *ppvInterface = (IParticleBroker*)this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

