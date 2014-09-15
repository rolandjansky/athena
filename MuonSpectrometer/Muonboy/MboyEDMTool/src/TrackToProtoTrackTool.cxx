/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MboyCoreEvt/MbProtoTrack.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "MboyCoreEvt/MbDRAT.h"
#include "Identifier/Identifier.h"

#include "MboyEDMToolInterfaces/IMeasurementBaseToDratTool.h"

#include "MboyEDMTool/TrackToProtoTrackTool.h"

Trk::TrackToProtoTrackTool::TrackToProtoTrackTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
 p_IMeasurementBaseToDratTool    ( "Trk::MeasurementBaseToDratTool/MeasurementBaseToDratTool" ) 
{

  declareInterface<ITrackToProtoTrackTool>(this);

  declareProperty("IMeasurementBaseToDratTool"    , p_IMeasurementBaseToDratTool ) ;
  
}

Trk::TrackToProtoTrackTool::~TrackToProtoTrackTool(){}

// Initialize
StatusCode Trk::TrackToProtoTrackTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }

//Retrieve p_IMeasurementBaseToDratTool
  if ( p_IMeasurementBaseToDratTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMeasurementBaseToDratTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMeasurementBaseToDratTool );

  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode Trk::TrackToProtoTrackTool::finalize(){return StatusCode::SUCCESS;}

MbDRAT*  Trk::TrackToProtoTrackTool::MbDRATToUpdateFitParameters(
                    MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                    const Trk::Track* pTrack
)
{   

  ATH_MSG_DEBUG( "MbDRATToUpdateFitParameters: In " );
  if ( pTrack == 0 ) return 0;

  ATH_MSG_DEBUG( "MbDRATToUpdateFitParameters: 1 " );
  std::vector< const Trk::TrackParameters* > VectorOfTrackParameters_Unclustered ;
  std::vector< const Trk::MeasurementBase* > VectorOfMeasurementBase_Unclustered ;

  p_IMeasurementBaseToDratTool->PickUpMeasurementBase(
                                                      &VectorOfTrackParameters_Unclustered ,
                                                      &VectorOfMeasurementBase_Unclustered ,
                                                      pTrack
                                                     );

  ATH_MSG_DEBUG( "MbDRATToUpdateFitParameters: 2 VectorOfMeasurementBase_Unclustered.size() " << VectorOfMeasurementBase_Unclustered.size() );
  int    Max_Done    = 0  ;
  double Max_DistIP  = 0. ;
  int    Max_ISC     = 0  ;
  const Trk::TrackParameters* Max_pTrackParameters = 0 ;
  
  std::vector< const Trk::MeasurementBase* >::const_iterator it_MeasurementBase     = VectorOfMeasurementBase_Unclustered.begin() ;
  std::vector< const Trk::MeasurementBase* >::const_iterator it_MeasurementBase_End = VectorOfMeasurementBase_Unclustered.end()   ;
  std::vector< const Trk::TrackParameters* >::const_iterator it_TrackParameters     = VectorOfTrackParameters_Unclustered.begin() ;
  for( ; it_MeasurementBase != it_MeasurementBase_End; ++it_MeasurementBase,++it_TrackParameters){
    const Trk::MeasurementBase* pMeasurementBase = (*it_MeasurementBase) ;
    const Trk::TrackParameters* pTrackParameters = (*it_TrackParameters) ;

    if ( p_IMeasurementBaseToDratTool->CouldBeMboySeed(pMeasurementBase) != 0 ){
      ATH_MSG_DEBUG( "MbDRATToUpdateFitParameters: 3 CouldBeMboySeed Ok " );
    
      const Amg::Vector3D TheGlobalPosition =  pTrackParameters->position() ;
      double DistIP = TheGlobalPosition.mag() ;
      if ( Max_Done == 0 || DistIP >= Max_DistIP ){
        Max_Done = 1 ;
        Max_DistIP = DistIP ;
        Max_ISC = p_IMeasurementBaseToDratTool->GetISC(pMeasurementBase);
        Max_pTrackParameters = pTrackParameters ;
      }
      
    }
  }
  
  ATH_MSG_DEBUG( "MbDRATToUpdateFitParameters: 4 " );
  if ( Max_Done == 0 ) return 0 ;
  
  ATH_MSG_DEBUG( "MbDRATToUpdateFitParameters: 5 " );
  MbDRAT* pMbDRAT = new MbDRAT() ;
  
  pMbDRAT->m_CCDRAT = 0 ;;
  
  pMbDRAT->m_STRICT =  0 ;

  const Trk::TrackParameters* pTrackParameters = Max_pTrackParameters ;

  const Amg::Vector3D TheGlobalPosition =  pTrackParameters->position() ;
  pMbDRAT->m_Xpoint = TheGlobalPosition.x() ;  
  pMbDRAT->m_Ypoint = TheGlobalPosition.y() ;  
  pMbDRAT->m_Zpoint = TheGlobalPosition.z() ;  

  Amg::Vector3D TheGlobalDirection = pTrackParameters->momentum() ;

  pMbDRAT->m_Mome   = TheGlobalDirection.mag() ;  
  pMbDRAT->m_Charge = pTrackParameters->charge() ;  

  TheGlobalDirection.normalize() ;
  pMbDRAT->m_Xdir = TheGlobalDirection.x() ;  
  pMbDRAT->m_Ydir = TheGlobalDirection.y() ;  
  pMbDRAT->m_Zdir = TheGlobalDirection.z() ;  
  
  it_MeasurementBase     = VectorOfMeasurementBase_Unclustered.begin() ;
  it_MeasurementBase_End = VectorOfMeasurementBase_Unclustered.end()   ;
  for( ; it_MeasurementBase != it_MeasurementBase_End; ++it_MeasurementBase){
    const Trk::MeasurementBase* pMeasurementBase = (*it_MeasurementBase) ;
    if ( p_IMeasurementBaseToDratTool->GetISC(pMeasurementBase) == Max_ISC ){
      p_IMeasurementBaseToDratTool->MeasurementBaseToMbDRAT(
                                                            pMbDRAT,
                                                            pMuonRecDigiStoreAth ,
                                                            pMeasurementBase
                                                           ) ;
    }
  }
  ATH_MSG_DEBUG( "MbDRATToUpdateFitParameters: 6 " );

 return pMbDRAT;

}

MbProtoTrack* Trk::TrackToProtoTrackTool::convert (
                    MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                    const Trk::Track* pTrack
)
{   

  if ( pTrack == 0 ) return 0 ;

  std::vector<int> VectorOfISC ; 
  std::vector< const Trk::TrackParameters* > VectorOfTrackParameters ; 
  std::vector< std::vector<const Trk::MeasurementBase* > > VectorOfVectorOfMeasurementBase ; 

  p_IMeasurementBaseToDratTool->PickUpMeasurementBaseAndCluster(
                                                                &VectorOfISC  ,
                                                                &VectorOfTrackParameters ,
                                                                &VectorOfVectorOfMeasurementBase,
                                                                pTrack
                                                               );

  if ( VectorOfISC.size() == 0 ) return 0 ;
  
  MbProtoTrack* pMbProtoTrack = new MbProtoTrack() ;
  pMbProtoTrack->m_Own_m_VectorOfMbDRAT = 1 ;
  
  std::vector<int>::iterator it_ISC                           = VectorOfISC.begin() ;
  std::vector<int>::iterator it_ISC_End                       = VectorOfISC.end()   ;
  std::vector<const Trk::TrackParameters*>::const_iterator it_TrackParameters = VectorOfTrackParameters.begin() ;
  std::vector< std::vector<const Trk::MeasurementBase* > >::const_iterator it_VectorOfMeasurementBase = VectorOfVectorOfMeasurementBase.begin()   ;
  for( ; it_ISC != it_ISC_End; ++it_ISC,++it_TrackParameters,++it_VectorOfMeasurementBase){
    std::vector<const Trk::MeasurementBase*> VectorOfMeasurementBase = (*it_VectorOfMeasurementBase) ;
    if ( VectorOfMeasurementBase.size() >= 4 ){
      MbDRAT* pMbDRAT = new MbDRAT() ;
      
      pMbDRAT->m_CCDRAT = 0 ;;
      
      pMbDRAT->m_STRICT =  0 ;

      const Trk::TrackParameters* pTrackParameters = (*it_TrackParameters) ;

      const Amg::Vector3D TheGlobalPosition =  pTrackParameters->position() ;
      pMbDRAT->m_Xpoint = TheGlobalPosition.x() ;  
      pMbDRAT->m_Ypoint = TheGlobalPosition.y() ;  
      pMbDRAT->m_Zpoint = TheGlobalPosition.z() ;  

      Amg::Vector3D TheGlobalDirection =  pTrackParameters->momentum() ;

      pMbDRAT->m_Mome   = TheGlobalDirection.mag() ;  
      pMbDRAT->m_Charge = pTrackParameters->charge() ;  

      TheGlobalDirection.normalize() ;
      pMbDRAT->m_Xdir = TheGlobalDirection.x() ;  
      pMbDRAT->m_Ydir = TheGlobalDirection.y() ;  
      pMbDRAT->m_Zdir = TheGlobalDirection.z() ;  
      
      (pMbDRAT->m_VectorOfMbIDTagMDT    ).clear();
      (pMbDRAT->m_VectorOfMbIDTagCSCclu ).clear();
      (pMbDRAT->m_VectorOfMbIDTagRPC    ).clear();
      (pMbDRAT->m_VectorOfMbIDTagTGC    ).clear();

      std::vector<const Trk::MeasurementBase* >::const_iterator it_MeasurementBase     = VectorOfMeasurementBase.begin() ;
      std::vector<const Trk::MeasurementBase* >::const_iterator it_MeasurementBase_End = VectorOfMeasurementBase.end()   ;
      for( ; it_MeasurementBase != it_MeasurementBase_End; ++it_MeasurementBase){
        const Trk::MeasurementBase* pMeasurementBase = (*it_MeasurementBase) ;
        p_IMeasurementBaseToDratTool->MeasurementBaseToMbDRAT(
                                                              pMbDRAT,
                                                              pMuonRecDigiStoreAth ,
                                                              pMeasurementBase
                                                             ) ;
      }
      (pMbProtoTrack->m_VectorOfMbDRAT).push_back(  pMbDRAT ) ;
    }
  }

  return pMbProtoTrack ;

}
