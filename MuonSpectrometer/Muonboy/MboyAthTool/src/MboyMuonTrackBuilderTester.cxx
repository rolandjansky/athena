/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

#include "MuonTrackFindingEvent/MuPatTrack.h"
#include "MuonTrackFindingEvent/MuPatCandidateBase.h"
#include "MuonTrackFindingEvent/MuPatSegment.h"
#include "MuonSegment/MuonSegment.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TrkParameters/TrackParameters.h"
#include "EventPrimitives/EventPrimitives.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MboyMuonTrackBuilderTester.h"

MboyMuonTrackBuilderTester::MboyMuonTrackBuilderTester(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
 m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
 m_segmentSelector("Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool"),
 m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
 p_OwnEDMHelper                  ( "MboyAthToolHelper" ),
 p_IMuonSegmentInfoExtender ( "Muon::MboyMuonTrackBuilder/MboyMuonTrackBuilder" ) ,
 p_IMuonTrackBuilder ( "Muon::MboyMuonTrackBuilder/MboyMuonTrackBuilder" ) ,
 p_IMuonTrackRefiner ( "Muon::MboyMuonTrackBuilder/MboyMuonTrackBuilder" ) ,
 p_MboyMuonTrackBuilder ( "Muon::MboyMuonTrackBuilder/MboyMuonTrackBuilder" ) 
{

  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;
  declareProperty("MuonSegmentInfoExtender" , p_IMuonSegmentInfoExtender ) ;
  declareProperty("MuonTrackBuilder" , p_IMuonTrackBuilder ) ;
  declareProperty("MuonTrackRefiner" , p_IMuonTrackRefiner ) ;
  declareProperty("MboyMuonTrackBuilder" , p_MboyMuonTrackBuilder ) ;

  declareProperty( "SegmentCollectionLocation" , m_SegmentCollectionLocation = "ConvertedMBoySegments" ) ;
  declareProperty( "TrackCollectionLocation"   , m_TrackCollectionLocation   = "ConvertedMBoyTracks"   ) ;

}
MboyMuonTrackBuilderTester::~MboyMuonTrackBuilderTester(){}


// Initialize
StatusCode MboyMuonTrackBuilderTester::initialize(){

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are        " );
  ATH_MSG_INFO( "= SegmentCollectionLocation           " << m_SegmentCollectionLocation        );
  ATH_MSG_INFO( "= TrackCollectionLocation             " << m_TrackCollectionLocation          );
  ATH_MSG_INFO( "================================" );


//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper ) ;

//Retrieve p_IMuonSegmentInfoExtender
  if ( p_IMuonSegmentInfoExtender.retrieve().isFailure() ) {
   ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonSegmentInfoExtender );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonSegmentInfoExtender );
  
//Retrieve p_IMuonTrackBuilder
  if ( p_IMuonTrackBuilder.retrieve().isFailure() ) {
   ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonTrackBuilder );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonTrackBuilder );
  
//Retrieve p_IMuonTrackRefiner
  if ( p_IMuonTrackRefiner.retrieve().isFailure() ) {
   ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonTrackRefiner );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonTrackRefiner );
  
//Retrieve p_MboyMuonTrackBuilder
  if ( p_MboyMuonTrackBuilder.retrieve().isFailure() ) {
   ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyMuonTrackBuilder );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyMuonTrackBuilder );


//JFL: Taken from MuPatCandidateTool::createSegInfo Wed May  4 17:16:24 CEST 2011
  if( m_idHelperTool.retrieve().isFailure() ){
    ATH_MSG_ERROR("Could not get " << m_idHelperTool); 
    return StatusCode::FAILURE;
  }

  if( m_segmentSelector.retrieve().isFailure() ){
    ATH_MSG_ERROR("Could not get " << m_segmentSelector); 
    return StatusCode::FAILURE;
  }

  if( m_helperTool.retrieve().isFailure() ){
    ATH_MSG_ERROR("Could not get " << m_helperTool); 
    return StatusCode::FAILURE;
  }


  ATH_MSG_INFO( "Initialisation ended     " );


  return StatusCode::SUCCESS;

}

// Execute
StatusCode MboyMuonTrackBuilderTester::execute(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( setiosflags(std::ios::fixed) );
  ATH_MSG_INFO( "/////////////////////////////////////////////////////////" );

  ATH_MSG_INFO( "*** Work with Segment container " << m_SegmentCollectionLocation );
  const Trk::SegmentCollection* pSegmentCollection ;
  sc = evtStore()->retrieve(pSegmentCollection, m_SegmentCollectionLocation);
  if (sc.isFailure()){
    ATH_MSG_FATAL( "pSegmentCollection not found at " << m_SegmentCollectionLocation ) ;
    return( StatusCode::FAILURE );
  }
  ATH_MSG_INFO( "*** The nber of Segment is  " << pSegmentCollection->size() );
  
  int NberOfMuonSegments = 0 ;
  for (Trk::SegmentCollection::const_iterator it = pSegmentCollection->begin(); it!=pSegmentCollection->end(); ++it){
    const Trk::Segment* pSegment = (*it);
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if ( pMuonSegment == 0 ) continue ;
    NberOfMuonSegments += 1 ;
  }
  ATH_MSG_INFO( "*** The nber of MuonSegment is  " << NberOfMuonSegments );
  
  ATH_MSG_INFO( "  " );
  NberOfMuonSegments = -1 ;
  std::vector<Muon::MuPatSegment*> VectorOfMuPatSegmentLocal ; VectorOfMuPatSegmentLocal.clear() ;
  VectorOfMuPatSegmentLocal.reserve( pSegmentCollection->size() );
  for (Trk::SegmentCollection::const_iterator it = pSegmentCollection->begin(); it!=pSegmentCollection->end(); ++it){
    const Trk::Segment* pSegment = (*it);
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if ( pMuonSegment == 0 ) continue ;
    NberOfMuonSegments += 1 ;
    Muon::MuPatSegment* pMuPatSegment = createSegInfo(*pMuonSegment);
    ATH_MSG_INFO( "*MuonSegment nber " << NberOfMuonSegments );
    DumpMuPatSegment(pMuPatSegment);
    VectorOfMuPatSegmentLocal.push_back(pMuPatSegment);
  }
  ATH_MSG_INFO( "  " );
  
  
  ATH_MSG_INFO( "*** Work with Track container " << m_TrackCollectionLocation );
  const TrackCollection* pTrackCollection ;
  sc = evtStore()->retrieve(pTrackCollection, m_TrackCollectionLocation);
  if (sc.isFailure()){
    for (std::vector< Muon::MuPatSegment* >::iterator it = VectorOfMuPatSegmentLocal.begin(); it!=VectorOfMuPatSegmentLocal.end(); ++it) delete (*it);
    VectorOfMuPatSegmentLocal.clear() ;
    ATH_MSG_FATAL( "pTrackCollection not found at " << m_TrackCollectionLocation ) ;
    return( StatusCode::FAILURE );
  }
  ATH_MSG_INFO( "*** The nber of Tracks is  " << pTrackCollection->size() );
  
  ATH_MSG_INFO( "  " );
  int Kounter_Track = -1 ;
  for (TrackCollection::const_iterator it_t = pTrackCollection->begin(); it_t!=pTrackCollection->end(); ++it_t){
    Kounter_Track += 1 ;
    ATH_MSG_INFO( "*Track nber " << Kounter_Track );
    Trk::Track*    pTrack = (*it_t) ;
    
    std::vector<int>  VectorOfKounterOnMuPatSegment ;
    std::vector<int>  VectorOfScore                 ;
    std::vector<int>  VectorOfISC0                  ;
    int Kounter_MuPatSegment = -1 ;
    for (std::vector< Muon::MuPatSegment* >::iterator it_s = VectorOfMuPatSegmentLocal.begin(); it_s!=VectorOfMuPatSegmentLocal.end(); ++it_s){
      Kounter_MuPatSegment += 1 ;
      Muon::MuPatSegment* pMuPatSegment = (*it_s) ;
      int CommonHit = ScoreCommonHits( pMuPatSegment ,pTrack ) ;
      ATH_MSG_INFO( "*   MuPatSegment Nber " << Kounter_MuPatSegment << " " << p_OwnEDMHelper->SegmentAssociatedStationName( pMuPatSegment->segment ) << " shares " << CommonHit << " hits with this track");

      if ( CommonHit != 0 ){    
        int Found = 0 ;
        unsigned int ItemNberEnd = VectorOfKounterOnMuPatSegment.size() ;
        for (unsigned int ItemNber=0; ItemNber<ItemNberEnd ; ItemNber++){
          std::string SegmentName1 = p_OwnEDMHelper->SegmentAssociatedStationName( 
              ( VectorOfMuPatSegmentLocal[ VectorOfKounterOnMuPatSegment[ItemNber] ] )->segment 
                                                        ) ;
          std::string SegmentName2 = p_OwnEDMHelper->SegmentAssociatedStationName( 
              pMuPatSegment->segment 
                                                        ) ;
          if ( ( SegmentName1.substr(0,2) ==  SegmentName2.substr(0,2)       )  
          ||   ( VectorOfISC0[ItemNber] == (pMuPatSegment->mboyInfo)->m_ISC0 ) ){
            Found = 1 ;
            if ( VectorOfScore[ItemNber] <= CommonHit ){
              VectorOfKounterOnMuPatSegment[ItemNber] = Kounter_MuPatSegment ;
              VectorOfScore[ItemNber]                 = CommonHit            ;
            }
          }
          if ( Found == 1 ) break ;
        }
        if ( Found == 0 ){
              VectorOfKounterOnMuPatSegment.push_back( Kounter_MuPatSegment ) ;
              VectorOfScore.push_back( CommonHit );
              VectorOfISC0.push_back( (pMuPatSegment->mboyInfo)->m_ISC0 );
        }
      }
    }
    ATH_MSG_INFO( "  " );
    
    std::vector<double>  VectorOfDist2IPD           ;
    unsigned int ItemNberEnd = VectorOfKounterOnMuPatSegment.size() ;
    for (unsigned int ItemNber=0; ItemNber<ItemNberEnd ; ItemNber++){
      const Muon::MuPatSegment* pMuPatSegment = VectorOfMuPatSegmentLocal[ VectorOfKounterOnMuPatSegment[ItemNber] ] ;
      const Amg::Vector3D TheGlobalPosition =  (pMuPatSegment->segment)->globalPosition() ;
      double Dist2IPD  = sqrt(
                             TheGlobalPosition.x() * TheGlobalPosition.x()
                           + TheGlobalPosition.y() * TheGlobalPosition.y()
                           + TheGlobalPosition.z() * TheGlobalPosition.z()
                             );
      VectorOfDist2IPD.push_back(Dist2IPD) ;
    }
    
    std::vector<int>  VectorOrdored ;
    sort(VectorOfDist2IPD,VectorOrdored);
    
    ATH_MSG_INFO( "*   So this track is associated with the segments  " );
    ItemNberEnd = VectorOrdored.size() ;
    for (unsigned int ItemNber=0; ItemNber<ItemNberEnd ; ItemNber++){
      unsigned int ItemMberOrdored = VectorOrdored[ItemNber] ;
      ATH_MSG_INFO(  
                    "*        Segment Nber: " << VectorOfKounterOnMuPatSegment[ItemMberOrdored] 
                 << "  shared hits: " << VectorOfScore[ItemMberOrdored] 
                 << "  distance from IP: " << std::setw(20)<<std::setprecision(3) << VectorOfDist2IPD[ItemMberOrdored] 
                 );
      DumpMuPatSegment(VectorOfMuPatSegmentLocal[ VectorOfKounterOnMuPatSegment[ItemMberOrdored] ]);
    }
    ATH_MSG_INFO( "  " );
   
    ATH_MSG_INFO( "* Now try find method  " );

    Muon::MuPatSegment* pMuPatSegment_Seed =  VectorOfMuPatSegmentLocal[ VectorOfKounterOnMuPatSegment[ VectorOrdored[0] ] ] ;
    Muon::MuPatTrack* pStartingMuPatTrack = p_MboyMuonTrackBuilder->DoMuPaTrackFromMuPatSegment( pMuPatSegment_Seed ) ;
    if ( pStartingMuPatTrack == 0 ){
       ATH_MSG_INFO( "   pStartingMuPatTrack is null!!!  " );
       continue ;
    }
    
    std::vector<Muon::MuPatTrack*>* pVectorOfMuPatTrack = new std::vector<Muon::MuPatTrack*>() ;
    pVectorOfMuPatTrack->push_back(pStartingMuPatTrack);

    int StageNber = 1 ;
    TryFind( pTrack, pVectorOfMuPatTrack, VectorOrdored ,  VectorOfKounterOnMuPatSegment, VectorOfMuPatSegmentLocal, StageNber ) ;
  
    for (std::vector< Muon::MuPatTrack* >::iterator it = pVectorOfMuPatTrack->begin(); it!=pVectorOfMuPatTrack->end(); ++it) delete (*it);
    pVectorOfMuPatTrack->clear() ;
    delete pVectorOfMuPatTrack ;
      
  }
  
  for (std::vector< Muon::MuPatSegment* >::iterator it = VectorOfMuPatSegmentLocal.begin(); it!=VectorOfMuPatSegmentLocal.end(); ++it) delete (*it);
  VectorOfMuPatSegmentLocal.clear() ;

  return StatusCode::SUCCESS;

}

void MboyMuonTrackBuilderTester::TryFind(
                const Trk::Track* pTrack ,
                std::vector<Muon::MuPatTrack*>*& pVectorOfMuPatTrack_StagePrevious , 
                std::vector<int>&  VectorOrdored ,
                std::vector<int>&  VectorOfKounterOnMuPatSegment ,
                std::vector<Muon::MuPatSegment*>& VectorOfMuPatSegmentLocal ,
                int StageNber
){
  
  if ( pVectorOfMuPatTrack_StagePrevious == 0 ) return ;
  
  if ( int(VectorOrdored.size()) < (StageNber+1) ) return ;
  
  Muon::MuPatSegment* pMuPatSegment_Target =  VectorOfMuPatSegmentLocal[ VectorOfKounterOnMuPatSegment[ VectorOrdored[StageNber] ] ] ;   
         
  for (std::vector< Muon::MuPatTrack* >::iterator it = pVectorOfMuPatTrack_StagePrevious->begin(); it!=pVectorOfMuPatTrack_StagePrevious->end(); ++it) {

    ATH_MSG_INFO( "* " );
    ATH_MSG_INFO( "*       ===> STAGE "<< StageNber << " <====== " );
    ATH_MSG_INFO( "* " );

    std::vector<Muon::MuPatSegment*> MuPatSegmentFree ; MuPatSegmentFree.push_back ( pMuPatSegment_Target ) ;

    ATH_MSG_INFO( "*   The STARTING MuPatTrack is" )  ; DumpMuPatTrack( (*it) )               ;
    ATH_MSG_INFO( "*   The TARGET   MuTagSegments are" ); DumpMuPatSegmentS( &MuPatSegmentFree ) ;

    std::vector<Muon::MuPatTrack*>* pVectorOfMuPatTrack_StageNext = p_IMuonTrackBuilder->find( *(*it) , MuPatSegmentFree ) ;
    ATH_MSG_INFO( "*   The returned MuPatTracks are " );
    DumpMuPatTrackS(pVectorOfMuPatTrack_StageNext);

    if (pVectorOfMuPatTrack_StageNext) { 
      int kounter = -1 ;
      for (std::vector< Muon::MuPatTrack* >::iterator it = pVectorOfMuPatTrack_StageNext->begin(); it!=pVectorOfMuPatTrack_StageNext->end(); ++it) {
        kounter += 1 ;
        if ( int( ((*it)->segments()).size() ) >= 2 ) {
          ATH_MSG_INFO( "* Now try refine for track " << kounter );
          Muon::MuPatTrack* pMuPatTrack_refined = p_IMuonTrackRefiner->refine( *(*it) );
          ATH_MSG_INFO( "* The refined track is " << kounter );
          DumpMuPatTrack( pMuPatTrack_refined );
          if ( pMuPatTrack_refined ) CompareTrkTrack( pTrack, &(pMuPatTrack_refined->track()) );
          delete pMuPatTrack_refined;
        }else{
          ATH_MSG_INFO( "* Dont try to refine since too few segments " << (*it)->segments() );
        }
      }
    }

    int StageNberNext = StageNber + 1 ;
    TryFind( pTrack ,pVectorOfMuPatTrack_StageNext, VectorOrdored,  VectorOfKounterOnMuPatSegment, VectorOfMuPatSegmentLocal, StageNberNext ) ;

    if (pVectorOfMuPatTrack_StageNext) { 
      for (std::vector< Muon::MuPatTrack* >::iterator it = pVectorOfMuPatTrack_StageNext->begin(); it!=pVectorOfMuPatTrack_StageNext->end(); ++it) delete (*it);
      pVectorOfMuPatTrack_StageNext->clear() ;
    }
    delete pVectorOfMuPatTrack_StageNext ;
    
  }
  
}
// Finalize
StatusCode MboyMuonTrackBuilderTester::finalize(){return StatusCode::SUCCESS;}

void MboyMuonTrackBuilderTester::DumpMuPatTrackS(const std::vector<Muon::MuPatTrack*>* pVectorOfMuPatTrack)
{
 if (pVectorOfMuPatTrack==0) {
    ATH_MSG_INFO( "pVectorOfMuPatTrack is 0 " );
   return ;
 }
 unsigned int NberOfTracks = pVectorOfMuPatTrack->size() ;
 if ( NberOfTracks == 0 ){
    ATH_MSG_INFO( "NberOfTracks is 0 " );
 }else{
   ATH_MSG_INFO( "* The nber of tracks in  pVectorOfMuPatTrack is " <<  NberOfTracks);
   int kounter = -1 ;
   std::vector<Muon::MuPatTrack*>::const_iterator it = pVectorOfMuPatTrack->begin();
   for (; it!=pVectorOfMuPatTrack->end(); ++it){
     kounter += 1 ;
     ATH_MSG_INFO( "* track " << kounter );
     DumpMuPatTrack( (*it) );
   }
 }
 
}
void MboyMuonTrackBuilderTester::DumpMuPatTrack(const Muon::MuPatTrack* pMuPatTrack)
{
 if (pMuPatTrack==0) {
    ATH_MSG_INFO( "pMuPatTrack is 0 " );
   return ;
 }
 
 if ( (pMuPatTrack->mboyInfo) == 0 ){
   ATH_MSG_INFO( "(pMuPatTrack->mboyInfo) is 0 " );
 }else{
   ATH_MSG_INFO( "                  m_MboyStatus "   << std::setw(6) << (pMuPatTrack->mboyInfo)->m_MboyStatus );
   ATH_MSG_INFO( "                  m_ISC0       "   << std::setw(6) << (pMuPatTrack->mboyInfo)->m_ISC0 );
   ATH_MSG_INFO( "                  m_DZT/DS 1     " << std::setw(20)<<std::setprecision(3) << (pMuPatTrack->mboyInfo)->m_DZT1 
                                              << " " << std::setw(20)<<std::setprecision(3) << (pMuPatTrack->mboyInfo)->m_DS1 );
   ATH_MSG_INFO( "                  m_DZT/DS 2     " << std::setw(20)<<std::setprecision(3) << (pMuPatTrack->mboyInfo)->m_DZT2 
                                              << " " << std::setw(20)<<std::setprecision(3) << (pMuPatTrack->mboyInfo)->m_DS2 );
   ATH_MSG_INFO( "                  m_SMU        "   << std::setw(20)<<std::setprecision(6) << (pMuPatTrack->mboyInfo)->m_SMU  );
   ATH_MSG_INFO( "                  m_CF         "   << std::setw(20)<<std::setprecision(6) << (pMuPatTrack->mboyInfo)->m_CFI  
                                              << " " << std::setw(20)<<std::setprecision(6) << (pMuPatTrack->mboyInfo)->m_CFA  );
   ATH_MSG_INFO( "                  m_SF         "   << std::setw(20)<<std::setprecision(6) << (pMuPatTrack->mboyInfo)->m_SFI  
                                              << " " << std::setw(20)<<std::setprecision(6) << (pMuPatTrack->mboyInfo)->m_SFA  );
   ATH_MSG_INFO( "                  m_S/Z/T 1    "   << std::setw(20)<<std::setprecision(3) << (pMuPatTrack->mboyInfo)->m_S1   
                                              << " " << std::setw(20)<<std::setprecision(3) << (pMuPatTrack->mboyInfo)->m_Z1 
                                              << " " << std::setw(20)<<std::setprecision(3) << (pMuPatTrack->mboyInfo)->m_T1   );
   ATH_MSG_INFO( "                  m_S/Z/T 2    "   << std::setw(20)<<std::setprecision(3) << (pMuPatTrack->mboyInfo)->m_S2   
                                              << " " << std::setw(20)<<std::setprecision(3) << (pMuPatTrack->mboyInfo)->m_Z2 
                                              << " " << std::setw(20)<<std::setprecision(3) << (pMuPatTrack->mboyInfo)->m_T2   );
 }
 ATH_MSG_INFO( " Dump of the Trk::Track " );
 ATH_MSG_INFO( " " << pMuPatTrack->track() );
 
 ATH_MSG_INFO( "* In  this track  " );
 DumpMuPatSegmentS( &(pMuPatTrack->segments()) ) ;
 
}
void MboyMuonTrackBuilderTester::DumpMuPatSegmentS(const std::vector<Muon::MuPatSegment*>* pVectorOfMuPatSegment)
{
 if (pVectorOfMuPatSegment==0) {
    ATH_MSG_INFO( "pVectorOfMuPatSegment is 0 " );
   return ;
 }
 unsigned int NberOfSegments = pVectorOfMuPatSegment->size() ;
 if ( NberOfSegments == 0 ){
    ATH_MSG_INFO( "NberOfSegments is 0 " );
 }else{
   ATH_MSG_INFO( "* The nber of segments in  pVectorOfMuPatSegment is " <<  NberOfSegments);
   int kounter = -1 ;
   std::vector<Muon::MuPatSegment*>::const_iterator it = pVectorOfMuPatSegment->begin();
   for (; it!=pVectorOfMuPatSegment->end(); ++it){
     kounter += 1 ;
     ATH_MSG_INFO( "* segment " << kounter );
     DumpMuPatSegment( (*it) );
   }
 }
 
}
void MboyMuonTrackBuilderTester::DumpMuPatSegment(const Muon::MuPatSegment* pMuPatSegment)
{
 
  if (pMuPatSegment==0){
    ATH_MSG_INFO( "pMuPatSegment is 0 " );
    return ;
  }
  if (pMuPatSegment->mboyInfo==0){
    ATH_MSG_INFO( "pMuPatSegment->mboyInfo is 0 " );
    return ;
  }
  if (pMuPatSegment->segment==0){
    ATH_MSG_INFO( "pMuPatSegment->segment is 0 " );
    return ;
  }
  
  std::string SegmentName = p_OwnEDMHelper->SegmentAssociatedStationName( pMuPatSegment->segment ) ;

  const Amg::Vector3D TheGlobalPosition =  (pMuPatSegment->segment)->globalPosition() ;
  double Dist2IPD  = sqrt(
                         TheGlobalPosition.x() * TheGlobalPosition.x()
                       + TheGlobalPosition.y() * TheGlobalPosition.y()
                       + TheGlobalPosition.z() * TheGlobalPosition.z()
                         );
  
  ATH_MSG_INFO( "    " << std::setw(6) << SegmentName 
             << "    " << std::setw(20)<<std::setprecision(3) << Dist2IPD    );
  
  ATH_MSG_INFO( "                              m_ISC0    " << std::setw(6) << (pMuPatSegment->mboyInfo)->m_ISC0     
                                          << " m_IWDRAT  " << std::setw(6) << (pMuPatSegment->mboyInfo)->m_IWDRAT1 
                                                    << " " << std::setw(6) << (pMuPatSegment->mboyInfo)->m_IWDRAT2  
                                          << " m_JFDRAT  " << std::setw(6) << (pMuPatSegment->mboyInfo)->m_JFDRAT  );
  ATH_MSG_INFO( "                              m_FFDRAT   m_GGDRAT " << std::setw(20)<<std::setprecision(6) << (pMuPatSegment->mboyInfo)->m_FFDRAT 
                                                              << " " << std::setw(20)<<std::setprecision(6) << (pMuPatSegment->mboyInfo)->m_GGDRAT  
                                                    << " m_EZDRAT  " << std::setw(20)<<std::setprecision(3) << (pMuPatSegment->mboyInfo)->m_EZDRAT  );
  ATH_MSG_INFO( "                              m_SS/ZZ/TTDRAT 1 "    << std::setw(20)<<std::setprecision(3) << (pMuPatSegment->mboyInfo)->m_SSDRAT1 
                                                             << " "  << std::setw(20)<<std::setprecision(3) << (pMuPatSegment->mboyInfo)->m_ZZDRAT1  
                                                             << " "  << std::setw(20)<<std::setprecision(3) << (pMuPatSegment->mboyInfo)->m_TTDRAT1 );
  ATH_MSG_INFO( "                              m_SS/ZZ/TTDRAT 2 "    << std::setw(20)<<std::setprecision(3) << (pMuPatSegment->mboyInfo)->m_SSDRAT2 
                                                             << " "  << std::setw(20)<<std::setprecision(3) << (pMuPatSegment->mboyInfo)->m_ZZDRAT2  
                                                             << " "  << std::setw(20)<<std::setprecision(3) << (pMuPatSegment->mboyInfo)->m_TTDRAT2 );

}

int MboyMuonTrackBuilderTester::ScoreCommonHits( 
                       const Muon::MuPatSegment* pMuPatSegment,
                       const Trk::Track*    pTrack
){

  if ( pMuPatSegment==0 ) return 0 ;
  const Muon::MuonSegment* pMuonSegment = pMuPatSegment->segment ;
  if ( pMuonSegment == 0 ) return 0 ;
  if ( pTrack==0 ) return 0 ;
  if ( pTrack->measurementsOnTrack() == 0 ) return 0 ;
  
  int ToBeReturned = 0 ;
  
  const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
  std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = pRIOSet.begin();
  for (; rotIter!=pRIOSet.end(); ++rotIter){
    if ( (*rotIter)!=0 ){
      std::vector<const Trk::MeasurementBase*>::const_iterator im    = pTrack->measurementsOnTrack()->begin(); 
      std::vector<const Trk::MeasurementBase*>::const_iterator imEnd = pTrack->measurementsOnTrack()->end(); 
      for ( ; im!=imEnd; ++im){
        const Trk::RIO_OnTrack* RotTrack = dynamic_cast<const Trk::RIO_OnTrack*>(*im);
        if ( RotTrack != 0 ){
          const Identifier& Id1 = (*rotIter)->identify() ;
          const Identifier& Id2 = RotTrack->identify() ;
          if ( Id1 == Id2 ) {
            ToBeReturned +=1 ;
            break ;
          }
        }
      }
    }
  }
  
  return ToBeReturned ;
    

}
void MboyMuonTrackBuilderTester::sort(
             std::vector<double>& VectorOfDist2IPD,
             std::vector<int>&    VectorOrdored
)
{
  VectorOrdored.clear();
  
  int    iNew = 1 ;
  
  int TotalNberOfElementsTOBESORTED = VectorOfDist2IPD.size();
  while ( iNew == 1 ){
    iNew = 0 ; 
    int    IndexMax    = 0   ;
    double Dist2IPDMax = -1. ; 
    for (int Index=0; Index<TotalNberOfElementsTOBESORTED; Index++){
      int IsExcluded = 0 ;
      int TotalNberOfElementsSorted = VectorOrdored.size(); 
      for (int iSorted=0; iSorted<TotalNberOfElementsSorted; iSorted++){
        if ( VectorOrdored[iSorted] == Index ) {
          IsExcluded = 1 ;
          break ;
        }
      }
      if ( IsExcluded == 0 ){
        iNew = 1 ;
        if ( VectorOfDist2IPD[Index] >= Dist2IPDMax ){
          Dist2IPDMax = VectorOfDist2IPD[Index] ;
          IndexMax    = Index                   ;
        }
      }
    }
    if ( iNew == 1 ){
      VectorOrdored.push_back(IndexMax);
    }
  }

}

//JFL: Taken from MuPatCandidateTool::createSegInfo Wed May  4 17:16:24 CEST 2011
 Muon::MuPatSegment* MboyMuonTrackBuilderTester::createSegInfo( const Muon::MuonSegment& segment ) const
  {
    Muon::MuPatSegment* info = new Muon::MuPatSegment();
    info->segment = &segment;
    info->chid = m_helperTool->chamberId(segment);
    info->chIndex = m_idHelperTool->chamberIndex(info->chid);
    std::set<Identifier> chIds = m_helperTool->chamberIds(segment);
    std::set<Identifier>::iterator chit = chIds.begin();
    std::set<Identifier>::iterator chit_end = chIds.end();
    for( ;chit!=chit_end;++chit ){
      Muon::MuonStationIndex::ChIndex chIdx = m_idHelperTool->chamberIndex(*chit);
//Protected Method    info->addChamber(chIdx);
      // set default name
      if ( !info->name.empty() ) info->name += "+";
      // stationname_eta_phi
      std::ostringstream oss;
      oss << Muon::MuonStationIndex::chName(chIdx)
          << "_" << m_idHelperTool->stationEta(*chit) << "_" << m_idHelperTool->stationPhi(*chit);
      info->name += oss.str();
    }
    info->stIndex = m_idHelperTool->stationIndex(info->chid);
    info->isEndcap = m_idHelperTool->isEndcap(info->chid);
    info->isMdt = m_idHelperTool->isMdt(info->chid);
    info->usedInFit = 0;
    info->quality = m_segmentSelector->quality( segment );
    info->segQuality = dynamic_cast<const Muon::MuonSegmentQuality*>(segment.fitQuality());

    info->segPars = m_helperTool->createTrackParameters( segment, 5000., 0. );    
    if( !info->segPars ) {
      ATH_MSG_WARNING(" failed to create track parameter for segment " );
    }

    if (p_IMuonSegmentInfoExtender) p_IMuonSegmentInfoExtender->extendInfo( info ); 

    return info;
  }
void MboyMuonTrackBuilderTester::CompareTrkTrack(
                        const Trk::Track* pTrack_Original ,
                        const Trk::Track* pTrack_Refined  
){

  ATH_MSG_INFO( "* CompareTrkTrack " );

  if ( pTrack_Original == 0 ) {
    ATH_MSG_INFO( "* pTrack_Original  == 0   " );
    return ;
  }
  if ( pTrack_Refined == 0 ) {
    ATH_MSG_INFO( "* pTrack_Refined == 0   " );
    return ;
  }

  const Trk::Perigee* pMeasuredPerigee_Original = 0 ;
  
  std::vector<const Trk::TrackParameters*>::const_iterator TrkPar    = pTrack_Original->trackParameters()->begin(); 
  std::vector<const Trk::TrackParameters*>::const_iterator TrkParEnd = pTrack_Original->trackParameters()->end(); 
  for ( ; TrkPar!=TrkParEnd; ++TrkPar){
    const Trk::Perigee* pPerigee = dynamic_cast<const Trk::Perigee*>(*TrkPar);
    if ( pPerigee ) {
      pMeasuredPerigee_Original = dynamic_cast<const Trk::Perigee*>(pPerigee);
      if ( pMeasuredPerigee_Original ) break; 
    }
  }
  if ( pMeasuredPerigee_Original == 0 ){
    ATH_MSG_INFO( "* pMeasuredPerigee_Original == 0   " );
    return ;
  }

  const Trk::Perigee* pMeasuredPerigee_Refined = 0 ;
  
  TrkPar    = pTrack_Refined->trackParameters()->begin(); 
  TrkParEnd = pTrack_Refined->trackParameters()->end(); 
  for ( ; TrkPar!=TrkParEnd; ++TrkPar){
    const Trk::Perigee* pPerigee = dynamic_cast<const Trk::Perigee*>(*TrkPar);
    if ( pPerigee ) {
      pMeasuredPerigee_Refined = dynamic_cast<const Trk::Perigee*>(pPerigee);
      if ( pMeasuredPerigee_Refined ) break; 
    }
  }
  if ( pMeasuredPerigee_Refined == 0 ){
    ATH_MSG_INFO( "* pMeasuredPerigee_Refined == 0   " );
    return ;
  }
  
  const AmgSymMatrix(5) TheCovarianceMatrix_Original =  *(pMeasuredPerigee_Original->covariance()) ;
  // const AmgSymMatrix(5) TheCovarianceMatrix_Refined  =  *(pMeasuredPerigee_Refined->covariance()) ;
   
  ATH_MSG_INFO( "*parameters()[Trk::d0]       " << std::setw(12)<<std::setprecision(5)<< ( pMeasuredPerigee_Refined->parameters()[Trk::d0]     - pMeasuredPerigee_Original->parameters()[Trk::d0]     ) / sqrt(TheCovarianceMatrix_Original(Trk::d0    , Trk::d0)    ) );
  ATH_MSG_INFO( " parameters()[Trk::z0]       " << std::setw(12)<<std::setprecision(5)<< ( pMeasuredPerigee_Refined->parameters()[Trk::z0]     - pMeasuredPerigee_Original->parameters()[Trk::z0]     ) / sqrt(TheCovarianceMatrix_Original(Trk::z0    , Trk::z0)    ) );  
  ATH_MSG_INFO( " parameters()[Trk::phi0]     " << std::setw(12)<<std::setprecision(5)<< ( pMeasuredPerigee_Refined->parameters()[Trk::phi0]   - pMeasuredPerigee_Original->parameters()[Trk::phi0]   ) / sqrt(TheCovarianceMatrix_Original(Trk::phi0  , Trk::phi0)  ) );  
  ATH_MSG_INFO( " parameters()[Trk::theta]    " << std::setw(12)<<std::setprecision(5)<< ( pMeasuredPerigee_Refined->parameters()[Trk::theta]  - pMeasuredPerigee_Original->parameters()[Trk::theta]  ) / sqrt(TheCovarianceMatrix_Original(Trk::theta , Trk::theta) ) );  
  ATH_MSG_INFO( " parameters()[Trk::qOverP]   " << std::setw(12)<<std::setprecision(5)<< ( pMeasuredPerigee_Refined->parameters()[Trk::qOverP] - pMeasuredPerigee_Original->parameters()[Trk::qOverP] ) / sqrt(TheCovarianceMatrix_Original(Trk::qOverP, Trk::qOverP)) );  

}
