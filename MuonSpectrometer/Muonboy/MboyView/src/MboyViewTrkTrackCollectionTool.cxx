/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/ToolHandle.h"
/////////////////////////////////////////////////////////
#include "MboyView/MboyViewTrkTrackCollectionTool.h"

static const InterfaceID IID_IMboyViewTrkTrackCollectionTool("MboyViewTrkTrackCollectionTool", 1, 0);

const InterfaceID& MboyViewTrkTrackCollectionTool::interfaceID()
{
  return IID_IMboyViewTrkTrackCollectionTool;
}

MboyViewTrkTrackCollectionTool::MboyViewTrkTrackCollectionTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
, p_MboyViewPointerDumperTool        ( "MboyViewPointerDumperTool/MboyViewPointerDumperTool"              ) 
, p_MboyViewTrTrackDumpTool        ( "MboyViewTrTrackDumpTool/MboyViewTrTrackDumpTool"              ) 
, p_MboyViewTSOSDumpTool        ( "MboyViewTSOSDumpTool/MboyViewTSOSDumpTool"              ) 
{

  declareInterface<MboyViewTrkTrackCollectionTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

  declareProperty("DoCom"        , m_DoCom  = 0    ) ; 
  declareProperty("DoTSO"        , m_DoTSO  = 1    ) ; 

  declareProperty("MboyViewPointerDumperTool"        , p_MboyViewPointerDumperTool        ) ; 
  declareProperty("MboyViewTrTrackDumpTool"          , p_MboyViewTrTrackDumpTool          ) ; 
  declareProperty("MboyViewTSOSDumpTool"          , p_MboyViewTSOSDumpTool          ) ; 

}

MboyViewTrkTrackCollectionTool::~MboyViewTrkTrackCollectionTool(){}

// Initialize
StatusCode MboyViewTrkTrackCollectionTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " );

  ATH_MSG_INFO( " DoCom  :     " <<  m_DoCom );
  ATH_MSG_INFO( " DoTSO  :     " <<  m_DoTSO );

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }

//Retrieve p_MboyViewPointerDumperTool
  if ( p_MboyViewPointerDumperTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyViewPointerDumperTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyViewPointerDumperTool );

//Retrieve p_MboyViewTrTrackDumpTool
  if ( p_MboyViewTrTrackDumpTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyViewTrTrackDumpTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyViewTrTrackDumpTool );

//Retrieve p_MboyViewTSOSDumpTool
  if ( p_MboyViewTSOSDumpTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyViewTSOSDumpTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_MboyViewTSOSDumpTool );

  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode MboyViewTrkTrackCollectionTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewTrkTrackCollectionTool::ViewTrkTrackCollection(
               std::string TrkTrackCollectionLocation,
               std::ofstream* pOut
){

//Access
  if (!(evtStore()->contains<TrackCollection>(TrkTrackCollectionLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage) {
      ATH_MSG_INFO( " Not existing " << TrkTrackCollectionLocation );
    }
    return( StatusCode::SUCCESS );
  }

  StatusCode sc = StatusCode::SUCCESS;

  const DataVector<Trk::Track>* pContainerTracks ;
  sc = evtStore()->retrieve(pContainerTracks, TrkTrackCollectionLocation);
  if (sc.isFailure()){
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Selection: pContainerTracks not found at " << TrkTrackCollectionLocation );
    }
    return( StatusCode::SUCCESS);
  }

//Dump
  if ( m_DoCom == 1 ) *pOut << "COM" << " "
        << "TrkTrack" << " "
        << "=============================================" << " "
        << TrkTrackCollectionLocation << " "
        << pContainerTracks->size()
        << std::endl;

  for (DataVector<Trk::Track>::const_iterator it = pContainerTracks->begin(); it!=pContainerTracks->end(); ++it){
    const Trk::Track* pTrack = (*it);
    if ( pTrack == 0 ){
      if ( m_DoCom == 1 ) *pOut << "COM*" << " "
            << "TrkTrack" << " "
            << "pTrack is 0 "  
            << std::endl;
    }
    if ( pTrack == 0 )  continue ;
    const Trk::Perigee* pPerigee = pTrack->perigeeParameters();
    if ( pPerigee == 0 ){
      if ( m_DoCom == 1 ) *pOut << "COM" << " "
            << "TrkTrack" << " "
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
    if ( (pTrack->info()).patternRecoInfo(Trk::TrackInfo::ReverseOrderedTrack) ) pdg_code = -1 ;
    
    *pOut << "TRK" << " "
          << "TrkTrack" << " "
          << TrkTrackCollectionLocation << " "
          << "XYZ"<< " "
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
void MboyViewTrkTrackCollectionTool::GetKinePerigee(
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

