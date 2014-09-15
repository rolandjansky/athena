/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkMeasurementBase/MeasurementBase.h"

#include "MboyCoreEvt/MbDRAT.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

#include "MboyCoreEvt/MbIDTagMDT.h"
#include "MboyCoreEvt/MbIDTagCSCclu.h"
#include "MboyCoreEvt/MbIDTagRPC.h"
#include "MboyCoreEvt/MbIDTagTGC.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "TrkPrepRawData/PrepRawData.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "MboyEDMTool/MeasurementBaseToDratTool.h"

#include "MboyEDMToolInterfaces/IMboyEDMHelper.h"

Trk::MeasurementBaseToDratTool::MeasurementBaseToDratTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
 p_OwnEDMHelper              ( "MboyEDMHelper" ) 
{

  declareInterface<IMeasurementBaseToDratTool>(this);

  declareProperty( "AddIdent" , m_AddIdent = 0 ) ;
  declareProperty("MboyEDMHelper" , p_OwnEDMHelper ) ;

}

Trk::MeasurementBaseToDratTool::~MeasurementBaseToDratTool(){}

// Initialize
StatusCode Trk::MeasurementBaseToDratTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are     " );
  ATH_MSG_INFO( "= AddIdent                 " << m_AddIdent );
  ATH_MSG_INFO( "================================" );

//Set pointer on AtlasDetectorID
  sc = detStore()->retrieve(m_detID, "AtlasID" );
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not get AtlasDetectorID ");
    return( StatusCode::FAILURE );
  }
  ATH_MSG_INFO( "Found AtlasDetectorID ");

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

//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved tool " << p_OwnEDMHelper ) ;

  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode Trk::MeasurementBaseToDratTool::finalize(){return StatusCode::SUCCESS;}

void  Trk::MeasurementBaseToDratTool::MeasurementBaseToMbDRAT( 
                   MbDRAT* pMbDRAT,
                   MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                   const Trk::MeasurementBase* pMeasurementBase
){

  if ( pMbDRAT == 0 ) return ;
  if ( pMeasurementBase == 0 ) return ;
  
  double TheISC = GetISC(pMeasurementBase) ;
  ATH_MSG_DEBUG( "MeasurementBaseToMbDRAT :  Before  GetISC  " << TheISC );
  if ( TheISC == -1 ) return ;
  ATH_MSG_DEBUG( "MeasurementBaseToMbDRAT :  After   GetISC  " << TheISC );
  
  const Trk::RIO_OnTrack* pRIO_OnTrack = dynamic_cast<const Trk::RIO_OnTrack*>(pMeasurementBase);
  if ( pRIO_OnTrack ){
    ATH_MSG_DEBUG( "MeasurementBaseToMbDRAT :  cast Trk::RIO_OnTrack  Done" );
    const Identifier channelId = pRIO_OnTrack->identify();
    if ( m_detID->is_muon(channelId) ){
      ATH_MSG_DEBUG( "MeasurementBaseToMbDRAT :  Test channelId  Done" );
      const Trk::PrepRawData* pPrepRawData = pRIO_OnTrack->prepRawData() ; 
      const Muon::MdtPrepData* pMdtPrepData = dynamic_cast<const Muon::MdtPrepData*>(pPrepRawData);
      if (pMdtPrepData){
        ATH_MSG_DEBUG( "MeasurementBaseToMbDRAT :  Test pMdtPrepData  Done" );
        (pMbDRAT->m_VectorOfStatusMDT     ).push_back( pMdtPrepData->status()                    ) ;
        (pMbDRAT->m_VectorOfMbIDTagMDT    ).push_back( GetMbIDTagMDT( pMdtPrepData->identify() ) ) ;
        if ( m_AddIdent == 1 && pMuonRecDigiStoreAth != 0 ) {
          pMbDRAT->m_Ident = pMuonRecDigiStoreAth->FindRank(pMdtPrepData) ;
          ATH_MSG_DEBUG( " MDT    m_Ident " << pMbDRAT->m_Ident );
        }
        return ;
      } 
      const Muon::CscPrepData* pCscPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepRawData);
      if (pCscPrepData){
        ATH_MSG_DEBUG( "MeasurementBaseToMbDRAT :  Test pCscPrepData  Done" );
        (pMbDRAT->m_VectorOfStatusCSCclu     ).push_back( pCscPrepData->status()                       ) ;
        (pMbDRAT->m_VectorOfMbIDTagCSCclu    ).push_back( GetMbIDTagCSCclu( pCscPrepData->identify() ) ) ;
        if ( m_AddIdent == 1 && pMuonRecDigiStoreAth != 0 ) {
          pMbDRAT->m_Ident = pMuonRecDigiStoreAth->FindRank(pCscPrepData) ;
          ATH_MSG_DEBUG( " CSC    m_Ident " << pMbDRAT->m_Ident );
        }
        return ;
      }
      const Muon::RpcPrepData* pRpcPrepData = dynamic_cast<const Muon::RpcPrepData*>(pPrepRawData);
      if (pRpcPrepData){
        ATH_MSG_DEBUG( "MeasurementBaseToMbDRAT :  Test pRpcPrepData  Done" );
        (pMbDRAT->m_VectorOfMbIDTagRPC    ).push_back( GetMbIDTagRPC( pRpcPrepData->identify() ) ) ;
        if ( m_AddIdent == 1 && pMuonRecDigiStoreAth != 0 ) {
          pMbDRAT->m_Ident = pMuonRecDigiStoreAth->FindRank(pRpcPrepData) ;
          ATH_MSG_DEBUG( " RPC    m_Ident " << pMbDRAT->m_Ident );
        }
        return ;
      }
      const Muon::TgcPrepData* pTgcPrepData = dynamic_cast<const Muon::TgcPrepData*>(pPrepRawData);
      if (pTgcPrepData){
        ATH_MSG_DEBUG( "MeasurementBaseToMbDRAT :  Test pTgcPrepData  Done" );
        (pMbDRAT->m_VectorOfMbIDTagTGC    ).push_back( GetMbIDTagTGC( pTgcPrepData->identify() ) ) ;
        if ( m_AddIdent == 1 && pMuonRecDigiStoreAth != 0 ) {
          pMbDRAT->m_Ident = pMuonRecDigiStoreAth->FindRank(pTgcPrepData) ;
          ATH_MSG_DEBUG( " TGC    m_Ident " << pMbDRAT->m_Ident );
        }
        return ;
      }
    }
  }
   
}

void  Trk::MeasurementBaseToDratTool::PickUpMeasurementBaseAndCluster(
                   std::vector<int>* pVectorOfISC  ,
                   std::vector< const Trk::TrackParameters* >* pVectorOfTrackParameters ,
                   std::vector< std::vector<const Trk::MeasurementBase* > >* pVectorOfVectorOfMeasurementBase,
                   const Trk::Track* pTrack
){

  std::vector< const Trk::TrackParameters* > VectorOfTrackParameters_Unclustered ;
  std::vector< const Trk::MeasurementBase* > VectorOfMeasurementBase_Unclustered ;

  PickUpMeasurementBase(
                        &VectorOfTrackParameters_Unclustered ,
                        &VectorOfMeasurementBase_Unclustered ,
                        pTrack
                       );

  std::vector< const Trk::MeasurementBase* >::const_iterator it_MeasurementBase     = VectorOfMeasurementBase_Unclustered.begin() ;
  std::vector< const Trk::MeasurementBase* >::const_iterator it_MeasurementBase_End = VectorOfMeasurementBase_Unclustered.end()   ;
  std::vector< const Trk::TrackParameters* >::const_iterator it_TrackParameters     = VectorOfTrackParameters_Unclustered.begin() ;
  for( ; it_MeasurementBase != it_MeasurementBase_End; ++it_MeasurementBase,++it_TrackParameters){
    const Trk::MeasurementBase* pMeasurementBase = (*it_MeasurementBase) ;
    const Trk::TrackParameters* pTrackParameters = (*it_TrackParameters) ;
    int TheISC = GetISC(pMeasurementBase);
    if ( TheISC != -1 ){
      int IsNew = 1 ;
      std::vector<int>::iterator it_ISC                           = pVectorOfISC->begin() ;
      std::vector<int>::iterator it_ISC_End                       = pVectorOfISC->end()   ;
      std::vector< std::vector<const Trk::MeasurementBase* > >::const_iterator it_VectorOfMeasurementBase = pVectorOfVectorOfMeasurementBase->begin()   ;
      for( ; it_ISC != it_ISC_End; ++it_ISC,++it_VectorOfMeasurementBase){
        std::vector<const Trk::MeasurementBase* > VectorOfMeasurementBase = (*it_VectorOfMeasurementBase) ;
        if ( TheISC == (*it_ISC) ){
          VectorOfMeasurementBase.push_back( pMeasurementBase );
           IsNew = 0 ;
           break;
        }
      }
   
      if (IsNew == 1){
        pVectorOfISC->push_back(TheISC);
        pVectorOfTrackParameters->push_back( pTrackParameters );
        std::vector<const Trk::MeasurementBase* > VectorOfMeasurementBase ;
        VectorOfMeasurementBase.push_back( pMeasurementBase ) ;
        pVectorOfVectorOfMeasurementBase->push_back(VectorOfMeasurementBase);
      }
      
    }
  }

}

void  Trk::MeasurementBaseToDratTool::PickUpMeasurementBase(
                   std::vector< const Trk::TrackParameters* >* pVectorOfTrackParameters ,
                   std::vector< const Trk::MeasurementBase* >* pVectorOfMeasurementBase ,
                   const Trk::Track* pTrack
){

  DataVector<const Trk::TrackStateOnSurface>::const_iterator TrackStateOnSurfaceCurrent    = pTrack->trackStateOnSurfaces()->begin(); 
  DataVector<const Trk::TrackStateOnSurface>::const_iterator TrackStateOnSurfaceCurrentEnd = pTrack->trackStateOnSurfaces()->end(); 
  for ( ; TrackStateOnSurfaceCurrent!=TrackStateOnSurfaceCurrentEnd; ++TrackStateOnSurfaceCurrent){
    const Trk::MeasurementBase* pMeasurementBase = (*TrackStateOnSurfaceCurrent)->measurementOnTrack();
    const Trk::TrackParameters* pTrackParameters = (*TrackStateOnSurfaceCurrent)->trackParameters () ;
    PickUpMeasurementBase(
                          pVectorOfTrackParameters ,
                          pVectorOfMeasurementBase ,
                          pTrackParameters,
                          pMeasurementBase
                         );
  }
 
}

void  Trk::MeasurementBaseToDratTool::PickUpMeasurementBase (
                   std::vector< const Trk::TrackParameters* >* pVectorOfTrackParameters ,
                   std::vector< const Trk::MeasurementBase* >* pVectorOfMeasurementBase ,
                   const Trk::TrackParameters* pTrackParameters,
                   const Trk::MeasurementBase* pMeasurementBase
){

  const Muon::MuonSegment*    pMuonSegment     = dynamic_cast<const Muon::MuonSegment*>(pMeasurementBase);
  if ( pMuonSegment ){
    const std::vector<const Trk::MeasurementBase*> VectorOfMeasurementBase = pMuonSegment->containedMeasurements(); 
    std::vector<const Trk::MeasurementBase*>::const_iterator it_MeasurementBase     = VectorOfMeasurementBase.begin(); 
    std::vector<const Trk::MeasurementBase*>::const_iterator it_MeasurementBase_end = VectorOfMeasurementBase.end()  ; 
    for (; it_MeasurementBase!=it_MeasurementBase_end; ++it_MeasurementBase){ 
      const Trk::MeasurementBase* pMeasurementBase_New = (*it_MeasurementBase) ;
      PickUpMeasurementBase(
                            pVectorOfTrackParameters,
                            pVectorOfMeasurementBase,
                            pTrackParameters,
                            pMeasurementBase_New
                           ) ;
    }
  }else{
    const Muon::CompetingMuonClustersOnTrack* pCompetingMuonClustersOnTrack = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(pMeasurementBase);
    if ( pCompetingMuonClustersOnTrack ){
      const std::vector<const Muon::MuonClusterOnTrack*> VectorOfMuonClusterOnTrack = pCompetingMuonClustersOnTrack->containedROTs(); 
      std::vector<const Muon::MuonClusterOnTrack*>::const_iterator it_MuonClusterOnTrack    = VectorOfMuonClusterOnTrack.begin(); 
      std::vector<const Muon::MuonClusterOnTrack*>::const_iterator it_MuonClusterOnTrack_end = VectorOfMuonClusterOnTrack.end()  ; 
      for (; it_MuonClusterOnTrack!=it_MuonClusterOnTrack_end; ++it_MuonClusterOnTrack){ 
        const Trk::MeasurementBase* pMeasurementBase_New = (*it_MuonClusterOnTrack) ;
        PickUpMeasurementBase(
                              pVectorOfTrackParameters,
                              pVectorOfMeasurementBase,
                              pTrackParameters,
                              pMeasurementBase_New
                             ) ;
      }
    }else{
      const Trk::RIO_OnTrack* pRIO_OnTrack = dynamic_cast<const Trk::RIO_OnTrack*>(pMeasurementBase);
      if ( pRIO_OnTrack ){
        const Identifier channelId = pRIO_OnTrack->identify();
        if ( m_detID->is_muon(channelId) ) {
          const Trk::PrepRawData* pPrepRawData = pRIO_OnTrack->prepRawData() ; 
          if ( pPrepRawData ) {
            int IOK = 0 ;
            const Muon::MdtPrepData* pMdtPrepData = dynamic_cast<const Muon::MdtPrepData*>(pPrepRawData);
            if ( pMdtPrepData){ IOK = 1 ;
            }else{
              const Muon::CscPrepData* pCscPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepRawData);
              if ( pCscPrepData ) { IOK = 1 ;
              }else{
                const Muon::RpcPrepData* pRpcPrepData = dynamic_cast<const Muon::RpcPrepData*>(pPrepRawData);
                if ( pRpcPrepData ) { IOK = 1 ;
                }else{
                  const Muon::TgcPrepData* pTgcPrepData = dynamic_cast<const Muon::TgcPrepData*>(pPrepRawData);
                  if ( pTgcPrepData ) { IOK = 1 ;
                  }
                }
              }
            }
            if( IOK == 1 ){
              pVectorOfTrackParameters->push_back( pTrackParameters ) ;
              pVectorOfMeasurementBase->push_back( pMeasurementBase ) ;
            }
          }
        }
      }
    }
  }

}

int Trk::MeasurementBaseToDratTool::CouldBeMboySeed(const Trk::MeasurementBase* pMeasurementBase){

 if ( GetISC(pMeasurementBase) == -1 ) return 0 ;

 const Trk::RIO_OnTrack* pRIO_OnTrack = dynamic_cast<const Trk::RIO_OnTrack*>(pMeasurementBase);
 if ( pRIO_OnTrack == 0 ) return 0 ;

 const Trk::PrepRawData* pPrepRawData = pRIO_OnTrack->prepRawData() ; 

 const Muon::MdtPrepData* pMdtPrepData = dynamic_cast<const Muon::MdtPrepData*>(pPrepRawData);
 if (pMdtPrepData) {
   std::string TheSetSGStationName = m_mdtId->stationNameString( m_mdtId->stationName( pMdtPrepData->identify() ) );
   ATH_MSG_DEBUG( "CouldBeMboySeed: Test Station Name " << TheSetSGStationName );
   if ( TheSetSGStationName.substr(0,2) == "EO" ) return 0 ;
   return 1 ;
 }

 const Muon::CscPrepData* pCscPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepRawData);
 if (pCscPrepData) return 1;

 return 0 ;

}

int Trk::MeasurementBaseToDratTool::GetISC(const Trk::MeasurementBase* pMeasurementBase){ return p_OwnEDMHelper->GetISC(pMeasurementBase);}
int Trk::MeasurementBaseToDratTool::GetISC(const Trk::RIO_OnTrack* pRIO_OnTrack){ return p_OwnEDMHelper->GetISC(pRIO_OnTrack);}
int Trk::MeasurementBaseToDratTool::GetISC(const Identifier& channelId){ return p_OwnEDMHelper->GetISC(channelId);}
int Trk::MeasurementBaseToDratTool::GetISC(int StationNber){ return p_OwnEDMHelper->GetISC(StationNber);}

MbIDTagMDT*    Trk::MeasurementBaseToDratTool::GetMbIDTagMDT(const Identifier& channelId) 
{
  MbIDTagMDT* pMbIDTagLocal = new MbIDTagMDT() ;  pMbIDTagLocal->SetAreAmdbIds(0);

  int         TheSGStationNber    = m_mdtId->stationName(channelId)              ; 
  std::string TheSetSGStationName = m_mdtId->stationNameString(TheSGStationNber) ;

  pMbIDTagLocal->SetSGStationNber ( TheSGStationNber               );
  pMbIDTagLocal->SetSGStationName ( TheSetSGStationName            );
  pMbIDTagLocal->SetSGStationEta  ( m_mdtId->stationEta(channelId) );
  pMbIDTagLocal->SetSGStationPhi  ( m_mdtId->stationPhi(channelId) );
  
  pMbIDTagLocal->SetSGMultilayer  ( m_mdtId->multilayer(channelId) );
  pMbIDTagLocal->SetSGTubeLayer   ( m_mdtId->tubeLayer(channelId)  );
  pMbIDTagLocal->SetSGTube        ( m_mdtId->tube(channelId)       );

  return pMbIDTagLocal ;
}

MbIDTagCSCclu* Trk::MeasurementBaseToDratTool::GetMbIDTagCSCclu(const Identifier& channelId)  
{
  MbIDTagCSCclu* pMbIDTagLocal= new MbIDTagCSCclu() ;  pMbIDTagLocal->SetAreAmdbIds(0);

  int         TheSGStationNber    = m_cscId->stationName(channelId)              ; 
  std::string TheSetSGStationName = m_cscId->stationNameString(TheSGStationNber) ;

  pMbIDTagLocal->SetSGStationNber  ( TheSGStationNber                 );
  pMbIDTagLocal->SetSGStationName  ( TheSetSGStationName              );
  pMbIDTagLocal->SetSGStationEta   ( m_cscId->stationEta(channelId)   );
  pMbIDTagLocal->SetSGStationPhi   ( m_cscId->stationPhi(channelId)   );
  
  pMbIDTagLocal->SetSGChamberLayer ( m_cscId->chamberLayer(channelId) );
  pMbIDTagLocal->SetSGWireLayer    ( m_cscId->wireLayer(channelId)    );
  pMbIDTagLocal->SetSGMeasuresPhi  ( m_cscId->measuresPhi(channelId)  );
  pMbIDTagLocal->SetSGStrip        ( m_cscId->strip(channelId)        );

  return pMbIDTagLocal ;
}

MbIDTagRPC*    Trk::MeasurementBaseToDratTool::GetMbIDTagRPC(const Identifier& channelId) 
{
  MbIDTagRPC* pMbIDTagLocal = new MbIDTagRPC() ;  pMbIDTagLocal->SetAreAmdbIds(0);

  int         TheSGStationNber    = m_rpcId->stationName(channelId)              ; 
  std::string TheSetSGStationName = m_rpcId->stationNameString(TheSGStationNber) ;

  pMbIDTagLocal->SetSGStationNber ( TheSGStationNber                );
  pMbIDTagLocal->SetSGStationName ( TheSetSGStationName             );
  pMbIDTagLocal->SetSGStationEta  ( m_rpcId->stationEta(channelId)  );
  pMbIDTagLocal->SetSGStationPhi  ( m_rpcId->stationPhi(channelId)  );
                                                                    
  pMbIDTagLocal->SetSGDoubletR    ( m_rpcId->doubletR(channelId)    );
  pMbIDTagLocal->SetSGDoubletZ    ( m_rpcId->doubletZ(channelId)    );
  pMbIDTagLocal->SetSGDoubletPhi  ( m_rpcId->doubletPhi(channelId)  );
  pMbIDTagLocal->SetSGGasGap      ( m_rpcId->gasGap(channelId)      );
  pMbIDTagLocal->SetSGMeasuresPhi ( m_rpcId->measuresPhi(channelId) );
  pMbIDTagLocal->SetSGStrip       ( m_rpcId->strip(channelId)       );

  return pMbIDTagLocal ;
}

MbIDTagTGC*    Trk::MeasurementBaseToDratTool::GetMbIDTagTGC(const Identifier& channelId) 
{
  MbIDTagTGC* pMbIDTagLocal = new MbIDTagTGC() ;  pMbIDTagLocal->SetAreAmdbIds(0);

  int         TheSGStationNber    = m_tgcId->stationName(channelId)              ; 
  std::string TheSetSGStationName = m_tgcId->stationNameString(TheSGStationNber) ;

  pMbIDTagLocal->SetSGStationNber ( TheSGStationNber               );
  pMbIDTagLocal->SetSGStationName ( TheSetSGStationName            );
  pMbIDTagLocal->SetSGStationEta  ( m_tgcId->stationEta(channelId) );
  pMbIDTagLocal->SetSGStationPhi  ( m_tgcId->stationPhi(channelId) );

  pMbIDTagLocal->SetSGGasGap      ( m_tgcId->gasGap(channelId)     );
  pMbIDTagLocal->SetSGIsStrip     ( m_tgcId->isStrip(channelId)    );
  pMbIDTagLocal->SetSGChannel     ( m_tgcId->channel(channelId)    );

  return pMbIDTagLocal ;
}

