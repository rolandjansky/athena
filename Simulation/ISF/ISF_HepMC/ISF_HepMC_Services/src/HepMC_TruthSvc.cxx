/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HepMC_TruthSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "HepMC_TruthSvc.h"
// other ISF_HepMC includes
#include "ISF_HepMC_Interfaces/ITruthStrategy.h"
// ISF includes
#include "ISF_Event/ITruthIncident.h"
// Framework
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
// Barcode
#include "BarcodeInterfaces/IBarcodeSvc.h"
// HepMC includes
#include "HepMC/SimpleVector.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
// CLHEP includes
#include "CLHEP/Geometry/Point3D.h"

// G4TruthStrategies
#include "G4TruthStrategies/BremsstrahlungStrategy.h"
#include "G4TruthStrategies/ComptonStrategy.h"
#include "G4TruthStrategies/ConversionStrategy.h"
#include "G4TruthStrategies/DecayStrategy.h"
#include "G4TruthStrategies/HadronicInteractionStrategy.h"
#include "G4TruthStrategies/IonizationStrategy.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegionHelper.h"

/** Constructor **/
ISF::HepMC_TruthSvc::HepMC_TruthSvc(const std::string& name,ISvcLocator* svc) :
  AthService(name,svc),
  m_storeGate(0),
  m_barcodeSvc("BarcodeSvc",name),
  m_barcodeSvcQuick(0),
  m_collectionName("TruthEvent"),
  m_mcEventCollection(0),
  m_mcEvent(0),
  m_geoStrategies(),
  m_numStrategies(),
  m_skipIfNoChildren(true),
  m_skipIfNoParentBarcode(true),
  m_ignoreUndefinedBarcodes(false),
  m_screenOutputPrefix("isf >> "),
  m_screenEmptyPrefix(),
  m_storeExtraBCs(true),
  m_passWholeVertex(true),
  m_forceEndVtxRegionsVec(),
  m_forceEndVtx(),
  m_quasiStableParticlesIncluded(false)
{
    // the particle stack filler tool
    declareProperty("McEventCollection",                 m_collectionName          );
    // refine the screen output for debugging
    declareProperty("ScreenOutputPrefix",                m_screenOutputPrefix      );
    // the barcode service (used to compute Vertex Barco des)
    declareProperty("BarcodeSvc",                        m_barcodeSvc              );
    // MCTruth writing strategies
    declareProperty("SkipIfNoChildren",                  m_skipIfNoChildren        );
    declareProperty("SkipIfNoParentBarcode",             m_skipIfNoParentBarcode   );
    declareProperty("IgnoreUndefinedBarcodes",           m_ignoreUndefinedBarcodes );
    declareProperty("PassWholeVertices",                 m_passWholeVertex         );
    // the truth strategies for the different AtlasDetDescr regions
    declareProperty("BeamPipeTruthStrategies",           m_geoStrategyHandles[AtlasDetDescr::fAtlasForward] );
    declareProperty("IDTruthStrategies",                 m_geoStrategyHandles[AtlasDetDescr::fAtlasID]      );
    declareProperty("CaloTruthStrategies",               m_geoStrategyHandles[AtlasDetDescr::fAtlasCalo]    );
    declareProperty("MSTruthStrategies",                 m_geoStrategyHandles[AtlasDetDescr::fAtlasMS]      );
    declareProperty("CavernTruthStrategies",             m_geoStrategyHandles[AtlasDetDescr::fAtlasCavern]  );
    // attach end-vertex if parent particle dies for the different AtlasDetDescr regions
    declareProperty("ForceEndVtxInRegions",              m_forceEndVtxRegionsVec );

    declareProperty("StoreExtraBarcodes",                m_storeExtraBCs);
    declareProperty("QuasiStableParticlesIncluded",      m_quasiStableParticlesIncluded);
}

ISF::HepMC_TruthSvc::~HepMC_TruthSvc()
{}


/** Query the interfaces. */
StatusCode ISF::HepMC_TruthSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
 if ( IID_ITruthSvc == riid )
    *ppvInterface = (ITruthSvc*)this;
 else  {
   // Interface is not directly available: try out a base class
   return Service::queryInterface(riid, ppvInterface);
 }
 addRef();
 return StatusCode::SUCCESS;
}


/** framework methods */
StatusCode ISF::HepMC_TruthSvc::initialize()
{
    ATH_MSG_VERBOSE( "initialize()" );

    // Screen output
    for (size_t prl = 0; prl < m_screenOutputPrefix.size(); ++prl) m_screenEmptyPrefix += " ";
    ATH_MSG_DEBUG ( m_screenOutputPrefix << "--------------------------------------------------------" );

    // retrieve StoreGateSvc
    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
    if ( svcLocator->service("StoreGateSvc", m_storeGate).isFailure()) {
      ATH_MSG_WARNING("AthenaHitsCollectionHelper: could not accessStoreGateSvc!");
      return StatusCode::FAILURE;
    }

    // retrieve BarcodeSvc
    if ( m_barcodeSvc.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_screenOutputPrefix <<  "Could not retrieve BarcodeService. Abort.");
      return StatusCode::FAILURE;
    }
    // store a point directly to the BarcodeSvc class
    // (removes the gaudi overhead in each call)
    m_barcodeSvcQuick = &(*m_barcodeSvc);

    // retrieve the strategies for each atlas region (Athena Alg Tools)
    // and setup whether we want to write end-vertices in this region whenever a truth particle dies
    for ( unsigned short geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions; ++geoID) {
      if ( m_geoStrategyHandles[geoID].retrieve().isFailure() ) {
        ATH_MSG_FATAL( m_screenOutputPrefix <<  "Could not retrieve TruthStrategy Tool Array for SimGeoID="
                       << AtlasDetDescr::AtlasRegionHelper::getName(geoID) << ". Abort.");
        return StatusCode::FAILURE;
      }

      // copy a pointer to the strategy instance to the local
      // array of pointers (for faster access)
      unsigned short curNumStrategies = m_geoStrategyHandles[geoID].size();
      m_numStrategies[geoID] = curNumStrategies;
      m_geoStrategies[geoID] = new ISF::ITruthStrategy*[curNumStrategies];
      for ( unsigned short i = 0; i < curNumStrategies; ++i) {
        m_geoStrategies[geoID][i] = &(*m_geoStrategyHandles[geoID][i]);
      }

      // create an end-vertex for all truth particles ending in the current AtlasRegion?
      bool forceEndVtx = std::find( m_forceEndVtxRegionsVec.begin(),
                                    m_forceEndVtxRegionsVec.end(),
                                    geoID ) != m_forceEndVtxRegionsVec.end();
      m_forceEndVtx[geoID] = forceEndVtx;
    }

    ATH_MSG_VERBOSE( "initialize() successful" );
    return StatusCode::SUCCESS;
}


/** framework methods */
StatusCode ISF::HepMC_TruthSvc::finalize()
{
    ATH_MSG_VERBOSE ( m_screenOutputPrefix << "Finalizing ..." );
    return StatusCode::SUCCESS;
}


/** Initialize the HepMC_TruthSvc and the truthSvc */
StatusCode ISF::HepMC_TruthSvc::initializeTruthCollection()
{
  m_mcEventCollection = 0;

  // retrieve McEventCollection from storegate
  if ( m_collectionName.empty()) {
    StatusCode status = m_storeGate->retrieve( m_mcEventCollection);
    if (status.isFailure())
        ATH_MSG_WARNING( "Unable to retrieve McEventCollection with name=" << m_collectionName);
    else
        ATH_MSG_DEBUG( "Sucessfully retrieved McEventCollection with name=" << m_collectionName);
  } else {
    if (m_storeGate->contains<McEventCollection>( m_collectionName)) {
      StatusCode status = m_storeGate->retrieve( m_mcEventCollection, m_collectionName);
      if (status.isFailure())
        ATH_MSG_WARNING( "Unable to retrieve McEventCollection with name=" << m_collectionName
                         << ". Will create new collection.");
      else
          ATH_MSG_DEBUG( "Sucessfully retrieved McEventCollection with name=" << m_collectionName);
    }
  }

  // in case no McEventCollection is present in storegate -> create new one and record it on SG
  if (! m_mcEventCollection) {
    McEventCollection* newcoll = new McEventCollection();
    HepMC::GenEvent *evt = new HepMC::GenEvent();
    newcoll->push_back(evt);
    StatusCode status = m_storeGate->record(newcoll, m_collectionName, true);
    if (status.isFailure()) {
      ATH_MSG_ERROR( "Unable to record newly created McEventCollection with name=" << m_collectionName);
      return StatusCode::FAILURE;
    }

    status = m_storeGate->retrieve( m_mcEventCollection, m_collectionName);
    if (status.isFailure()) {
      ATH_MSG_ERROR( "Unable to retrieve newly created McEventCollection with name=" << m_collectionName);
      return StatusCode::FAILURE;
    }
  }

  // collect last GenEvent from McEventCollection
  m_mcEvent = m_mcEventCollection->back();
  if (!m_mcEvent) {
    // no GenEvent present
    //  -> create new (emtpy) GenEvent and add it to McEventCollection
    m_mcEvent = new HepMC::GenEvent();
    m_mcEventCollection->push_back( m_mcEvent);
  }

  return StatusCode::SUCCESS;
}


StatusCode ISF::HepMC_TruthSvc::releaseEvent() {
  // set the output collection to const
  return m_storeGate->setConst(m_mcEventCollection);
}


/** Register a truth incident */
void ISF::HepMC_TruthSvc::registerTruthIncident( ISF::ITruthIncident& ti) {

  // pass whole vertex or individual child particles
  ti.setPassWholeVertices(m_passWholeVertex);

  // the GeoID
  AtlasDetDescr::AtlasRegion geoID = ti.geoID();

  // check geoID assigned to the TruthIncident
  if ( !validAtlasRegion(geoID) ) {
    ATH_MSG_ERROR("Unable to register truth incident with unknown SimGeoID="<< geoID);
    return;
  }

  ATH_MSG_VERBOSE( "Registering TruthIncident for SimGeoID="
                   << AtlasDetDescr::AtlasRegionHelper::getName(geoID) );

  // number of child particles
  unsigned short numSec = ti.numberOfChildren();
  if ( m_skipIfNoChildren && (numSec==0) ) {
    ATH_MSG_VERBOSE( "No child particles present in the TruthIncident,"
                     << " will not record this TruthIncident.");
    return;
  }

  // the parent particle -> get its barcode
  Barcode::ParticleBarcode parentBC = ti.parentBarcode();
  if ( m_skipIfNoParentBarcode && (parentBC==Barcode::fUndefinedBarcode) ) {
    ATH_MSG_VERBOSE( "Parent particle in TruthIncident does not have a barcode,"
                     << " will not record this TruthIncident.");
    return;
  }

  // loop over registered truth strategies for given geoID
  bool pass = false;
  for ( unsigned short stratID=0; (!pass) && (stratID<m_numStrategies[geoID]); stratID++) {
    // (*) test if given TruthIncident passes current strategy
    pass = m_geoStrategies[geoID][stratID]->pass(ti);
  }

  if (pass) {
    // at least one truth stategy returend true
    //  -> record incident
    recordIncidentToMCTruth( ti);

  } else {
    // none of the truth strategies returned true
    //  -> child particles will NOT be added to the TruthEvent collection
    
    // attach parent particle end vertex if it gets killed by this interaction
    if ( m_forceEndVtx[geoID] && !ti.parentSurvivesIncident() ) {
      HepMC::GenVertex *vtx = createGenVertexFromTruthIncident( ti);
      m_mcEvent->add_vertex( vtx);
    }

    //  -> assign shared barcode to all child particles (if barcode service supports it)
    setSharedChildParticleBarcode( ti);
  }

  return;
}

/** Record the given truth incident to the MC Truth */
void ISF::HepMC_TruthSvc::recordIncidentToMCTruth( ISF::ITruthIncident& ti) {
  // FIXME: shouldn't we use the barcode service to get these numbers, rather
  //        thank hard-code them?
  static int myLowestVertex = -200000 , myFirstSecondary = 200000;
  //if (myLowestVertex>0){ // first time only
  //  myLowestVertex = m_barcodeSvcQuick->getProperty( "FirstVertexBarcode" ) + 1;
  //  myFirstSecondary = m_barcodeSvcQuick->getProperty( "FirstSecondaryBarcode" ) - 1;
  //}

  Barcode::PhysicsProcessCode processCode = ti.physicsProcessCode();
  Barcode::ParticleBarcode       parentBC = ti.parentBarcode();

  // record the GenVertex
  HepMC::GenVertex *vtx = createGenVertexFromTruthIncident(ti);

  ATH_MSG_VERBOSE ( "Outgoing particles:" );
  // update parent barcode and add it to the vertex as outgoing particle
  Barcode::ParticleBarcode newPrimBC = m_barcodeSvcQuick->incrementBarcode( parentBC, processCode);
  if ( newPrimBC == Barcode::fUndefinedBarcode) {
    if (m_ignoreUndefinedBarcodes) {
      ATH_MSG_WARNING("Unable to generate new Particle Barcode. Continuing due to 'IgnoreUndefinedBarcodes'==True");
    } else {
      ATH_MSG_ERROR("Unable to generate new Particle Barcode. Aborting");
      abort();
    }
  }

  HepMC::GenParticle *parentAfterIncident = ti.parentParticleAfterIncident( newPrimBC );
  if(parentAfterIncident) {
    ATH_MSG_VERBOSE ( "Parent After Incident: " << *parentAfterIncident);
    vtx->add_particle_out( parentAfterIncident );
  }

  // update all _extra_ barcodes of child particles with parent info
  // MB: sofar extra barcode only contains parent info, so can be the same for each child
  if (m_storeExtraBCs) {
    // the parent particle -> get its extra barcode
    Barcode::ParticleBarcode parentExtraBC = ti.parentExtraBarcode();

    ti.setAllChildrenExtraBarcodes( parentExtraBC );
  }

  // add child particles to the vertex
  unsigned short numSec = ti.numberOfChildren();
  for ( unsigned short i=0; i<numSec; ++i) {

    bool writeOutChild = m_passWholeVertex || ti.childPassedFilters(i);

    if (writeOutChild) {
      // generate a new barcode for the child particle
      Barcode::ParticleBarcode secBC = m_barcodeSvcQuick->newSecondary( parentBC, processCode);
      if ( secBC == Barcode::fUndefinedBarcode) {
        if (m_ignoreUndefinedBarcodes)
          ATH_MSG_WARNING("Unable to generate new Secondary Particle Barcode. Continuing due to 'IgnoreUndefinedBarcodes'==True");
        else {
          ATH_MSG_ERROR("Unable to generate new Secondary Particle Barcode. Aborting");
          abort();
        }
      }
      HepMC::GenParticle *p = ti.childParticle(i, secBC );
      ATH_MSG_VERBOSE ( "Writing out " << i << "th child particle: " << *p);
      // add particle to vertex
      vtx->add_particle_out( p);

      // Check to see if this is meant to be a "parent" vertex
      if ( p && p->barcode() < myFirstSecondary){
        vtx->suggest_barcode( myLowestVertex );
        ++myLowestVertex;
        if (m_quasiStableParticlesIncluded){
          ATH_MSG_VERBOSE( "Found a case of low barcode (" << p->barcode() << " < " << myFirstSecondary << " changing vtx barcode to " << myLowestVertex+1 );
        } else {
          ATH_MSG_WARNING( "Modifying vertex barcode, but no apparent quasi-stable particle simulation enabled." );
          ATH_MSG_WARNING( "This means that you have encountered a very strange configuration.  Watch out!" );
        }
      }
    } // <-- if write out child particle
    else {
      ATH_MSG_VERBOSE ( "Not writing out " << i << "th child particle." );
    }

  } // <-- loop over all child particles

  // finally add the vertex to the current GenEvent
  m_mcEvent->add_vertex( vtx);
}

/** Record the given truth incident to the MC Truth */
HepMC::GenVertex *ISF::HepMC_TruthSvc::createGenVertexFromTruthIncident( ISF::ITruthIncident& ti) {

  Barcode::PhysicsProcessCode processCode = ti.physicsProcessCode();
  Barcode::ParticleBarcode       parentBC = ti.parentBarcode();

  std::vector<double> weights(1);
  Barcode::ParticleBarcode primaryBC = parentBC % m_barcodeSvcQuick->particleGenerationIncrement();
  weights[0] = static_cast<double>( primaryBC );

  // Check for a previous end vertex on this particle.  If one existed, then we should put down next to this
  //  a new copy of the particle.  This is the agreed upon version of the quasi-stable particle truth, where
  //  the vertex at which we start Q-S simulation no longer conserves energy, but we keep both copies of the
  //  truth particles
  HepMC::GenParticle *parent = ti.parentParticle();
  if (!parent) {
    ATH_MSG_ERROR("Unable to write particle interaction to MC truth due to missing parent HepMC::GenParticle instance");
    abort();
  }

  // generate vertex
  Barcode::VertexBarcode vtxbcode = m_barcodeSvcQuick->newVertex( parentBC, processCode );
  if ( vtxbcode == Barcode::fUndefinedBarcode) {
    if (m_ignoreUndefinedBarcodes) {
      ATH_MSG_WARNING("Unable to generate new Truth Vertex Barcode. Continuing due to 'IgnoreUndefinedBarcodes'==True");
    } else {
      ATH_MSG_ERROR("Unable to generate new Truth Vertex Barcode. Aborting");
      abort();
    }
  }
  int vtxID = 1000 + static_cast<int>(processCode);
  HepMC::GenVertex *vtx = new HepMC::GenVertex( ti.position(), vtxID, weights );
  vtx->suggest_barcode( vtxbcode );

  if (parent->end_vertex()){
    if(!m_quasiStableParticlesIncluded) {
      ATH_MSG_WARNING("Parent particle found with an end vertex attached.  This should only happen");
      ATH_MSG_WARNING("in the case of simulating quasi-stable particles.  That functionality");
      ATH_MSG_WARNING("is not yet validated in ISF, so you'd better know what you're doing.");
      ATH_MSG_WARNING("Will delete the old vertex and swap in the new one.");
    }
    HepMC::GenParticle *new_parent = new HepMC::GenParticle( *parent ); // Copy the old guy

    // Change the barcode
    new_parent->suggest_barcode( m_barcodeSvcQuick->newSecondary( parentBC ) );

    // Add the new parent to the old production vertex
    parent->production_vertex()->add_particle_out( new_parent );

    // Now add the new vertex to the new parent
    vtx->add_particle_in( new_parent );
    ATH_MSG_VERBOSE ( "QS End Vertex representing process: " << processCode << ", for parent with barcode "<<parentBC<<". Creating." );
    ATH_MSG_VERBOSE ( "Parent: " << *parent);
  } else { // Normal simulation
    // add parent particle to vtx
    vtx->add_particle_in( parent );
    ATH_MSG_VERBOSE ( "End Vertex representing process: " << processCode << ", for parent with barcode "<<parentBC<<". Creating." );
    ATH_MSG_VERBOSE ( "Parent: " << *parent);
  }

  return vtx;
}

/** Set shared barcode for child particles particles */
void ISF::HepMC_TruthSvc::setSharedChildParticleBarcode( ISF::ITruthIncident& ti) {
  Barcode::PhysicsProcessCode processCode = ti.physicsProcessCode();
  Barcode::ParticleBarcode       parentBC = ti.parentBarcode();

  ATH_MSG_VERBOSE ( "End Vertex representing process: " << processCode << ". TruthIncident failed cuts. Skipping.");

  // generate one new barcode for all child particles
  Barcode::ParticleBarcode childBC = m_barcodeSvcQuick->sharedChildBarcode( parentBC, processCode);

  // propagate this barcode into the TruthIncident only if
  // it is a proper barcode, ie !=fUndefinedBarcode
  if (childBC != Barcode::fUndefinedBarcode) {
    ti.setAllChildrenBarcodes( childBC );
  }

  // update all _extra_ barcodes of child particles with parent info
  // MB: sofar extra barcode only contains parent info, so can be the same for each child particle
  if (m_storeExtraBCs) {
    // the parent particle -> get its extra barcode
    Barcode::ParticleBarcode parentExtraBC = ti.parentExtraBarcode();

    ti.setAllChildrenExtraBarcodes( parentExtraBC );
  }
}
