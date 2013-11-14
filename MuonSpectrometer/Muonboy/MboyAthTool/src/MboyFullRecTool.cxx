/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

#include "Particle/TrackParticleContainer.h"

#include "MboyAthToolInterfaces/IMboyFullRecTool.h"

#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MboyFullRecTool.h"

#include "MboyCoreEvt/MboyPosDigiChoice.h"

#include "MboyAthEvt/MuonRecDigiStoreAth.h"
#include "MboyAthEvt/MB_SegmentStoreAth.h"
#include "MboyAthEvt/MB_TrackStoreAth.h"

#include "MboyAthEvt/MuonRecDigiStoreTDS.h"
#include "MboyAthEvt/MB_SegmentStoreTDS.h"
#include "MboyAthEvt/MB_TrackStoreTDS.h"

MboyFullRecTool::MboyFullRecTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
 p_OwnEDMHelper                  ( "MboyAthToolHelper" )
{

  declareInterface<IMboyFullRecTool>(this);

  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;

}

MboyFullRecTool::~MboyFullRecTool(){}

// Initialize
StatusCode MboyFullRecTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }

//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper );

  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyFullRecTool::finalize(){return StatusCode::SUCCESS;}

StatusCode  MboyFullRecTool::DoRec(
  const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,
  int PrintDigitsSummary , int AthPosOff , int SwitchOff , int ShortSummarySegment , int  ShortSummaryTrack, 
  std::string DigiStoreLocation , std::string TrackStoreLocation , std::string SegmentStoreLocation ,
  int ProcessingStage ,
  std::string SegmentCollectionLocation,
  std::string m_TrackCollectionLocation      , std::string TrackCollection_MSonlyLocation       ,
  std::string TrackParticleContainerLocation , std::string TrackParticleContainer_MSonlyLocation
  
){

  StatusCode sc = StatusCode::SUCCESS;

//Create MuonRecDigiStoreTDS
  MuonRecDigiStoreTDS* pMuonRecDigiStoreTDS = new MuonRecDigiStoreTDS;
  MuonRecDigiStoreAth* pMuonRecDigiStoreAth = &(pMuonRecDigiStoreTDS->m_MuonRecDigiStore);
  pMuonRecDigiStoreAth->ResetMuonRecDigiStoreAth();


//Fill MuonRecDigiStoreAth and update Digis
  sc = p_OwnEDMHelper->FillMuonRecDigiStoreWithVectorOfPrepData_Base( VectorOfpPrepData, pMuonRecDigiStoreAth );
  if ( sc.isFailure() ){
    ATH_MSG_FATAL( "FillMuonRecDigiStoreWithVectorOfPrepData_Base failed" );
    return( StatusCode::FAILURE);
  }


//Print Out MuonRecDigiStoreAth short header
  if (PrintDigitsSummary>=1) pMuonRecDigiStoreAth->PrintSummary();
  if (PrintDigitsSummary>=3) pMuonRecDigiStoreAth->PrintChecks();


//Create MB_SegmentStoreTDS 
  MB_SegmentStoreTDS* pMB_SegmentStoreTDS = new MB_SegmentStoreTDS;
  MB_SegmentStoreAth* pMB_SegmentStoreAth = &(pMB_SegmentStoreTDS->m_MB_SegmentStore);
  pMB_SegmentStoreAth->ResetMB_SegmentStoreAth();


//Create MB_TrackStoreTDS 
  MB_TrackStoreTDS* pMB_TrackStoreTDS = new MB_TrackStoreTDS;
  MB_TrackStoreAth* pMB_TrackStoreAth = &(pMB_TrackStoreTDS->m_MB_TrackStore);
  pMB_TrackStoreAth->ResetMB_TrackStoreAth();


//Do Reconstruction
  MboyPosDigiChoice aMboyPosDigiChoice ; 
  aMboyPosDigiChoice.SetPosOffMDT (AthPosOff) ;
  aMboyPosDigiChoice.SetPosOffRPC (AthPosOff) ;
  aMboyPosDigiChoice.SetPosOffTGC (AthPosOff) ;
  aMboyPosDigiChoice.SetPosOffCSC (AthPosOff) ;
  if (SwitchOff == 0) {
    sc = p_OwnEDMHelper->DoRec(
             &aMboyPosDigiChoice,
             pMuonRecDigiStoreAth,
             pMB_TrackStoreAth,
             pMB_SegmentStoreAth
            );
    if ( sc.isFailure() ){
      ATH_MSG_FATAL( "DoRec failed" );
      return( StatusCode::FAILURE);
    }
  }


//Consistenfify pMB_SegmentStoreAth
  pMB_SegmentStoreAth->Consistentify(pMuonRecDigiStoreAth);


//Consistenfify pMB_TrackStoreAth
  pMB_TrackStoreAth->Consistentify(pMuonRecDigiStoreAth);


//Print Out pMB_SegmentStoreAth short header
  if (ShortSummarySegment>=1) pMB_SegmentStoreAth->PrintSummary();


//Print Out pMB_TrackStoreAth short header
  if (ShortSummaryTrack>=1) pMB_TrackStoreAth->PrintSummary();


//Save MuonRecDigiStoreTDS
  sc = evtStore()->record(pMuonRecDigiStoreTDS,DigiStoreLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save pMuonRecDigiStoreTDS at " << DigiStoreLocation );
    return( StatusCode::FAILURE );
  }


//Save MB_TrackStoreTDS
  sc = evtStore()->record(pMB_TrackStoreTDS,TrackStoreLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save pMB_TrackStoreTDS at " << TrackStoreLocation );
    return( StatusCode::FAILURE );
  }


//Save MB_SegmentStoreTDS
  sc = evtStore()->record(pMB_SegmentStoreTDS,SegmentStoreLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save pMB_SegmentStoreTDS at " << SegmentStoreLocation );
    return( StatusCode::FAILURE );
  }

//SegmentCollection
  Trk::SegmentCollection* pSegmentCollection = new Trk::SegmentCollection;

  if (ProcessingStage >= 1 ){
    sc = p_OwnEDMHelper->GetSegmentCollection( pMB_SegmentStoreAth, pSegmentCollection );
    if ( sc.isFailure() ){ 
      ATH_MSG_FATAL( "GetSegmentCollection failed" );
      return( StatusCode::FAILURE);
    }
  }

  sc = evtStore()->record(pSegmentCollection, SegmentCollectionLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save SegmentCollection at " << SegmentCollectionLocation );
    return( StatusCode::FAILURE);
  }


//TrackCollection
  TrackCollection* pTrackCollection        = new TrackCollection;
  TrackCollection* pTrackCollection_MSonly = new TrackCollection;

  if (ProcessingStage >= 2 ){
    sc = p_OwnEDMHelper->GetTrackCollection(  pMB_TrackStoreAth, pTrackCollection, pTrackCollection_MSonly );
    if ( sc.isFailure() ){
      ATH_MSG_FATAL( "GetTrackCollection failed" );
      return( StatusCode::FAILURE);
    }
  }

  sc = evtStore()->record(pTrackCollection, m_TrackCollectionLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save TrackCollection at " << m_TrackCollectionLocation );
    return( StatusCode::FAILURE);
  }

  sc = evtStore()->record(pTrackCollection_MSonly, TrackCollection_MSonlyLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save TrackCollection at " << TrackCollection_MSonlyLocation );
    return( StatusCode::FAILURE);
  }


//TrackParticleContainer
  Rec::TrackParticleContainer* pTrackParticleContainer        = new Rec::TrackParticleContainer;
  Rec::TrackParticleContainer* pTrackParticleContainer_MSonly = new Rec::TrackParticleContainer;

  if (ProcessingStage >= 3 ){
    sc = p_OwnEDMHelper->GetParticleCollection( pTrackCollection, pTrackParticleContainer );
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL( "GetParticleCollection failed for pTrackCollection" );
      return( StatusCode::FAILURE);
    }
  }

  sc = p_OwnEDMHelper->GetParticleCollection( pTrackCollection_MSonly, pTrackParticleContainer_MSonly );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "GetParticleCollection failed for pTrackCollection_MSonly" );
    return( StatusCode::FAILURE);
  }

  sc = evtStore()->record(pTrackParticleContainer, TrackParticleContainerLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save TrackParticleContainer at " << TrackParticleContainerLocation );
    return( StatusCode::FAILURE);
  }

  sc = evtStore()->record(pTrackParticleContainer_MSonly, TrackParticleContainer_MSonlyLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save TrackParticleContainer at " << TrackParticleContainer_MSonlyLocation );
    return( StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyFullRecTool::KountMB_TrackStoreTDS(std::vector<int>&  KounterMB_TrackStoreTDS,std::string& TrackStoreLocation){

  StatusCode sc = StatusCode::SUCCESS;

//Access tracks
  unsigned SizeContainer = 0 ;
  const DataHandle<MB_TrackStoreTDS> pMB_TrackStoreTDS ;
  sc = evtStore()->retrieve(pMB_TrackStoreTDS,TrackStoreLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING( "pMB_TrackStoreTDS not found at " << TrackStoreLocation );
  }else{
    SizeContainer = (pMB_TrackStoreTDS->m_MB_TrackStore).NberOfMB_Track();
  }

//Count evts of given nber of Reconstructed tracks
  if ( KounterMB_TrackStoreTDS.size() < (SizeContainer+1) ){
    int ItemNberStart = KounterMB_TrackStoreTDS.size() + 1;
    int ItemNberEnd   = SizeContainer + 1      ;
    for (int ItemNber=ItemNberStart; ItemNber<=ItemNberEnd ; ItemNber++){
      int IntToAdd = 0 ;
      KounterMB_TrackStoreTDS.push_back(IntToAdd) ;
    }
  }
  KounterMB_TrackStoreTDS[SizeContainer] = KounterMB_TrackStoreTDS[SizeContainer] + 1 ;

  return StatusCode::SUCCESS;

}
StatusCode MboyFullRecTool::KountMB_SegmentStoreTDS(std::vector<int>&  KounterMB_SegmentStoreTDS,std::string& SegmentStoreLocation){

  StatusCode sc = StatusCode::SUCCESS;

//Access Segments
  unsigned SizeContainer = 0 ;
  const DataHandle<MB_SegmentStoreTDS> pMB_SegmentStoreTDS ;
  sc = evtStore()->retrieve(pMB_SegmentStoreTDS,SegmentStoreLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING( "pMB_SegmentStoreTDS not found at " << SegmentStoreLocation );
  }else{
    SizeContainer = (pMB_SegmentStoreTDS->m_MB_SegmentStore).NberOfMB_Segment();
  }

//Count evts of given nber of Reconstructed segments
  if ( KounterMB_SegmentStoreTDS.size() < (SizeContainer+1) ){
    int ItemNberStart = KounterMB_SegmentStoreTDS.size() + 1;
    int ItemNberEnd   = SizeContainer + 1      ;
    for (int ItemNber=ItemNberStart; ItemNber<=ItemNberEnd ; ItemNber++){
      int IntToAdd = 0 ;
      KounterMB_SegmentStoreTDS.push_back(IntToAdd) ;
    }
  }
  KounterMB_SegmentStoreTDS[SizeContainer] = KounterMB_SegmentStoreTDS[SizeContainer] + 1 ;

  return StatusCode::SUCCESS;

}
StatusCode MboyFullRecTool::KountSegmentCollection(std::vector<int>&  KounterSegmentCollection,std::string& SegmentCollectionLocation){

  StatusCode sc = StatusCode::SUCCESS;

//Access Segments
  unsigned SizeContainer = 0 ;
  const Trk::SegmentCollection* pSegmentCollection ;
  sc = evtStore()->retrieve(pSegmentCollection, SegmentCollectionLocation);
  if ( sc.isFailure() ){
    ATH_MSG_WARNING( "pSegmentCollection not found at " << SegmentCollectionLocation );
  }else{
    SizeContainer = pSegmentCollection->size() ;
  }


//Count evts of given nber of Reconstructed segments
  if ( KounterSegmentCollection.size() < (SizeContainer+1) ){
    int ItemNberStart = KounterSegmentCollection.size() + 1;
    int ItemNberEnd   = SizeContainer + 1      ;
    for (int ItemNber=ItemNberStart; ItemNber<=ItemNberEnd ; ItemNber++){
      int IntToAdd = 0 ;
      KounterSegmentCollection.push_back(IntToAdd) ;
    }
  }
  KounterSegmentCollection[SizeContainer] = KounterSegmentCollection[SizeContainer] + 1 ;

  return StatusCode::SUCCESS;

}
StatusCode MboyFullRecTool::KountTrackCollection(std::vector<int>&  KounterTrackCollection,std::string& TrackCollectionLocation){

  StatusCode sc = StatusCode::SUCCESS;

//Access tracks
  unsigned SizeContainer = 0 ;
  const TrackCollection* pTrackCollection ;
  sc = evtStore()->retrieve(pTrackCollection, TrackCollectionLocation);
  if ( sc.isFailure() ){
    ATH_MSG_WARNING( "pTrackCollection not found at " << TrackCollectionLocation );
  }else{
    SizeContainer = pTrackCollection->size() ;
  }

//Count evts of given nber of Reconstructed tracks
  if ( KounterTrackCollection.size() < (SizeContainer+1) ){
    int ItemNberStart = KounterTrackCollection.size() + 1;
    int ItemNberEnd   = SizeContainer + 1      ;
    for (int ItemNber=ItemNberStart; ItemNber<=ItemNberEnd ; ItemNber++){
      int IntToAdd = 0 ;
      KounterTrackCollection.push_back(IntToAdd) ;
    }
  }
  KounterTrackCollection[SizeContainer] = KounterTrackCollection[SizeContainer] + 1 ;

  return StatusCode::SUCCESS;

}
