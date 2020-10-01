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
//
#include "TruthUtils/HepMCHelpers.h" // for MC::findChildren(...)
// HepMC includes
#include "AtlasHepMC/SimpleVector.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
// CLHEP includes
#include "CLHEP/Geometry/Point3D.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegionHelper.h"

#include <sstream>

#undef DEBUG_TRUTHSVC

/** Constructor **/
ISF::TruthSvc::TruthSvc(const std::string& name,ISvcLocator* svc) :
  base_class(name,svc),
  m_barcodeSvc("BarcodeSvc",name),
  m_truthStrategies(this),
  m_geoStrategies(),
  m_numStrategies(),
  m_skipIfNoChildren(true),
  m_skipIfNoParentBarcode(true),
  m_ignoreUndefinedBarcodes(false),
  m_passWholeVertex(true),
  m_forceEndVtxRegionsVec(),
  m_forceEndVtx(),
  m_quasiStableParticlesIncluded(false)
{
  // the barcode service (used to compute Vertex Barcodes)
  declareProperty("BarcodeSvc",                        m_barcodeSvc              );
  // MCTruth writing strategies
  declareProperty("SkipIfNoChildren",                  m_skipIfNoChildren        );
  declareProperty("SkipIfNoParentBarcode",             m_skipIfNoParentBarcode   );
  declareProperty("IgnoreUndefinedBarcodes",           m_ignoreUndefinedBarcodes );
  declareProperty("PassWholeVertices",                 m_passWholeVertex         );
  declareProperty("TruthStrategies",                   m_truthStrategies);
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

  // copy a pointer to the strategy instance to the local
  // array of pointers (for faster access)
  ATH_CHECK(m_truthStrategies.retrieve());
  // Would be nicer to make m_geoStrategies a vector of vectors
  for ( unsigned short geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions; ++geoID) {
    m_numStrategies[geoID] = 0;
    for ( const auto &truthStrategy : m_truthStrategies) {
      if(truthStrategy->appliesToRegion(geoID)) {
        ++m_numStrategies[geoID];
      }
    }
  }
  for ( unsigned short geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions; ++geoID) {
    m_geoStrategies[geoID] = new ISF::ITruthStrategy*[m_numStrategies[geoID]];
    unsigned short curNumStrategies = m_truthStrategies.size();
    unsigned short nStrat(0);
    for ( unsigned short i = 0; i < curNumStrategies; ++i) {
      if(m_truthStrategies[i]->appliesToRegion(geoID)) {
        m_geoStrategies[geoID][nStrat++] = &(*m_truthStrategies[i]);
      }
    }

    // setup whether we want to write end-vertices in this region whenever a truth particle dies
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
  ATH_CHECK( m_barcodeSvc->initializeBarcodes() );
  return StatusCode::SUCCESS;
}

/** Delete child vertex */
void ISF::TruthSvc::deleteChildVertex(HepMC::GenVertexPtr vertex) const {
  std::vector<HepMC::GenVertexPtr> verticesToDelete;
  verticesToDelete.resize(0);
  verticesToDelete.push_back(vertex);
  for ( unsigned short i = 0; i<verticesToDelete.size(); ++i ) {
    HepMC::GenVertexPtr vtx = verticesToDelete.at(i);
    for (HepMC::GenVertex::particles_out_const_iterator iter = vtx->particles_out_const_begin();
         iter != vtx->particles_out_const_end(); ++iter) {
      if( (*iter) && (*iter)->end_vertex() ) {
        verticesToDelete.push_back( (*iter)->end_vertex() );
      }
    }
    vtx->parent_event()->remove_vertex(vtx);
  }
  return;
}


StatusCode ISF::TruthSvc::releaseEvent() {
  return StatusCode::SUCCESS;
}


/** Register a truth incident */
void ISF::TruthSvc::registerTruthIncident( ISF::ITruthIncident& ti) const {

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
    ATH_MSG_VERBOSE("At least one TruthStrategy passed.");
    // at least one truth strategy returned true
    //  -> record incident
    recordIncidentToMCTruth( ti);

  } else {
    // none of the truth strategies returned true
    //  -> child particles will NOT be added to the TruthEvent collection
    // attach parent particle end vertex if it gets killed by this interaction
    if ( m_forceEndVtx[geoID] && !ti.parentSurvivesIncident() ) {
      ATH_MSG_VERBOSE("No TruthStrategies passed and parent destroyed - create end vertex.");
      const bool replaceVertex(true);
      this->createGenVertexFromTruthIncident( ti, replaceVertex );

#ifdef DEBUG_TRUTHSVC
      const std::string survival = (ti.parentSurvivesIncident()) ? "parent survives" : "parent destroyed";
      const std::string vtxType = (ti.interactionClassification()==ISF::STD_VTX) ? "Normal" : "Quasi-stable";
      ATH_MSG_INFO("TruthSvc: " << vtxType << " vertex + " << survival
                   << ", TI Class: " << ti.interactionClassification()
                   << ", ProcessType: " << ti.physicsProcessCategory()
                   << ", ProcessSubType: " << ti.physicsProcessCode());
#endif

    }

    //  -> assign shared barcode to all child particles (if barcode service supports it)
    setSharedChildParticleBarcode( ti);
  }

  return;
}

/** Record the given truth incident to the MC Truth */
void ISF::TruthSvc::recordIncidentToMCTruth( ISF::ITruthIncident& ti) const {
#ifdef  DEBUG_TRUTHSVC
  ATH_MSG_INFO("Starting recordIncidentToMCTruth(...)");
#endif
  Barcode::PhysicsProcessCode processCode = ti.physicsProcessCode();
  Barcode::ParticleBarcode       parentBC = ti.parentBarcode();

  // record the GenVertex
  const bool replaceVertex(false);
  HepMC::GenVertexPtr  vtx = createGenVertexFromTruthIncident(ti, replaceVertex);
  const ISF::InteractionClass_t classification = ti.interactionClassification();
#ifdef DEBUG_TRUTHSVC
  const std::string survival = (ti.parentSurvivesIncident()) ? "parent survives" : "parent destroyed";
  const std::string vtxType = (ti.interactionClassification()==ISF::STD_VTX) ? "Normal" : "Quasi-stable";
  ATH_MSG_INFO("TruthSvc: " << vtxType << " vertex + " << survival
               << ", TI Class: " << ti.interactionClassification()
               << ", ProcessType: " << ti.physicsProcessCategory()
               << ", ProcessSubType: " << ti.physicsProcessCode());
#endif

  ATH_MSG_VERBOSE ( "Outgoing particles:" );
  // update parent barcode and add it to the vertex as outgoing particle
  Barcode::ParticleBarcode newPrimBC = Barcode::fUndefinedBarcode;
  if (classification == ISF::QS_SURV_VTX) {
    // Special case when a particle with a pre-defined decay interacts
    // and survives.
    // Set the barcode to the next available value below the simulation
    // barcode offset.
    newPrimBC = this->maxGeneratedParticleBarcode(ti.parentParticle()->parent_event())+1;
  }
  else {
    newPrimBC = m_barcodeSvc->incrementBarcode( parentBC, processCode);
  }
  if ( newPrimBC == Barcode::fUndefinedBarcode) {
    if (m_ignoreUndefinedBarcodes) {
      ATH_MSG_WARNING("Unable to generate new Particle Barcode. Continuing due to 'IgnoreUndefinedBarcodes'==True");
    } else {
      ATH_MSG_FATAL("Unable to generate new Particle Barcode. Aborting");
      abort();
    }
  }

  HepMC::GenParticlePtr  parentBeforeIncident = ti.parentParticle();
  HepMC::GenParticlePtr  parentAfterIncident = ti.parentParticleAfterIncident( newPrimBC ); // This call changes ti.parentParticle() output
  if(parentAfterIncident) {
    ATH_MSG_VERBOSE ( "Parent After Incident: " << *parentAfterIncident);
    if (classification==ISF::QS_SURV_VTX) {
      // Special case when a particle with a pre-defined decay
      // interacts and survives.
      // 1) As the parentParticleAfterIncident has a pre-defined decay
      // its status should be to 2.
      parentAfterIncident->set_status(2);
      // 2) A new GenVertex for the intermediate interaction should be
      // added.
      std::unique_ptr<HepMC::GenVertex> newVtx = std::make_unique<HepMC::GenVertex>( vtx->position(), vtx->id(), vtx->weights() );
#ifdef DEBUG_TRUTHSVC
      ATH_MSG_INFO("New GenVertex 1: " << *(newVtx.get()) );
      ATH_MSG_INFO("New QS GenVertex 1: " << *(newVtx.get()) );
#endif
      HepMC::GenEvent *mcEvent = parentBeforeIncident->parent_event();
      HepMC::suggest_barcode(newVtx.get(), this->maxGeneratedVertexBarcode(mcEvent)-1 );
#ifdef DEBUG_TRUTHSVC
      ATH_MSG_INFO("New QSGenVertex 2: " << *(newVtx.get()) );
#endif
      auto tmpVtx = newVtx.get();
#ifdef DEBUG_TRUTHSVC
      ATH_MSG_INFO("New QS GenVertex 3: " << (*tmpVtx) );
#endif
      if(!mcEvent->add_vertex( newVtx.release() )) {
        ATH_MSG_FATAL("Failed to add GenVertex to GenEvent.");
        abort();
      }
      tmpVtx->add_particle_in( parentBeforeIncident );
      tmpVtx->add_particle_out( parentAfterIncident );
      vtx->add_particle_in( parentAfterIncident );
      vtx = tmpVtx;
    }
    else {
      vtx->add_particle_out( parentAfterIncident );
    }
  }

  const bool isQuasiStableVertex = (classification == ISF::QS_PREDEF_VTX); // QS_DEST_VTX and QS_SURV_VTX should be treated as normal from now on.
  // add child particles to the vertex
  unsigned short numSec = ti.numberOfChildren();
  if (isQuasiStableVertex) {
    // Here we are checking if the existing GenVertex has the same
    // number of child particles as the truth incident.
    // FIXME should probably make this part a separate function and
    // also check if the pdgids of the child particles are the same
    // too.
    unsigned short nVertexChildren=vtx->particles_out_size();
    if(parentAfterIncident) { nVertexChildren-=1; }
    if(nVertexChildren!=numSec) {
      ATH_MSG_WARNING("Existing vertex has " << nVertexChildren << " children. " <<
                      "Number of secondaries in current truth incident = " << numSec);
    }
    ATH_MSG_VERBOSE("Existing vertex has " << nVertexChildren << " children. " <<
                 "Number of secondaries in current truth incident = " << numSec);
  }
  const std::vector<HepMC::GenParticlePtr> childParticleVector = (isQuasiStableVertex) ? MC::findChildren(ti.parentParticle()) : std::vector<HepMC::GenParticlePtr>();
  std::vector<HepMC::GenParticlePtr> matchedChildParticles;
  for ( unsigned short i=0; i<numSec; ++i) {

    bool writeOutChild = isQuasiStableVertex || m_passWholeVertex || ti.childPassedFilters(i);

    if (writeOutChild) {
      HepMC::GenParticlePtr  p = nullptr;
      if(isQuasiStableVertex) {
        //Find matching GenParticle in GenVertex
        const int childPDGcode= ti.childPdgCode(i);
        bool noMatch(true);
        for(auto childParticle : childParticleVector) {
          if( (childParticle->pdg_id() == childPDGcode) && std::count(matchedChildParticles.begin(),matchedChildParticles.end(),childParticle)==0) {
            noMatch=false;
            ATH_MSG_VERBOSE("Found a matching Quasi-stable GenParticle with PDGcode " << childPDGcode << ":\n\t" << *childParticle );
            matchedChildParticles.push_back(childParticle);
            // FIXME There is a weakness in the code here for
            // vertices with multiple children with the same
            // pdgid. The code relies on the order of the children in
            // childParticleVector being the same as in the
            // truthIncident...
            p = ti.updateChildParticle( i, childParticle );
            break;
          }
        }
        if (noMatch) {
          std::ostringstream warnStream;
          warnStream << "Failed to find a Quasi-stable GenParticle with PDGID " << childPDGcode << ". Options are: \n";
          for(auto childParticle : childParticleVector) {
            warnStream << childParticle->pdg_id() <<"\n";
          }
          ATH_MSG_WARNING(warnStream.str());
        }
      }
      else {
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
        p = ti.childParticle( i, secBC );
        // add particle to vertex
        vtx->add_particle_out( p);
      }
      ATH_MSG_VERBOSE ( "Writing out " << i << "th child particle: " << *p);
    } // <-- if write out child particle
    else {
      ATH_MSG_VERBOSE ( "Not writing out " << i << "th child particle." );
    }

  } // <-- loop over all child particles
  ATH_MSG_VERBOSE("--------------------------------------------------------");
}

/** Record the given truth incident to the MC Truth */
HepMC::GenVertexPtr  ISF::TruthSvc::createGenVertexFromTruthIncident( ISF::ITruthIncident& ti,
                                                                   bool replaceExistingGenVertex) const {

  Barcode::PhysicsProcessCode processCode = ti.physicsProcessCode();
  Barcode::ParticleBarcode       parentBC = ti.parentBarcode();

  std::vector<double> weights(1);
  Barcode::ParticleBarcode primaryBC = parentBC % m_barcodeSvc->particleGenerationIncrement();
  weights[0] = static_cast<double>( primaryBC );

  // Check for a previous end vertex on this particle.  If one existed, then we should put down next to this
  //  a new copy of the particle.  This is the agreed upon version of the quasi-stable particle truth, where
  //  the vertex at which we start Q-S simulation no longer conserves energy, but we keep both copies of the
  //  truth particles
  HepMC::GenParticlePtr  parent = ti.parentParticle();
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
  std::unique_ptr<HepMC::GenVertex> vtx = std::make_unique<HepMC::GenVertex>( ti.position(), vtxID, weights );
  HepMC::suggest_barcode( vtx.get(), vtxbcode );

  if (parent->end_vertex()){
    if(!m_quasiStableParticlesIncluded) {
      ATH_MSG_WARNING("Parent particle found with an end vertex attached.  This should only happen");
      ATH_MSG_WARNING("in the case of simulating quasi-stable particles.  That functionality");
      ATH_MSG_WARNING("is not yet validated in ISF, so you'd better know what you're doing.");
      ATH_MSG_WARNING("Will delete the old vertex and swap in the new one.");
    }
    auto* oldVertex = parent->end_vertex();
#ifdef DEBUG_TRUTHSVC
    ATH_MSG_VERBOSE("createGVfromTI Existing QS GenVertex 1: " << *oldVertex );
    ATH_MSG_VERBOSE("createGVfromTI QS Parent 1: " << *parent);
#endif
    if(replaceExistingGenVertex) {
      vtx->add_particle_in( parent );
      ATH_MSG_VERBOSE("createGVfromTI Replacement QS GenVertex: " << *(vtx.get()) );
      mcEvent->add_vertex( vtx.release() );
      // Delete oldVertex and children here
      this->deleteChildVertex(oldVertex);
    }
    else {
      //oldVertex->suggest_barcode( vtxbcode );
      oldVertex->set_position( ti.position() );
      oldVertex->set_id( vtxID );
      oldVertex->weights() = weights;
#ifdef DEBUG_TRUTHSVC
      ATH_MSG_VERBOSE("createGVfromTI Existing QS GenVertex 2: " << *oldVertex );
#endif
    }
#ifdef DEBUG_TRUTHSVC
    ATH_MSG_VERBOSE ( "createGVfromTI QS End Vertex representing process: " << processCode << ", for parent with barcode "<<parentBC<<". Creating." );
    ATH_MSG_VERBOSE ( "createGVfromTI QS Parent 2: " << *parent);
#endif
  } else { // Normal simulation
#ifdef DEBUG_TRUTHSVC
    ATH_MSG_VERBOSE ("createGVfromTI Parent 1: " << *parent);
#endif
    // add parent particle to vtx
    vtx->add_particle_in( parent );
#ifdef DEBUG_TRUTHSVC
    ATH_MSG_VERBOSE ( "createGVfromTI End Vertex representing process: " << processCode << ", for parent with barcode "<<parentBC<<". Creating." );
    ATH_MSG_VERBOSE ( "createGVfromTI Parent 2: " << *parent);
#endif
    mcEvent->add_vertex( vtx.release() );
  }

  return parent->end_vertex();
}

/** Set shared barcode for child particles particles */
void ISF::TruthSvc::setSharedChildParticleBarcode( ISF::ITruthIncident& ti) const {
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

int ISF::TruthSvc::maxGeneratedParticleBarcode(HepMC::GenEvent *genEvent) const {
  int maxBarcode=0;
  const int firstSecondaryParticleBarcode(m_barcodeSvc->secondaryParticleBcOffset());
  for (auto currentGenParticle: *genEvent) {
    const int barcode=HepMC::barcode(currentGenParticle);
    if(barcode > maxBarcode && barcode < firstSecondaryParticleBarcode) { maxBarcode=barcode; }
  }
  return maxBarcode;
}

int ISF::TruthSvc::maxGeneratedVertexBarcode(HepMC::GenEvent *genEvent) const {
  int maxBarcode=0;
  const int firstSecondaryVertexBarcode(m_barcodeSvc->secondaryVertexBcOffset());
  HepMC::GenEvent::vertex_const_iterator currentGenVertexIter;
  for (currentGenVertexIter= genEvent->vertices_begin();
       currentGenVertexIter!= genEvent->vertices_end();
       ++currentGenVertexIter) {
    const int barcode((*currentGenVertexIter)->barcode());
    if(barcode < maxBarcode && barcode > firstSecondaryVertexBarcode) { maxBarcode=barcode; }
  }
  return maxBarcode;
}
