/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"

#include "TrkParameters/TrackParameters.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewCombinedMuonContainerTool.h"

static const InterfaceID IID_IMboyViewCombinedMuonContainerTool("MboyViewCombinedMuonContainerTool", 1, 0);

const InterfaceID& MboyViewCombinedMuonContainerTool::interfaceID()
{
  return IID_IMboyViewCombinedMuonContainerTool;
}

MboyViewCombinedMuonContainerTool::MboyViewCombinedMuonContainerTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
, p_MboyViewPointerDumperTool        ( "MboyViewPointerDumperTool/MboyViewPointerDumperTool"              ) 
, p_MboyViewTrTrackDumpTool        ( "MboyViewTrTrackDumpTool/MboyViewTrTrackDumpTool"              ) 
, p_MboyViewTSOSDumpTool        ( "MboyViewTSOSDumpTool/MboyViewTSOSDumpTool"              ) 
{

  declareInterface<MboyViewCombinedMuonContainerTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

  declareProperty("DoCom"        , m_DoCom  = 0    ) ; 
  declareProperty("DoTSO"        , m_DoTSO  = 1    ) ; 

  declareProperty("MboyViewPointerDumperTool"        , p_MboyViewPointerDumperTool        ) ; 
  declareProperty("MboyViewTrTrackDumpTool"          , p_MboyViewTrTrackDumpTool          ) ; 
  declareProperty("MboyViewTSOSDumpTool"          , p_MboyViewTSOSDumpTool          ) ; 

}

MboyViewCombinedMuonContainerTool::~MboyViewCombinedMuonContainerTool(){}

// Initialize
StatusCode MboyViewCombinedMuonContainerTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  ATH_MSG_INFO( " DoCom  :     " <<  m_DoCom ) ;
  ATH_MSG_INFO( " DoTSO  :     " <<  m_DoTSO ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

//Retrieve p_MboyViewPointerDumperTool
  if ( p_MboyViewPointerDumperTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyViewPointerDumperTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyViewPointerDumperTool ) ;

//Retrieve p_MboyViewTrTrackDumpTool
  if ( p_MboyViewTrTrackDumpTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyViewTrTrackDumpTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyViewTrTrackDumpTool ) ;

//Retrieve p_MboyViewTSOSDumpTool
  if ( p_MboyViewTSOSDumpTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyViewTSOSDumpTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyViewTSOSDumpTool ) ;

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewCombinedMuonContainerTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewCombinedMuonContainerTool::ViewCombinedMuonContainer(
               std::string CombinedMuonContainerLocation,
               std::ofstream* pOut
){

//Access
  if (!(evtStore()->contains<Analysis::MuonContainer>(CombinedMuonContainerLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << CombinedMuonContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  StatusCode sc = StatusCode::SUCCESS;

  const Analysis::MuonContainer* pContainerTracks;
  sc = evtStore()->retrieve(pContainerTracks, CombinedMuonContainerLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_INFO( " pContainerTracks not found at existing " << CombinedMuonContainerLocation ) ;
    return( StatusCode::SUCCESS );
  }

//Dump
  if ( m_DoCom == 1 ) *pOut << "COM" << " "
        << "CombinedMuon" << " "
        << "=============================================" << " "
        << CombinedMuonContainerLocation << " "
        << pContainerTracks->size()
        << std::endl;

  for (Analysis::MuonContainer::const_iterator it = pContainerTracks->begin(); it!=pContainerTracks->end(); ++it){
    const Rec::TrackParticle* TheTp = 0 ;
    if ( (*it)->hasCombinedMuonTrackParticle() ){
      TheTp = (*it)->combinedMuonTrackParticle() ;
      if ( m_DoCom == 1 ) *pOut << "COM*" << " "
            << "CombinedMuon" << " "
            << "TP is combinedMuonTrackParticle "  
            << std::endl;
    }else{
      if ( (*it)->hasMuonExtrapolatedTrackParticle() ){
        TheTp = (*it)->muonExtrapolatedTrackParticle();
        if ( m_DoCom == 1 ) *pOut << "COM*" << " "
              << "CombinedMuon" << " "
              << "TP is muonExtrapolatedTrackParticle "  
              << std::endl;
      }else{
        if ( (*it)->hasInDetTrackParticle() ){
          TheTp = (*it)->inDetTrackParticle() ;
          if ( m_DoCom == 1 ) *pOut << "COM*" << " "
                << "CombinedMuon" << " "
                << "TP is hasInDetTrackParticle "  
                << std::endl;
        }
      }
    }
    if (TheTp){
      const Trk::Track* pTrack    = TheTp->originalTrack();
      if ( pTrack == 0 ){
        if ( m_DoCom == 1 ) *pOut << "COM*" << " "
              << "CombinedMuon" << " "
              << "pTrack is 0 "  
              << std::endl;
      }
      const Trk::Perigee* pPerigee = TheTp->measuredPerigee();
      if ( pPerigee == 0 ){
        if ( m_DoCom == 1 ) *pOut << "COM" << " "
              << "CombinedMuon" << " "
              << "pPerigee is 0 "  
              << std::endl;
      }
      if ( pPerigee == 0) continue ;
      double Charge  ;
      double PosX    ;
      double PosY    ;
      double PosZ    ;
      double PX      ;
      double PY      ;
      double PZ      ;      
      double Pt      ;
      GetKinePerigee(
       pPerigee,
       Charge  ,
       PosX    ,
       PosY    ,
       PosZ    ,
       PX      ,
       PY      ,
       PZ      ,      
       Pt      
      );

      int pdg_code = 1 ;
      if ( (TheTp->info()).patternRecoInfo(Trk::TrackInfo::ReverseOrderedTrack) ) pdg_code = -1 ;
    
      *pOut << "TRK" << " "
            << "CombinedMuon" << " "
            << CombinedMuonContainerLocation << " "
            << " dummy " << " "
            << std::setw(12)<<std::setprecision(3) <<PosX    << " "
            << std::setw(12)<<std::setprecision(3) <<PosY    << " "
            << std::setw(12)<<std::setprecision(3) <<PosZ    << " "
            << std::setw(12)<<std::setprecision(3) <<PX       << " "
            << std::setw(12)<<std::setprecision(3) <<PY      << " "
            << std::setw(12)<<std::setprecision(3) <<PZ      << " "
            << std::setw(3)<<std::setprecision(0) <<Charge  << " "
            << std::setw(8)<<std::setprecision(3) <<pdg_code  << " "
          ;
      p_MboyViewPointerDumperTool->DumpPointer(pOut,pTrack); 
      *pOut << std::endl;
      if (m_DoTSO==1) p_MboyViewTSOSDumpTool->DumpTSOSs(pTrack,pOut); 

      if ( m_DoCom == 1 ) *pOut << "COM" << " "
            << " " << (*it)->author ()
            << " || " << (*it)->isTight ()
            << " " << (*it)->isMedium ()
            << " " << (*it)->isLoose ()

            << std::endl;

//    Dump Trk::Track content
      p_MboyViewTrTrackDumpTool->ViewTrkTrack(pTrack,pOut); 

    }

  }

  return StatusCode::SUCCESS;

}
void MboyViewCombinedMuonContainerTool::GetKinePerigee(
 const Trk::Perigee* pPerigee,
 double& Charge  ,
 double& PosX    ,
 double& PosY    ,
 double& PosZ    ,
 double& PX      ,
 double& PY      ,
 double& PZ      ,
 double& Pt      
){

  Charge  = 0. ;
  PosX    = 0. ;
  PosY    = 0. ;
  PosZ    = 0. ;
  PX      = 0. ;
  PY      = 0. ;
  PZ      = 0. ;
  Pt      = 0. ;

  if (pPerigee){
    Charge  = pPerigee->charge()         ;
    PosX    = (pPerigee->position()).x() ;
    PosY    = (pPerigee->position()).y() ;
    PosZ    = (pPerigee->position()).z() ;
    PX      = (pPerigee->momentum()).x() ;
    PY      = (pPerigee->momentum()).y() ;
    PZ      = (pPerigee->momentum()).z() ;
    Pt      = pPerigee->pT()             ;
  }

}

