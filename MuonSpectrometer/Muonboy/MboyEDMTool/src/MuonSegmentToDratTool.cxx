/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MuonSegment/MuonSegment.h"
#include "MboyCoreEvt/MbDRAT.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "MuonSegment/MuonSegmentQuality.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "MboyEDMToolInterfaces/IMeasurementBaseToDratTool.h"

#include "MboyEDMTool/MuonSegmentToDratTool.h"

Trk::MuonSegmentToDratTool::MuonSegmentToDratTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
 p_IMeasurementBaseToDratTool    ( "Trk::MeasurementBaseToDratTool/MeasurementBaseToDratTool" ) 
{

  declareInterface<IMuonSegmentToDratTool>(this);

  declareProperty("IMeasurementBaseToDratTool"    , p_IMeasurementBaseToDratTool ) ;
  
}

Trk::MuonSegmentToDratTool::~MuonSegmentToDratTool(){}

// Initialize
StatusCode Trk::MuonSegmentToDratTool::initialize(){

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

StatusCode Trk::MuonSegmentToDratTool::finalize(){return StatusCode::SUCCESS;}

MbDRAT*  Trk::MuonSegmentToDratTool::convert (
                    MuonRecDigiStoreAth*     pMuonRecDigiStoreAth ,
                    const Muon::MuonSegment* pMuonSegment
)
{   
 ATH_MSG_DEBUG( "convert :   In " );
 if ( pMuonSegment == 0 ) return 0 ;

 MbDRAT* pMbDRAT = new MbDRAT() ;
 
 pMbDRAT->m_CCDRAT = (pMuonSegment->fitQuality())->chiSquared() ;
 
 pMbDRAT->m_STRICT =  0 ;
 const Muon::MuonSegmentQuality* pMuonSegmentQuality = dynamic_cast<const Muon::MuonSegmentQuality*>(pMuonSegment->fitQuality());
 if( pMuonSegmentQuality ){
   if ( pMuonSegmentQuality->isStrict() ) pMbDRAT->m_STRICT = 1 ;
 }

 const Amg::Vector3D TheGlobalPosition =  pMuonSegment->globalPosition() ;
 pMbDRAT->m_Xpoint = TheGlobalPosition.x() ;  
 pMbDRAT->m_Ypoint = TheGlobalPosition.y() ;  
 pMbDRAT->m_Zpoint = TheGlobalPosition.z() ;  

 Amg::Vector3D TheGlobalDirection =  pMuonSegment->globalDirection() ;

 pMbDRAT->m_Mome   = 100000000. ;
 pMbDRAT->m_Charge = 1.         ;

 TheGlobalDirection.normalize() ;

 pMbDRAT->m_Xdir = TheGlobalDirection.x() ;  
 pMbDRAT->m_Ydir = TheGlobalDirection.y() ;  
 pMbDRAT->m_Zdir = TheGlobalDirection.z() ;  

 std::vector< const Trk::TrackParameters* > VectorOfTrackParameters ;
 std::vector< const Trk::MeasurementBase* > VectorOfMeasurementBase ;
 const Trk::TrackParameters* pTrackParameters = 0 ;
 p_IMeasurementBaseToDratTool->PickUpMeasurementBase (
                                                      &VectorOfTrackParameters ,
                                                      &VectorOfMeasurementBase ,
                                                      pTrackParameters,
                                                      pMuonSegment
                                                     );
 ATH_MSG_DEBUG( "convert :   VectorOfTrackParameters.size()   " << VectorOfTrackParameters.size() );
                                                    
 std::vector< const Trk::MeasurementBase* >::const_iterator it_MeasurementBase     = VectorOfMeasurementBase.begin() ;
 std::vector< const Trk::MeasurementBase* >::const_iterator it_MeasurementBase_End = VectorOfMeasurementBase.end()   ;
 for( ; it_MeasurementBase != it_MeasurementBase_End; ++it_MeasurementBase){
   const Trk::MeasurementBase* pMeasurementBase = (*it_MeasurementBase) ;
   p_IMeasurementBaseToDratTool->MeasurementBaseToMbDRAT( 
                                                         pMbDRAT,
                                                         pMuonRecDigiStoreAth ,
                                                         pMeasurementBase
                                                        );
 }

 return pMbDRAT;

}
