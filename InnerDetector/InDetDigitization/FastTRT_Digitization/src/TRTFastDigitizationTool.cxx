/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRTFastDigitizationTool.cxx
//
//   Implementation file for class TRTFastDigitizationTool
//
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "FastTRT_Digitization/TRTFastDigitizationTool.h"

#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "InDetSimEvent/TRTHitIdHelper.h"
#include "InDetSimData/InDetSimData.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include "GaudiKernel/SystemOfUnits.h"

// Det descr includes:
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"

// Other includes
#include "PileUpTools/PileUpMergeSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"

// InDet stuff
#include "InDetPrepRawData/TRT_DriftCircle.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

// Conditions data
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "CxxUtils/make_unique.h"


static constexpr unsigned int crazyParticleBarcode( std::numeric_limits< int32_t >::max() );
// Barcodes at the HepMC level are int

TRTFastDigitizationTool::TRTFastDigitizationTool( const std::string &type,
                                                  const std::string &name,
                                                  const IInterface *parent )
  : PileUpToolBase( type, name, parent ),
    m_trtDriftFunctionTool( "TRT_DriftFunctionTool/FatrasTrtDriftFunctionTool" ),
    m_useTrtElectronPidTool( true ),
    m_trtElectronPidTool( "InDet::TRT_ElectronPidToolRun2/InDetTRT_ElectronPidTool" ),
    m_trtStrawStatusSummarySvc( "InDetTRTStrawStatusSummarySvc", name ),
    m_mergeSvc( "PileUpMergeSvc", name ),
    m_atRndmGenSvc ( "AtRndmGenSvc", name ),
    m_randomEngine( nullptr ),
    m_randomEngineName( "FatrasRnd" ),
    m_trtHitCollectionKey( "TRTUncompressedHits" ),
    m_trtDriftCircleContainer( "TRT_DriftCircles" ),
    m_trtPrdTruth( "PRD_MultiTruthTRT" ),
    m_thpctrt( nullptr ),
    m_trt_manager( nullptr ),
    m_trt_id( nullptr ),
    m_HardScatterSplittingMode( 0 ),
    m_HardScatterSplittingSkipper( false ),
    m_vetoThisBarcode( crazyParticleBarcode )
{
  declareInterface< ITRTFastDigitizationTool >( this );
  declareProperty( "TRT_DriftFunctionTool",       m_trtDriftFunctionTool );
  declareProperty( "TRT_ElectronPidTool",         m_trtElectronPidTool );
  declareProperty( "TRT_StrawStatusSummarySvc",   m_trtStrawStatusSummarySvc );
  declareProperty( "MergeSvc",                    m_mergeSvc );
  declareProperty( "RndmSvc",                     m_atRndmGenSvc );
  declareProperty( "RandomStreamName",            m_randomEngineName );
  declareProperty( "trtHitCollectionName",        m_trtHitCollectionKey );
  declareProperty( "trtDriftCircleContainer",     m_trtDriftCircleContainer );
  declareProperty( "trtPrdMultiTruthCollection",  m_trtPrdTruth );
  declareProperty( "HardScatterSplittingMode",    m_HardScatterSplittingMode, "Control pileup & signal splitting" );
  declareProperty( "ParticleBarcodeVeto",         m_vetoThisBarcode, "Barcode of particle to ignore");
}


StatusCode TRTFastDigitizationTool::initialize()
{
  ATH_MSG_DEBUG( "TRTFastDigitizationTool::initialize()" );

  // Get Random Service
  CHECK( m_atRndmGenSvc.retrieve() );
  m_randomEngine = m_atRndmGenSvc->GetEngine( m_randomEngineName );
  if ( !m_randomEngine ) {
    ATH_MSG_ERROR( "Could not get random engine '" << m_randomEngineName << "'" );
    return StatusCode::FAILURE;
  }

  // Get the TRT Detector Manager
  CHECK( detStore()->retrieve( m_trt_manager, "TRT" ) );
  ATH_MSG_DEBUG( "Retrieved TRT_DetectorManager with version "  << m_trt_manager->getVersion().majorNum() );

  CHECK( detStore()->retrieve( m_trt_id, "TRT_ID" ) );

  // PileUp Merge Service
  CHECK( m_mergeSvc.retrieve() );

  // Argon / Xenon
  CHECK( m_trtStrawStatusSummarySvc.retrieve() );
  ATH_MSG_DEBUG( "Retrieved TRT_StrawStatusSummarySvc " << m_trtStrawStatusSummarySvc );

  // Check data object name
  if ( m_trtHitCollectionKey == "" ) {
    ATH_MSG_FATAL( "Property trtHitCollectionName not set!" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Input hits: " << m_trtHitCollectionKey );
  }

  CHECK( m_trtDriftFunctionTool.retrieve() );

  if ( m_useTrtElectronPidTool ) {
    CHECK( m_trtElectronPidTool.retrieve() );
  } 

  return StatusCode::SUCCESS;
}


StatusCode TRTFastDigitizationTool::prepareEvent( unsigned int )
{
  m_trtHitCollList.clear();
  m_thpctrt = new TimedHitCollection< TRTUncompressedHit >();
  m_HardScatterSplittingSkipper = false;
  
  return StatusCode::SUCCESS;
}


StatusCode TRTFastDigitizationTool::processBunchXing( int /*bunchXing*/,
                                                      SubEventIterator bSubEvents,
                                                      SubEventIterator eSubEvents ) {

  // decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
  if ( m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; return StatusCode::SUCCESS; }
  if ( m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { return StatusCode::SUCCESS; }
  if ( m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }

  SubEventIterator iEvt( bSubEvents );
  while ( iEvt != eSubEvents ) {
    StoreGateSvc& seStore( *iEvt->ptr()->evtStore() );
    PileUpTimeEventIndex thisEventIndex( PileUpTimeEventIndex( static_cast< int >( iEvt->time() ),iEvt->index() ) );
    const TRTUncompressedHitCollection* seHitColl( nullptr );
    CHECK( seStore.retrieve( seHitColl, m_trtHitCollectionKey ) );
    // copy Hit Collection
    TRTUncompressedHitCollection* trtHitColl( new TRTUncompressedHitCollection( "TRTUncompressedHits") );
    // read hits from this collection
    for ( TRTUncompressedHitCollection::const_iterator itr = seHitColl->begin(); itr != seHitColl->end(); ++itr ) {
      const TRTUncompressedHit trthit( *itr );
      trtHitColl->Insert( trthit );
    }
    m_thpctrt->insert( thisEventIndex, trtHitColl );
    // store these for deletion at the end of mergeEvent
    m_trtHitCollList.push_back( trtHitColl );

    ++iEvt;
  }

  return StatusCode::SUCCESS;
}


StatusCode TRTFastDigitizationTool::produceDriftCircles() {

  // constants
  const double trtTailFraction( 4.7e-4 );                   // part of the fraction in Tails ( default 2% )
  const double trtSigmaDriftRadiusTail( 1.15470053838 );    // sigma of one TRT straw in R (Tail) [mm], 4./sqrt(12.)

  // { correction coefficient for TRT efficiency,
  //   correction coefficient for TRT sigma, adjust total/precise hits ratio
  //   narrow Gaus1 : wide Gaus2, Gaus1 sigma, Gaus2 sigma }
  const double cFit[ 8 ][ 5 ] = { { 0.884, 0.997, 0.900, 1.030, 2.400 },  // Barrel A-side Xenon
                                  { 0.921, 1.065, 0.900, 0.970, 2.400 },  // EndCap A-side Xenon
                                  { 0.877, 0.997, 0.900, 1.030, 2.400 },  // Barrel C-side Xenon
                                  { 0.941, 1.065, 0.900, 0.970, 2.400 },  // EndCap C-side Xenon
                                  { 0.982, 1.020, 0.800, 0.995, 2.400 },  // Barrel A-side Argon
                                  { 0.999, 1.040, 0.800, 0.935, 2.400 },  // EndCap A-side Argon
                                  { 0.986, 1.020, 0.800, 0.995, 2.400 },  // Barrel C-side Argon
                                  { 1.018, 1.040, 0.800, 0.935, 2.400 }   // EndCap C-side Argon
                                };

  float mu = 30.0;
  const xAOD::EventInfo* m_eventInfo = 0;
  if( StatusCode::SUCCESS == evtStore()->retrieve(m_eventInfo) ){
    mu = (float) m_eventInfo->averageInteractionsPerCrossing();
  }
  else{
    ATH_MSG_INFO("Cannot retrieve event info, use default value");
  }

  m_driftCircleMap.clear();

  TimedHitCollection< TRTUncompressedHit >::const_iterator itr1, itr2;
  while ( m_thpctrt->nextDetectorElement( itr1, itr2 ) ) {

    for ( ; itr1 != itr2; ++itr1 ) {

      const TimedHitPtr< TRTUncompressedHit > &hit = *itr1;

      // Get hitID
      int hitID = hit->GetHitID();

      if ( hitID & 0xc0000000 ) {
        ATH_MSG_ERROR( "Hit ID not Valid (" << MSG::hex << hitID << ")" << MSG::dec );
        continue;
      }

      // Convert hitID to Identifier
      IdentifierHash hash;
      Identifier layer_id;
      bool status;
      Identifier straw_id = getIdentifier( hitID, hash, layer_id, status );
      if ( !status ) {
        ATH_MSG_ERROR( "Ignoring simhits with suspicious identifier (1)" );
        continue;
      }

      const InDetDD::TRT_BaseElement *trtBaseElement = m_trt_manager->getElement( hash );
      Identifier hit_id = trtBaseElement->identify();
      
      int BEC = m_trt_id->barrel_ec( hit_id );
      bool isArgon = isArgonStraw( straw_id );
      int idx = ( BEC > 0 ? BEC : 2 - BEC ) + 4 * isArgon - 1;

      // Get 'track-to-wire' distance
      double driftRadiusLoc = getDriftRadiusFromXYZ( hit );

      double efficiency = strawEfficiency( driftRadiusLoc, abs( BEC ) );
      efficiency *= cFit[ idx ][ 0 ];

      // Decide wether to throw away this cluster or not
      if ( CLHEP::RandFlat::shoot( m_randomEngine ) < ( 1. - efficiency ) ) continue;

      // Decide core/tail fraction
      bool isTail = ( CLHEP::RandFlat::shoot( m_randomEngine ) < trtTailFraction );

      double sigmaTrt = trtSigmaDriftRadiusTail;
      if ( !isTail ) {
        double driftTime = m_trtDriftFunctionTool->approxDriftTime( fabs( driftRadiusLoc ) );
        sigmaTrt = m_trtDriftFunctionTool->errorOfDriftRadius( driftTime, hit_id, mu );
      }

      // driftRadiusLoc smearing procedure 
      double dR = 0;
      int ii = 0;
      do {
        double tailSmearing = CLHEP::RandFlat::shoot( m_randomEngine );
        dR = CLHEP::RandGauss::shoot( m_randomEngine, 0., ( tailSmearing < cFit[ idx ][ 2 ] ? cFit[ idx ][ 3 ] : cFit[ idx ][ 4 ] ) ) * sigmaTrt;
        ++ii;
        if ( ii > 50 ) {  // should not appear in simulation
          dR = 2. - driftRadiusLoc;
          break;
        }
      }
      while ( driftRadiusLoc + dR > 2. || driftRadiusLoc + dR < 0. );
      double smearedRadius = driftRadiusLoc + dR;

      Amg::Vector2D hitLocalPosition( smearedRadius, 0. );
      std::vector< Identifier > rdoList = { straw_id };

      Amg::MatrixX *hitErrorMatrix = new Amg::MatrixX( 1, 1 );
      ( *hitErrorMatrix )( Trk::driftRadius, Trk::driftRadius ) = sigmaTrt * sigmaTrt * cFit[ idx ][ 1 ] * cFit[ idx ][ 1 ];

      // the TRT word simulate only TR information for the moment
      // consult TRTElectronicsProcessing::EncodeDigit() in TRT_Digitization/src/TRTElectronicsProcessing.cxx
      unsigned int word = 0x00007c00;  // set to a standard low threshold hit: word = 0; for ( unsigned int j = 10; j < 15; ++j ) word += 1 << ( 25 - j - j / 8 );

      // set High Threshold bit
      HepGeom::Point3D< double > hitGlobalPosition = getGlobalPosition( hit );
      int particleEncoding = hit->GetParticleEncoding();
      float kineticEnergy = hit->GetKineticEnergy();

      if ( m_useTrtElectronPidTool ) {

        double position = ( BEC == 1 ? hitGlobalPosition.z() : hitGlobalPosition.perp() );
        // double probability = getProbHT( particleEncoding, kineticEnergy, straw_id, driftRadiusLoc, position );
        double probability = getProbHT( particleEncoding, kineticEnergy, straw_id, smearedRadius, position );
        if ( CLHEP::RandFlat::shoot( m_randomEngine ) < probability ) word |= 0x04020100;
      }
      else {

        double eta = hitGlobalPosition.pseudoRapidity();
        // double mass = particleMass( particle );
        // double p = sqrt( kineticEnergy * kineticEnergy + 2. * kineticEnergy * mass );
        float p = kineticEnergy;  // like in TRT_Digitization ( previously we also use zero mass due to bug in particleMass routine )

        if ( abs( particleEncoding ) == 11 && p > 5000. ) {  // electron
          double probability = ( p < 20000. ? HTProbabilityElectron_low_pt( eta ) : HTProbabilityElectron_high_pt( eta ) );
          if ( CLHEP::RandFlat::shoot( m_randomEngine ) < probability ) word |= 0x04020100;
        }
        else if ( abs( particleEncoding ) == 13 || abs( particleEncoding ) > 100 ) {  // muon or other particle
          double probability = ( p < 20000. ? HTProbabilityMuon_5_20( eta ) : HTProbabilityMuon_60( eta ) );
          if ( CLHEP::RandFlat::shoot( m_randomEngine ) < probability ) word |= 0x04020100;
        }

      }

      InDet::TRT_DriftCircle *trtDriftCircle = new InDet::TRT_DriftCircle( straw_id, hitLocalPosition, rdoList, hitErrorMatrix, trtBaseElement, word );
      if ( !trtDriftCircle ) continue;

      m_driftCircleMap.insert( std::multimap< Identifier, InDet::TRT_DriftCircle * >::value_type( straw_id, trtDriftCircle ) );

      if ( hit->particleLink().isValid() ) {
        const int barcode( hit->particleLink().barcode() );
        if ( barcode !=0 && barcode != m_vetoThisBarcode ) {
          m_trtPrdTruth->insert( std::make_pair( trtDriftCircle->identify(), hit->particleLink() ) );
          ATH_MSG_DEBUG( "Truth map filled with cluster " << trtDriftCircle << " and link = " << hit->particleLink() );
        }
      }
      else {
        ATH_MSG_DEBUG( "Particle link NOT valid!! Truth map NOT filled with cluster " << trtDriftCircle << " and link = " << hit->particleLink() );
      }

    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TRTFastDigitizationTool::processAllSubEvents() {

  ATH_MSG_DEBUG( "TRTFastDigitizationTool::processAllSubEvents()" );

  CHECK( this->createOutputContainers() );

  typedef PileUpMergeSvc::TimedList< TRTUncompressedHitCollection >::type HitCollectionTimedList;

  HitCollectionTimedList hitCollectionTimedList;
  unsigned int numberOfSimHits = 0;
  if ( m_mergeSvc->retrieveSubEvtsData( m_trtHitCollectionKey, hitCollectionTimedList, numberOfSimHits ).isFailure() && hitCollectionTimedList.empty() ) {
    ATH_MSG_ERROR( "Could not fill HitCollectionTimedList" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG( hitCollectionTimedList.size() << " TRTUncompressedHitCollections with key " << m_trtHitCollectionKey << " found" );
  }

  m_HardScatterSplittingSkipper = false;
  TimedHitCollection< TRTUncompressedHit > timedHitCollection( numberOfSimHits );
  for ( HitCollectionTimedList::iterator itr = hitCollectionTimedList.begin(); itr != hitCollectionTimedList.end(); ++itr ) {
    // decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
    if ( m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; continue; }
    if ( m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { continue; }
    if ( m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }
    timedHitCollection.insert( itr->first, static_cast< const TRTUncompressedHitCollection * >( itr->second ) );
  }
  m_thpctrt = &timedHitCollection;

  // Process the Hits straw by straw: get the iterator pairs for given straw
  CHECK( this->produceDriftCircles() );

  CHECK( this->createAndStoreRIOs() );
  ATH_MSG_DEBUG ( "createAndStoreRIOs() succeeded" );

  return StatusCode::SUCCESS;
}


StatusCode TRTFastDigitizationTool::createOutputContainers() {

  ATH_MSG_DEBUG( "TRTFastDigitizationTool::createOutputContainers()" );

  // Create OUTPUT TRT_DriftCircleContainer and register it in StoreGate
  if ( !m_trtDriftCircleContainer.isValid() ) {
    m_trtDriftCircleContainer = CxxUtils::make_unique< InDet::TRT_DriftCircleContainer >( m_trt_id->straw_layer_hash_max() );
    if ( !m_trtDriftCircleContainer.isValid() ) {
      ATH_MSG_FATAL( "Could not create TRT_DriftCircleContainer" );
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG( "InDet::TRT_DriftCircleContainer " << m_trtDriftCircleContainer.name() << " registered in StoreGate" );

  // Create OUTPUT PRD_MultiTruthCollection for TRT measurements
  if ( !m_trtPrdTruth.isValid() ) {
    m_trtPrdTruth = CxxUtils::make_unique< PRD_MultiTruthCollection >();
    if ( !m_trtPrdTruth.isValid() ) {
      ATH_MSG_FATAL( "Could not record collection " << m_trtPrdTruth.name() );
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG( "PRD_MultiTruthCollection " << m_trtPrdTruth.name() << " registered in StoreGate" );

  return StatusCode::SUCCESS;
}


StatusCode TRTFastDigitizationTool::mergeEvent() {

  ATH_MSG_DEBUG( "TRTFastDigitizationTool::mergeEvent()" );

  CHECK( this->createOutputContainers() );

  // Process the Hits straw by straw: get the iterator pairs for given straw
  if ( m_thpctrt != 0 ) {
    CHECK( this->produceDriftCircles() );
  }

  // Clean up temporary containers
  delete m_thpctrt;
  std::list< TRTUncompressedHitCollection * >::iterator trtHitColl( m_trtHitCollList.begin() );
  std::list< TRTUncompressedHitCollection * >::iterator trtHitCollEnd( m_trtHitCollList.end() );
  while( trtHitColl != trtHitCollEnd ) {
    delete ( *trtHitColl );
    ++trtHitColl;
  }
  m_trtHitCollList.clear();

  CHECK( this->createAndStoreRIOs() );
  ATH_MSG_DEBUG ( "createAndStoreRIOs() succeeded" );

  return StatusCode::SUCCESS;
}


StatusCode TRTFastDigitizationTool::createAndStoreRIOs()
{
  typedef std::multimap< Identifier, InDet::TRT_DriftCircle * >::iterator DriftCircleMapItr;
  typedef std::multimap< IdentifierHash, InDet::TRT_DriftCircle * >::iterator HashMapItr;

  std::multimap< IdentifierHash, InDet::TRT_DriftCircle * > idHashMap;

  for ( DriftCircleMapItr itr = m_driftCircleMap.begin() ; itr != m_driftCircleMap.end(); itr = m_driftCircleMap.upper_bound( itr->first ) ) {

    std::pair< DriftCircleMapItr, DriftCircleMapItr > hitsInOneStraw = m_driftCircleMap.equal_range( itr->first );
    // unsigned int numberOfHitsInOneStraw = m_driftCircleMap.count( itr->first );
    InDet::TRT_DriftCircle *trtDriftCircle = ( hitsInOneStraw.first )->second;
    IdentifierHash hash = trtDriftCircle->detectorElement()->identifyHash();
    idHashMap.insert( std::multimap<IdentifierHash, InDet::TRT_DriftCircle * >::value_type( hash, trtDriftCircle ) );
    // delete all driftCircles in TRT straw excert the first one, see ATLPHYSVAL-395
    for ( DriftCircleMapItr itr2 = ++( hitsInOneStraw.first ); itr2 != hitsInOneStraw.second; ++itr2 ) {
      InDet::TRT_DriftCircle *trtDriftCircle = itr2->second;
      delete trtDriftCircle;
    } 
  }

  for ( HashMapItr itr = idHashMap.begin(); itr != idHashMap.end(); itr = idHashMap.upper_bound( itr->first ) ) {

    std::pair< HashMapItr, HashMapItr > itrPair = idHashMap.equal_range( itr->first );

    const InDetDD::TRT_BaseElement *trtBaseElement = ( itrPair.first )->second->detectorElement();
    IdentifierHash hash = trtBaseElement->identifyHash();

    InDet::TRT_DriftCircleCollection *trtDriftCircleCollection = new InDet::TRT_DriftCircleCollection( hash );
    trtDriftCircleCollection->setIdentifier( trtBaseElement->identify() );

    for ( HashMapItr itr2 = itrPair.first; itr2 != itrPair.second; ++itr2 ) {
      InDet::TRT_DriftCircle *trtDriftCircle = itr2->second;
      trtDriftCircle->setHashAndIndex( trtDriftCircleCollection->identifyHash(), trtDriftCircleCollection->size() );
      trtDriftCircleCollection->push_back( trtDriftCircle );
    }

    if ( m_trtDriftCircleContainer->addCollection( trtDriftCircleCollection, hash ).isFailure() ) {
      ATH_MSG_WARNING( "Could not add collection to Identifyable container" );
    }
  }

  idHashMap.clear();
  m_driftCircleMap.clear();

  return StatusCode::SUCCESS;
}


double TRTFastDigitizationTool::getDriftRadiusFromXYZ( const TimedHitPtr< TRTUncompressedHit > &hit )
{
  HepGeom::Vector3D< double > vecEnter( hit->GetPreStepX(), hit->GetPreStepY(), hit->GetPreStepZ() );
  HepGeom::Vector3D< double > vecExit( hit->GetPostStepX(), hit->GetPostStepY(), hit->GetPostStepZ() );

  HepGeom::Vector3D< double > vecDir = vecExit - vecEnter;
  static const HepGeom::Vector3D< double > vecStraw( 0., 0., 1. );

  vecDir = vecDir.unit();

  double driftRadius = 0.;
  if ( fabs( vecDir.x() ) < 1.0e-6 && fabs( vecDir.y() ) < 1.0e-6 ) {
    driftRadius = vecEnter.perp();
  }
  else {
    double a = vecEnter.dot( vecStraw );
    double b = vecEnter.dot( vecDir );
    double c = vecDir.dot( vecStraw );

    double paramStraw = ( a - b*c ) / ( 1. - c*c );
    double paramTrack = -( b - a*c ) / ( 1. - c*c );

    HepGeom::Vector3D<double> vecClosestAppr = vecEnter + paramTrack * vecDir - paramStraw * vecStraw;
    driftRadius = vecClosestAppr.mag();
  }

  return driftRadius;
}


Identifier TRTFastDigitizationTool::getIdentifier( int hitID, IdentifierHash &hash, Identifier &layer_id, bool &status ) const
{
  status = true;

  Identifier straw_id;

  const int mask( 0x0000001F );
  const int word_shift( 5 );

  if ( hitID & 0x00200000 ) {  // endcap
    int strawID  = hitID & mask;
    int planeID  = ( hitID >> word_shift ) & mask;
    int sectorID = ( hitID >> 2 * word_shift ) & mask;
    int wheelID  = ( hitID >> 3 * word_shift ) & mask;
    int trtID    = ( hitID >> 4 * word_shift );

    // change trtID (which is 2/3 for endcaps) to use 0/1 in getEndcapElement
    trtID = ( trtID == 3 ? 0 : 1 );

    const InDetDD::TRT_EndcapElement *endcapElement = m_trt_manager->getEndcapElement( trtID, wheelID, planeID, sectorID );
    if ( endcapElement ) {
      hash = endcapElement->identifyHash();
      layer_id = endcapElement->identify();
      straw_id = m_trt_id->straw_id( layer_id, strawID );
    }
    else {
      ATH_MSG_ERROR( "Could not find detector element for endcap identifier with (ipos,iwheel,isector,iplane,istraw) = ("
                     << trtID << ", " << wheelID << ", " << sectorID << ", " << planeID << ", " << strawID << ")" << endreq
                     << "If this happens very rarely, don't be alarmed (it is a Geant4 'feature')" << endreq
                     << "If it happens a lot, you probably have misconfigured geometry in the sim. job." );
      status = false;
    }

  }
  else {  // barrel
    int strawID  = hitID & mask;
    int layerID  = ( hitID >> word_shift ) & mask;
    int moduleID = ( hitID >> 2 * word_shift ) & mask;
    int ringID   = ( hitID >> 3 * word_shift ) & mask;
    int trtID    = ( hitID >> 4 * word_shift );

    const InDetDD::TRT_BarrelElement *barrelElement = m_trt_manager->getBarrelElement( trtID, ringID, moduleID, layerID );
    if ( barrelElement ) {
      hash = barrelElement->identifyHash();
      layer_id = barrelElement->identify();
      straw_id = m_trt_id->straw_id( layer_id, strawID );
    } else {
      ATH_MSG_ERROR( "Could not find detector element for barrel identifier with (ipos,iring,imod,ilayer,istraw) = ("
                     << trtID << ", " << ringID << ", " << moduleID << ", " << layerID << ", " << strawID << ")" );
      status = false;
    }

  }

  return straw_id;
}


HepGeom::Point3D< double > TRTFastDigitizationTool::getGlobalPosition( const TimedHitPtr<TRTUncompressedHit> &hit )
{
  int hitID = hit->GetHitID();
  const HepGeom::Point3D< double > hitPreStep( hit->GetPreStepX(), hit->GetPreStepY(), hit->GetPreStepZ() );

  const int mask( 0x0000001F );
  const int word_shift( 5 );

  if ( hitID & 0x00200000 ) {  // endcap
    int strawID  = hitID & mask;
    int planeID  = ( hitID >> word_shift ) & mask;
    int sectorID = ( hitID >> 2 * word_shift ) & mask;
    int wheelID  = ( hitID >> 3 * word_shift ) & mask;
    int trtID    = ( hitID >> 4 * word_shift );

    // change trtID (which is 2/3 for endcaps) to use 0/1 in getEndcapElement
    trtID = ( trtID == 3 ? 0 : 1 );

    const InDetDD::TRT_EndcapElement *endcapElement = m_trt_manager->getEndcapElement( trtID, wheelID, planeID, sectorID );
    if ( endcapElement ) {
      return endcapElement->getAbsoluteTransform( strawID ) * hitPreStep;
    }

  }
  else {  // barrel
    int strawID  = hitID & mask;
    int layerID  = ( hitID >> word_shift ) & mask;
    int moduleID = ( hitID >> 2 * word_shift ) & mask;
    int ringID   = ( hitID >> 3 * word_shift ) & mask;
    int trtID    = ( hitID >> 4 * word_shift );

    const InDetDD::TRT_BarrelElement *barrelElement = m_trt_manager->getBarrelElement( trtID, ringID, moduleID, layerID );
    if ( barrelElement ) {
      return barrelElement->getAbsoluteTransform( strawID ) * hitPreStep;
    }

  }

  ATH_MSG_WARNING( "Could not find global coordinate of a straw - drifttime calculation will be inaccurate" );
  return HepGeom::Point3D< double >( 0., 0., 0. );
}


bool TRTFastDigitizationTool::isArgonStraw( const Identifier &straw_id ) const
{
  // TRTCond::StrawStatus::Good == Xenon
  // return ( m_trtStrawStatusSummarySvc->getStatusHT( straw_id ) != TRTCond::StrawStatus::Good ? true : false );
  return ( gasType( straw_id ) == 1 ? true : false );
} 


int TRTFastDigitizationTool::gasType( const Identifier &straw_id ) const
{
  // getStatusHT returns enum EStatus { Undefined, Dead, Good, Xenon, Argon, Krypton } // from 20.7.1
  // ​see InnerDetector/​InDetConditions/​TRT_ConditionsData/​TRT_ConditionsData/​StrawStatus.h
  // TRT representation of gasType = Xenon: 0, Argon: 1, Krypton: 2

  int status = m_trtStrawStatusSummarySvc->getStatusHT( straw_id );

  if ( status == 2 || status == 3 )
    return 0;
  else if ( status == 1 || status == 4 )
    return 1;
  else if ( status == 5 )
    return 2;
  else {
    ATH_MSG_WARNING( "TRTFastDigitizationTool::gasType() getStatusHT = " << status << ", must be in [1..5] range" );
    return -1;
  }
  
}


double TRTFastDigitizationTool::getProbHT( int particleEncoding, float kineticEnergy, const Identifier &straw_id, double rTrkWire, double hitGlobalPosition ) const {

  Trk::ParticleHypothesis hypothesis = Trk::pion;
  
  switch( abs( particleEncoding ) ) {

    case 11:
    hypothesis = Trk::electron;
    break;

    case 13:
    hypothesis = Trk::muon;
    break;
    
    case 321:
    hypothesis = Trk::kaon;
    break;
    
    case 211:
    default:
    hypothesis = Trk::pion;
    break;

  } // end of switch

  static Trk::ParticleMasses particleMasses;
  float pTrk = sqrt( kineticEnergy * kineticEnergy + 2. * kineticEnergy * particleMasses.mass[ hypothesis ] );
  if ( pTrk < 250. || pTrk > 7000000. ) return 0.;

  int layerOrWheel = m_trt_id->layer_or_wheel( straw_id );
  int strawLayer = m_trt_id->straw_layer( straw_id );

  // trtPart = Barrel: 0, EndcapA: 1, EndcapB: 2
  int trtPart = 0;
  if ( abs( m_trt_id->barrel_ec( straw_id ) ) == 2 ) trtPart = ( ( layerOrWheel < 6 ) ? 1 : 2 );

  // strawLayer = Barrel: 0-72, Endcap A-side: 0-95 (16 layers in 6 modules), EndcapB: 0-63 (8 layers in 8 modules)
  if ( trtPart == 0 ) {       // Barrel
    if ( layerOrWheel ) strawLayer += 19 + ( layerOrWheel == 1 ? 0 : 24 );
  }
  else if ( trtPart == 1 ) {  // EndcapA
    strawLayer += 16 * layerOrWheel;
  }
  else {                      // EndcapB
    strawLayer += 8 * ( layerOrWheel - 6 );
  }

  const int strawLayerMax[] = { 72, 95, 63 };
  if ( strawLayer > strawLayerMax[ trtPart ] || strawLayer < 0 ) {
    ATH_MSG_WARNING( "strawLayer was outside allowed range: trtPart = " << trtPart << ", strawLayer = " << strawLayer );
    return 0.;
  }

  const double hitGlobalPositionMin[] = {   0.,  630.,  630. };
  const double hitGlobalPositionMax[] = { 720., 1030., 1030. };

  if ( hitGlobalPosition < hitGlobalPositionMin[ trtPart ] ) {
    ATH_MSG_WARNING( "hitGlobalPosition was below allowed range (will be adjusted): trtPart = " << trtPart << ", hitGlobalPosition = " << hitGlobalPosition );
    hitGlobalPosition = hitGlobalPositionMin[ trtPart ] + 0.001;
  }
  if ( hitGlobalPosition > hitGlobalPositionMax[ trtPart ] ) {
    ATH_MSG_WARNING( "hitGlobalPosition was above allowed range (will be adjusted): trtPart = " << trtPart << ", hitGlobalPosition = " << hitGlobalPosition );
    hitGlobalPosition = hitGlobalPositionMax[ trtPart ] - 0.001;
  }

  if ( rTrkWire > 2.2 ) rTrkWire = 2.175;

  double Occupancy = 0.1;

  double probHT = m_trtElectronPidTool->probHTRun2( pTrk, hypothesis, trtPart, gasType( straw_id ), strawLayer, hitGlobalPosition, rTrkWire, Occupancy );
  if ( probHT == 0.5 || probHT == 1. ) probHT = 0.;
  probHT /= correctionHT( pTrk, hypothesis );

  return probHT;
}


double TRTFastDigitizationTool::HTProbabilityElectron_high_pt( double eta )
{
  static const std::vector< double > bins = { 0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 1.05, 1.4, 1.5, 1.6, 1.82 };
  static const std::vector< double > probability = { 0.210,             // [ 0.,   0.05 ]
                                                     0.218,             // [ 0.05, 0.15 ]
                                                     0.226,             // [ 0.15, 0.25 ]
                                                     0.234,             // [ 0.25, 0.35 ]
                                                     0.242,             // [ 0.35, 0.45 ]
                                                     0.250,             // [ 0.45, 0.55 ]
                                                     0.258,             // [ 0.55, 0.65 ]
                                                     0.266,             // [ 0.65, 0.75 ]
                                                     0.274,             // [ 0.75, 0.85 ]
                                                     0.280,             // [ 0.85, 1.05 ]
                                                     0.265,             // [ 1.05, 1.40 ]
                                                     0.275,             // [ 1.40, 1.50 ]
                                                     0.295,             // [ 1.50, 1.60 ]
                                                     0.330,             // [ 1.60, 1.82 ]
                                                     0.365              // > 1.82
                                                   };

  return probability[ std::distance( bins.begin(), std::lower_bound( bins.begin(), bins.end(), fabs( eta ) ) ) ];
}


double TRTFastDigitizationTool::HTProbabilityElectron_low_pt( double eta )
{
  static const std::vector< double > bins = { 0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 1.05, 1.4, 1.5, 1.6, 1.82 };
  static const std::vector< double > probability = { 0.210,             // [ 0.,   0.05 ]
                                                     0.218,             // [ 0.05, 0.15 ]
                                                     0.226,             // [ 0.15, 0.25 ]
                                                     0.234,             // [ 0.25, 0.35 ]
                                                     0.242,             // [ 0.35, 0.45 ]
                                                     0.250*0.88,        // [ 0.45, 0.55 ]
                                                     0.258*0.88,        // [ 0.55, 0.65 ]
                                                     0.266*0.88,        // [ 0.65, 0.75 ]
                                                     0.274*0.88,        // [ 0.75, 0.85 ]
                                                     0.280*0.88,        // [ 0.85, 1.05 ]
                                                     0.265*0.88,        // [ 1.05, 1.40 ]
                                                     0.275*0.88,        // [ 1.40, 1.50 ]
                                                     0.295*0.88,        // [ 1.50, 1.60 ]
                                                     0.330*0.88,        // [ 1.60, 1.82 ]
                                                     0.365              // > 1.82
                                                   };

  return probability[ std::distance( bins.begin(), std::lower_bound( bins.begin(), bins.end(), fabs( eta ) ) ) ];
}


double TRTFastDigitizationTool::HTProbabilityMuon_5_20( double eta )
{
  static const std::vector< double > bins = { -2.05, -1.95, -1.85, -1.75, -1.65, -1.55, -1.45, -1.35, -1.25, -1.15, -1.05, -0.95, -0.85,
                                              -0.75, -0.65, -0.55, -0.45, -0.35, -0.25, -0.15, -0.05, 0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65,
                                              0.75, 0.85, 0.95, 1.05, 1.15, 1.25, 1.35, 1.45, 1.55, 1.65, 1.75, 1.85, 1.95
                                            };

  static const std::vector< double > probability = { 0.04466501,        // < -2.05
                                                     0.05624099,        // [ -2.05, -1.95 ]
                                                     0.05715916,        // [ -1.95, -1.85 ]
                                                     0.05758092,        // [ -1.85, -1.75 ]
                                                     0.05713670,        // [ -1.75, -1.65 ]
                                                     0.05320733,        // [ -1.65, -1.55 ]
                                                     0.05447418,        // [ -1.55, -1.45 ]
                                                     0.05460775,        // [ -1.45, -1.35 ]
                                                     0.05443998,        // [ -1.35, -1.25 ]
                                                     0.05671015,        // [ -1.25, -1.15 ]
                                                     0.06043842,        // [ -1.15, -1.05 ]
                                                     0.06098093,        // [ -1.05, -0.95 ]
                                                     0.06124813,        // [ -0.95, -0.85 ]
                                                     0.05757168,        // [ -0.85, -0.75 ]
                                                     0.05230566,        // [ -0.75, -0.65 ]
                                                     0.05136644,        // [ -0.65, -0.55 ]
                                                     0.05021782,        // [ -0.55, -0.45 ]
                                                     0.05046960,        // [ -0.45, -0.35 ]
                                                     0.04935652,        // [ -0.35, -0.25 ]
                                                     0.05074021,        // [ -0.25, -0.15 ]
                                                     0.04959613,        // [ -0.15, -0.05 ]
                                                     0.05090863,        // [ -0.05,  0.05 ]
                                                     0.05185448,        // [  0.05,  0.15 ]
                                                     0.05083610,        // [  0.15,  0.25 ]
                                                     0.05113032,        // [  0.25,  0.35 ]
                                                     0.05158703,        // [  0.35,  0.45 ]
                                                     0.05255587,        // [  0.45,  0.55 ]
                                                     0.05343067,        // [  0.55,  0.65 ]
                                                     0.05695859,        // [  0.65,  0.75 ]
                                                     0.06233243,        // [  0.75,  0.85 ]
                                                     0.06418306,        // [  0.85,  0.95 ]
                                                     0.06027916,        // [  0.95,  1.05 ]
                                                     0.05693816,        // [  1.05,  1.15 ]
                                                     0.05514142,        // [  1.15,  1.25 ]
                                                     0.05557067,        // [  1.25,  1.35 ]
                                                     0.05436613,        // [  1.35,  1.45 ]
                                                     0.05360627,        // [  1.45,  1.55 ]
                                                     0.05266918,        // [  1.55,  1.65 ]
                                                     0.05237728,        // [  1.65,  1.75 ]
                                                     0.05439599,        // [  1.75,  1.85 ]
                                                     0.05630533,        // [  1.85,  1.95 ]
                                                     0.06067052         // >  1.95
                                                   };

  return probability[ std::distance( bins.begin(), std::lower_bound( bins.begin(), bins.end(), eta ) ) ];
}


double TRTFastDigitizationTool::HTProbabilityMuon_60( double eta )
{
  static const std::vector< double > bins = { -2.05, -1.95, -1.85, -1.75, -1.65, -1.55, -1.45, -1.35, -1.25, -1.15, -1.05, -0.95, -0.85,
                                              -0.75, -0.65, -0.55, -0.45, -0.35, -0.25, -0.15, -0.05, 0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65,
                                              0.75, 0.85, 0.95, 1.05, 1.15, 1.25, 1.35, 1.45, 1.55, 1.65, 1.75, 1.85, 1.95
                                            };

  static const std::vector< double > probability = { 0.058,             // < -2.05
                                                     0.06316120,        // [ -2.05, -1.95 ]
                                                     0.06552401,        // [ -1.95, -1.85 ]
                                                     0.06462226,        // [ -1.85, -1.75 ]
                                                     0.06435722,        // [ -1.75, -1.65 ]
                                                     0.06404993,        // [ -1.65, -1.55 ]
                                                     0.06324595,        // [ -1.55, -1.45 ]
                                                     0.06318947,        // [ -1.45, -1.35 ]
                                                     0.06381197,        // [ -1.35, -1.25 ]
                                                     0.06366957,        // [ -1.25, -1.15 ]
                                                     0.06561866,        // [ -1.15, -1.05 ]
                                                     0.07307306,        // [ -1.05, -0.95 ]
                                                     0.07682944,        // [ -0.95, -0.85 ]
                                                     0.07430728,        // [ -0.85, -0.75 ]
                                                     0.06897150,        // [ -0.75, -0.65 ]
                                                     0.06393667,        // [ -0.65, -0.55 ]
                                                     0.06049334,        // [ -0.55, -0.45 ]
                                                     0.05774767,        // [ -0.45, -0.35 ]
                                                     0.05544898,        // [ -0.35, -0.25 ]
                                                     0.05456561,        // [ -0.25, -0.15 ]
                                                     0.05378204,        // [ -0.15, -0.05 ]
                                                     0.05196537,        // [ -0.05,  0.05 ]
                                                     0.05391259,        // [  0.05,  0.15 ]
                                                     0.05474811,        // [  0.15,  0.25 ]
                                                     0.05734638,        // [  0.25,  0.35 ]
                                                     0.05959219,        // [  0.35,  0.45 ]
                                                     0.06266565,        // [  0.45,  0.55 ]
                                                     0.06806432,        // [  0.55,  0.65 ]
                                                     0.07347304,        // [  0.65,  0.75 ]
                                                     0.07654586,        // [  0.75,  0.85 ]
                                                     0.07328693,        // [  0.85,  0.95 ]
                                                     0.06541597,        // [  0.95,  1.05 ]
                                                     0.06348016,        // [  1.05,  1.15 ]
                                                     0.06322222,        // [  1.15,  1.25 ]
                                                     0.06428555,        // [  1.25,  1.35 ]
                                                     0.06299531,        // [  1.35,  1.45 ]
                                                     0.06469499,        // [  1.45,  1.55 ]
                                                     0.06560785,        // [  1.55,  1.65 ]
                                                     0.06777871,        // [  1.65,  1.75 ]
                                                     0.06843851,        // [  1.75,  1.85 ]
                                                     0.06727150,        // [  1.85,  1.95 ]
                                                     0.05935051         // >  1.95
                                                   };

  return probability[ std::distance( bins.begin(), std::lower_bound( bins.begin(), bins.end(), eta ) ) ];
}


// parametrization of TRT efficiency as function of 'track-to-wire' distance
double TRTFastDigitizationTool::strawEfficiency( double driftRadius, int BEC )
{
  const double p[][ 5 ] = { { 0.478,    0.9386,  0.9325,  0.2509,   0.03232 }, // old parametrization
                            { 0.477001, 1.02865, 1.02910, 0.185082, 0. },      // Barrel
                            { 0.482528, 1.03601, 1.03693, 0.182581, 0. }       // EndCap
                          };
  
  const double &trtFitAmplitude = p[ BEC ][ 0 ];
  const double &trtFitMu = p[ BEC ][ 1 ];
  const double &trtFitR = p[ BEC ][ 2 ];
  const double &trtFitSigma = p[ BEC ][ 3 ];
  const double &trtFitConstant = p[ BEC ][ 4 ];

  double efficiency = trtFitAmplitude * ( erf( ( trtFitMu + trtFitR - driftRadius ) / ( std::sqrt( 2 ) * trtFitSigma ) )
                                                + erf( ( trtFitMu + trtFitR + driftRadius ) / ( std::sqrt( 2 ) * trtFitSigma ) )
                                                - erf( ( trtFitMu - trtFitR - driftRadius ) / ( std::sqrt( 2 ) * trtFitSigma ) )
                                                - erf( ( trtFitMu - trtFitR + driftRadius ) / ( std::sqrt( 2 ) * trtFitSigma ) )
                                            )
                      + trtFitConstant;

  return efficiency;
  
  /*
  static const std::vector< double > bins = { 0.05, 1.43, 1.48, 1.53, 1.58, 1.63, 1.68, 1.73, 1.78, 1.83, 1.88, 1.93, 1.99 };
  static const std::vector< double > efficiency = { 0.94,        // [ 0., 0.05 ]
                                                    0.96,        // [ 0.05, 1.43 ]
                                                    0.955,       // [ 1.43, 1.48 ]
                                                    0.95,        // [ 1.48, 1.53 ]
                                                    0.945,       // [ 1.53, 1.58 ]
                                                    0.94,        // [ 1.58, 1.63 ]
                                                    0.93,        // [ 1.63, 1.68 ]
                                                    0.92,        // [ 1.68, 1.73 ]
                                                    0.89,        // [ 1.73, 1.78 ]
                                                    0.86,        // [ 1.78, 1.83 ]
                                                    0.8,         // [ 1.83, 1.88 ]
                                                    0.74,        // [ 1.88, 1.93 ]
                                                    0.695,       // [ 1.93, 1.99 ]
                                                    0.65         // > 1.99
                                                  };

  return efficiency[ std::distance( bins.begin(), std::lower_bound( bins.begin(), bins.end(), driftRadius ) ) ];
  */ 
}


double TRTFastDigitizationTool::correctionHT( double momentum, Trk::ParticleHypothesis hypothesis )
{
  const double par[][ 6 ] = { { 5.96038,  0.797671,  1.28832, -2.02763, -2.24630, 21.6857  },  // pion
                              { 0.522755, 0.697029, -3.90787,  6.32952,  1.06347,  3.51847 }   // electron
                            };

  int j = ( hypothesis == Trk::electron ? 1 : 0 );
  double x0 = momentum * 1.e-3;
  double x1 = 1. / ( x0 + par[ j ][ 0 ] );
  double x2 = log( x0 ) * x1;
  double value = par[ j ][ 1 ] + par[ j ][ 2 ] * x1 + par[ j ][ 3 ] * x1 * x1 + par[ j ][ 4 ] * x2 + par[ j ][ 5 ] * x1 * x2;
  return ( momentum > 1500. ? value : 1. );
}


StatusCode TRTFastDigitizationTool::finalize()
{
  ATH_MSG_DEBUG( "TRTFastDigitizationTool::finalize()" );

  return StatusCode::SUCCESS;
}
