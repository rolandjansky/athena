/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkParameters/TrackParameters.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MuonBackTracker.h"

#include "MboyCoreEvt/MboyVarByName.h"

#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"
#include "MboyEDMToolInterfaces/IMboyProperTrackRepresentationTransformTool.h"
#include "MboyEDMToolInterfaces/IMboyTrackUndressingTool.h"
#include "MboyEDMToolInterfaces/IMboyDetectorLimitsTool.h"
#include "MboyEDMToolInterfaces/IMboyTrackRepresentationTransformTool.h"
#include "MboyEDMToolInterfaces/IMboyProperTrackRepresentationTransformTool.h"

#include "MboyEDMToolInterfaces/IMuonboyToTrackTool.h"
#include "MboyAthEvt/MB_TrackAth.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

namespace Muon {

//---------------------------------------------------------------------------------------------------------------//
MuonBackTracker::MuonBackTracker(const std::string& t, 
                                       const std::string& n,
                                       const IInterface*  p ):AthAlgTool(t,n,p)
, p_OwnEDMHelper                  ( "MboyAthToolHelper" )
, p_IMboyProperTrackRepresentationTransformTool                 ( "Muon::MboyProperTrackRepresentationTransformTool/MboyProperTrackRepresentationTransformTool" ) 
, p_IMboyTrackUndressingTool         ( "Muon::MboyTrackUndressingTool/MboyTrackUndressingTool" ) 
, p_IMboyDetectorLimitsTool          ( "Muon::MboyDetectorLimitsTool/MboyDetectorLimitsTool" )
, p_IMboyTrackRepresentationTransformTool          ( "Muon::MboyTrackRepresentationTransformTool/MboyTrackRepresentationTransformTool" )
, p_IMuonboyToTrackTool     ( "Trk::MuonboyToTrackTool/MuonboyToTrackTool"                )
{

  declareInterface<IMuonBackTracker>(this);
    
//Create a Variables carrier
  m_NberOfCtrlPpty = 0 ;
  p_MboyVarByName = 0 ;

//Define Proprieties
  declareControledProperty( "PrintLevel" , -1     , "MuonBoy print level for debug (from -1 to 6) ...................... " );
  declareControledProperty("TagadaTagada" , -100 );

  declareProperty("ToolName"          , m_ToolName          = n                           ) ;

  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;
  declareProperty("IMboyProperTrackRepresentationTransformTool"            , p_IMboyProperTrackRepresentationTransformTool ) ;
  declareProperty("IMboyTrackUndressingTool"    , p_IMboyTrackUndressingTool ) ;
  declareProperty("IMboyDetectorLimitsTool"    , p_IMboyDetectorLimitsTool ) ;
  declareProperty("IMboyTrackRepresentationTransformTool"    , p_IMboyTrackRepresentationTransformTool ) ;
  declareProperty("IMuonboyToTrackTool"     , p_IMuonboyToTrackTool     ) ;

}

MuonBackTracker::~MuonBackTracker(){
 delete p_MboyVarByName;
}

// Initialize
StatusCode MuonBackTracker::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are     " ) ;
  ATH_MSG_INFO( "= ToolName                               " << m_ToolName                               ) ;
  PrintControledProperty() ;
  ATH_MSG_INFO( "================================" ) ;


//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper ) ;

//Retrieve p_IMboyProperTrackRepresentationTransformTool
  if ( p_IMboyProperTrackRepresentationTransformTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyProperTrackRepresentationTransformTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyProperTrackRepresentationTransformTool );

//Retrieve p_IMboyTrackUndressingTool
  if ( p_IMboyTrackUndressingTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyTrackUndressingTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyTrackUndressingTool );

//Retrieve p_IMboyDetectorLimitsTool
  if ( p_IMboyDetectorLimitsTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyDetectorLimitsTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyDetectorLimitsTool );

//Retrieve p_IMboyTrackRepresentationTransformTool
  if ( p_IMboyTrackRepresentationTransformTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyTrackRepresentationTransformTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyTrackRepresentationTransformTool );

//Retrieve p_IMuonboyToTrackTool
  if ( p_IMuonboyToTrackTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToTrackTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToTrackTool );


  return StatusCode::SUCCESS;

}

StatusCode MuonBackTracker::finalize(){return StatusCode::SUCCESS;}

//---------------------------------------------------------------------------------------------------------------//
// Controled proprieties

void MuonBackTracker::SetOwnMboyVarByName()
{
  if (!p_MboyVarByName ) p_MboyVarByName = p_OwnEDMHelper->SetSettingsForCall(
                                                 m_ToolName          ,  
                                                 m_NberOfCtrlPpty    , 
                                                 m_NameOfCtrlPpty    , 
                                                 m_ValueOfCtrlPpty   , 
                                                 m_CommentOfCtrlPpty   
                                                ) ;
}
void MuonBackTracker::declareControledProperty( std::string Name1, double Value, std::string Comment)
{
  m_NameOfCtrlPpty.push_back(Name1);
  m_ValueOfCtrlPpty[m_NberOfCtrlPpty] = Value ;
  m_CommentOfCtrlPpty.push_back(Comment);
  declareProperty(Name1, m_ValueOfCtrlPpty[m_NberOfCtrlPpty]);
  m_NberOfCtrlPpty = m_NberOfCtrlPpty + 1;
}
void MuonBackTracker::PrintControledProperty()
{
  int itemMax = m_NberOfCtrlPpty;
  for (int item = 0 ; item < itemMax ; item++){    
    std::string Name1   = m_NameOfCtrlPpty[item];
    double      Value   = m_ValueOfCtrlPpty[item] ;
    std::string Comment = m_CommentOfCtrlPpty[item];
    Name1.resize(30,' ');
    Comment.resize(70,' ');
    ATH_MSG_INFO( "= " << Name1  << " " << Comment << " " << Value ) ;
  }
}

//---------------------------------------------------------------------------------------------------------------//
Trk::Track*   MuonBackTracker::MuonBackTrack(const Trk::Track*   pTrack  ){

  SetOwnMboyVarByName();

  if ( pTrack == 0 ) {
    ATH_MSG_DEBUG( " MuonBackTrack returns 0 since pTrack is 0 ") ;
    return 0 ;
  }else{
    ATH_MSG_DEBUG( " MuonBackTrack OK pTrack is NOT 0 ") ;
  }
  
  bool isSL = (pTrack->info()).trackProperties(Trk::TrackInfo::StraightTrack);
  if ( isSL )  {
    ATH_MSG_DEBUG( " MuonBackTrack returns 0 since isSL is NOT 0 ") ;
    return 0 ;
  }else{
    ATH_MSG_DEBUG( " MuonBackTrack OK since isSL is 0 ") ;
  }

  DataVector<const Trk::TrackStateOnSurface>* trackStates = 
    p_IMboyTrackUndressingTool->UndressTrackTSOSs(pTrack);
  if ( trackStates == 0 ) {
    ATH_MSG_DEBUG( " MuonBackTrack returns 0 since trackStates is 0 ") ;
    return 0 ;
  }else{
    ATH_MSG_DEBUG( " MuonBackTrack OK since trackStates is NOT 0 ") ;
  }

  const Trk::Perigee* pMeasuredPerigee = 0 ;
  const Trk::TrackStateOnSurface* pTrackStateOnSurfaceOfpMeasuredPerigee = 0 ;
  for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoS = trackStates->begin(); itTSoS!=trackStates->end(); ++itTSoS)
  {
    if ( (*itTSoS)->trackParameters() != 0 ){
      const Trk::TrackParameters* pTrackParameters = (*itTSoS)->trackParameters() ;
      int TheAreaTag = p_IMboyDetectorLimitsTool->AreaTag( pTrackParameters );
      if ( TheAreaTag >= 3){
        pMeasuredPerigee  = dynamic_cast<const Trk::Perigee*>(pTrackParameters);
        if (pMeasuredPerigee) {
          pTrackStateOnSurfaceOfpMeasuredPerigee = (*itTSoS) ;
          break  ;
        }
      }
    } 
  }
  
  if ( pMeasuredPerigee == 0 ) {
    ATH_MSG_DEBUG( " MuonBackTrack returns 0 since pMeasuredPerigee is 0 ") ;
    delete trackStates ;
    return 0 ;
  }else{
    ATH_MSG_DEBUG( " MuonBackTrack OK since pMeasuredPerigee is NOT 0 ") ;
  }

  const Trk::TrackParameters* pTrackParameters_AtACOrAtAD = 
    p_IMboyTrackRepresentationTransformTool->MeasuredPerigee2AtACOrAtAD(pMeasuredPerigee) ;

  if ( pTrackParameters_AtACOrAtAD == 0 ) {
    ATH_MSG_DEBUG( " MuonBackTrack returns 0 since pTrackParameters_AtACOrAtAD is 0 ") ;
    delete trackStates ;
    return 0 ;
  }else{
    ATH_MSG_DEBUG( " MuonBackTrack OK since pTrackParameters_AtACOrAtAD is NOT 0 ") ;
  }

  MB_Track_SpecEntr* pMB_Track_SpecEntr  = 
    p_IMboyProperTrackRepresentationTransformTool->AtaCylinderOrAtaDiscToTrkParMboy(pTrackParameters_AtACOrAtAD) ;

  if ( pMB_Track_SpecEntr == 0 ) {
    ATH_MSG_DEBUG( " MuonBackTrack returns 0 since pMB_Track_SpecEntr is 0 ") ;
    delete trackStates ;
    delete pTrackParameters_AtACOrAtAD ;
    return 0 ;
  }else{
    ATH_MSG_DEBUG( " MuonBackTrack OK since pMB_Track_SpecEntr is NOT 0 ") ;
  }

  int reverse = 0 ;if ( (pTrack->info()).patternRecoInfo(Trk::TrackInfo::ReverseOrderedTrack) ) reverse = 1 ;  pMB_Track_SpecEntr->SetIsToBeReversed(reverse) ;
  int strLine = 0 ;if ( (pTrack->info()).trackProperties(Trk::TrackInfo::StraightTrack)       ) strLine = 1 ;  pMB_Track_SpecEntr->SetIsStraightLine(strLine) ;

//Create a MB_Track_CaloEntr
  MB_Track_CaloEntr aMB_Track_CaloEntr ;
//Create a MB_Track_Perigee
  MB_Track_Perigee aMB_Track_Perigee ;

  double QualityFactor = (pTrack->fitQuality())->chiSquared ();
  int    NberOfDOF     = (pTrack->fitQuality())->numberDoF  ();  
  double Chi2Typ0      = (pTrack->fitQuality())->chiSquared ();
  double Chi2Typ1      = (pTrack->fitQuality())->chiSquared ();
  double Chi2Typ2      = (pTrack->fitQuality())->chiSquared ();
  
//Collect indexes of digis
  MbIOObjectStore    aMbIOObjectStore ;
  int FakeRunNber = 9999 ; aMbIOObjectStore.SetRunNber  ( FakeRunNber );
  int FakeEvtNber = 9999 ; aMbIOObjectStore.SetEventNber( FakeEvtNber );

//Create a MB_Track and store it in the store
  MB_Track aMB_Track(
   *pMB_Track_SpecEntr,aMB_Track_CaloEntr,aMB_Track_Perigee,
   QualityFactor,NberOfDOF,
   Chi2Typ0,Chi2Typ1,Chi2Typ2,
   aMbIOObjectStore
  );

//BackTrack
  MB_Track* pMB_Track_BackTracked =  MboyMuonBackTrack(&aMB_Track);

  if ( pMB_Track_BackTracked == 0 ){
    ATH_MSG_DEBUG( " MuonBackTrack returns 0 since pMB_Track_BackTracked is 0 ") ;
    delete pMB_Track_SpecEntr ;
    delete trackStates ;
    delete pTrackParameters_AtACOrAtAD ;
    return 0 ;
  }else{
    ATH_MSG_DEBUG( " MuonBackTrack OK since pMB_Track_BackTracked is NOT 0 ") ;
  }

  if ( pMB_Track_BackTracked->IsValidVT() != 1 ){
    ATH_MSG_DEBUG( " MuonBackTrack returns 0 since IsValidVT is not 1 ") ;
    delete pMB_Track_BackTracked ;
    delete pMB_Track_SpecEntr ;
    delete trackStates ;
    delete pTrackParameters_AtACOrAtAD ;
    return 0 ;
  }else{
    ATH_MSG_DEBUG( " MuonBackTrack OK since IsValidVT is 1 ") ;
  }

  MuonRecDigiStoreAth aMuonRecDigiStoreAth ;
  MB_TrackAth aMB_TrackAth(*pMB_Track_BackTracked,&aMuonRecDigiStoreAth);
  MB_TrackAth* pMB_TrackAth_BackTracked = &aMB_TrackAth ;

  int AddCaloEntranceToMain       = 1 ;
  int AddMuonSystemEntranceToMain = 0 ;
  DataVector<const Trk::TrackStateOnSurface>* pTrackStateOnSurfaceS = 
       p_IMuonboyToTrackTool->GiveAllTosForFullTrack(pMB_TrackAth_BackTracked,AddCaloEntranceToMain,AddMuonSystemEntranceToMain) ;

  if ( pTrackStateOnSurfaceS == 0) {
    ATH_MSG_DEBUG( " MuonBackTrack returns 0 since pTrackStateOnSurfaceS is 0 ") ;
    delete pMB_Track_BackTracked ;
    delete pMB_Track_SpecEntr ;
    delete trackStates ;
    delete pTrackParameters_AtACOrAtAD ;
    return 0 ;
  }else{
    ATH_MSG_DEBUG( " MuonBackTrack OK since pTrackStateOnSurfaceS is NOT 0 ") ;
  }

  for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoS = trackStates->begin(); itTSoS!=trackStates->end(); ++itTSoS)
  {
   if ( (*itTSoS) != pTrackStateOnSurfaceOfpMeasuredPerigee ) pTrackStateOnSurfaceS->push_back( (*itTSoS)->clone() );
  }
  
  Trk::Track* pTrack_BackTracked = new Trk::Track(   
                        Trk::TrackInfo(pTrack->info()),
                        pTrackStateOnSurfaceS,
                        (pTrack->fitQuality())->clone()
                       );

  if ( pTrack_BackTracked == 0) {
    ATH_MSG_DEBUG( " MuonBackTrack ARGGGH since pTrack_BackTracked is 0 ") ;
  }else{
    ATH_MSG_DEBUG( " MuonBackTrack OK since pTrack_BackTracked is NOT 0 ") ;
  }
  
  delete trackStates ;
  delete pMB_Track_BackTracked ;
  delete pMB_Track_SpecEntr ;
  delete pTrackParameters_AtACOrAtAD ;
  return pTrack_BackTracked ;

}

MB_Track* MuonBackTracker::MboyMuonBackTrack(MB_Track* pMB_Track )
{
  return p_OwnEDMHelper->MboyMuonBackTrack(p_MboyVarByName,pMB_Track); 
}


//---------------------------------------------------------------------------------------------------------------//
}
