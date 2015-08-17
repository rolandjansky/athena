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
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

// Conditions data
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

// Math
#include <cmath>

static constexpr unsigned int crazyParticleBarcode(std::numeric_limits<int32_t>::max());
//Barcodes at the HepMC level are int

TRTFastDigitizationTool::TRTFastDigitizationTool(const std::string &type,
                                                 const std::string &name,
                                                 const IInterface *parent )
  : PileUpToolBase( type, name, parent ),
    m_trtDriftFunctionTool( "TRT_DriftFunctionTool/FatrasTrtDriftFunctionTool" ),
    m_mergeSvc( "PileUpMergeSvc", name ),
    m_atRndmGenSvc ( "AtRndmGenSvc", name ),
    m_randomEngine( nullptr ),
    m_randomEngineName("FatrasRnd"),
    //m_partPropSvc( "PartPropSvc", "PartPropSvc" ),
    //m_particleDataTable( nullptr ),
    m_trtHitCollectionKey( "TRTUncompressedHits" ),
    m_thpctrt( nullptr ),
    m_trtDriftCircleKey( "TRT_DriftCircles" ),
    m_trtDriftCircleContainer( nullptr ),
    m_prdMultiTruthKey( "PRD_MultiTruthTRT" ),
    m_prdMultiTruthCollection( nullptr ),
    m_trt_manager( nullptr ),
    m_trt_id( nullptr ),
    m_trtTRsimulation( true ),
    m_trtTailFraction( 0.02 ),
    m_trtSigmaDriftRadius( 145. * Gaudi::Units::micrometer ),
    m_trtSigmaDriftRadiusTail( 1154.70053838 * Gaudi::Units::micrometer ),
    m_trtFitAmplitude( 0.478 ),
    m_trtFitMu( 0.9386 ),
    m_trtFitR( 0.9325 ),
    m_trtFitSigma( 0.2509 ),
    m_trtFitConstant( 0.03232 ),
    m_trtTRprobParC1( 0.036 ),
    m_trtTRprobParC2( 0.175 ),
    m_trtTRprobParC3( 3.395 ),
    m_trtTRprobParC4( 0.258 ),
    m_HardScatterSplittingMode(0),
    m_HardScatterSplittingSkipper(false),
    m_vetoThisBarcode(crazyParticleBarcode)
{
  declareInterface< ITRTFastDigitizationTool >( this );
  declareProperty( "TRT_DriftFunctionTool",       m_trtDriftFunctionTool );
  declareProperty( "MergeSvc",                    m_mergeSvc );
  declareProperty( "RndmSvc",                     m_atRndmGenSvc );
  declareProperty( "RandomStreamName",            m_randomEngineName );
  declareProperty( "trtHitCollectionName",        m_trtHitCollectionKey );
  declareProperty( "trtDriftCircleContainerName", m_trtDriftCircleKey );
  declareProperty( "prdMultiTruthCollectionName", m_prdMultiTruthKey );
  declareProperty( "HardScatterSplittingMode",    m_HardScatterSplittingMode, "Control pileup & signal splitting" );
  declareProperty( "ParticleBarcodeVeto",         m_vetoThisBarcode, "Barcode of particle to ignore");
}

//_____________________________________________________________________________
StatusCode TRTFastDigitizationTool::initialize()
{
  ATH_MSG_DEBUG( "TRTFastDigitizationTool::initialize()" );

  if ( detStore()->retrieve( m_trt_id, "TRT_ID" ).isFailure() ) {
    ATH_MSG_ERROR( "Could not get TRT ID helper" );
    return StatusCode::FAILURE;
  }

  // Get the TRT Detector Manager
  if ( detStore()->retrieve( m_trt_manager, "TRT" ).isFailure() ) {
    ATH_MSG_ERROR( "Can't get TRT_DetectorManager" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Retrieved TRT_DetectorManager with version "  << m_trt_manager->getVersion().majorNum() );
  }

  // PileUp Merge Service
  if ( m_mergeSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Could not find PileUpMergeSvc" );
    return StatusCode::FAILURE;
  }

  // Check data object name
  if ( m_trtHitCollectionKey == "" ) {
    ATH_MSG_FATAL( "Property trtHitCollectionName not set!" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Input hits: " << m_trtHitCollectionKey );
  }

  // Get Random Service
  if ( m_atRndmGenSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Could not initialize Random Number Service" );
    return StatusCode::FAILURE;
  }
  m_randomEngine = m_atRndmGenSvc->GetEngine( m_randomEngineName );
  if ( !m_randomEngine ) {
    ATH_MSG_ERROR( "Could not get random engine '" << m_randomEngineName << "'" );
    return StatusCode::FAILURE;
  }

  if ( m_trtDriftFunctionTool.retrieve().isFailure() ) {
    ATH_MSG_WARNING( "Could not retrieve " << m_trtDriftFunctionTool << endreq << "-> Switching to simplified cluster creation!" );
  }

  // // Particle Properties Service
  // if ( m_partPropSvc.retrieve().isFailure() ) {
  //   ATH_MSG_ERROR( "Could not initialize Particle Properties Service" );
  //   return StatusCode::FAILURE;
  // }
  // m_particleDataTable = m_partPropSvc->PDT();

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTFastDigitizationTool::prepareEvent(unsigned int)
{

  m_trtHitCollList.clear();
  m_thpctrt = new TimedHitCollection<TRTUncompressedHit>();
  m_HardScatterSplittingSkipper = false;
  return StatusCode::SUCCESS;

}

//_____________________________________________________________________________
StatusCode TRTFastDigitizationTool::processBunchXing(int /*bunchXing*/,
                                                     PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                                                     PileUpEventInfo::SubEvent::const_iterator eSubEvents) {

  //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
  if (m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }

  PileUpEventInfo::SubEvent::const_iterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->pSubEvtSG);
    PileUpTimeEventIndex thisEventIndex(PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index()));
    const TRTUncompressedHitCollection* seHitColl(nullptr);
    if (!seStore.retrieve(seHitColl,m_trtHitCollectionKey).isSuccess()) {
      ATH_MSG_ERROR ( "SubEvent TRTUncompressedHitCollection not found in StoreGate " << seStore.name() );
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE ( "TRTUncompressedHitCollection found with " << seHitColl->size() << " hits" );
    //Copy Hit Collection
    TRTUncompressedHitCollection* trtHitColl(new TRTUncompressedHitCollection("TRTUncompressedHits"));
    TRTUncompressedHitCollection::const_iterator i(seHitColl->begin());
    TRTUncompressedHitCollection::const_iterator e(seHitColl->end());
    // Read hits from this collection
    for (; i!=e; ++i) {
      const TRTUncompressedHit trthit(*i);
      trtHitColl->Insert(trthit);
    }
    m_thpctrt->insert(thisEventIndex, trtHitColl);
    //store these for deletion at the end of mergeEvent
    m_trtHitCollList.push_back(trtHitColl);

    ++iEvt;
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTFastDigitizationTool::produceDriftCircles() {

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

      double driftRadiusLoc = getDriftRadiusFromXYZ( hit );

      double efficiency = m_trtFitAmplitude * ( erf( ( m_trtFitMu + m_trtFitR - driftRadiusLoc ) / ( std::sqrt( 2 ) * m_trtFitSigma ) )
                                                + erf( ( m_trtFitMu + m_trtFitR + driftRadiusLoc ) / ( std::sqrt( 2 ) * m_trtFitSigma ) )
                                                - erf( ( m_trtFitMu - m_trtFitR - driftRadiusLoc ) / ( std::sqrt( 2 ) * m_trtFitSigma ) )
                                                - erf( ( m_trtFitMu - m_trtFitR + driftRadiusLoc ) / ( std::sqrt( 2 ) * m_trtFitSigma ) )
                                              )
                          + m_trtFitConstant;

      efficiency *= ( hitID & 0x00200000 ? 1.11 : 0.98 ); // endcap : barrel

      // Decide wether to throw away this cluster or not
      if ( CLHEP::RandFlat::shoot( m_randomEngine ) < ( 1. - efficiency ) ) continue;

      // Decide core/tail fraction
      bool isTail = ( CLHEP::RandFlat::shoot( m_randomEngine ) < m_trtTailFraction );
      // Start with the constant smearing
      double sigmaTrt = ( isTail ? m_trtSigmaDriftRadiusTail : m_trtSigmaDriftRadius );

      // Take driftfunction tool only when it is not a tail
      if ( !isTail ) {
        double driftTime = m_trtDriftFunctionTool->approxDriftTime( fabs( driftRadiusLoc ) );
        sigmaTrt = m_trtDriftFunctionTool->errorOfDriftRadius( driftTime, hit_id );
      }

      double smearedRadius = driftRadiusLoc;
      double tail_smearing = CLHEP::RandFlat::shoot( m_randomEngine );

      smearedRadius += CLHEP::RandGauss::shoot( m_randomEngine, 0., ( tail_smearing < 0.77 ? 0.75 : 2.2 ) ) * sigmaTrt;

      int ii = 0;
      while ( smearedRadius > 2. ) {
        smearedRadius += CLHEP::RandGauss::shoot( m_randomEngine, 0., ( tail_smearing < 0.77 ? 0.75 : 2.2 ) ) * sigmaTrt;
        ++ii;
        if ( ii > 50 ) {
          smearedRadius = 2.;
          break;
        }
      }

      if ( smearedRadius < 0. ) {
        smearedRadius = 0.;
        sigmaTrt = m_trtSigmaDriftRadiusTail;
      }

      if ( smearedRadius < 1.0e-5 ) {
        sigmaTrt = m_trtSigmaDriftRadiusTail;
      }

      Amg::Vector2D hitLocalPosition( smearedRadius, 0. );

      std::vector< Identifier > rdoList = { straw_id };

      Amg::MatrixX *hitErrorMatrix = new Amg::MatrixX( 1, 1 );
      ( *hitErrorMatrix )( Trk::driftRadius, Trk::driftRadius ) = sigmaTrt * sigmaTrt;

      // the TRT word simulate only TR information for the moment
      // consult TRTElectronicsProcessing::EncodeDigit() in TRT_Digitization/src/TRTElectronicsProcessing.cxx
      unsigned int word = 0x00007c00;  // set to a standard low threshold hit: word = 0; for ( unsigned int j = 10; j < 15; ++j ) word += 1 << ( 25 - j - j / 8 );

      if ( m_trtTRsimulation ) {

        HepGeom::Point3D< double > hitGlobalPosition = getGlobalPosition( hit );
        double eta = hitGlobalPosition.pseudoRapidity();

        int particleEncoding = hit->GetParticleEncoding();
        float kineticEnergy = hit->GetKineticEnergy();

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
      }  // end of m_trtTRsimulation

      InDet::TRT_DriftCircle *trtDriftCircle = new InDet::TRT_DriftCircle( straw_id, hitLocalPosition, rdoList, hitErrorMatrix, trtBaseElement, word );

      if ( !trtDriftCircle ) continue;

      m_driftCircleMap.insert( std::multimap< Identifier, InDet::TRT_DriftCircle * >::value_type( straw_id, trtDriftCircle ) );

      if ( hit->particleLink().isValid() ) {
        const int barcode( hit->particleLink().barcode());
        if ( barcode !=0 && barcode != m_vetoThisBarcode ) {
          m_prdMultiTruthCollection->insert( std::make_pair( trtDriftCircle->identify(), hit->particleLink() ) );
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

  // Create OUTPUT TRT_DriftCircleContainer and register it in StoreGate
  m_trtDriftCircleContainer = new InDet::TRT_DriftCircleContainer( m_trt_id->straw_layer_hash_max() );
  if( !m_trtDriftCircleContainer ) {
    ATH_MSG_FATAL( "Could not create TRT_DriftCircleContainer" );
    return StatusCode::FAILURE;
  }
  if ( evtStore()->record( m_trtDriftCircleContainer, m_trtDriftCircleKey ).isFailure() ) {
    ATH_MSG_FATAL( "Container " << m_trtDriftCircleKey << " could not be registered in StoreGate" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG( "Container " << m_trtDriftCircleKey << " registered in StoreGate" );
  }

  // Create (or retrieve) OUTPUT PRD_MultiTruthCollection
  m_prdMultiTruthCollection = new PRD_MultiTruthCollection;
  if ( evtStore()->contains< PRD_MultiTruthCollection >( m_prdMultiTruthKey )
       && evtStore()->retrieve( m_prdMultiTruthCollection, m_prdMultiTruthKey ).isFailure() ) {
    ATH_MSG_FATAL( "Could not retrieve collection " << m_prdMultiTruthKey );
    return StatusCode::FAILURE;
  }
  else if ( evtStore()->record( m_prdMultiTruthCollection, m_prdMultiTruthKey ).isFailure() ) {
    ATH_MSG_FATAL("Could not record collection " << m_prdMultiTruthKey);
    return StatusCode::FAILURE;
  }

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
    //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
    if (m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; continue; }
    if (m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { continue; }
    if (m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }
    timedHitCollection.insert( itr->first, static_cast< const TRTUncompressedHitCollection * >( itr->second ) );
  }
  m_thpctrt = & timedHitCollection;

  // Process the Hits straw by straw: get the iterator pairs for given straw
  if ( produceDriftCircles().isFailure() ) {
    ATH_MSG_FATAL( "produceDriftCircles() method failed!" );
    return StatusCode::FAILURE;
  }

  if ( createAndStoreRIOs().isFailure() ) {
    ATH_MSG_FATAL( "createAndStoreRIOs() failed" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG( "createAndStoreRIOs() succeeded" );
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________


StatusCode TRTFastDigitizationTool::mergeEvent() {

  //  ATH_MSG_INFO ( "--- In mergeEvent() ---" );


  m_trtDriftCircleContainer = new InDet::TRT_DriftCircleContainer(m_trt_id->straw_layer_hash_max());

  if(!m_trtDriftCircleContainer) {
    ATH_MSG_FATAL( "[ --- ] Could not create TRT_DriftCircleContainer");
    return StatusCode::FAILURE;
  }

  //  Register it to StoreGate
  if ( evtStore()->record(m_trtDriftCircleContainer,"TRT_DriftCircles").isFailure() ) {
    ATH_MSG_FATAL( "Container TRT_DriftCircles could not be registered in StoreGate !" );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_DEBUG ( "Container TRT_DriftCircles registered in StoreGate" );

  // Process the Hits straw by straw: get the iterator pairs for given straw
  if(this->produceDriftCircles().isFailure()) {
    ATH_MSG_FATAL ( "produceDriftCircles method failed!" );
    return StatusCode::FAILURE;
  }


  delete m_thpctrt;
  std::list<TRTUncompressedHitCollection*>::iterator trtHitColl(m_trtHitCollList.begin());
  std::list<TRTUncompressedHitCollection*>::iterator trtHitCollEnd(m_trtHitCollList.end());
  while(trtHitColl!=trtHitCollEnd)
    {
      delete (*trtHitColl);
      ++trtHitColl;
    }
  m_trtHitCollList.clear();


  if (createAndStoreRIOs().isFailure()) {
    ATH_MSG_FATAL ( "createAndStoreRIOs() failed!" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( "createAndStoreRIOs() succeeded" );

  }


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

    if ( trtDriftCircleCollection && m_trtDriftCircleContainer->addCollection( trtDriftCircleCollection, hash ).isFailure() ) {
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


// double TRTFastDigitizationTool::particleMass( int particleEncoding ) const
// {
//   double mass = 0.;

//   const HepPDT::ParticleData *particle = m_particleDataTable->particle( HepPDT::ParticleID( abs( particleEncoding ) ) );
//   if ( particle ) {
//     mass = particle->mass().value();
//   }

//   return mass;
// }


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


double TRTFastDigitizationTool::strawEfficiency( double driftRadius )
{
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
}


StatusCode TRTFastDigitizationTool::finalize() {

  ATH_MSG_DEBUG( "TRTFastDigitizationTool::finalize()" );
  return StatusCode::SUCCESS;
}
