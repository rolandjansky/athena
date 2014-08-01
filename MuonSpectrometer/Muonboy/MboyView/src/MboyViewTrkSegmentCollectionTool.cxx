/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkEventPrimitives/FitQuality.h"
/////////////////////////////////////////////////////////
#include "MboyView/MboyViewTrkSegmentCollectionTool.h"

static const InterfaceID IID_IMboyViewTrkSegmentCollectionTool("MboyViewTrkSegmentCollectionTool", 1, 0);

const InterfaceID& MboyViewTrkSegmentCollectionTool::interfaceID()
{
  return IID_IMboyViewTrkSegmentCollectionTool;
}

MboyViewTrkSegmentCollectionTool::MboyViewTrkSegmentCollectionTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewTrkSegmentCollectionTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

}

MboyViewTrkSegmentCollectionTool::~MboyViewTrkSegmentCollectionTool(){}

// Initialize
StatusCode MboyViewTrkSegmentCollectionTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewTrkSegmentCollectionTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewTrkSegmentCollectionTool::ViewTrkSegmentCollection(
               std::string TrkSegmentCollectionLocation,
               std::ofstream* pOut
){

//Access
  if (!(evtStore()->contains<Trk::SegmentCollection>(TrkSegmentCollectionLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << TrkSegmentCollectionLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  StatusCode sc = StatusCode::SUCCESS;

  const DataVector<Trk::Segment>* pContainerSegments ;
  sc = evtStore()->retrieve(pContainerSegments, TrkSegmentCollectionLocation);
  if (sc.isFailure()){
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( "pContainerSegments not found at " << TrkSegmentCollectionLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

//Dump
  for (DataVector<Trk::Segment>::const_iterator it = pContainerSegments->begin(); it!=pContainerSegments->end(); ++it){
    const Trk::Segment* pSegment = (*it);
    if ( pSegment == 0 ) continue ;
    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if ( pMuonSegment == 0 ) continue ;
    double PosX    ;
    double PosY    ;
    double PosZ    ;
    double DirX    ;
    double DirY    ;
    double DirZ    ;      
    GetKineMuonSgement(
     pMuonSegment,
     PosX    ,
     PosY    ,
     PosZ    ,
     DirX    ,
     DirY    ,
     DirZ    
    );
    double t0  = 9999. ;
    double et0 = 9999. ;
    if (pMuonSegment->hasFittedT0() ) {
      t0  = pMuonSegment->time()      ;
      et0 = pMuonSegment->errorTime() ;
    }
    *pOut << "SEG" << " "
          << "MuonSegment" << " "
          << TrkSegmentCollectionLocation << " "
          << std::setw(12)<<std::setprecision(3) <<PosX    << " "
          << std::setw(12)<<std::setprecision(3) <<PosY    << " "
          << std::setw(12)<<std::setprecision(3) <<PosZ    << " "
          << std::setw(12)<<std::setprecision(6) <<DirX     << " "
          << std::setw(12)<<std::setprecision(6) <<DirY    << " "
          << std::setw(12)<<std::setprecision(6) <<DirZ    << " "
          << std::setw(12)<<std::setprecision(4) <<pMuonSegment->fitQuality()->chiSquared()   << " " 
          << std::setw(12)<<std::setprecision(4) <<t0        << " "  
          << std::setw(12)<<std::setprecision(4) <<et0   << " " 
        << std::endl;
  }

  return StatusCode::SUCCESS;

}
void MboyViewTrkSegmentCollectionTool::GetKineMuonSgement(
 const Muon::MuonSegment* pMuonSegment,
 double& PosX    ,
 double& PosY    ,
 double& PosZ    ,
 double& DirX    ,
 double& DirY    ,
 double& DirZ    
){

  PosX    = 0. ;
  PosY    = 0. ;
  PosZ    = 0. ;
  DirX    = 0. ;
  DirY    = 0. ;
  DirZ    = 0. ;

  if (pMuonSegment){
    PosX    = (pMuonSegment->globalPosition()).x() ;
    PosY    = (pMuonSegment->globalPosition()).y() ;
    PosZ    = (pMuonSegment->globalPosition()).z() ;
    DirX    = (pMuonSegment->globalDirection()).x() ;
    DirY    = (pMuonSegment->globalDirection()).y() ;
    DirZ    = (pMuonSegment->globalDirection()).z() ;
  }

}

