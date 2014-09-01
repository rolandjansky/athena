/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenEventStackFiller.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ISF_HepMC_Tools/GenEventStackFiller.h"

// framework
#include "GaudiKernel/IPartPropSvc.h"
// ISF_HepMC include
#include "ISF_HepMC_Event/HepMC_TruthBinding.h"
#include "ISF_HepMC_Interfaces/IGenEventManipulator.h"
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"
// barcode interfaces for bc creation
#include "BarcodeInterfaces/IBarcodeSvc.h"
#include "BarcodeInterfaces/Barcode.h"
#include "BarcodeServices/BitCalculator.h"
// ISF_Event include
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ParticleUserInformation.h"
//ISF include
#include "ISF_Interfaces/IStackFiller.h"
// HepMC includes
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"
// McEventCollection
#include "GeneratorObjects/McEventCollection.h"
// CLHEP includes
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
// HepPDT
#include "HepPDT/ParticleID.hh"
#include "HepPDT/DecayData.hh"
#include "HepPDT/ParticleDataTable.hh"

#include "PileUpTools/PileUpMergeSvc.h"

/** Constructor **/
ISF::GenEventStackFiller::GenEventStackFiller(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t,n,p),
    m_particlePropSvc("PartPropSvc",n),
    m_particleDataTable(0),
    m_inputMcEventCollection("GEN_EVENT"),
    m_outputMcEventCollection("TruthEvent"),
    m_recordOnlySimulated(false),
    m_useGeneratedParticleMass(false),
    m_genEventManipulators(),
    m_genParticleFilters(),
    m_barcodeSvc("ISF_BarcodeService",n),
    m_pMergeSvc("PileUpMergeSvc", n),
    m_largestBc(0),
    m_uniqueBc(0),
    m_current_event_time(0.0),
    m_current_event_index(0),
    m_current_bcid(0),
    m_bunch_spacing(50)
{
  declareInterface<ISF::IStackFiller>(this);
  // inout out mc event collection
  declareProperty("InputMcEventCollection",
                  m_inputMcEventCollection,
                  "StoreGate collection name of the generator McEventCollection.");
  declareProperty("OutputMcEventCollection",
                  m_outputMcEventCollection,
                  "StoreGate collection name of the truth McEventCollection.");
  // record only the particles that were sent into the ISF simulation
  declareProperty("PurgeOutputCollectionToSimulatedParticlesOnly",
                  m_recordOnlySimulated,
                  "Record only particles that were taken as Input for the Simulation.");
  // particle mass from particle data table?
  declareProperty("UseGeneratedParticleMass",
                  m_useGeneratedParticleMass,
                  "Use particle mass assigned to GenParticle.");
  // event manipulators
  declareProperty("GenEventManipulators",
                  m_genEventManipulators,
                  "StackFiller will send the read-in GenEvent to each individual IGenEventManipulator.");
  // particle filters
  declareProperty("GenParticleFilters",
                  m_genParticleFilters,
                  "Tools for filtering out GenParticles.");
  // the barcode service
  declareProperty("BarcodeService",
                  m_barcodeSvc,
                  "The Particle/Vertex BarcodeService used in ISF.");
  // the particle property service
  declareProperty("ParticlePropertyService",
                  m_particlePropSvc,
                  "ParticlePropertyService to retrieve the PDT.");
  // bunch spacing
  declareProperty( "BunchSpacing", m_bunch_spacing = 50.0 );
  // the pileup merge service
  declareProperty("PileUpMergeService",
                  m_pMergeSvc,
                  "The pile-up merge service.");
}


// Athena algtool's Hooks
StatusCode  ISF::GenEventStackFiller::initialize()
{
  ATH_MSG_VERBOSE("initialize() begin");

  // setup PDT if required (to get particle masses)
  if (!m_useGeneratedParticleMass) {
    // retrieve the ParticleProperties service
    if ( m_particlePropSvc.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Could not retrieve " << m_particlePropSvc << ". Abort. " );
      return StatusCode::FAILURE;
    }
    // and the particle data table
    m_particleDataTable = m_particlePropSvc->PDT();
    if (m_particleDataTable==0) {
      ATH_MSG_FATAL( "Could not get ParticleDataTable from " << m_particlePropSvc << ". Abort" );
      return StatusCode::FAILURE;
    }
  }

  // the Tools for event manipulation
  if (!m_genEventManipulators.empty() && m_genEventManipulators.retrieve().isFailure()){
    ATH_MSG_ERROR( "Could not retrieve " << m_genEventManipulators << ". Abort.");
    return StatusCode::FAILURE;
  }

  // the barcode service for consistent BC handling
  if (m_barcodeSvc.retrieve().isFailure()){
    ATH_MSG_ERROR( "Could not retrieve " << m_barcodeSvc << ". Abort.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Returns the Particle Stack, should register truth */
StatusCode ISF::GenEventStackFiller::fillStack(ISF::ISFParticleContainer& particleColl) const
{
  // check is pileup merge service has been configured.
  // if so, retrieve merged mceventcollection set and process that
  // if not, retrieve regular mceventcollection
  if(!m_pMergeSvc.empty()) {
    if (!(m_pMergeSvc.retrieve()).isSuccess()) {
      msg(MSG::WARNING) << "fillStack(): Can not find PileUpMergeSvc. Assuming no merge pileup collections at EvGen level." << endreq;
      //return StatusCode::RECOVERABLE;
    }
  }

  // MB : reset barcode counter for each event
  m_uniqueBc = 0;
  m_largestBc = 0;

  if (!m_pMergeSvc.empty()) {
    // 1. pileup merge service has been configured.
    // retrieve and process merged mceventcollection set
    return processMergedColls(particleColl);
  } else {
    // 2. try to retrieve and process regular mceventcollection, as usual
    return processSingleColl(particleColl);
  }

  // register the largest barcode appearing in GEN_EVENT
  m_barcodeSvc->registerLargestGenEvtParticleBC( m_largestBc );
  //<! @TODO: support multi-vertex input files
  //m_barcodeSvc->registerLargestGenEvtVtxBC( largestVtxBc);

  return StatusCode::SUCCESS;
}


/** process merged (hard-scatter+pileup) mcevent collections */
StatusCode
ISF::GenEventStackFiller::processSingleColl(ISF::ISFParticleContainer& particleColl) const
{
  // Retrieve Truth from StoreGate
  const McEventCollection* inputCollection = 0;
  if (evtStore()->retrieve(inputCollection, m_inputMcEventCollection).isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve " << m_inputMcEventCollection << " from StoreGateSvc. Abort.");
    return StatusCode::FAILURE;
  }

  // create a copy
  McEventCollection *mcCollection = new McEventCollection(*inputCollection);

  // Loop over McEventCollection
  StatusCode status = mcEventCollLooper( particleColl , mcCollection );
  if ( status != StatusCode::SUCCESS ) { return status; }

  // record the GenEvent as 'TruthEvent' to StoreGate
  bool allowMods(true);
  if (evtStore()->record(mcCollection, m_outputMcEventCollection, allowMods).isFailure()) {
    ATH_MSG_ERROR( "Could not record " << m_outputMcEventCollection << " to StoreGateSvc. Abort.");
    //delete mcCollection;  // do we need this? (let's wait for mr. coverity's opinion)
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE( "Created particle collection with length " << particleColl.size() );

  return StatusCode::SUCCESS;
}


/** process single (hard-scatter-only) mcevent collection */
StatusCode
ISF::GenEventStackFiller::processMergedColls(ISF::ISFParticleContainer& particleColl) const
{
  //PRECONDITIONS
  //first get the list of McEventCollections
  typedef PileUpMergeSvc::TimedList<McEventCollection>::type TimedTruthList;
  TimedTruthList truthList;
  if (!m_pMergeSvc->retrieveSubEvtsData(m_inputMcEventCollection, truthList).isSuccess() ) {
    ATH_MSG_ERROR("processMergedColls(): Cannot find TimedTruthList with key " << m_inputMcEventCollection );
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE( "Size of truthlist " << truthList.size() );

  m_number_of_gen_minbias = 0;
  //     number_of_gen_events
  TimedTruthList::iterator timedTruthListIter(truthList.begin()), endOfTimedTruthList(truthList.end());
  //loop over the McEventCollections (each one assumed to containing exactly one GenEvent) of the various input events
  bool is_intime_pileup = true;

  int number_of_mb_events=0;

  for (; timedTruthListIter != endOfTimedTruthList; ++timedTruthListIter, ++number_of_mb_events ){
    const PileUpTimeEventIndex& currentPileUpTimeEventIndex(timedTruthListIter->first);
    McEventCollection *mcCollection = new McEventCollection(*(timedTruthListIter->second));

    m_current_event_time = currentPileUpTimeEventIndex.time();
    m_current_event_index = currentPileUpTimeEventIndex.index();

    // Loop over McEventCollection
    StatusCode status = mcEventCollLooper( particleColl , mcCollection, number_of_mb_events );
    if ( status != StatusCode::SUCCESS ) { return status; }

    // record the GenEvent as 'TruthEvent' to StoreGate
    if( is_intime_pileup ){
      bool allowMods(true);
      if (evtStore()->record(mcCollection, m_outputMcEventCollection, allowMods).isFailure()) {
        ATH_MSG_ERROR( "Could not record " << m_outputMcEventCollection << " to StoreGateSvc. Abort.");
        //       delete mcCollection;  // do we need this? (let's wait for mr. coverity's opinion)
        return StatusCode::FAILURE;
      }
    }
    ATH_MSG_VERBOSE( "Created particle collection with length " << particleColl.size() );
    is_intime_pileup = false;
  }

  ATH_MSG_VERBOSE(" number_of_gen_minbias " << m_number_of_gen_minbias << " number_of_mb_events " << number_of_mb_events);

  return StatusCode::SUCCESS;
}


/** common code to loop over mcevent collection */
StatusCode
ISF::GenEventStackFiller::mcEventCollLooper(ISF::ISFParticleContainer& particleColl, McEventCollection* mcCollection, int nPileupCounter) const
{
  // ensure unique barcode for merged pileup collisions
  int bcPileupOffset = ( nPileupCounter>0 ? m_largestBc+1 : 0 ); 

  McEventCollection::iterator eventIt    = mcCollection->begin();
  McEventCollection::iterator eventItEnd = mcCollection->end();

  // loop over the event in the mc collection
  for ( ; eventIt != eventItEnd; ++eventIt) {
    // skip empty events
    if ( *eventIt == 0) continue;

    ATH_MSG_VERBOSE(" signal_process_id " << (*eventIt)->signal_process_id() );
    ATH_MSG_VERBOSE(" event number " << (*eventIt)->event_number() );
    ATH_MSG_VERBOSE(" event time " << m_current_event_time << " event index " << m_current_event_index );

    // manipulate the GenEvent if needed : validating, filtering, smearing, dispersion, ...
    GenEventManipulators::const_iterator manipulatorIt  = m_genEventManipulators.begin();
    GenEventManipulators::const_iterator manipulatorEnd = m_genEventManipulators.end();
    for ( ; manipulatorIt != manipulatorEnd; ++manipulatorIt) {
      // call manipulate(..) in the current GenEventManipulator
      if ( (*manipulatorIt)->manipulate(**eventIt).isFailure() ) {
        ATH_MSG_ERROR( "GenEvent manipulation failed. Abort." );
        // in case of error: first clean up memory, then return with error code
        delete mcCollection;
        return StatusCode::FAILURE;
      }
    }

    // ---- LOOP over GenParticles  -------------------------------------------------------
    HepMC::GenEvent::particle_const_iterator part  = (*eventIt)->particles_begin();
    HepMC::GenEvent::particle_const_iterator partE = (*eventIt)->particles_end();
    for( ; part != partE ; part++ ) {
      // the current particle
      HepMC::GenParticle* tParticle = *part;

      // store largest appearing barcode
      int pBarcode = tParticle->barcode();
      // for 1 genevent collection ( = no pileup merging ), the isf particle's barcode is the genparticle barcode
      m_uniqueBc = bcPileupOffset + pBarcode ; 
      if ( m_uniqueBc > m_largestBc) m_largestBc = m_uniqueBc;

      m_number_of_gen_minbias++;

      ATH_MSG_VERBOSE( "Picking up GenParticle with Barcode '" << pBarcode << " tParticle " << tParticle->barcode() );
      //tParticle->print();

      // loop over all GenParticleFilters
      //   -> don't add it to the 'particleColl' if one of them returns 'false'
      bool passFilter = true;
      GenParticleFilters::const_iterator filterIt  = m_genParticleFilters.begin();
      GenParticleFilters::const_iterator filterEnd = m_genParticleFilters.end();
      for ( ; passFilter && filterIt!=filterEnd; ++filterIt) {
        // determine if the particle passes current filter
        passFilter = (*filterIt)->pass(*tParticle);
        ATH_MSG_DEBUG("GenParticleFilter '" << (*filterIt).typeAndName() << "' returned: "
                      << (passFilter ? "true, will keep particle."
                          : "false, will remove particle.") );
      }

      // if the GenParticleFilters were not passed
      //  -> skip particle
      if (!passFilter) {

        // remove the particle from the TruthEvent if requested
        if (m_recordOnlySimulated) {
          // the production vertex
          HepMC::GenVertex *vtx = tParticle->production_vertex();
          if (vtx) {
            // remove particle from its production-vertex
            vtx->remove_particle( tParticle);

            // remove the vertex form the GenEvent if there
            // are no more particles emerging from it
            if ( vtx->particles_out_size()==0) {
              (**eventIt).remove_vertex( vtx);
              delete vtx;
            }
          }

          // remove particle from its end-vertex
          vtx = tParticle->end_vertex();
          if (vtx) vtx->remove_particle( tParticle);

          // destructor should automatically remove particle from GenEvent
          delete tParticle;
        }

        // go to the next particle
        continue;
      }

      ATH_MSG_DEBUG( "GenParticle with Barcode '" << pBarcode
                     << "' passed all cuts, adding it to the initial ISF particle list.");

      // -> particle origin (TODO: add proper GeoID, collision/cosmics)
      DetRegionSvcIDPair origin( AtlasDetDescr::fUndefinedAtlasRegion, ISF::fEventGeneratorSimID);
      // -> truth binding
      ISF::HepMC_TruthBinding* tBinding = new ISF::HepMC_TruthBinding(*tParticle);
      // -> 4-vectors: vertex, momentum
      HepMC::GenVertex   *pVertex   = tParticle->production_vertex();
      // setup the particle position...
      HepGeom::Point3D<double> *pos = 0;
      // ...based on the Vertex coordinates in the GenEvent
      if  (pVertex) {
        const HepMC::ThreeVector vtxPos(pVertex->point3d());
        pos = new HepGeom::Point3D<double>( vtxPos.x(), vtxPos.y(), vtxPos.z());
      } else {
        ATH_MSG_ERROR( "GenParticle with Barcode '" << pBarcode <<
                       "' has an unset production_vertex, setting it to (0,0,0).");
        pos = new HepGeom::Point3D<double>(0.,0.,0.);
      }

      const HepMC::FourVector &pMomentum = tParticle->momentum();
      // get the pdg_id, mass & time
      int     pPdgId   = tParticle->pdg_id();
      double  pMass    = getParticleMass( *tParticle);
      double  pTime    = 0.0;  // !@TODO fix this
      double  charge   = HepPDT::ParticleID(pPdgId).charge();

      HepGeom::Vector3D<double> mom(pMomentum.px(),pMomentum.py(),pMomentum.pz());
      ISF::ISFParticle* sParticle = new ISF::ISFParticle( *pos,
                                                          mom,
                                                          pMass,
                                                          charge,
                                                          pPdgId,
                                                          pTime,
                                                          origin,
                                                          m_uniqueBc,
                                                          tBinding );

      // MB : need this (for now) to store extra barcode information
      Barcode::ParticleBarcode extrabc;
      sParticle->setUserInformation( new ISF::ParticleUserInformation() );
      if ( m_barcodeSvc->hasBitCalculator() ) {
	// first gen-event in the list is the hard scatter.
	if (m_current_event_index==0) { m_barcodeSvc->getBitCalculator()->SetHS(extrabc,true); }
	else { m_barcodeSvc->getBitCalculator()->SetHS(extrabc,false); }
	// bcid
	int bcid = int( m_current_event_time / m_bunch_spacing );
	m_barcodeSvc->getBitCalculator()->SetBCID(extrabc,bcid);
	// parent pid
	int absPDG = abs( pPdgId );
	if (absPDG>=1024) { absPDG=0; } // MB : assigned 10 bits to parent PID
	m_barcodeSvc->getBitCalculator()->SetPIDofParent(extrabc,absPDG);
      }
      // and store the extra bc (for now this is done in the particle's user information
      sParticle->setExtraBC( extrabc );

      // free up memory
      delete pos;

      // push back the particle into the collection
      particleColl.push_back(sParticle);
    }
  }

  return StatusCode::SUCCESS;
}


/** get right GenParticle mass */
double ISF::GenEventStackFiller::getParticleMass(const HepMC::GenParticle &part) const {
  // default value: generated particle mass
  double mass = part.generated_mass();

  // 1. use PDT mass?
  if ( !m_useGeneratedParticleMass) {
    int absPDG = abs(part.pdg_id());
    HepPDT::ParticleData const *pData = (m_particleDataTable)
      ? m_particleDataTable->particle(absPDG)
      : 0 ;
    if (pData)
      mass = pData->mass();
    else
      ATH_MSG_WARNING( "Unable to find mass of particle with PDG ID '" << absPDG << "' in ParticleDataTable. Will set mass to generated_mass: " << mass);
  }
  return mass;
}


StatusCode  ISF::GenEventStackFiller::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}
