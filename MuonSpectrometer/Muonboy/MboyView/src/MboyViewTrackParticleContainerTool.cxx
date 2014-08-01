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

#include "TrkParameters/TrackParameters.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewTrackParticleContainerTool.h"

static const InterfaceID IID_IMboyViewTrackParticleContainerTool("MboyViewTrackParticleContainerTool", 1, 0);

const InterfaceID& MboyViewTrackParticleContainerTool::interfaceID()
{
  return IID_IMboyViewTrackParticleContainerTool;
}

MboyViewTrackParticleContainerTool::MboyViewTrackParticleContainerTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
, p_MboyViewPointerDumperTool        ( "MboyViewPointerDumperTool/MboyViewPointerDumperTool"              ) 
, p_MboyViewTrTrackDumpTool        ( "MboyViewTrTrackDumpTool/MboyViewTrTrackDumpTool"              ) 
, p_MboyViewTSOSDumpTool        ( "MboyViewTSOSDumpTool/MboyViewTSOSDumpTool"              ) 
{

  declareInterface<MboyViewTrackParticleContainerTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

  declareProperty("DoCom"        , m_DoCom  = 0    ) ; 
  declareProperty("DoTSO"        , m_DoTSO  = 1    ) ; 

  declareProperty("MboyViewPointerDumperTool"        , p_MboyViewPointerDumperTool        ) ; 
  declareProperty("MboyViewTrTrackDumpTool"          , p_MboyViewTrTrackDumpTool          ) ; 
  declareProperty("MboyViewTSOSDumpTool"          , p_MboyViewTSOSDumpTool          ) ; 

}

MboyViewTrackParticleContainerTool::~MboyViewTrackParticleContainerTool(){}

// Initialize
StatusCode MboyViewTrackParticleContainerTool::initialize(){

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

StatusCode MboyViewTrackParticleContainerTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewTrackParticleContainerTool::ViewTrackParticleContainer(
               std::string TrackParticleContainerLocation,
               std::ofstream* pOut
){

//Access
  if (!(evtStore()->contains<Rec::TrackParticleContainer>(TrackParticleContainerLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
     ATH_MSG_INFO( " Not existing " << TrackParticleContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }
  const Rec::TrackParticleContainer* pContainerTracks ;
  StatusCode sc = evtStore()->retrieve(pContainerTracks, TrackParticleContainerLocation);
  if (sc.isFailure()) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
     ATH_MSG_INFO( "pContainerTracks not found at " << TrackParticleContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

//Dump
  if ( m_DoCom == 1 ) *pOut << "COM" << " "
        << "TrackParticle" << " "
        << "=============================================" << " "
        << TrackParticleContainerLocation << " "
        << pContainerTracks->size()
        << std::endl;
        
  for (Rec::TrackParticleContainer::const_iterator it = pContainerTracks->begin(); it!=pContainerTracks->end(); ++it){
    const Rec::TrackParticle* TheTp = (*it) ;
    const Trk::Track* pTrack    = TheTp->originalTrack();
    if ( pTrack == 0 ){
      if ( m_DoCom == 1 ) *pOut << "COM*" << " "
            << "TrackParticle" << " "
            << "pTrack is 0 "  
            << std::endl;
    }
    const Trk::Perigee * pPerigee = TheTp->measuredPerigee();
    if ( pPerigee == 0 ){
      if ( m_DoCom == 1 ) *pOut << "COM" << " "
            << "TrackParticle" << " "
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
          << "TrackParticle" << " "
          << TrackParticleContainerLocation << " "
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


//  Dump Trk::Track content
    p_MboyViewTrTrackDumpTool->ViewTrkTrack(pTrack,pOut); 

  }

  return StatusCode::SUCCESS;

}
void MboyViewTrackParticleContainerTool::GetKinePerigee(
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

