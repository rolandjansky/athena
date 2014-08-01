/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkTrack/TrackStateOnSurface.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewTSOSDumpTool.h"

static const InterfaceID IID_IMboyViewTSOSDumpTool("MboyViewTSOSDumpTool", 1, 0);

const InterfaceID& MboyViewTSOSDumpTool::interfaceID()
{
  return IID_IMboyViewTSOSDumpTool;
}

MboyViewTSOSDumpTool::MboyViewTSOSDumpTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewTSOSDumpTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

}

MboyViewTSOSDumpTool::~MboyViewTSOSDumpTool(){}

// Initialize
StatusCode MboyViewTSOSDumpTool::initialize(){

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

StatusCode MboyViewTSOSDumpTool::finalize(){return StatusCode::SUCCESS;}

void MboyViewTSOSDumpTool::DumpTSOSs(
               const Trk::Track* pTrack ,
               std::ofstream* pOut
){
 
  if ( pTrack == 0 ) return ;

  const DataVector< const Trk::TrackStateOnSurface >* pVecTSOS  = pTrack->trackStateOnSurfaces() ;  

  DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoS    =  pVecTSOS->begin() ;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoSEnd =  pVecTSOS->end()   ;
  for( ; itTSoS!=itTSoSEnd; ++itTSoS ){
    const Trk::TrackParameters* pTrackParameters = (*itTSoS)->trackParameters() ;
    if (pTrackParameters){
      *pOut 
        << "TSO "
        << std::setw(12)<<std::setprecision(3)<< (pTrackParameters->position()).x()
        << std::setw(12)<<std::setprecision(3)<< (pTrackParameters->position()).y()
        << std::setw(12)<<std::setprecision(3)<< (pTrackParameters->position()).z()
        << std::setw(12)                      << (pTrackParameters->momentum()).x()
        << std::setw(12)                      << (pTrackParameters->momentum()).y()
        << std::setw(12)                      << (pTrackParameters->momentum()).z()
        ;
      *pOut << std::endl;

    }
  }

}
