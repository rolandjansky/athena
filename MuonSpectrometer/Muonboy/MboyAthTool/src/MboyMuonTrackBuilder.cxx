/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "MuonTrackFindingEvent/MuPatTrack.h"
#include "MuonTrackFindingEvent/MuPatCandidateBase.h"
#include "MuonTrackFindingEvent/MuPatSegment.h"
#include "MuonSegment/MuonSegment.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MboyMuonTrackBuilder.h"

#include "MboyCoreEvt/MboyVarByName.h"

#include "MboyCoreEvt/MbDRAT.h"
#include "MboyCoreEvt/MbProtoTrack.h"

#include "MboyAthEvt/MuonRecDigiStoreAth.h"

#include "MboyAthToolInterfaces/IFindMonitor.h"
#include "MboyAthToolInterfaces/IRefineMonitor.h"
#include "MboyEDMToolInterfaces/IMuonboyToTrackParametersTool.h"
#include "MboyEDMToolInterfaces/IMboyGetPrepDataFromEventTool.h"
#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"

namespace Muon {

//---------------------------------------------------------------------------------------------------------------//
MboyMuonTrackBuilder::MboyMuonTrackBuilder(const std::string& t, 
                                       const std::string& n,
                                       const IInterface*  p ):AthAlgTool(t,n,p),
 p_IFindMonitor ( "Muon::FindMonitor/FindMonitor" ),
 p_IRefineMonitor ( "Muon::RefineMonitor/RefineMonitor" ),
 p_IMuonboyToTrackParametersTool ( "Trk::MuonboyToTrackParametersTool/MuonboyToTrackParametersTool" ),
 p_IMboyGetPrepDataFromEventTool ( "MboyGetPrepDataFromEventTool" ) ,
 p_OwnEDMHelper                  ( "MboyAthToolHelper" ),
 p_IIncidentSvc("IncidentSvc",n)
{

  declareInterface<IMuonTrackBuilder>(this);
  declareInterface<IMuonSegmentInfoExtender>(this);
  declareInterface<IMuonTrackRefiner>(this);
  declareInterface<MboyMuonTrackBuilder>(this);
    
//Create a Variables carrier
  m_NberOfCtrlPpty = 0 ;
  p_MboyVarByName = 0 ;
  p_MuonRecDigiStoreAth = 0 ;
 
  m_EventShouldBeLoaded = 0 ;

//Define Proprieties
  declareControledProperty( "PrintLevel" , -1     , "MuonBoy print level for debug (from -1 to 6) ...................... " );
  declareControledProperty("TagadaTagada" , -100 );

  declareProperty("ToolName"          , m_ToolName          = n                           ) ;

  declareProperty("FindMonitor"  , p_IFindMonitor ) ;
  declareProperty("RefineMonitor"  , p_IRefineMonitor ) ;
  declareProperty("MuonboyToTrackParametersTool"  , p_IMuonboyToTrackParametersTool ) ;
  declareProperty("MboyGetPrepDataFromEventTool" , p_IMboyGetPrepDataFromEventTool ) ;
  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;

  declareProperty("DoDBg"          , m_DoDBg          = 0                           ) ;
  declareProperty("MonitorFind"     , m_MonitorFind          = 0                           ) ;
  declareProperty("MonitorRefine"   , m_MonitorRefine        = 0                           ) ;

}

MboyMuonTrackBuilder::~MboyMuonTrackBuilder(){
 delete p_MboyVarByName;
}

// Initialize
StatusCode MboyMuonTrackBuilder::initialize(){

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
  ATH_MSG_INFO( "= DoDBg                                  " << m_DoDBg                               ) ;
  ATH_MSG_INFO( "= MonitorFind                            " << m_MonitorFind                               ) ;
  ATH_MSG_INFO( "= MonitorRefine                          " << m_MonitorRefine                             ) ;
  PrintControledProperty() ;
  ATH_MSG_INFO( "================================" ) ;


//Retrieve p_IFindMonitor
  if ( p_IFindMonitor.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IFindMonitor );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IFindMonitor );


//Retrieve p_IRefineMonitor
  if ( p_IRefineMonitor.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IRefineMonitor );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IRefineMonitor );


//Retrieve p_IMuonboyToTrackParametersTool
  if ( p_IMuonboyToTrackParametersTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMuonboyToTrackParametersTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMuonboyToTrackParametersTool );


//Retrieve p_IMboyGetPrepDataFromEventTool
  if ( p_IMboyGetPrepDataFromEventTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyGetPrepDataFromEventTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyGetPrepDataFromEventTool );

//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper ) ;

// Retrieve p_IIncidentSvc
  if( p_IIncidentSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Could not get " << p_IIncidentSvc);
    return StatusCode::FAILURE;     
  }
  p_IIncidentSvc->addListener( this, std::string("BeginEvent") ) ;
  ATH_MSG_INFO( "Retrieved p_IIncidentSvc " << p_IIncidentSvc ) ;

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyMuonTrackBuilder::finalize(){return StatusCode::SUCCESS;}

//---------------------------------------------------------------------------------------------------------------//
// Controled proprieties

void MboyMuonTrackBuilder::SetOwnMboyVarByName()
{
  if (!p_MboyVarByName ) p_MboyVarByName = p_OwnEDMHelper->SetSettingsForCall(
                                                 m_ToolName          ,  
                                                 m_NberOfCtrlPpty    , 
                                                 m_NameOfCtrlPpty    , 
                                                 m_ValueOfCtrlPpty   , 
                                                 m_CommentOfCtrlPpty   
                                                ) ;
}
void MboyMuonTrackBuilder::declareControledProperty( std::string Name1, double Value, std::string Comment)
{
  m_NameOfCtrlPpty.push_back(Name1);
  m_ValueOfCtrlPpty[m_NberOfCtrlPpty] = Value ;
  m_CommentOfCtrlPpty.push_back(Comment);
  declareProperty(Name1, m_ValueOfCtrlPpty[m_NberOfCtrlPpty]);
  m_NberOfCtrlPpty = m_NberOfCtrlPpty + 1;
}
void MboyMuonTrackBuilder::PrintControledProperty()
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

void MboyMuonTrackBuilder::handle(const Incident& incident) {
  if (incident.type() != "BeginEvent" ) return ;
  m_EventShouldBeLoaded = 1 ;
  
  if ( m_MonitorFind   >= 1 ) p_IFindMonitor->Reset() ;
  if ( m_MonitorRefine >= 1 ) p_IRefineMonitor->Reset() ;

} 
//---------------------------------------------------------------------------------------------------------------//
std::vector<Muon::MuPatTrack*>* MboyMuonTrackBuilder::find( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree ) {

  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "======================================================== " );
  }
  
  if ( m_MonitorFind >= 1 ){
  
    if ( m_DoDBg == 1 ) {
      ATH_MSG_DEBUG( "FIND This input segments configuration is : " ) ;
      p_IFindMonitor->PrintSegConfig(candidate,MuPatSegmentFree);
    }

    bool AlreadySuccessfullyTried =  p_IFindMonitor->AlreadySuccessfullyTried(candidate,MuPatSegmentFree) ;
    if (AlreadySuccessfullyTried){
      ATH_MSG_DEBUG( " " ) ;
      ATH_MSG_DEBUG( "ALREADY SUCESSFULLY TRIED! " ) ;
      if ( m_MonitorFind >= 2 ){
        ATH_MSG_DEBUG( "Return null pointer on MuPatTrack vector ! " ) ;
        ATH_MSG_DEBUG( "    " ) ;
        return 0 ;
      }
    }else{
      ATH_MSG_DEBUG( "NOT ALREADY SUCESSFULLY TRIED! " ) ;
    }
    ATH_MSG_DEBUG( " " ) ;

  }

  std::vector<Muon::MuPatTrack*>* Output_findProtected = findProtected(candidate,MuPatSegmentFree);
  
  if ( m_MonitorFind >= 1 ) p_IFindMonitor->RegisterIfSuccessful(candidate,MuPatSegmentFree,Output_findProtected) ;
  
  return Output_findProtected ;
  
}

//---------------------------------------------------------------------------------------------------------------//
std::vector<Muon::MuPatTrack*>* MboyMuonTrackBuilder::findProtected( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree ) {

  SetOwnMboyVarByName();
  LoadEvent();
  
  if ( MuPatSegmentFree.size() == 0 ){
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "find FAILURE  " ) ;
    ATH_MSG_DEBUG( "Empty Set of Target Segments ! " ) ;
    ATH_MSG_DEBUG( "Return null pointer on MuPatTrack vector ! " ) ;
    ATH_MSG_DEBUG( "    " ) ;
    return 0 ;
  }
  
  Muon::MuPatSegment* pMuPatSegmentFix = dynamic_cast<Muon::MuPatSegment*> (&candidate);
  Muon::MuPatTrack*   pMuPatTrackFix   = dynamic_cast<Muon::MuPatTrack*>   (&candidate);
  if ( pMuPatSegmentFix ==0 && pMuPatTrackFix ==0 ) {
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "find FAILURE  " ) ;
    ATH_MSG_DEBUG( "Candidate can not be cast into  MuPatSegment/MuPatTrack    " ) ;
    ATH_MSG_DEBUG( "Return null pointer on MuPatTrack vector ! " ) ;
    ATH_MSG_DEBUG( "    " ) ;
    return 0 ;
  }
  
//One checks that the target MuonSegment associated is not already in the candidate
  std::vector<Muon::MuPatSegment*> MuPatSegmentFree_NEW ;
  if ( pMuPatTrackFix ) {
    std::vector< Muon::MuPatSegment* >::const_iterator it_cd      = (pMuPatTrackFix->segments()).begin() ;  
    std::vector< Muon::MuPatSegment* >::const_iterator it_cd_end  = (pMuPatTrackFix->segments()).end()   ;
    std::vector< Muon::MuPatSegment* >::const_iterator it_tg      = MuPatSegmentFree.begin() ;  
    std::vector< Muon::MuPatSegment* >::const_iterator it_tg_end  = MuPatSegmentFree.end()   ;
    for(; it_tg!=it_tg_end;++it_tg ){
      const Muon::MuonSegment* pMuonSegment_tg =  (*it_tg)->segment ;
      int IsSegmentFound = 0 ;
      for(; it_cd!=it_cd_end;++it_cd ){
        const Muon::MuonSegment* pMuonSegment_cd =  (*it_cd)->segment ;
        if ( pMuonSegment_tg == pMuonSegment_cd ){
          IsSegmentFound = 1 ;
          break ;
        }
      }
      if ( IsSegmentFound == 0 ) MuPatSegmentFree_NEW.push_back( (*it_tg) );
    }
    if ( MuPatSegmentFree_NEW.size() == 0 ){
      ATH_MSG_DEBUG( "    " ) ;
      ATH_MSG_DEBUG( "find FAILURE  " ) ;
      ATH_MSG_DEBUG( "All target segments found in candidate ! " ) ;
      ATH_MSG_DEBUG( "Return null pointer on MuPatTrack vector ! " ) ;
      ATH_MSG_DEBUG( "    " ) ;
      return 0 ;
    }
  }
  if (pMuPatSegmentFix) {
    const Muon::MuonSegment* pMuonSegment_cd =  pMuPatSegmentFix->segment ;
    std::vector< Muon::MuPatSegment* >::const_iterator it_tg      = MuPatSegmentFree.begin() ;  
    std::vector< Muon::MuPatSegment* >::const_iterator it_tg_end  = MuPatSegmentFree.end()   ;
    for(; it_tg!=it_tg_end;++it_tg ){
      const Muon::MuonSegment* pMuonSegment_tg =  (*it_tg)->segment ;
      if ( pMuonSegment_tg != pMuonSegment_cd ) MuPatSegmentFree_NEW.push_back( (*it_tg) ) ; 
    }
    if ( MuPatSegmentFree_NEW.size() == 0 ){
      ATH_MSG_DEBUG( "    " ) ;
      ATH_MSG_DEBUG( "find FAILURE  " ) ;
      ATH_MSG_DEBUG( "All target segments found in candidate ! " ) ;
      ATH_MSG_DEBUG( "Return null pointer on MuPatTrack vector ! " ) ;
      ATH_MSG_DEBUG( "    " ) ;
      return 0 ;
    }
  }
  
  
//In case the input is a MuPatTrack, if there is no mboyInfo one should check that initial kinematics can be constructed
  if (pMuPatTrackFix){
    if ( pMuPatTrackFix->mboyInfo == 0 ){
      if ( (pMuPatTrackFix->segments()).size() == 0 ){
        ATH_MSG_DEBUG( "    " ) ;
        ATH_MSG_DEBUG( "find FAILURE  " ) ;
        ATH_MSG_DEBUG( "The input candidatre is a MuPatTrack with no mboyInfo info " );
        ATH_MSG_DEBUG( "AND " );
        ATH_MSG_DEBUG( "the input MuPatTrack has no segment " );
        ATH_MSG_DEBUG( "Return null pointer on MuPatTrack vector ! " ) ;
        ATH_MSG_DEBUG( "    " ) ;
        return 0 ;
      }
    }
  }


//Extend Info
// if the MuPatSegment were already extendInfoed, there will be not changed by call to extendInfo 
  std::vector< Muon::MuPatSegment* >::const_iterator it     ; 
  std::vector< Muon::MuPatSegment* >::const_iterator it_end ;
  it     = MuPatSegmentFree.begin() ; 
  it_end = MuPatSegmentFree.end()   ;
  for(; it!=it_end;++it ){ extendInfo( (*it) ); }
  if (pMuPatSegmentFix) extendInfo(pMuPatSegmentFix);
  if (pMuPatTrackFix) { 
    Muon::MuPatSegment* pMuPatSegment_Seed = pMuPatTrackFix->seedSegment() ;
    if (pMuPatSegment_Seed) extendInfo(pMuPatSegment_Seed);
    it     = (pMuPatTrackFix->segments()).begin() ; 
    it_end = (pMuPatTrackFix->segments()).end()   ;
    for(; it!=it_end;++it ){ extendInfo( (*it) ); }
  }

//Dump
  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "find: dumps " );
    ATH_MSG_DEBUG( " " );
    if (pMuPatTrackFix)   {
      ATH_MSG_DEBUG( " " );
      ATH_MSG_DEBUG( "dump pMuPatTrackFix " );
      DumpMuPatTrack(pMuPatTrackFix);
    }
    if (pMuPatSegmentFix) {
      ATH_MSG_DEBUG( " " );
      ATH_MSG_DEBUG( "dump pMuPatSegmentFix " );
      DumpMuPatSegment(pMuPatSegmentFix);
    }
    ATH_MSG_DEBUG( " " );
    ATH_MSG_DEBUG( "dump MuPatSegmentFree_NEW " );
    it     = MuPatSegmentFree_NEW.begin() ; 
    it_end = MuPatSegmentFree_NEW.end()   ;
    for(; it!=it_end;++it ){ DumpMuPatSegment( (*it) ); }
  }
  
  std::vector<Muon::MuPatTrack*>* Output_findUnprotected = findUnprotected(candidate,MuPatSegmentFree_NEW);
  
  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "======================================================== " );
  }  
  
  return Output_findUnprotected ;
  
}

//---------------------------------------------------------------------------------------------------------------//
std::vector<Muon::MuPatTrack*>* MboyMuonTrackBuilder::findUnprotected( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree ) {
  
  Muon::MuPatSegment* pMuPatSegmentFix = dynamic_cast<Muon::MuPatSegment*> (&candidate);
  Muon::MuPatTrack*   pMuPatTrackFix   = dynamic_cast<Muon::MuPatTrack*>   (&candidate);

  std::vector< Muon::MuPatSegment* >::const_iterator it     ; 
  std::vector< Muon::MuPatSegment* >::const_iterator it_end ;

//Copy parameters  
  MbProtoTrack aMbProtoTrack ;
  int Iok = 0 ;
  if (pMuPatSegmentFix) p_OwnEDMHelper->CopyMuPatSegmentToMbProtoTrack ( 
                                                                        Iok, 
                                                                        pMuPatSegmentFix  , 
                                                                        &aMbProtoTrack 
                                                                       );
  if (pMuPatTrackFix  ) p_OwnEDMHelper->CopyMuPatTrackToMbProtoTrack   ( 
                                                                         Iok ,
                                                                         p_MboyVarByName , 
                                                                         p_MuonRecDigiStoreAth, 
                                                                         pMuPatTrackFix    , 
                                                                         &aMbProtoTrack 
                                                                       );
  if ( Iok == 0) {
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "find FAILURE  " ) ;
    ATH_MSG_DEBUG( "The copy of Input Fit parameters failed! " );
    ATH_MSG_DEBUG( "Return null pointer on MuPatTrack vector ! " ) ;
    ATH_MSG_DEBUG( "    " ) ;
    return 0 ;
  }
  
//Prepare stores for MuPatSegment  (VectorOfMuPatSegmentLocal will NOT onw the MuPatSegment) 
//and MbDRAT objects (VectorOfMbDRATLocal will onw the MbDRAT)
  int TotalNberOfObjects = 0 ;
   
//For free segments
  MbProtoTrack aTargetSegmentsProtoTrack ;
  (aTargetSegmentsProtoTrack.m_VectorOfMbDRAT).reserve( MuPatSegmentFree.size() ); 
  TotalNberOfObjects += MuPatSegmentFree.size() ;
  
//For fixed segments
  if (pMuPatSegmentFix) {
   (aMbProtoTrack.m_VectorOfMbDRAT).reserve( 1 );
    TotalNberOfObjects += 1 ;
  }else{
    (aMbProtoTrack.m_VectorOfMbDRAT).reserve( (pMuPatTrackFix->segments()).size() );
    TotalNberOfObjects += (pMuPatTrackFix->segments()).size() ;
  } 
  
//For All segments
  int kounter = -1 ;
  std::vector<Muon::MuPatSegment*> VectorOfMuPatSegmentLocal ;  VectorOfMuPatSegmentLocal.reserve( TotalNberOfObjects ); 
  std::vector<MbDRAT*>             VectorOfMbDRATLocal       ;  VectorOfMbDRATLocal.reserve( TotalNberOfObjects ); 
  
//Store "free" segments 
  it     = MuPatSegmentFree.begin() ; 
  it_end = MuPatSegmentFree.end()   ;
  for(; it!=it_end;++it ){
    Muon::MuPatSegment* pMuPatSegment = (*it) ; 
    kounter += 1 ;
    VectorOfMuPatSegmentLocal.push_back(pMuPatSegment);
    MbDRAT* pMbDRAT = p_OwnEDMHelper->GiveMuPatSegmentToMbDRAT(pMuPatSegment,kounter) ;
    if ( pMbDRAT) {
      VectorOfMbDRATLocal.push_back(  pMbDRAT ) ;
      (aTargetSegmentsProtoTrack.m_VectorOfMbDRAT).push_back(  pMbDRAT ) ;
    }
    
  }
  
//Store "fixed" segments  
  if (pMuPatSegmentFix) {
    Muon::MuPatSegment* pMuPatSegment = pMuPatSegmentFix ;
    kounter += 1 ;    
    VectorOfMuPatSegmentLocal.push_back(pMuPatSegment);
    MbDRAT* pMbDRAT = p_OwnEDMHelper->GiveMuPatSegmentToMbDRAT(pMuPatSegment,kounter) ;
    if ( pMbDRAT) {
      VectorOfMbDRATLocal.push_back(  pMbDRAT ) ;
      (aMbProtoTrack.m_VectorOfMbDRAT).push_back(  pMbDRAT ) ;
    }

  }else{
    it     = (pMuPatTrackFix->segments()).begin() ; 
    it_end = (pMuPatTrackFix->segments()).end()   ;
    for(; it!=it_end;++it ){ 
      Muon::MuPatSegment* pMuPatSegment = (*it) ;
      kounter += 1 ;
      VectorOfMuPatSegmentLocal.push_back(pMuPatSegment);
      MbDRAT* pMbDRAT = p_OwnEDMHelper->GiveMuPatSegmentToMbDRAT(pMuPatSegment,kounter) ;
      if ( pMbDRAT) {
        VectorOfMbDRATLocal.push_back(  pMbDRAT ) ;
        (aMbProtoTrack.m_VectorOfMbDRAT).push_back(  pMbDRAT ) ;
      }

    }
  }

//Find tracks
  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "  " ) ;
    ATH_MSG_DEBUG( " Call p_OwnEDMHelper->find " ) ;
    ATH_MSG_DEBUG( " size of segments aMbProtoTrack " << (aMbProtoTrack.m_VectorOfMbDRAT).size() ) ;
    ATH_MSG_DEBUG( " size of segments aTargetSegmentsProtoTrack " << (aTargetSegmentsProtoTrack.m_VectorOfMbDRAT).size() ) ;
  }
  MbProtoTrack* pMbProtoTrack = p_OwnEDMHelper->find( p_MboyVarByName , &aMbProtoTrack,  &aTargetSegmentsProtoTrack);
  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "  " ) ;
    ATH_MSG_DEBUG( " Call p_OwnEDMHelper->find DONE" ) ;
  }

//Failure case
  if ( pMbProtoTrack == 0 ) {
    ATH_MSG_DEBUG( " pMbProtoTrack == 0 " ) ;
    std::vector< MbDRAT* >::iterator itMbDRAT       = VectorOfMbDRATLocal.begin() ; 
    std::vector< MbDRAT* >::iterator itMbDRAT_end   = VectorOfMbDRATLocal.end()   ;
    for(; itMbDRAT!=itMbDRAT_end;++itMbDRAT ){ delete (*itMbDRAT) ; }
    VectorOfMbDRATLocal.clear();
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "find FAILURE  " ) ;
    ATH_MSG_DEBUG( "p_OwnEDMHelper->find failled!   " ) ;
    ATH_MSG_DEBUG( "Return null pointer on MuPatTrack vector ! " ) ;
    return 0 ;
  }
  
  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( " size of segments pMbProtoTrack " << (pMbProtoTrack->m_VectorOfMbDRAT).size() ) ;
  }
  
//Success case
  Muon::MuPatTrack* New_pMuPatTrack =  0 ;

//Store segments
  std::vector<Muon::MuPatSegment*> MuPatSegmentS_for_New_pMuPatTrack ;
  std::vector< MbDRAT* >::iterator itDRAT      = (pMbProtoTrack->m_VectorOfMbDRAT).begin() ;
  std::vector< MbDRAT* >::iterator itDRAT_end  = (pMbProtoTrack->m_VectorOfMbDRAT).end() ;
  for(; itDRAT!=itDRAT_end;++itDRAT ){
    int Thekounter = (*itDRAT)->m_kounter ;
    MuPatSegmentS_for_New_pMuPatTrack.push_back( VectorOfMuPatSegmentLocal[Thekounter] ) ;
  }
  if ( MuPatSegmentS_for_New_pMuPatTrack.size() ){
    Trk::Track* pTrack = p_OwnEDMHelper->MbProtoTrack2Track(pMbProtoTrack, MuPatSegmentS_for_New_pMuPatTrack) ;
    Muon::MuPatSegment* pMuPatSegment_Seed = 0 ;
    if (pMuPatSegmentFix) pMuPatSegment_Seed = pMuPatSegmentFix ;
    if (pMuPatTrackFix  ) pMuPatSegment_Seed = pMuPatTrackFix->seedSegment() ;
    New_pMuPatTrack = new Muon::MuPatTrack( MuPatSegmentS_for_New_pMuPatTrack , pTrack , pMuPatSegment_Seed ) ;
    p_OwnEDMHelper->UpdateMuPatTrackFromMbProtoTrack (pMbProtoTrack, New_pMuPatTrack);
  }

//Failure case
  if ( New_pMuPatTrack == 0 ){
    delete pMbProtoTrack ;
    std::vector< MbDRAT* >::iterator itMbDRAT       = VectorOfMbDRATLocal.begin() ; 
    std::vector< MbDRAT* >::iterator itMbDRAT_end   = VectorOfMbDRATLocal.end()   ;
    for(; itMbDRAT!=itMbDRAT_end;++itMbDRAT ){ delete (*itMbDRAT) ; }
    VectorOfMbDRATLocal.clear();
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "find FAILURE  " ) ;
    ATH_MSG_DEBUG( "New_pMuPatTrack == 0!   " ) ;
    ATH_MSG_DEBUG( "Return null pointer on MuPatTrack vector ! " ) ;
    ATH_MSG_DEBUG( "    " ) ;
    return 0 ;
  }
  
//Clean  
  delete pMbProtoTrack ;
  std::vector< MbDRAT* >::iterator itMbDRAT       = VectorOfMbDRATLocal.begin() ; 
  std::vector< MbDRAT* >::iterator itMbDRAT_end   = VectorOfMbDRATLocal.end()   ;
  for(; itMbDRAT!=itMbDRAT_end;++itMbDRAT ){ delete (*itMbDRAT) ; }
  VectorOfMbDRATLocal.clear();

//Fill  pVectorOfMuPatTrack
  std::vector<Muon::MuPatTrack*>* pVectorOfMuPatTrack = new std::vector<Muon::MuPatTrack*>() ;
  pVectorOfMuPatTrack->push_back(New_pMuPatTrack);

  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( " " );
    ATH_MSG_DEBUG( "dump pVectorOfMuPatTrack " );
    DumpMuPatTrackS(pVectorOfMuPatTrack);
    ATH_MSG_DEBUG( " " );
  }
  
  return pVectorOfMuPatTrack ;
  
}

void MboyMuonTrackBuilder::extendInfo( 
                              MuPatSegment * pMuPatSegment 
) 
{

 ATH_MSG_DEBUG( " extendInfo: In" );

 SetOwnMboyVarByName();
 LoadEvent();

 if ( pMuPatSegment == 0 ) {
   ATH_MSG_DEBUG( " extendInfo: pMuPatSegment == 0 " );
   return ;
 }
 
 ATH_MSG_DEBUG( " extendInfo: Before test mboyInfo " );
 if ( (pMuPatSegment->mboyInfo) != 0 ) {
   ATH_MSG_DEBUG( " extendInfo: pMuPatSegment->mboyInfo) != 0 " );
   if ( m_DoDBg == 1 )  DumpMuPatSegment(pMuPatSegment) ;
   return ;
 }
 ATH_MSG_DEBUG( " extendInfo: After  test mboyInfo " );

 MbDRAT* pMbDRAT = p_OwnEDMHelper->convertMuPatSegmentToMbDRAT( p_MuonRecDigiStoreAth, pMuPatSegment );
 if ( pMbDRAT == 0 ) return ;
 ATH_MSG_DEBUG( " extendInfo: After convertMuPatSegmentToMbDRAT  " );

 p_OwnEDMHelper->SetAMbDRAT( p_MboyVarByName , pMbDRAT );
 
 p_OwnEDMHelper->CopyMbDRATToMuPatSegment ( pMbDRAT , pMuPatSegment ) ;

 if ( m_DoDBg == 1 )  DumpMuPatSegment(pMuPatSegment) ;

 delete pMbDRAT ;

}

void MboyMuonTrackBuilder::LoadEvent() 
{

 SetOwnMboyVarByName();
 
 if ( m_EventShouldBeLoaded == 0 ) return ;

  ATH_MSG_DEBUG( "MboyMuonTrackBuilder::LoadEvent" );
  
//Collect PrepRawData
  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ;

  StatusCode sc=p_IMboyGetPrepDataFromEventTool->CollectPrepData(VectorOfpPrepData);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING( " CollectPrepData failed" );
  }
  
//Create MuonRecDigiStoreAth
  delete p_MuonRecDigiStoreAth ;
  p_MuonRecDigiStoreAth = new MuonRecDigiStoreAth;
  p_MuonRecDigiStoreAth->ResetMuonRecDigiStoreAth();


//Fill MuonRecDigiStoreAth 
  sc = p_OwnEDMHelper->FillMuonRecDigiStoreWithVectorOfPrepData_Base( VectorOfpPrepData, p_MuonRecDigiStoreAth );
  if ( sc.isFailure() ){
    ATH_MSG_WARNING( "FillMuonRecDigiStoreWithVectorOfPrepData_Base failed" );
  }
  
//p_MuonRecDigiStoreAth->PrintSummary();

//Load the Event
  p_OwnEDMHelper->LoadEvent( p_MboyVarByName ,p_MuonRecDigiStoreAth) ;

  m_EventShouldBeLoaded = 0 ;

}

void MboyMuonTrackBuilder::DumpMuPatTrack(const Muon::MuPatTrack* pMuPatTrack)
{

 if (m_DoDBg == 0 ) return ;
 
 if (pMuPatTrack==0) {
    ATH_MSG_DEBUG( "pMuPatTrack is 0 " );
   return ;
 }
 
 if ( (pMuPatTrack->mboyInfo) == 0 ){
   ATH_MSG_DEBUG( "(pMuPatTrack->mboyInfo) is 0 " );
 }else{
   ATH_MSG_DEBUG( "                  m_MboyStatus " << (pMuPatTrack->mboyInfo)->m_MboyStatus );
   ATH_MSG_DEBUG( "                  m_ISC0       " << (pMuPatTrack->mboyInfo)->m_ISC0 );
   ATH_MSG_DEBUG( "                  m_DZT        " << (pMuPatTrack->mboyInfo)->m_DZT1 << " " << (pMuPatTrack->mboyInfo)->m_DZT2 );
   ATH_MSG_DEBUG( "                  m_DS         " << (pMuPatTrack->mboyInfo)->m_DS1  << " " << (pMuPatTrack->mboyInfo)->m_DS2  );
   ATH_MSG_DEBUG( "                  m_SMU        " << (pMuPatTrack->mboyInfo)->m_SMU  );
   ATH_MSG_DEBUG( "                  m_CF         " << (pMuPatTrack->mboyInfo)->m_CFI  << " " << (pMuPatTrack->mboyInfo)->m_CFA  );
   ATH_MSG_DEBUG( "                  m_SF         " << (pMuPatTrack->mboyInfo)->m_SFI  << " " << (pMuPatTrack->mboyInfo)->m_SFA  );
   ATH_MSG_DEBUG( "                  m_Z          " << (pMuPatTrack->mboyInfo)->m_Z1   << " " << (pMuPatTrack->mboyInfo)->m_Z2   );
   ATH_MSG_DEBUG( "                  m_T          " << (pMuPatTrack->mboyInfo)->m_T1   << " " << (pMuPatTrack->mboyInfo)->m_T2   );
   ATH_MSG_DEBUG( "                  m_S          " << (pMuPatTrack->mboyInfo)->m_S1   << " " << (pMuPatTrack->mboyInfo)->m_S2   );
 }
 
 unsigned int NberOfSegments = (pMuPatTrack->segments()).size() ;
 if ( NberOfSegments == 0 ){
    ATH_MSG_DEBUG( "NberOfSegments is 0 " );
 }else{
   ATH_MSG_DEBUG( "* The nber of segments in  this track is " <<  NberOfSegments);
   int kounter = -1 ;
   for (unsigned int ItemNber=0; ItemNber<NberOfSegments ; ItemNber++){
     kounter += 1 ;
     ATH_MSG_DEBUG( "* segments " << kounter );
     DumpMuPatSegment( (pMuPatTrack->segments())[ItemNber] );
   }
 }
 
}
void MboyMuonTrackBuilder::DumpMuPatTrackS(const std::vector<Muon::MuPatTrack*>* pVectorOfMuPatTrack)
{
 if (m_DoDBg == 0 ) return ;
 
 if (pVectorOfMuPatTrack==0) {
    ATH_MSG_DEBUG( "pVectorOfMuPatTrack is 0 " );
   return ;
 }
 unsigned int NberOfTracks = pVectorOfMuPatTrack->size() ;
 if ( NberOfTracks == 0 ){
    ATH_MSG_DEBUG( "NberOfTracks is 0 " );
 }else{
   ATH_MSG_DEBUG( "* The nber of tracks in  pVectorOfMuPatTrack is " <<  NberOfTracks);
   int kounter = -1 ;
   std::vector<Muon::MuPatTrack*>::const_iterator it = pVectorOfMuPatTrack->begin();
   for (; it!=pVectorOfMuPatTrack->end(); ++it){
     kounter += 1 ;
     ATH_MSG_DEBUG( "* track " << kounter );
     DumpMuPatTrack( (*it) );
   }
 }
 
}
void MboyMuonTrackBuilder::DumpMuPatSegment(const Muon::MuPatSegment* pMuPatSegment)
{
 if (m_DoDBg == 0 ) return ;
 
 
  if (pMuPatSegment==0){
    ATH_MSG_DEBUG( "pMuPatSegment is 0 " );
    return ;
  }
  if (pMuPatSegment->mboyInfo==0){
    ATH_MSG_DEBUG( "pMuPatSegment->mboyInfo is 0 " );
    return ;
  }
  if (pMuPatSegment->segment==0){
    ATH_MSG_DEBUG( "pMuPatSegment->segment is 0 " );
    return ;
  }
  
  std::string SegmentName = p_OwnEDMHelper->SegmentAssociatedStationName( pMuPatSegment->segment ) ;

  const Amg::Vector3D TheGlobalPosition =  (pMuPatSegment->segment)->globalPosition() ;
  double Dist2IPD  = sqrt(
                         TheGlobalPosition.x() * TheGlobalPosition.x()
                       + TheGlobalPosition.y() * TheGlobalPosition.y()
                       + TheGlobalPosition.z() * TheGlobalPosition.z()
                         );
  
  ATH_MSG_DEBUG( "    " << SegmentName << "    " << Dist2IPD    );

  std::vector<std::string > VectorOfStationNames ;
  p_OwnEDMHelper->SegmentAssociatedStationNames( pMuPatSegment->segment ,VectorOfStationNames ) ;
  ATH_MSG_DEBUG( "    All stations in this segment: " );
  int VectorOfStationNamesSize = VectorOfStationNames.size();
  for (int ItemNber=0; ItemNber<VectorOfStationNamesSize ; ItemNber++){
    ATH_MSG_DEBUG( "    ->" << VectorOfStationNames[ItemNber]  );
  }
  
  ATH_MSG_DEBUG( "                              m_ISC0    " << (pMuPatSegment->mboyInfo)->m_ISC0    );
  ATH_MSG_DEBUG( "                              m_IWDRAT  " << (pMuPatSegment->mboyInfo)->m_IWDRAT1 << " " << (pMuPatSegment->mboyInfo)->m_IWDRAT2 );
  ATH_MSG_DEBUG( "                              m_JFDRAT  " << (pMuPatSegment->mboyInfo)->m_JFDRAT  );
  ATH_MSG_DEBUG( "                              m_FFDRAT   m_GGDRAT " << (pMuPatSegment->mboyInfo)->m_FFDRAT << " " << (pMuPatSegment->mboyInfo)->m_GGDRAT );
  ATH_MSG_DEBUG( "                              m_EZDRAT  " << (pMuPatSegment->mboyInfo)->m_EZDRAT  );
  ATH_MSG_DEBUG( "                              m_SSDRAT  " << (pMuPatSegment->mboyInfo)->m_SSDRAT1 << " " << (pMuPatSegment->mboyInfo)->m_SSDRAT2 );
  ATH_MSG_DEBUG( "                              m_ZZDRAT  " << (pMuPatSegment->mboyInfo)->m_ZZDRAT1 << " " << (pMuPatSegment->mboyInfo)->m_ZZDRAT2 );
  ATH_MSG_DEBUG( "                              m_TTDRAT  " << (pMuPatSegment->mboyInfo)->m_TTDRAT1 << " " << (pMuPatSegment->mboyInfo)->m_TTDRAT2 );

}

//---------------------------------------------------------------------------------------------------------------//
Muon::MuPatTrack* MboyMuonTrackBuilder::refine( Muon::MuPatTrack& MuPatTrackToBeRefined )
{

  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "======================================================== " );
  }
   
  if ( m_MonitorRefine >= 1 ){
  
    if ( m_DoDBg == 1 ) {
      ATH_MSG_DEBUG( "REFINE This input segments configuration is : " ) ;
      p_IRefineMonitor->PrintSegConfig(MuPatTrackToBeRefined);
    }

    bool AlreadySuccessfullyTried =  p_IRefineMonitor->AlreadySuccessfullyTried(MuPatTrackToBeRefined) ;
    if (AlreadySuccessfullyTried){
      ATH_MSG_DEBUG( " " ) ;
      ATH_MSG_DEBUG( "ALREADY SUCESSFULLY TRIED! " ) ;
      if ( m_MonitorRefine >= 2 ){
        ATH_MSG_DEBUG( "Return null pointer on MuPatTrack vector ! " ) ;
        ATH_MSG_DEBUG( "    " ) ;
        return 0 ;
      }
    }else{
      ATH_MSG_DEBUG( "NOT ALREADY SUCESSFULLY TRIED! " ) ;
    }
    ATH_MSG_DEBUG( " " ) ;

  }

  Muon::MuPatTrack*  Output_RefineProtected = refineProtected(MuPatTrackToBeRefined);
  
  if ( m_MonitorRefine >= 1 ) p_IRefineMonitor->RegisterIfSuccessful(MuPatTrackToBeRefined,Output_RefineProtected) ;
  
  return Output_RefineProtected ;
  
}
//---------------------------------------------------------------------------------------------------------------//
Muon::MuPatTrack* MboyMuonTrackBuilder::refineProtected( Muon::MuPatTrack& MuPatTrackToBeRefined )
{

  SetOwnMboyVarByName();
  LoadEvent();
   
//If there is no mboyInfo one should check that initial kinematics can be constructed
  if ( MuPatTrackToBeRefined.mboyInfo == 0 ){
    if ( (MuPatTrackToBeRefined.segments()).size() == 0 ){
      ATH_MSG_DEBUG( "    " ) ;
      ATH_MSG_DEBUG( "refine FAILURE  " ) ;
      ATH_MSG_DEBUG( "There is no mboyInfo info in the input MuPatTrack " );
      ATH_MSG_DEBUG( "AND " );
      ATH_MSG_DEBUG( "the input MuPatTrack has no segment " );
      ATH_MSG_DEBUG( "Return null MuPatTrack pointer ! " ) ;
      ATH_MSG_DEBUG( "    " ) ;
      return 0 ;
    }
  }

  Muon::MuPatTrack*   pMuPatTrackFix = &MuPatTrackToBeRefined ;
  
//Extend Info
// if the MuPatSegment were already extendInfoed, there will be not changed by call to extendInfo 
  std::vector< Muon::MuPatSegment* >::const_iterator it     ; 
  std::vector< Muon::MuPatSegment* >::const_iterator it_end ;
  Muon::MuPatSegment* pMuPatSegment_Seed = pMuPatTrackFix->seedSegment() ;
  if (pMuPatSegment_Seed) extendInfo(pMuPatSegment_Seed);
  it     = (pMuPatTrackFix->segments()).begin() ; 
  it_end = (pMuPatTrackFix->segments()).end()   ;
  for(; it!=it_end;++it ){ extendInfo( (*it) ); }

//Dump
  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "refine: dumps " );
    ATH_MSG_DEBUG( " " );
    ATH_MSG_DEBUG( " " );
    ATH_MSG_DEBUG( "dump pMuPatTrackFix " );
    DumpMuPatTrack(pMuPatTrackFix);
  }

//Copy parameters  
  MbProtoTrack aMbProtoTrack ;
  int Iok = 0 ;
  p_OwnEDMHelper->CopyMuPatTrackToMbProtoTrack   (
                                                  Iok,
                                                  p_MboyVarByName , 
                                                  p_MuonRecDigiStoreAth, 
                                                  pMuPatTrackFix    , 
                                                  &aMbProtoTrack 
                                                 );
  if ( Iok == 0) {
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "refine FAILURE  " ) ;
    ATH_MSG_DEBUG( "The copy of Input Fit parameters failed! " );
    ATH_MSG_DEBUG( "Return null MuPatTrack pointer ! " ) ;
    ATH_MSG_DEBUG( "    " ) ;
    return 0 ;
  }

//Prepare stores for MuPatSegment  (VectorOfMuPatSegmentLocal will NOT onw the MuPatSegment) 
//and MbDRAT objects (VectorOfMbDRATLocal will onw the MbDRAT)
  int TotalNberOfObjects = 0 ;

//For fixed segments
  (aMbProtoTrack.m_VectorOfMbDRAT).reserve( (pMuPatTrackFix->segments()).size() );
  TotalNberOfObjects += (pMuPatTrackFix->segments()).size() ;

//For All segments
  int kounter = -1 ;
  std::vector<Muon::MuPatSegment*> VectorOfMuPatSegmentLocal ;  VectorOfMuPatSegmentLocal.reserve( TotalNberOfObjects ); 
  std::vector<MbDRAT*>             VectorOfMbDRATLocal       ;  VectorOfMbDRATLocal.reserve( TotalNberOfObjects ); 

//Store "fixed" segments  
  it     = (pMuPatTrackFix->segments()).begin() ; 
  it_end = (pMuPatTrackFix->segments()).end()   ;
  for(; it!=it_end;++it ){ 
    Muon::MuPatSegment* pMuPatSegment = (*it) ;
    kounter += 1 ;    
    VectorOfMuPatSegmentLocal.push_back(pMuPatSegment);
    MbDRAT* pMbDRAT = p_OwnEDMHelper->GiveMuPatSegmentToMbDRAT(pMuPatSegment,kounter) ;
    if ( pMbDRAT) {
      VectorOfMbDRATLocal.push_back(  pMbDRAT ) ;
      (aMbProtoTrack.m_VectorOfMbDRAT).push_back(  pMbDRAT ) ;
    }

  }

//Refine Track
  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "  " ) ;
    ATH_MSG_DEBUG( "Call p_OwnEDMHelper->refine" ) ;
    ATH_MSG_DEBUG( " size of segments aMbProtoTrack " << (aMbProtoTrack.m_VectorOfMbDRAT).size() ) ;
  }
  Trk::Track* pTrackRefined = 0 ;
  MbProtoTrack* pMbProtoTrack = p_OwnEDMHelper->refine( p_MboyVarByName , &aMbProtoTrack, pTrackRefined, p_MuonRecDigiStoreAth );
  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "  " ) ;
    ATH_MSG_DEBUG( "Call p_OwnEDMHelper->refine DONE" ) ;
  }

//Failure case
  if ( pMbProtoTrack == 0 ) {
    ATH_MSG_DEBUG( " pMbProtoTrack == 0 " ) ;
    std::vector< MbDRAT* >::iterator itMbDRAT       = VectorOfMbDRATLocal.begin() ; 
    std::vector< MbDRAT* >::iterator itMbDRAT_end   = VectorOfMbDRATLocal.end()   ;
    for(; itMbDRAT!=itMbDRAT_end;++itMbDRAT ){ delete (*itMbDRAT) ; }
    VectorOfMbDRATLocal.clear();
    delete pTrackRefined;
  }
  if ( pTrackRefined == 0 ) {
    ATH_MSG_DEBUG( " pTrackRefined == 0 " ) ;
    std::vector< MbDRAT* >::iterator itMbDRAT       = VectorOfMbDRATLocal.begin() ; 
    std::vector< MbDRAT* >::iterator itMbDRAT_end   = VectorOfMbDRATLocal.end()   ;
    for(; itMbDRAT!=itMbDRAT_end;++itMbDRAT ){ delete (*itMbDRAT) ; }
    VectorOfMbDRATLocal.clear();
    delete pMbProtoTrack;
  }
  if ( pMbProtoTrack == 0 || pTrackRefined == 0 ) {
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "refine FAILURE  " ) ;
    ATH_MSG_DEBUG( "p_OwnEDMHelper->refine failled!   " ) ;
    ATH_MSG_DEBUG( "Return null MuPatTrack pointer ! " ) ;
    ATH_MSG_DEBUG( "    " ) ;
    return 0 ;
  }
 
//Success case
  Muon::MuPatTrack* New_pMuPatTrack =  0 ;

//Store segments
  std::vector<MuPatSegment*> MuPatSegmentS_for_New_pMuPatTrack ;
  std::vector< MbDRAT* >::iterator itDRAT      = (pMbProtoTrack->m_VectorOfMbDRAT).begin() ;
  std::vector< MbDRAT* >::iterator itDRAT_end  = (pMbProtoTrack->m_VectorOfMbDRAT).end() ;
  for(; itDRAT!=itDRAT_end;++itDRAT ){
    int Thekounter = (*itDRAT)->m_kounter ;
    MuPatSegmentS_for_New_pMuPatTrack.push_back( VectorOfMuPatSegmentLocal[Thekounter] ) ;
  }
  if ( MuPatSegmentS_for_New_pMuPatTrack.size() ){
    New_pMuPatTrack = new Muon::MuPatTrack( MuPatSegmentS_for_New_pMuPatTrack , pTrackRefined , pMuPatSegment_Seed ) ;
    p_OwnEDMHelper->UpdateMuPatTrackFromMbProtoTrack (pMbProtoTrack, New_pMuPatTrack);
  }

//Failure case
  if ( New_pMuPatTrack == 0 ){
    delete pMbProtoTrack ;
    std::vector< MbDRAT* >::iterator itMbDRAT       = VectorOfMbDRATLocal.begin() ; 
    std::vector< MbDRAT* >::iterator itMbDRAT_end   = VectorOfMbDRATLocal.end()   ;
    for(; itMbDRAT!=itMbDRAT_end;++itMbDRAT ){ delete (*itMbDRAT) ; }
    VectorOfMbDRATLocal.clear();
    delete pTrackRefined;
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "refine FAILURE  " ) ;
    ATH_MSG_DEBUG( "New_pMuPatTrack == 0!   " ) ;
    ATH_MSG_DEBUG( "Return null MuPatTrack pointer ! " ) ;
    ATH_MSG_DEBUG( "    " ) ;
    return 0 ;
  }

//Clean  
  delete pMbProtoTrack ;
  std::vector< MbDRAT* >::iterator itMbDRAT       = VectorOfMbDRATLocal.begin() ; 
  std::vector< MbDRAT* >::iterator itMbDRAT_end   = VectorOfMbDRATLocal.end()   ;
  for(; itMbDRAT!=itMbDRAT_end;++itMbDRAT ){ delete (*itMbDRAT) ; }
  VectorOfMbDRATLocal.clear();
  
  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( " " );
    ATH_MSG_DEBUG( "dump  New_pMuPatTrack" );
    DumpMuPatTrack(New_pMuPatTrack);
    ATH_MSG_DEBUG( " " );
  }
  
  if ( m_DoDBg == 1 ){
    ATH_MSG_DEBUG( "======================================================== " );
  }
   
  return New_pMuPatTrack ;
  
}

Muon::MuPatTrack* MboyMuonTrackBuilder::DoMuPaTrackFromMuPatSegment(
                                                 Muon::MuPatSegment* pMuPatSegment 
){

  SetOwnMboyVarByName();
  LoadEvent();
   
   
  const Muon::MuonSegment* pMuonSegment1  = pMuPatSegment->segment ;
  if ( pMuonSegment1 == 0 ){
    ATH_MSG_INFO( "DoMuPaTrackFromMuPatSegment: No MuonSegment from pMuPatSegment !! " );
    return 0 ;
  }

// Take a intermediate MbDRAT for the Seed
  MbDRAT* pMbDRAT = p_OwnEDMHelper->convertMuPatSegmentToMbDRAT(p_MuonRecDigiStoreAth , pMuPatSegment );
  if ( pMbDRAT == 0 ){
    ATH_MSG_INFO( "DoMuPaTrackFromMuPatSegment: pMbDRAT is null !! " );
    return 0 ;
  }
  pMbDRAT->m_Mome   = 100000000. ;
  pMbDRAT->m_Charge = 1.         ;
 
  DataVector<const Trk::TrackStateOnSurface>* pTrackStateOnSurface_DataVector =  new DataVector<const Trk::TrackStateOnSurface> ;

  const Muon::MuonSegment* pMuonSegment   = (pMuPatSegment->segment)->clone() ;

//Compute the TrackParameters
  const Trk::TrackParameters* pTrackParameters = 0 ;
  p_IMuonboyToTrackParametersTool->DoTheTrackParametersCorrectedSurface(
                                                                        &(pMuonSegment->associatedSurface()),
                                                                        pMbDRAT->m_Xpoint ,
                                                                        pMbDRAT->m_Ypoint ,
                                                                        pMbDRAT->m_Zpoint ,
                                                                        pMbDRAT->m_Xdir   ,
                                                                        pMbDRAT->m_Ydir   ,
                                                                        pMbDRAT->m_Zdir   ,
                                                                        pMbDRAT->m_Mome   ,
                                                                        pMbDRAT->m_Charge ,
                                                                        pTrackParameters
                                                                       );
  const Trk::FitQualityOnSurface* pFitQualityOnSurface = 0;

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
  pattern.set(Trk::TrackStateOnSurface::Measurement);

  Trk::TrackStateOnSurface* pTrackStateOnSurface = new  Trk::TrackStateOnSurface( pMuonSegment ,pTrackParameters, pFitQualityOnSurface,  0 , pattern) ;
  pTrackStateOnSurface_DataVector->push_back(pTrackStateOnSurface);

  Trk::Track* pTrack = 0;
  
  Trk::TrackInfo aTrackInfo(Trk::TrackInfo::Unknown, Trk::muon) ;
  aTrackInfo.setPatternRecognitionInfo( Trk::TrackInfo::Muonboy);
  aTrackInfo.setTrackFitter(Trk::TrackInfo::MuonboyFitter);
  
  const Trk::FitQuality* fitQuality = 0 ;
 
  pTrack = new Trk::Track(aTrackInfo, pTrackStateOnSurface_DataVector, fitQuality);

  Muon::MuPatTrack* pMuPatTrack = new Muon::MuPatTrack( pMuPatSegment , pTrack ) ;
  
// Delete intermediate MbDRAT
  delete pMbDRAT ;

  return pMuPatTrack;

}
 
}
