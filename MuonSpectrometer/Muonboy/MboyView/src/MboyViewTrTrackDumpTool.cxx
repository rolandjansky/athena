/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkTrack/TrackStateOnSurface.h"

#include "TrkParameters/TrackParameters.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "TrkPrepRawData/PrepRawData.h"

#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/CscStripPrepData.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewTrTrackDumpTool.h"

static const InterfaceID IID_IMboyViewTrTrackDumpTool("MboyViewTrTrackDumpTool", 1, 0);

const InterfaceID& MboyViewTrTrackDumpTool::interfaceID()
{
  return IID_IMboyViewTrTrackDumpTool;
}

MboyViewTrTrackDumpTool::MboyViewTrTrackDumpTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewTrTrackDumpTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

  declareProperty("DoCom"        , m_DoCom  = 0    ) ; 

}

MboyViewTrTrackDumpTool::~MboyViewTrTrackDumpTool(){}

// Initialize
StatusCode MboyViewTrTrackDumpTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  ATH_MSG_INFO( " DoCom  :     " <<  m_DoCom ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

//Set pointer on AtlasDetectorID
  sc = detStore()->retrieve(m_detID, "AtlasID" );
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not get AtlasDetectorID ") ;
    return( StatusCode::FAILURE );
  }
  ATH_MSG_INFO( "Found AtlasDetectorID ") ;

  sc = detStore()->retrieve(m_mdtId,"MDTIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve MDTIDHELPER" ) ;
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_cscId,"CSCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve CSCIDHELPER" ) ;
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_rpcId,"RPCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve RPCIDHELPER" ) ;
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_tgcId,"TGCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve TGCIDHELPER" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewTrTrackDumpTool::finalize(){return StatusCode::SUCCESS;}

void MboyViewTrTrackDumpTool::ViewTrkTrack(
               const Trk::Track* pTrack ,
               std::ofstream* pOut
){
 
  if ( m_DoCom != 1 ) return ;

  if ( pTrack == 0 ){
    *pOut << "COM" << " "
          << "pTrack is 0 "  
          << std::endl;
  }
    if ( pTrack == 0 ) return ;
  *pOut << "COM" << " "
        << (pTrack->info()).dumpInfo() << " "
        << std::endl;

  const DataVector< const Trk::TrackStateOnSurface >* pVecTSOS  = pTrack->trackStateOnSurfaces() ;  

  DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoS    =  pVecTSOS->begin() ;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoSEnd =  pVecTSOS->end()   ;

  *pOut << "COMTSS" << " "
        << std::setw(20) <<pVecTSOS->size()
        << std::endl;

  itTSoS    =  pVecTSOS->begin() ;
  for( ; itTSoS!=itTSoSEnd; ++itTSoS ){

    *pOut << "COMTSS" << " " ;

    *pOut << std::setw(30)<<(*itTSoS)->dumpType() ;

    ViewTrackParameters( (*itTSoS)->trackParameters() , pOut );
      
    ViewMeasurementBase( (*itTSoS)->measurementOnTrack() , pOut );

    *pOut << std::endl;

  }

}
void MboyViewTrTrackDumpTool::ViewMeasurementBase(
               const Trk::MeasurementBase* pMeasurementBase ,
               std::ofstream* pOut
){

  if ( !pMeasurementBase ) {
    *pOut 
        << " MEBS 0 "
        ;
    return ;
  }
  const Trk::RIO_OnTrack* pRIO_OnTrack = dynamic_cast<const Trk::RIO_OnTrack*>(pMeasurementBase) ;
  if ( !pRIO_OnTrack ){
    *pOut 
        << " RIOO 0 "
        ;
    return ;
  }
  const Identifier& channelId = pRIO_OnTrack->identify();
  if (!(m_detID->is_muon(channelId))){
    *pOut 
        << " MURI 0 "
        ;
    *pOut 
        << " NotMuonPrep "
        ;
    return ;
  }
    *pOut 
        << " MURI 1 "
        ;
    *pOut 
        << m_detID->show_to_string(channelId)
        ;
  const Trk::PrepRawData* pPrepRawData = pRIO_OnTrack->prepRawData() ;
  if ( !pPrepRawData ){
    *pOut 
        << " NoPrep      "
        ;
    return ;
  }
  const Muon::MdtPrepData* pMdtPrepData = dynamic_cast<const Muon::MdtPrepData*>(pPrepRawData);
  if (pMdtPrepData) {
    *pOut 
        << " MdtPrepData "
        ;
  }
  const Muon::RpcPrepData* pRpcPrepData = dynamic_cast<const Muon::RpcPrepData*>(pPrepRawData);
  if (pRpcPrepData) { 
    *pOut 
        << " RpcPrepData "
        ;
  }
  const Muon::TgcPrepData* pTgcPrepData = dynamic_cast<const Muon::TgcPrepData*>(pPrepRawData);
  if (pTgcPrepData) { 
    *pOut 
        << " TgcPrepData "
        ;
  }
  const Muon::CscStripPrepData* pCscStripPrepData = dynamic_cast<const Muon::CscStripPrepData*>(pPrepRawData);
  if (pCscStripPrepData) { 
    *pOut 
        << " CscStripPrepData "
        ;
  }
  const Muon::CscPrepData* pCscPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepRawData);
  if (pCscPrepData) {
    *pOut 
        << " CscPrepData "
        ;
  }

}

void MboyViewTrTrackDumpTool::ViewTrackParameters(
               const Trk::TrackParameters* pTrackParameters ,
               std::ofstream* pOut
){

  std::string TypeTrackParameters = "" ;
 
  if ( !pTrackParameters ) {
    double ValBidon = 0. ;
    *pOut 
        << " ISTP 0 "
        << std::setw(20)<< TypeTrackParameters
        << std::setw(12)<<std::setprecision(3)<< ValBidon 
        << std::setw(12)<<std::setprecision(3)<< ValBidon
        << std::setw(12)<<std::setprecision(3)<< ValBidon
        << std::setw(12)                      << ValBidon
        << std::setw(12)                      << ValBidon
        << std::setw(12)                      << ValBidon
        ;
    return ;
  }

  const Trk::Perigee* pPerigee = dynamic_cast<const Trk::Perigee*>(pTrackParameters);
  if (pPerigee) { TypeTrackParameters = "Trk::Perigee";
  }else{
    const Trk::AtaCylinder* pAtaCylinder = dynamic_cast<const Trk::AtaCylinder*>(pTrackParameters);
    if (pAtaCylinder) { TypeTrackParameters = "Trk::AtaCylinder";
    }else{
      const Trk::AtaDisc* pAtaDisc = dynamic_cast<const Trk::AtaDisc*>(pTrackParameters);
      if (pAtaDisc) { TypeTrackParameters = "Trk::AtaDisc";
      }else{
        const Trk::AtaPlane* pAtaPlane = dynamic_cast<const Trk::AtaPlane*>(pTrackParameters);
        if (pAtaPlane) { TypeTrackParameters = "Trk::AtaPlane";
        }else{
          const Trk::AtaStraightLine* pAtaStraightLine = dynamic_cast<const Trk::AtaStraightLine*>(pTrackParameters);
          if (pAtaStraightLine) { TypeTrackParameters = "Trk::AtaStraightLine";
          }else{
            TypeTrackParameters = "UNKNOWN" ;
          }
        }
      }
    }
  }

  
  *pOut 
        << " ISTP 1 "
        << std::setw(20)<< TypeTrackParameters
        << std::setw(12)<<std::setprecision(3)<< (pTrackParameters->position()).x()
        << std::setw(12)<<std::setprecision(3)<< (pTrackParameters->position()).y()
        << std::setw(12)<<std::setprecision(3)<< (pTrackParameters->position()).z()
        << std::setw(12)                      << (pTrackParameters->momentum()).x()
        << std::setw(12)                      << (pTrackParameters->momentum()).y()
        << std::setw(12)                      << (pTrackParameters->momentum()).z()
        ;

}
