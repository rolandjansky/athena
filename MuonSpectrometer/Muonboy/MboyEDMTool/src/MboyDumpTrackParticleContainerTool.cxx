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

#include "Particle/TrackParticleContainer.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyDumpTrackParticleContainerTool.h"

//----------------------------------------------------------------//
#include "MboyEDMToolInterfaces/IMboyEDMHelper.h"

MboyDumpTrackParticleContainerTool::MboyDumpTrackParticleContainerTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
 p_OwnEDMHelper              ( "MboyEDMHelper" ) 
{

  declareInterface<IMboyDumpTrackParticleContainerTool>(this);

  declareProperty("MboyEDMHelper" , p_OwnEDMHelper ) ;

}

MboyDumpTrackParticleContainerTool::~MboyDumpTrackParticleContainerTool(){}

// Initialize
StatusCode MboyDumpTrackParticleContainerTool::initialize(){

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

StatusCode MboyDumpTrackParticleContainerTool:: DoDump(
                     std::string TrackParticleContainerLocation,
		     std::ofstream* pOutFile
){

  StatusCode sc = StatusCode::SUCCESS;

  p_OutCurrent = pOutFile ;
  sc = dump_Container(TrackParticleContainerLocation);
  if (sc.isFailure()) ATH_MSG_WARNING( "dump_Container failed " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyDumpTrackParticleContainerTool::dump_Container(
                             std::string ContainerName
){

  StatusCode sc = StatusCode::SUCCESS;

  const Rec::TrackParticleContainer * pTrackParticleContainer;
  sc = evtStore()->retrieve(pTrackParticleContainer, ContainerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( " pTrackParticleContainer not found at " << ContainerName ) ;
    return StatusCode::SUCCESS;
  }

  const DataHandle<EventInfo> TheEventInfo;
  sc = evtStore()->retrieve(TheEventInfo);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING( " retrieve TheEventInfo failed" ) ;
    return StatusCode::SUCCESS;
  }

  *p_OutCurrent 
      << " " 
      <<std::endl;
  *p_OutCurrent 
      << "* For the event : " 
      << TheEventInfo->event_ID()->event_number()
      << std::endl;

  int Kounter = 0 ;
  for (Rec::TrackParticleContainer::const_iterator it = pTrackParticleContainer->begin(); it!=pTrackParticleContainer->end(); ++it){

    const Trk::Track* pTrack    = (*it)->originalTrack();

    Kounter = Kounter + 1 ;
    *p_OutCurrent 
        << " " 
        <<std::endl;
    *p_OutCurrent 
        << "* Track number  " 
        << Kounter 
        <<std::endl;

    *p_OutCurrent <<  "* dump it " <<std::endl;
    *p_OutCurrent <<   *(*it)      <<std::endl;

    if (pTrack->measurementsOnTrack()!=0) 
      *p_OutCurrent <<  "*  It has " << pTrack->measurementsOnTrack()->size()<< " RIO_OnTrack(s)"<<std::endl;

    if (pTrack->trackStateOnSurfaces()!=0) 
      *p_OutCurrent <<  "*  It has " << pTrack->trackStateOnSurfaces()->size()<< " TrackStateOnSurface(s)"<<std::endl;

    if (pTrack->trackParameters()!=0)
      *p_OutCurrent  << "*  It has " << pTrack->trackParameters()->size()     << " tracks parameter(s)"<<std::endl;

    *p_OutCurrent   << " -----TrackSummary---------  " << std::endl;
    if ((*it)->trackSummary()) {
      *p_OutCurrent   <<*((*it)->trackSummary())          << std::endl;
    }else{
      *p_OutCurrent   << "((*it)->trackSummary()) is zero "         << std::endl;
    }

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

StatusCode MboyDumpTrackParticleContainerTool::finalize(){return StatusCode::SUCCESS;}
