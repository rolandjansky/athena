/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
 
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "Identifier/Identifier.h"

//----------------------------------------------------------------//
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"

#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepData.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyPrepDataToDigiStoreTool.h"

//----------------------------------------------------------------//
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

MboyPrepDataToDigiStoreTool::MboyPrepDataToDigiStoreTool(const std::string& t, 
                                       const std::string& n,
                                       const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<IMboyPrepDataToDigiStoreTool>(this);

//Declare the properties

  declareProperty( "DoReserve" , m_DoReserve = 1 ) ;
  declareProperty( "DoDbg"     , m_DoDbg     = 0 ) ;

}

MboyPrepDataToDigiStoreTool::~MboyPrepDataToDigiStoreTool(){}

// Initialize
StatusCode MboyPrepDataToDigiStoreTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }


  ATH_MSG_INFO(  "================================" ) ;
  ATH_MSG_INFO(  "=Proprieties are     " ) ;
  ATH_MSG_INFO(  "= DoReserve                         " << m_DoReserve           ) ;
  ATH_MSG_INFO(  "= DoDbg                             " << m_DoDbg               ) ;
  ATH_MSG_INFO(  "================================" ) ;


  sc = detStore()->retrieve(m_mdtId,"MDTIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL(  "Cannot retrieve MDTIDHELPER" ) ;
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_cscId,"CSCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL(  "Cannot retrieve CSCIDHELPER" ) ;
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_rpcId,"RPCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL(  "Cannot retrieve RPCIDHELPER" ) ;
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_tgcId,"TGCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL(  "Cannot retrieve TGCIDHELPER" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO(  "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyPrepDataToDigiStoreTool::finalize(){return StatusCode::SUCCESS;}

// Fill MuonRecDigiStoreAth with Vector of PrepRawData
void MboyPrepDataToDigiStoreTool::FillMuonRecDigiStoreWithVectorOfPrepData_Base(
                                         const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
){

  if ( m_DoReserve >= 1 ) {
    int  SpaceForMDT    = 10 ;
    int  SpaceForRPC    = 10 ;
    int  SpaceForTGC    = 10 ;
    int  SpaceForCSC    = 10 ;
    int  SpaceForCSCClu = 10 ;
    std::vector<const Trk::PrepRawData*>::const_iterator it = VectorOfpPrepData.begin() ;
    for( ; it != VectorOfpPrepData.end(); ++it){
      const Trk::PrepRawData* pPrepData = (*it) ;
      const Muon::MdtPrepData* pMdtPrepData = dynamic_cast<const Muon::MdtPrepData*>(pPrepData);
      if (pMdtPrepData) {
        SpaceForMDT = SpaceForMDT + 1;
        continue ;
      }
      const Muon::RpcPrepData* pRpcPrepData = dynamic_cast<const Muon::RpcPrepData*>(pPrepData);
      if (pRpcPrepData) { 
        SpaceForRPC = SpaceForRPC + 1;
        continue ;
      }
      const Muon::TgcPrepData* pTgcPrepData = dynamic_cast<const Muon::TgcPrepData*>(pPrepData);
      if (pTgcPrepData) { 
        SpaceForTGC = SpaceForTGC + 1;
        continue ;
      }
      const Muon::CscStripPrepData* pCscStripPrepData = dynamic_cast<const Muon::CscStripPrepData*>(pPrepData);
      if (pCscStripPrepData) { 
        SpaceForCSC = SpaceForCSC + 1;
        continue ;
      }
      const Muon::CscPrepData* pCscPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepData);
      if (pCscPrepData) {
        SpaceForCSCClu = SpaceForCSCClu + 1 ;
        continue ;
      }
    } 
    pMuonRecDigiStoreAth->ReserveMuonRecDigiStoreAth(
                                                     m_DoReserve,
                                                     SpaceForMDT   ,
                                                     SpaceForRPC   ,
                                                     SpaceForTGC   ,
                                                     SpaceForCSC   ,
                                                     SpaceForCSCClu
                                                    );                                                
  }

  std::vector<const Trk::PrepRawData*>::const_iterator it = VectorOfpPrepData.begin() ;
  for( ; it != VectorOfpPrepData.end(); ++it){
    const Trk::PrepRawData* pPrepData = (*it) ;
    FillMuonRecDigiStoreWithPrepData_Base(pPrepData,pMuonRecDigiStoreAth);
  } 

}

// Fill MuonRecDigiStoreAth with PrepRawData
void MboyPrepDataToDigiStoreTool::FillMuonRecDigiStoreWithPrepData_Base(
                                         const Trk::PrepRawData* pPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
){

  const Muon::MdtPrepData* pMdtPrepData = dynamic_cast<const Muon::MdtPrepData*>(pPrepData);
  if (pMdtPrepData) {
    FillMuonRecDigiStoreWithMDT_Base(pMdtPrepData,pMuonRecDigiStoreAth);
    if ( m_DoDbg == 1 ){
      ATH_MSG_INFO(  "IT IS A pMdtPrepData " ) ;
    }
    return ;
  }
  
  const Muon::RpcPrepData* pRpcPrepData = dynamic_cast<const Muon::RpcPrepData*>(pPrepData);
  if (pRpcPrepData) { 
    FillMuonRecDigiStoreWithRPC_Base(pRpcPrepData,pMuonRecDigiStoreAth);
    if ( m_DoDbg == 1 ){
      ATH_MSG_INFO(  "IT IS A pRpcPrepData " ) ;
    }
    return ;
  }

  const Muon::TgcPrepData* pTgcPrepData = dynamic_cast<const Muon::TgcPrepData*>(pPrepData);
  if (pTgcPrepData) { 
    FillMuonRecDigiStoreWithTGC_Base(pTgcPrepData,pMuonRecDigiStoreAth);
    if ( m_DoDbg == 1 ){
      ATH_MSG_INFO(  "IT IS A pTgcPrepData " ) ;
    }
    return ;
  }

  const Muon::CscStripPrepData* pCscStripPrepData = dynamic_cast<const Muon::CscStripPrepData*>(pPrepData);
  if (pCscStripPrepData) {
    FillMuonRecDigiStoreWithCSC_Base(pCscStripPrepData,pMuonRecDigiStoreAth);
    if ( m_DoDbg == 1 ){
      ATH_MSG_INFO(  "IT IS A pCscStripPrepData " ) ;
    }
    return ;
  }

  const Muon::CscPrepData* pCscPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepData);
  if (pCscPrepData) {
    FillMuonRecDigiStoreWithCSCCluster_Base(pCscPrepData,pMuonRecDigiStoreAth);
    if ( m_DoDbg == 1 ){
      ATH_MSG_INFO(  "IT IS A pCscPrepData " ) ;
    }
    return ;
  }

  if ( m_DoDbg == 1 ){
    ATH_MSG_INFO(  "IT IS A UNKNOWN  <===========   " ) ;
  }

}
// Fill MuonRecDigiStoreAth with MDT
void MboyPrepDataToDigiStoreTool::FillMuonRecDigiStoreWithMDT_Base(
                                         const Muon::MdtPrepData* pMdtPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
){

  if (pMuonRecDigiStoreAth) {
  if (pMdtPrepData) {
    MuonRecDigitMDT ToBeAdded;

    int AreAmdbIds = 0 ;
    ToBeAdded.SetAreAmdbIds(AreAmdbIds);

    Identifier channelId = pMdtPrepData->identify();
    int         TheSGStationNber    = m_mdtId->stationName(channelId)              ; 
    std::string TheSetSGStationName = m_mdtId->stationNameString(TheSGStationNber) ;
    ToBeAdded.SetSGStationNber ( TheSGStationNber                                         );
    ToBeAdded.SetSGStationName ( TheSetSGStationName                                      );
    ToBeAdded.SetSGStationEta  ( m_mdtId->stationEta(channelId)                           );
    ToBeAdded.SetSGStationPhi  ( m_mdtId->stationPhi(channelId)                           );
    ToBeAdded.SetSGMultilayer  ( m_mdtId->multilayer(channelId)                           );
    ToBeAdded.SetSGTubeLayer   ( m_mdtId->tubeLayer(channelId)                            );
    ToBeAdded.SetSGTube        ( m_mdtId->tube(channelId)                                 );

//     //This is valid for 2004 H8 Test beam only
//     if ( ToBeAdded.GetSGStationName() == "BIL" && ToBeAdded.GetSGStationEta() == 0 ) {
//       std::string NewName = "BIR" ;
//       ToBeAdded.SetSGStationName ( NewName );
//     } 

    int TheTDC = pMdtPrepData->tdc();
    ToBeAdded.SetTDCcount( TheTDC              );
    ToBeAdded.SetADCcount( pMdtPrepData->adc() );

    ToBeAdded.SetSignalDriftTime( TheTDC );

    Amg::Vector3D globalPosition = pMdtPrepData->globalPosition();
    ToBeAdded.SetX( globalPosition.x() );
    ToBeAdded.SetY( globalPosition.y() );
    ToBeAdded.SetZ( globalPosition.z() );

    ToBeAdded.SetStatus( pMdtPrepData->status() );

    pMuonRecDigiStoreAth->AddDigit(
                                   ToBeAdded,
                                   pMdtPrepData
                                  );


  } 
  } 

}

// Fill MuonRecDigiStoreAth with RPC
void MboyPrepDataToDigiStoreTool::FillMuonRecDigiStoreWithRPC_Base(
                                         const Muon::RpcPrepData* pRpcPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
){

  if (pMuonRecDigiStoreAth) {
  if (pRpcPrepData) {
    MuonRecDigitRPC ToBeAdded;

    int AreAmdbIds = 0 ;
    ToBeAdded.SetAreAmdbIds(AreAmdbIds);
    
    Identifier channelId = pRpcPrepData->identify();
    int         TheSGStationNber    = m_rpcId->stationName(channelId)              ; 
    std::string TheSetSGStationName = m_rpcId->stationNameString(TheSGStationNber) ;
    ToBeAdded.SetSGStationNber ( TheSGStationNber                                         );
    ToBeAdded.SetSGStationName ( TheSetSGStationName                                      );
    ToBeAdded.SetSGStationEta  ( m_rpcId->stationEta(channelId)                           );
    ToBeAdded.SetSGStationPhi  ( m_rpcId->stationPhi(channelId)                           );
    ToBeAdded.SetSGDoubletR    ( m_rpcId->doubletR(channelId)                             );
    ToBeAdded.SetSGDoubletZ    ( m_rpcId->doubletZ(channelId)                             );
    ToBeAdded.SetSGDoubletPhi  ( m_rpcId->doubletPhi(channelId)                           );
    ToBeAdded.SetSGGasGap      ( m_rpcId->gasGap(channelId)                               );
    ToBeAdded.SetSGMeasuresPhi ( m_rpcId->measuresPhi(channelId)                          );
    ToBeAdded.SetSGStrip       ( m_rpcId->strip(channelId)                                );

    ToBeAdded.SetRpcTime( pRpcPrepData->time() );

    Amg::Vector3D globalPosition = pRpcPrepData->globalPosition();
    ToBeAdded.SetX( globalPosition.x() );
    ToBeAdded.SetY( globalPosition.y() );
    ToBeAdded.SetZ( globalPosition.z() );
    
    pMuonRecDigiStoreAth->AddDigit(
                                   ToBeAdded,
                                   pRpcPrepData
                                  );

  }  
  }  

}

 
// Fill MuonRecDigiStoreAth with TGC
void MboyPrepDataToDigiStoreTool::FillMuonRecDigiStoreWithTGC_Base(
                                         const Muon::TgcPrepData* pTgcPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
){
  
  if (pMuonRecDigiStoreAth) {
  if (pTgcPrepData) {
    MuonRecDigitTGC ToBeAdded;

    int AreAmdbIds = 0 ;
    ToBeAdded.SetAreAmdbIds(AreAmdbIds);
    
    Identifier channelId = pTgcPrepData->identify();
    int         TheSGStationNber    = m_tgcId->stationName(channelId)              ; 
    std::string TheSetSGStationName = m_tgcId->stationNameString(TheSGStationNber) ;
    ToBeAdded.SetSGStationNber ( TheSGStationNber                                         );
    ToBeAdded.SetSGStationName ( TheSetSGStationName                                      );
    ToBeAdded.SetSGStationEta  ( m_tgcId->stationEta(channelId)                           );
    ToBeAdded.SetSGStationPhi  ( m_tgcId->stationPhi(channelId)                           );
    ToBeAdded.SetSGGasGap      ( m_tgcId->gasGap(channelId)                               );
    ToBeAdded.SetSGIsStrip     ( m_tgcId->isStrip(channelId)                              );
    ToBeAdded.SetSGChannel     ( m_tgcId->channel(channelId)                              );

    Amg::Vector3D globalPosition = pTgcPrepData->globalPosition();
    ToBeAdded.SetX( globalPosition.x() );
    ToBeAdded.SetY( globalPosition.y() );
    ToBeAdded.SetZ( globalPosition.z() );

    pMuonRecDigiStoreAth->AddDigit(
                                   ToBeAdded,
                                   pTgcPrepData
                                  );

  } 
  } 

}

// Fill MuonRecDigiStoreAth with CSC
void MboyPrepDataToDigiStoreTool::FillMuonRecDigiStoreWithCSC_Base(
                                         const Muon::CscStripPrepData* pCscStripPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
){

  if (pMuonRecDigiStoreAth) {
  if (pCscStripPrepData) {
    MuonRecDigitCSC ToBeAdded;

    int AreAmdbIds = 0 ;
    ToBeAdded.SetAreAmdbIds(AreAmdbIds);

    Identifier channelId = pCscStripPrepData->identify();
    int         TheSGStationNber    = m_cscId->stationName(channelId)              ; 
    std::string TheSetSGStationName = m_cscId->stationNameString(TheSGStationNber) ;
    ToBeAdded.SetSGStationNber  ( TheSGStationNber                                         );
    ToBeAdded.SetSGStationName  ( TheSetSGStationName                                      );
    ToBeAdded.SetSGStationEta   ( m_cscId->stationEta(channelId)                           );
    ToBeAdded.SetSGStationPhi   ( m_cscId->stationPhi(channelId)                           );
    ToBeAdded.SetSGChamberLayer ( m_cscId->chamberLayer(channelId)                         );
    ToBeAdded.SetSGWireLayer    ( m_cscId->wireLayer(channelId)                            );
    ToBeAdded.SetSGMeasuresPhi  ( m_cscId->measuresPhi(channelId)                          );
    ToBeAdded.SetSGStrip        ( m_cscId->strip(channelId)                                );

    ToBeAdded.SetDepositedCharge( pCscStripPrepData->timeOfFirstSample() );

    Amg::Vector3D globalPosition = pCscStripPrepData->globalPosition();
    ToBeAdded.SetX( globalPosition.x() );
    ToBeAdded.SetY( globalPosition.y() );
    ToBeAdded.SetZ( globalPosition.z() );

    pMuonRecDigiStoreAth->AddDigit(
                                   ToBeAdded,
                                   pCscStripPrepData
                                  );


  }
  }

}
// Fill MuonRecDigiStoreAth with CSC clusters
void MboyPrepDataToDigiStoreTool::FillMuonRecDigiStoreWithCSCCluster_Base(
                                         const Muon::CscPrepData* pCscPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
){

  if (pMuonRecDigiStoreAth) {
  if (pCscPrepData) {
    MuonRecDigitCSCClu ToBeAdded;
    
    int AreAmdbIds = 0 ;
    ToBeAdded.SetAreAmdbIds(AreAmdbIds);
    
    Identifier channelId = pCscPrepData->identify();
    int         TheSGStationNber    = m_cscId->stationName(channelId)              ; 
    std::string TheSetSGStationName = m_cscId->stationNameString(TheSGStationNber) ;
    ToBeAdded.SetSGStationNber  ( TheSGStationNber                                           );
    ToBeAdded.SetSGStationName  ( TheSetSGStationName                                        );
    ToBeAdded.SetSGStationEta   ( m_cscId->stationEta(channelId)                             );
    ToBeAdded.SetSGStationPhi   ( m_cscId->stationPhi(channelId)                             );
    ToBeAdded.SetSGChamberLayer ( m_cscId->chamberLayer(channelId)                           );
    ToBeAdded.SetSGWireLayer    ( m_cscId->wireLayer(channelId)                              );
    ToBeAdded.SetSGMeasuresPhi  ( m_cscId->measuresPhi(channelId)                            );
    ToBeAdded.SetSGStrip        ( m_cscId->strip(channelId)                                  );

    Amg::Vector3D Gposition = pCscPrepData->globalPosition();
    ToBeAdded.SetX( Gposition.x() );
    ToBeAdded.SetY( Gposition.y() );
    ToBeAdded.SetZ( Gposition.z() );

    float cluster_error      = sqrt((pCscPrepData->localCovariance())(0,0));
    ToBeAdded.SetError( cluster_error );

    ToBeAdded.SetDepositedCharge( pCscPrepData->charge() );

    ToBeAdded.SetTime( pCscPrepData->time() );

    ToBeAdded.SetStatus( pCscPrepData->status() );

    ToBeAdded.SetTimeStatus( pCscPrepData->timeStatus() );

    pMuonRecDigiStoreAth->AddDigit(
                                   ToBeAdded,
                                   pCscPrepData
                                   );


  }
  }

}
