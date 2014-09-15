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
 
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyDumpTrackCollectionTool.h"

//----------------------------------------------------------------//
#include "MboyEDMToolInterfaces/IMboyEDMHelper.h"

MboyDumpTrackCollectionTool::MboyDumpTrackCollectionTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
p_OwnEDMHelper ( "MboyEDMHelper" ) 
{

  declareInterface<IMboyDumpTrackCollectionTool>(this);

  declareProperty("MboyEDMHelper" , p_OwnEDMHelper ) ;

}

MboyDumpTrackCollectionTool::~MboyDumpTrackCollectionTool(){}

// Initialize
StatusCode MboyDumpTrackCollectionTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }


//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved tool " << p_OwnEDMHelper ) ;


  return StatusCode::SUCCESS;

}

StatusCode MboyDumpTrackCollectionTool::DoDump(
                     std::string TrackCollectionLocation,
		     std::ofstream* pOutFile
){

  StatusCode sc = StatusCode::SUCCESS;

  p_OutCurrent = pOutFile ;
  std::string ContainerName    = TrackCollectionLocation ;
  sc = dump_Container(ContainerName);
  if (sc.isFailure()){
    ATH_MSG_ERROR( "dump_Container failed " ) ;
  }

  return StatusCode::SUCCESS;
  
}


// Dumps track
StatusCode MboyDumpTrackCollectionTool::dump_Container(
                             std::string ContainerName
){

  StatusCode sc = StatusCode::SUCCESS;

  const TrackCollection* pTrackCollection ;
  sc = evtStore()->retrieve(pTrackCollection, ContainerName); 
  if (sc.isFailure()){
    ATH_MSG_ERROR( "pTrackCollection not found at" << ContainerName ) ;
    return( StatusCode::SUCCESS );
  }

  const DataHandle<EventInfo> TheEventInfo;
  sc = evtStore()->retrieve(TheEventInfo);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " retrieve TheEventInfo failed" ) ;
    return( StatusCode::FAILURE );
  }

  *p_OutCurrent 
      << " " 
      <<std::endl;
  *p_OutCurrent 
      << "* For the event : " 
      << TheEventInfo->event_ID()->event_number()
      << std::endl;

  int Kounter = 0 ;
  for (TrackCollection::const_iterator it = pTrackCollection->begin(); it!=pTrackCollection->end(); ++it){

    const Trk::Track* pTrack = (*it);

    Kounter = Kounter + 1 ;
    *p_OutCurrent 
        << " " 
        <<std::endl;
    *p_OutCurrent 
        << "* Track number  " 
        << Kounter 
        <<std::endl;

    const Trk::FitQuality* pFitQuality = pTrack->fitQuality() ;
    *p_OutCurrent 
        << "Fit Quality   " 
        << " " << std::setw(12)<<std::setprecision(5) << pFitQuality->chiSquared ()
        <<std::endl;
    *p_OutCurrent 
        << "Nber of dof   " 
        << " " << std::setw(12) << pFitQuality->numberDoF ()
        <<std::endl;

    if (pTrack->measurementsOnTrack()!=0) 
      *p_OutCurrent <<  "*  It has " << pTrack->measurementsOnTrack()->size()<< " RIO_OnTrack(s)"<<std::endl;

    if (pTrack->trackStateOnSurfaces()!=0) 
      *p_OutCurrent <<  "*  It has " << pTrack->trackStateOnSurfaces()->size()<< " TrackStateOnSurface(s)"<<std::endl;

    if (pTrack->trackParameters()!=0)
      *p_OutCurrent  << "*  It has " << pTrack->trackParameters()->size()     << " tracks parameter(s)"<<std::endl;

    double Eloss = 0. ;
    sc = p_OwnEDMHelper->BackTrackingEloss(pTrack,Eloss) ;
    if (sc.isSuccess()){
     *p_OutCurrent <<  "*  Its Eloss is: "        
                            << std::setw(12)<<std::setprecision(5)
                            << Eloss 
                            << std::endl;
    }else{
     *p_OutCurrent <<  "*  Its Eloss can not be computed "        
                            << std::endl;
    }

    sc = p_OwnEDMHelper->dump_TrkTrack(pTrack,p_OutCurrent);
    if (sc.isFailure()) ATH_MSG_WARNING( "dump_TrkTrack failed" ) ;

    *p_OutCurrent <<  "*  Its digits are:"<<std::endl;
    sc = p_OwnEDMHelper->dump_TrkTrackDigits(pTrack,p_OutCurrent);
    if (sc.isFailure()) ATH_MSG_WARNING( "dump_TrkTrackDigits failed" ) ;

  }

  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyDumpTrackCollectionTool::finalize(){return StatusCode::SUCCESS;}

