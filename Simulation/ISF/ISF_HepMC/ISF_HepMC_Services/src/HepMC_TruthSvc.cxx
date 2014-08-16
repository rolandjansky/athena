/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HepMC_TruthSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_HepMC_Services/HepMC_TruthSvc.h"
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
  m_skipIfNoSecondaries(true),
  m_skipIfNoPrimaryBarcode(true),
  m_ignoreUndefinedBarcodes(false),
  m_screenOutputPrefix("isf >> "),
  m_screenEmptyPrefix(),
  m_storeExtraBCs(true)
{
    // the particle stack filler tool
    declareProperty("McEventCollection",                m_collectionName          );
    // refine the screen output for debugging
    declareProperty("ScreenOutputPrefix",               m_screenOutputPrefix      );
    // the barcode service (used to compute Vertex Barcodes)
    declareProperty("BarcodeSvc",                       m_barcodeSvc              );
    // MCTruth writing strategies
    declareProperty("SkipIfNoSecondaries",              m_skipIfNoSecondaries     );
    declareProperty("SkipIfNoPrimaryBarcode",           m_skipIfNoPrimaryBarcode  );
    declareProperty("IgnoreUndefinedBarcodes",          m_ignoreUndefinedBarcodes );

    // the truth strategies for the different SimGeoIDs
    declareProperty("BeamPipeTruthStrategies",          m_geoStrategyHandles[AtlasDetDescr::fAtlasForward] );
    declareProperty("IDTruthStrategies",                m_geoStrategyHandles[AtlasDetDescr::fAtlasID]      );
    declareProperty("CaloTruthStrategies",              m_geoStrategyHandles[AtlasDetDescr::fAtlasCalo]    );
    declareProperty("MSTruthStrategies",                m_geoStrategyHandles[AtlasDetDescr::fAtlasMS]      );

    declareProperty("StoreExtraBarcodes",               m_storeExtraBCs);

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
    ATH_MSG_INFO( "initialize()" );

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

    // retrieve all registered geo strategies (Athena Alg Tools)
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
    }

    ATH_MSG_INFO( "initialize() successful" );
    return StatusCode::SUCCESS;
}


/** framework methods */
StatusCode ISF::HepMC_TruthSvc::finalize()
{
    ATH_MSG_INFO ( m_screenOutputPrefix << "Finalizing ..." );
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
void ISF::HepMC_TruthSvc::registerTruthIncident( ISF::ITruthIncident& truth) {

  // the GeoID
  AtlasDetDescr::AtlasRegion geoID = truth.geoID();

  // check geoID assigned to the TruthIncident
  if ( !validAtlasRegion(geoID) ) {
    ATH_MSG_ERROR("Unable to register truth incident with unknown SimGeoID="<< geoID);
    return;
  }

  ATH_MSG_VERBOSE( "Registering TruthIncident for SimGeoID="
                   << AtlasDetDescr::AtlasRegionHelper::getName(geoID) );

  // number of secondary particles
  unsigned short numSec = truth.numberOfSecondaries();
  if ( (numSec==0) && (m_skipIfNoSecondaries) ) {
    ATH_MSG_VERBOSE( "No secondary particles present in the TruthIncident,"
                     << " will not record this TruthIncident.");
    return;
  }

  // the primary particle -> get its barcode
  Barcode::ParticleBarcode primBC = truth.primaryBarcode();
  if ( (primBC==Barcode::fUndefinedBarcode) && (m_skipIfNoPrimaryBarcode) ) {
    ATH_MSG_VERBOSE( "Primary particle in TruthIncident does not have a barcode,"
                     << " will not record this TruthIncident.");
    return;
  }

  //// the primary particle -> get its extra barcode
  Barcode::ParticleBarcode primExtraBC = truth.primaryExtraBarcode();

  // process code causing this truth incident
  Barcode::PhysicsProcessCode processCode = truth.physicsProcessCode();

  // loop over registered truth strategies for given geoID
  bool pass = false;
  for ( unsigned short stratID=0; (!pass) && (stratID<m_numStrategies[geoID]); stratID++) {
    // (*) test if given TruthIncident passes current strategy
    pass = m_geoStrategies[geoID][stratID]->pass(truth);
  }

  //
  // a truth stategy returend true -> record incident
  //
  if (pass) {
    ATH_MSG_DEBUG("TruthIncident passed cuts. Will record this one.");
    // passed -> create and record vertex now
    //
    std::vector<double> weights(2);
    weights[0] = static_cast<double>(primBC);
    weights[1] = static_cast<double>(processCode);

    // generate vertex
    HepMC::GenVertex *vtx = new HepMC::GenVertex( truth.position(), 0, weights ); // 0 = barcode, overwritten below
    Barcode::VertexBarcode vtxbcode = m_barcodeSvcQuick->newVertex( primBC, processCode );
    if ( vtxbcode == Barcode::fUndefinedBarcode) {
      if (m_ignoreUndefinedBarcodes)
        ATH_MSG_WARNING("Unable to generate new Truth Vertex Barcode. Continuing due to 'IgnoreUndefinedBarcodes'==True");
      else {
        ATH_MSG_ERROR("Unable to generate new Truth Vertex Barcode. Aborting");
        abort();
      }
    }
    vtx->suggest_barcode( vtxbcode);

    // add primary particle to vtx
    bool setPersistent = true;
    HepMC::GenParticle *prim = truth.primaryParticle( setPersistent);
    vtx->add_particle_in( prim );
  
    // update primary barcode and add it to the vertex as outgoing particle
    Barcode::ParticleBarcode newPrimBC = m_barcodeSvcQuick->incrementBarcode( primBC, processCode);
    if ( newPrimBC == Barcode::fUndefinedBarcode) {
      if (m_ignoreUndefinedBarcodes)
        ATH_MSG_WARNING("Unable to generate new Particle Barcode. Continuing due to 'IgnoreUndefinedBarcodes'==True");
      else {
        ATH_MSG_ERROR("Unable to generate new Particle Barcode. Aborting");
        abort();
      }
    }
    prim = truth.primaryParticleAfterIncident( newPrimBC, setPersistent);
    vtx->add_particle_out( prim ); // prim==0 is checked inside add_particle_out(..)

    // update all _extra_ barcodes of secondary particles with parent info
    // MB: sofar extra barcode only contains parent info, so can be the same for each secondary
    if (m_storeExtraBCs)
      truth.setAllSecondaryExtraBarcodes( primExtraBC );
    
    // add all secondary particles to the vertex
    for ( unsigned short i=0; i<numSec; ++i) {
      // generate a new barcode for the secondary particle
      Barcode::ParticleBarcode secBC = m_barcodeSvcQuick->newSecondary( primBC, processCode);
      if ( secBC == Barcode::fUndefinedBarcode) {
        if (m_ignoreUndefinedBarcodes)
          ATH_MSG_WARNING("Unable to generate new Secondary Particle Barcode. Continuing due to 'IgnoreUndefinedBarcodes'==True");
        else {
          ATH_MSG_ERROR("Unable to generate new Secondary Particle Barcode. Aborting");
          abort();
        }
      }
      HepMC::GenParticle *p = truth.secondaryParticle(i, secBC, setPersistent);

      // add particle to vertex
      vtx->add_particle_out( p);
    }

    // finally add the vertex to the current GenEvent
    m_mcEvent->add_vertex( vtx);
  }

  //
  // particles will not be entered into the TruthEvent collection
  //   -> assign shared barcode for all child particles
  //
  else {
    ATH_MSG_VERBOSE("TruthIncident did not pass cuts. Will not record this one.");

    // generate one new barcode for all secondary particles
    Barcode::ParticleBarcode childBC = m_barcodeSvcQuick->sharedChildBarcode( primBC, processCode);
    // propagate this barcode into the TruthIncident only if
    // it is a proper barcode, ie !=fUndefinedBarcode
    if (childBC != Barcode::fUndefinedBarcode) {
      truth.setAllSecondaryBarcodes( childBC);
    }

    // update all _extra_ barcodes of secondary particles with parent info
    // MB: sofar extra barcode only contains parent info, so can be the same for each secondary
    if (m_storeExtraBCs)
      truth.setAllSecondaryExtraBarcodes( primExtraBC );
  }

  return;
}

