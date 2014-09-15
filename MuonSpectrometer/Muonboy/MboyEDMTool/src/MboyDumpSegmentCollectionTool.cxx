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
 
#include "Identifier/Identifier.h"

#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyDumpSegmentCollectionTool.h"

//----------------------------------------------------------------//
#include "MboyEDMToolInterfaces/IMboyEDMHelper.h"

MboyDumpSegmentCollectionTool::MboyDumpSegmentCollectionTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
 p_OwnEDMHelper              ( "MboyEDMHelper" ) 
{

  declareInterface<IMboyDumpSegmentCollectionTool>(this);

  declareProperty("MboyEDMHelper" , p_OwnEDMHelper ) ;

}

MboyDumpSegmentCollectionTool::~MboyDumpSegmentCollectionTool(){}

// Initialize
StatusCode MboyDumpSegmentCollectionTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
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

StatusCode MboyDumpSegmentCollectionTool::DoDump(
                     std::string m_SegmentCollectionLocation,
                     std::string m_TrackCollectionLocation,
		     std::ofstream* pOutFile
){

  StatusCode sc = StatusCode::SUCCESS;


  p_OutCurrent = pOutFile ;
  std::string ContainerName    = m_SegmentCollectionLocation ;
  std::string ContainerNameAux = m_TrackCollectionLocation  ;
  sc = dump_Container(ContainerName,ContainerNameAux);
  if (sc.isFailure()){
    ATH_MSG_WARNING( "dump_Container failed " ) ;
  }
  
  
  return StatusCode::SUCCESS;

}
StatusCode MboyDumpSegmentCollectionTool::DoDump(
                     std::string m_SegmentCollectionLocation,
		     std::ofstream* pOutFile
){

  StatusCode sc = StatusCode::SUCCESS;


  p_OutCurrent = pOutFile ;
  std::string ContainerName    = m_SegmentCollectionLocation ;
  sc = dump_Container(ContainerName);
  if (sc.isFailure()){
    ATH_MSG_WARNING( "dump_Container failed " ) ;
  }
  
  
  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyDumpSegmentCollectionTool::finalize(){return StatusCode::SUCCESS;}

// Dumps Segments
StatusCode MboyDumpSegmentCollectionTool::dump_Container(
                             std::string ContainerName,
                             std::string ContainerNameAux
){

  StatusCode sc = StatusCode::SUCCESS;

  const Trk::SegmentCollection* pSegmentCollection ;
  sc = evtStore()->retrieve(pSegmentCollection, ContainerName);
  if (sc.isFailure()){
    ATH_MSG_FATAL( "pSegmentCollection not found at " << ContainerName ) ;
    return( StatusCode::FAILURE );
  }

  const TrackCollection* pTrackCollection ;
  sc = evtStore()->retrieve(pTrackCollection, ContainerNameAux);
  if (sc.isFailure()){
    ATH_MSG_FATAL( "pTrackCollection not found at" << ContainerNameAux ) ;
    return( StatusCode::FAILURE );
  }

//Check event
  const DataHandle<EventInfo> TheEventInfo;
  sc = evtStore()->retrieve(TheEventInfo);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " retrieve TheEventInfo failed" ) ;
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
  for (Trk::SegmentCollection::const_iterator it = pSegmentCollection->begin(); it!=pSegmentCollection->end(); ++it){

    const Trk::Segment* pSegment = (*it);

    Kounter = Kounter + 1 ;
    *p_OutCurrent 
        << " " 
        <<std::endl;
    *p_OutCurrent 
        << "* Segment number  " 
        << Kounter 
        <<std::endl;
    *p_OutCurrent 
        << "Segment Station Name   " 
        << " " << p_OwnEDMHelper->SegmentAssociatedStationName(pSegment)
        <<std::endl;
    *p_OutCurrent 
        << "Fit Quality   " 
        << " " << std::setw(12)<<std::setprecision(5) << p_OwnEDMHelper->GetfitQuality(pSegment)
        <<std::endl;
    *p_OutCurrent << "Nber of Digits   " << " " << p_OwnEDMHelper->GetNberOfDigi(pSegment) <<std::endl;
    *p_OutCurrent << "Nber of Digits MDT  " << " " << p_OwnEDMHelper->GetNberOfDigiMDT (pSegment) <<std::endl;
    *p_OutCurrent << "Nber of Digits RPC  " << " " << p_OwnEDMHelper->GetNberOfDigiRPC (pSegment) <<std::endl;
    *p_OutCurrent << "Nber of Digits TGC  " << " " << p_OwnEDMHelper->GetNberOfDigiTGC (pSegment) <<std::endl;
    *p_OutCurrent << "Nber of Digits CSC  " << " " << p_OwnEDMHelper->GetNberOfDigiCSC (pSegment) <<std::endl;

    int SharingState = 0 ;
    for (TrackCollection::const_iterator itTrack = pTrackCollection->begin(); itTrack!=pTrackCollection->end(); ++itTrack){
      const Trk::Track* pTrack = (*itTrack);
      if ( p_OwnEDMHelper->IsThisSegmentInThisTrack(pSegment,pTrack) == 1 ){
        SharingState = 1 ;
        break;
      }
    }
    *p_OutCurrent 
       << "Segment Sharing Status " 
       << " " << std::setw(12)<<std::setprecision(5) <<SharingState
       <<std::endl;

    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if ( pMuonSegment ){
      *p_OutCurrent << "t0        "   << " " << pMuonSegment->time()        <<std::endl;
      *p_OutCurrent << "t0Error   "   << " " << pMuonSegment->errorTime()   <<std::endl;
      *p_OutCurrent << "hasFittedT0 " << " " << pMuonSegment->hasFittedT0() <<std::endl;
      const Muon::MuonSegmentQuality* pMuonSegmentQuality = dynamic_cast<const Muon::MuonSegmentQuality*>(pMuonSegment->fitQuality());
      if( pMuonSegmentQuality ){
        if ( pMuonSegmentQuality->isStrict() ) {
          *p_OutCurrent<< " Strict Segment "
	     << std::endl;
        }else{
          *p_OutCurrent<< " Loose Segment  "
	     << std::endl;
        }
      }
    }
    
    sc = p_OwnEDMHelper->dump_TrkSegment(pSegment,p_OutCurrent);
    if (sc.isFailure()) ATH_MSG_WARNING( "dump_TrkSegment failed" ) ;
    *p_OutCurrent <<  "*  Its digits are:"<<std::endl;
    sc = p_OwnEDMHelper->dump_TrkSegmentDigits(pSegment,p_OutCurrent);
    if (sc.isFailure()) ATH_MSG_WARNING( "dump_TrkSegmentDigits failed" ) ;
    *p_OutCurrent <<  "*  Its holes are:"<<std::endl;
    sc = p_OwnEDMHelper->dump_TrkSegmentHoles(pSegment,p_OutCurrent);
    if (sc.isFailure()) ATH_MSG_WARNING( "dump_TrkSegmentDigits failed" ) ;

  }

  return StatusCode::SUCCESS;

}
StatusCode MboyDumpSegmentCollectionTool::dump_Container(
                             std::string ContainerName
){

  StatusCode sc = StatusCode::SUCCESS;

  const Trk::SegmentCollection* pSegmentCollection ;
  sc = evtStore()->retrieve(pSegmentCollection, ContainerName);
  if (sc.isFailure()){
    ATH_MSG_FATAL( "pSegmentCollection not found at " << ContainerName ) ;
    return( StatusCode::FAILURE );
  }

//Check event
  const DataHandle<EventInfo> TheEventInfo;
  sc = evtStore()->retrieve(TheEventInfo);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " retrieve TheEventInfo failed" ) ;
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
  for (Trk::SegmentCollection::const_iterator it = pSegmentCollection->begin(); it!=pSegmentCollection->end(); ++it){

    const Trk::Segment* pSegment = (*it);

    Kounter = Kounter + 1 ;
    *p_OutCurrent 
        << " " 
        <<std::endl;
    *p_OutCurrent 
        << "* Segment number  " 
        << Kounter 
        <<std::endl;
    *p_OutCurrent 
        << "Segment Station Name   " 
        << " " << p_OwnEDMHelper->SegmentAssociatedStationName(pSegment)
        <<std::endl;
    *p_OutCurrent 
        << "Fit Quality   " 
        << " " << std::setw(12)<<std::setprecision(5) << p_OwnEDMHelper->GetfitQuality(pSegment)
        <<std::endl;
    *p_OutCurrent << "Nber of Digits   " << " " << p_OwnEDMHelper->GetNberOfDigi(pSegment) <<std::endl;
    *p_OutCurrent << "Nber of Digits MDT  " << " " << p_OwnEDMHelper->GetNberOfDigiMDT (pSegment) <<std::endl;
    *p_OutCurrent << "Nber of Digits RPC  " << " " << p_OwnEDMHelper->GetNberOfDigiRPC (pSegment) <<std::endl;
    *p_OutCurrent << "Nber of Digits TGC  " << " " << p_OwnEDMHelper->GetNberOfDigiTGC (pSegment) <<std::endl;
    *p_OutCurrent << "Nber of Digits CSC  " << " " << p_OwnEDMHelper->GetNberOfDigiCSC (pSegment) <<std::endl;

    const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
    if ( pMuonSegment ){
      *p_OutCurrent << "t0        "   << " " << pMuonSegment->time()        <<std::endl;
      *p_OutCurrent << "t0Error   "   << " " << pMuonSegment->errorTime()   <<std::endl;
      *p_OutCurrent << "hasFittedT0 " << " " << pMuonSegment->hasFittedT0() <<std::endl;
      const Muon::MuonSegmentQuality* pMuonSegmentQuality = dynamic_cast<const Muon::MuonSegmentQuality*>(pMuonSegment->fitQuality());
      if( pMuonSegmentQuality ){
        if ( pMuonSegmentQuality->isStrict() ) {
          *p_OutCurrent<< " Strict Segment "
	     << std::endl;
        }else{
          *p_OutCurrent<< " Loose Segment  "
	     << std::endl;
        }
      }
    }
    
    sc = p_OwnEDMHelper->dump_TrkSegment(pSegment,p_OutCurrent);
    if (sc.isFailure()) ATH_MSG_WARNING( "dump_TrkSegment failed" ) ;
    *p_OutCurrent <<  "*  Its digits are:"<<std::endl;
    sc = p_OwnEDMHelper->dump_TrkSegmentDigits(pSegment,p_OutCurrent);
    if (sc.isFailure()) ATH_MSG_WARNING( "dump_TrkSegmentDigits failed" ) ;
    *p_OutCurrent <<  "*  Its holes are:"<<std::endl;
    sc = p_OwnEDMHelper->dump_TrkSegmentHoles(pSegment,p_OutCurrent);
    if (sc.isFailure()) ATH_MSG_WARNING( "dump_TrkSegmentDigits failed" ) ;

  }

  return StatusCode::SUCCESS;

}
