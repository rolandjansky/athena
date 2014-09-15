/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyTrackUndressingTool.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/TrackInfo.h"
//----------------------------------------------------------------//
#include "MboyEDMToolInterfaces/IMboyDetectorLimitsTool.h"
#include "MboyEDMToolInterfaces/IMboyTrackRepresentationTransformTool.h"

namespace Muon {

MboyTrackUndressingTool::MboyTrackUndressingTool(const std::string& t, 
                                             const std::string& n,
                                             const IInterface*  p ):AthAlgTool(t,n,p)
, p_IMboyDetectorLimitsTool          ( "Muon::MboyDetectorLimitsTool/MboyDetectorLimitsTool" )
, p_IMboyTrackRepresentationTransformTool          ( "Muon::MboyTrackRepresentationTransformTool/MboyTrackRepresentationTransformTool" )
{

  declareInterface<IMboyTrackUndressingTool>(this);

//Declare the properties

  declareProperty("IMboyDetectorLimitsTool"    , p_IMboyDetectorLimitsTool ) ;
  declareProperty("IMboyTrackRepresentationTransformTool"    , p_IMboyTrackRepresentationTransformTool ) ;

  declareProperty("ToolName"          , m_ToolName          = n                           ) ;

}

MboyTrackUndressingTool::~MboyTrackUndressingTool(){}

// Initialize
StatusCode MboyTrackUndressingTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are     " );
  ATH_MSG_INFO( "= ToolName                               " << m_ToolName                               ) ;
  ATH_MSG_INFO( "================================" );

//Retrieve p_IMboyDetectorLimitsTool
  if ( p_IMboyDetectorLimitsTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyDetectorLimitsTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyDetectorLimitsTool );

  ATH_MSG_INFO(  "Initialisation ended     " );

//Retrieve p_IMboyTrackRepresentationTransformTool
  if ( p_IMboyTrackRepresentationTransformTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyTrackRepresentationTransformTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyTrackRepresentationTransformTool );

  ATH_MSG_INFO(  "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode MboyTrackUndressingTool::finalize(){return StatusCode::SUCCESS;}


Trk::Track* MboyTrackUndressingTool::UndressTrack (const Trk::Track* pTrack_atIP)
{
  if ( pTrack_atIP == 0 ) {
    ATH_MSG_DEBUG( " UndressTrack returns 0 since pTrack_atIP is 0 ") ;
    return 0 ;
  }
  
  DataVector<const Trk::TrackStateOnSurface>* trackStates = UndressTrackTSOSs(pTrack_atIP);
  if ( trackStates == 0 )  {
    ATH_MSG_DEBUG( " UndressTrack returns 0 since trackStates is 0 ") ;
    return 0 ;
  }

  return new Trk::Track(   
                        Trk::TrackInfo(pTrack_atIP->info()),
                        trackStates,
                        (pTrack_atIP->fitQuality())->clone()
                       );

}

DataVector<const Trk::TrackStateOnSurface>* MboyTrackUndressingTool::UndressTrackTSOSs(const Trk::Track* pTrack_atIP)
{

  if ( pTrack_atIP == 0) {
    ATH_MSG_DEBUG( " UndressTrackTSOSs returns 0 since pTrack_atIP is 0 ") ;
    return 0 ;
  }

  const DataVector<const Trk::TrackStateOnSurface>* pTrack_OldStates = pTrack_atIP->trackStateOnSurfaces();
  if (pTrack_OldStates == 0) {
    ATH_MSG_DEBUG( " UndressTrackTSOSs returns 0 since pTrack_OldStates is 0 ") ;
    return 0 ;
  }

  DataVector<const Trk::TrackStateOnSurface>* trackStates = new DataVector<const Trk::TrackStateOnSurface>;
  
  bool DoCloning        = false ;
  bool DoReturnNewStuff = false ;
  
  for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoS = pTrack_OldStates->begin(); itTSoS!=pTrack_OldStates->end(); ++itTSoS) {

    if ( !DoCloning ){
      if ( (*itTSoS)->trackParameters() != 0 ){
        const Trk::TrackParameters* pTrackParameters = (*itTSoS)->trackParameters() ;
        int TheAreaTag = p_IMboyDetectorLimitsTool->AreaTag( pTrackParameters );
        if ( TheAreaTag >= 3) DoCloning = true ;
      } 
    }
    
    if ( DoCloning && (*itTSoS) != 0 ) {
      DoReturnNewStuff = true ;        
      Trk::TrackStateOnSurface* pTSOS  = AtACOrAtAD2TSOS( (*itTSoS)->trackParameters() ) ;
      if (pTSOS){
        trackStates->push_back( (*itTSoS)->clone() );
        trackStates->push_back( pTSOS );
        continue ;
      }
      trackStates->push_back( (*itTSoS)->clone() );
    }
    
  }

  if (DoReturnNewStuff) return trackStates ;

  delete trackStates ;
  
  ATH_MSG_DEBUG( " UndressTrackTSOSs returns 0 since DoReturnNewStuff was false") ;
  return 0 ;
  
}

Trk::TrackStateOnSurface* MboyTrackUndressingTool::AtACOrAtAD2TSOS(const Trk::TrackParameters* pTrackParameters)
{
 
  if (!pTrackParameters) return 0 ;
  
  const Trk::TrackParameters* pTrackParametersN  = p_IMboyTrackRepresentationTransformTool->AtACOrAtAD2MeasuredPerigee( pTrackParameters ) ;
  if (!pTrackParametersN) return 0 ;

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern;
  pattern.set(Trk::TrackStateOnSurface::Perigee);

  return new Trk::TrackStateOnSurface(0 , pTrackParametersN, 0,  0, pattern ) ;

}

}
