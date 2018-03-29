/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "TruthSvc.h"
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

// DetectorDescription
#include "AtlasDetDescr/AtlasRegionHelper.h"

/** Constructor **/
ISF::TruthSvc::TruthSvc(const std::string& name,ISvcLocator* svc) :
  base_class(name,svc),
  m_barcodeSvc("BarcodeSvc",name),
  m_geoStrategies(),
  m_numStrategies(),
  m_skipIfNoChildren(true),
  m_skipIfNoParentBarcode(true),
  m_ignoreUndefinedBarcodes(false),
  m_passWholeVertex(true),
  m_forceEndVtxRegionsVec(),
  m_forceEndVtx(),
  m_quasiStableParticlesIncluded(false),
  m_secondaryParticleBcOffset(Barcode::fUndefinedBarcode),
  m_myLowestVertexBC(Barcode::fUndefinedBarcode)
{
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

    declareProperty("QuasiStableParticlesIncluded",      m_quasiStableParticlesIncluded);
}

ISF::TruthSvc::~TruthSvc()
{}


/** framework methods */
StatusCode ISF::TruthSvc::initialize()
{
    ATH_MSG_VERBOSE( "initialize()" );

    // Screen output
    ATH_MSG_DEBUG("--------------------------------------------------------");

    // retrieve BarcodeSvc
    if ( m_barcodeSvc.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Could not retrieve BarcodeService. Abort.");
      return StatusCode::FAILURE;
    }

    // retrieve the strategies for each atlas region (Athena Alg Tools)
    // and setup whether we want to write end-vertices in this region whenever a truth particle dies
    for ( unsigned short geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions; ++geoID) {
      if ( m_geoStrategyHandles[geoID].retrieve().isFailure() ) {
        ATH_MSG_FATAL("Could not retrieve TruthStrategy Tool Array for SimGeoID="
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

    ATH_MSG_VERBOSE("initialize() successful");
    return StatusCode::SUCCESS;
}


/** framework methods */
StatusCode ISF::TruthSvc::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}


/** Initialize the TruthSvc and the truthSvc */
StatusCode ISF::TruthSvc::initializeTruthCollection()
{
  m_myLowestVertexBC          = m_barcodeSvc->secondaryVertexBcOffset();
  m_secondaryParticleBcOffset = m_barcodeSvc->secondaryParticleBcOffset();

  return StatusCode::SUCCESS;
}

/** Delete child vertex */
void ISF::TruthSvc::deleteChildVertex(HepMC::GenVertex *vtx) {

  for (HepMC::GenVertex::particles_out_const_iterator iter = vtx->particles_out_const_begin();
       iter != vtx->particles_out_const_end(); ++iter) {
     if( (*iter) && (*iter)->end_vertex() ) {
       verticesToDelete.push_back( (*iter)->end_vertex() );
     }
  }

  vtx->parent_event()->remove_vertex(vtx);
   
  return;
}


StatusCode ISF::TruthSvc::releaseEvent() {
  return StatusCode::SUCCESS;
}


/** Register a truth incident */
void ISF::TruthSvc::registerTruthIncident( ISF::ITruthIncident& ti) {

  // pass whole vertex or individual child particles
  ti.setPassWholeVertices(m_passWholeVertex);

  // the GeoID
  AtlasDetDescr::AtlasRegion geoID = ti.geoID();

  // check geoID assigned to the TruthIncident
  if ( !validAtlasRegion(geoID) ) {
    const auto& position = ti.position();
    ATH_MSG_ERROR("Unable to register truth incident with unknown SimGeoID="<< geoID
                  << " at position z=" << position.z() << " r=" << position.perp());
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
    // at least one truth strategy returned true
    //  -> record incident
    recordIncidentToMCTruth( ti);

  } else {
    // none of the truth strategies returned true
    //  -> child particles will NOT be added to the TruthEvent collection
    
    // attach parent particle end vertex if it gets killed by this interaction
    if ( m_forceEndVtx[geoID] && !ti.parentSurvivesIncident() ) {
      createGenVertexFromTruthIncident( ti);
    }

    //  -> assign shared barcode to all child particles (if barcode service supports it)
    setSharedChildParticleBarcode( ti);
  }

  return;
}

/** Record the given truth incident to the MC Truth */
void ISF::TruthSvc::recordIncidentToMCTruth( ISF::ITruthIncident& ti) {

  Barcode::PhysicsProcessCode processCode = ti.physicsProcessCode();
  Barcode::ParticleBarcode       parentBC = ti.parentBarcode();

  // record the GenVertex
  HepMC::GenVertex *vtx = createGenVertexFromTruthIncident(ti);

  ATH_MSG_VERBOSE ( "Outgoing particles:" );
  // update parent barcode and add it to the vertex as outgoing particle
  Barcode::ParticleBarcode newPrimBC = m_barcodeSvc->incrementBarcode( parentBC, processCode);
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

  // add child particles to the vertex
  unsigned short numSec = ti.numberOfChildren();
  for ( unsigned short i=0; i<numSec; ++i) {

    bool writeOutChild = m_passWholeVertex || ti.childPassedFilters(i);

    if (writeOutChild) {
      // generate a new barcode for the child particle
      Barcode::ParticleBarcode secBC = m_barcodeSvc->newSecondary( parentBC, processCode);
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
      if ( p && p->barcode() < m_secondaryParticleBcOffset ){
        vtx->suggest_barcode( m_myLowestVertexBC );
        ++m_myLowestVertexBC;
        if (m_quasiStableParticlesIncluded){
          ATH_MSG_VERBOSE( "Found a case of low barcode (" << p->barcode() << " < " <<
                           m_secondaryParticleBcOffset  << " changing vtx barcode to " << m_myLowestVertexBC+1 );
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
}

/** Record the given truth incident to the MC Truth */
HepMC::GenVertex *ISF::TruthSvc::createGenVertexFromTruthIncident( ISF::ITruthIncident& ti) {

  Barcode::PhysicsProcessCode processCode = ti.physicsProcessCode();
  Barcode::ParticleBarcode       parentBC = ti.parentBarcode();

  std::vector<double> weights(1);
  Barcode::ParticleBarcode primaryBC = parentBC % m_barcodeSvc->particleGenerationIncrement();
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
  HepMC::GenEvent *mcEvent = parent->parent_event();
  if (!mcEvent) {
    ATH_MSG_ERROR("Unable to write particle interaction to MC truth due to missing parent HepMC::GenEvent instance");
    abort();
  }

  // generate vertex
  Barcode::VertexBarcode vtxbcode = m_barcodeSvc->newVertex( parentBC, processCode );
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

    // Remove the old vertex from the event
    //parent->parent_event()->remove_vertex( parent->end_vertex() );
     
    // KB: Remove the old vertex and their all child vertices  from the event
    verticesToDelete.resize(0);
    verticesToDelete.push_back(parent->end_vertex());
    for ( unsigned short i = 0; i<verticesToDelete.size(); ++i ) {
       this->deleteChildVertex(verticesToDelete.at(i));
    }

    // Now add the new vertex to the new parent
    vtx->add_particle_in( parent );
    ATH_MSG_VERBOSE ( "QS End Vertex representing process: " << processCode << ", for parent with barcode "<<parentBC<<". Creating." );
    ATH_MSG_VERBOSE ( "Parent: " << *parent);
  } else { // Normal simulation
    // add parent particle to vtx
    vtx->add_particle_in( parent );
    ATH_MSG_VERBOSE ( "End Vertex representing process: " << processCode << ", for parent with barcode "<<parentBC<<". Creating." );
    ATH_MSG_VERBOSE ( "Parent: " << *parent);
  }

  mcEvent->add_vertex( vtx );

  return vtx;
}

/** Set shared barcode for child particles particles */
void ISF::TruthSvc::setSharedChildParticleBarcode( ISF::ITruthIncident& ti) {
  Barcode::PhysicsProcessCode processCode = ti.physicsProcessCode();
  Barcode::ParticleBarcode       parentBC = ti.parentBarcode();

  ATH_MSG_VERBOSE ( "End Vertex representing process: " << processCode << ". TruthIncident failed cuts. Skipping.");

  // generate one new barcode for all child particles
  Barcode::ParticleBarcode childBC = m_barcodeSvc->sharedChildBarcode( parentBC, processCode);

  // propagate this barcode into the TruthIncident only if
  // it is a proper barcode, ie !=fUndefinedBarcode
  if (childBC != Barcode::fUndefinedBarcode) {
    ti.setAllChildrenBarcodes( childBC );
  }
}
