/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_TrigStandaloneTrackFinder
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 16/05/2008 Iwona Grabowska-Bold
///////////////////////////////////////////////////////////////////

#include "TRT_TrigStandaloneTrackFinder/TRT_TrigStandaloneTrackFinder.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkTrack/TrackInfo.h"

//Eigen
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "InDetRecToolInterfaces/ITRT_SegmentToTrackTool.h"

using Amg::Vector3D;
using CLHEP::mm;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrigStandaloneTrackFinder::TRT_TrigStandaloneTrackFinder
(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
    m_nprint(0),
    m_segToTrackTool("InDet::TRT_SegmentToTrackTool"),
    m_ntimesInvoked(0)
{
  m_minNumDriftCircles = 15                                   ;       //Minimum number of drift circles for TRT segment tracks
  m_matEffects         = 0                                    ;
  m_resetPRD           = false                                ;       //Reset PRD association tool during sub-detector pattern

  declareProperty("MinNumDriftCircles"         ,m_minNumDriftCircles); //Minimum number of drift circles for TRT segment tracks
  declareMonitoredVariable("numTRTSeg"         ,m_nTrtSeg           ); //monitor input TRT segments
  declareMonitoredVariable("numTRTTracks"      ,m_nTRTTracks        ); //monitor output TRT standalone tracks
  declareProperty("MaterialEffects"            ,m_matEffects        ); //Particle hypothesis during track fitting
  declareProperty("ResetPRD"                   ,m_resetPRD          ); //Reset PRD association tool during sub-detector pattern
  declareProperty("TRT_SegToTrackTool"         ,m_segToTrackTool    );
}

InDet::TRT_TrigStandaloneTrackFinder::~TRT_TrigStandaloneTrackFinder() 
{} 

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::TRT_TrigStandaloneTrackFinder::hltInitialize()
{
  StatusCode  sc;

  msg() << MSG::INFO << "Initializing TRT_TrigStandaloneTrackFinder" << endreq;

  // Get tTools servise
  //
  IToolSvc* toolSvc;
  if ((sc=service("ToolSvc", toolSvc)).isFailure())  {
    msg() << MSG::FATAL << "Toll service not found !" << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  sc = m_segToTrackTool.retrieve();
  if (sc.isFailure()) {
    msg() << MSG::FATAL << "Failed to retrieve tool " << m_segToTrackTool << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }else{
    msg() << MSG::INFO << "Retrieved tool " << m_segToTrackTool << endreq;
  }


  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg() << MSG::DEBUG << (*this) << endreq;
  }

  //Global counters. See the include file for definitions
  m_nTrtSegTotal = 0; m_nTrtSegGoodTotal = 0; m_nBckTrkTotal = 0; m_nUsedSegTotal = 0;
  

  return HLT::OK;

}
///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::TRT_TrigStandaloneTrackFinder::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE)
{

  int outputLevel = msgLvl();
  ++m_ntimesInvoked;

  //Counters. See the include file for definitions
  m_nTrtSeg = 0; m_nTrtSegGood = 0; m_nBckTrk = 0; m_nUsedSeg = 0;
  m_nTRTTracks = 0;

  //Clear all caches
  m_segToTrackTool->resetAll();

  // Clear PRD association tool
  if(m_resetPRD) m_segToTrackTool->resetAssoTool();

  ///Retrieve segments from HLT navigation
  if ( ( HLT::OK != getFeature(outputTE, m_Segments) ) || !m_Segments ) {
    msg() << MSG::ERROR << " Input TRT segments col<lection could not be found " << endreq;

    return HLT::NAV_ERROR;
  }



  m_nTrtSeg = int(m_Segments->size());
  if(outputLevel <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "TRT track container size " << m_nTrtSeg << endreq;
    msg() << MSG::DEBUG << "Begin looping over all TRT segments in the event" << endreq;
  }

  Trk::SegmentCollection::const_iterator iseg    = m_Segments->begin();
  Trk::SegmentCollection::const_iterator isegEnd = m_Segments->end();
  for(; iseg != isegEnd; ++ iseg) {
    
    bool toLower(false);

    ///Get the track segment
    const Trk::TrackSegment *trackTRT = dynamic_cast<const Trk::TrackSegment*>(*iseg);

    if(!trackTRT){
      msg() << MSG::ERROR << "No pointer to segment !" << endreq;
      continue;

    } else {
      
      ///Check if segment has already been assigned a Si extension
      if(m_segToTrackTool->segIsUsed(*trackTRT)) {m_nUsedSeg++; continue;}
      
      ///Cases where the min number of required TRT drift circles drops to 10
      if(int(trackTRT->numberOfMeasurementBases())<=m_minNumDriftCircles) {
	toLower = m_segToTrackTool->toLower(*trackTRT);
      }
      
      ///Get the number of the TRT track segment ROTs
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Number Of ROTs " << (trackTRT->numberOfMeasurementBases()) << endreq;
      
      //Ask for a minimum number of TRT hits in order to process
      if((int(trackTRT->numberOfMeasurementBases())>m_minNumDriftCircles) || toLower){
        m_nTrtSegGood++;

	///Transform the original TRT segment into a track
	Trk::Track* trtSeg = 0;trtSeg = m_segToTrackTool->segToTrack(*trackTRT);
	if(!trtSeg){
	  if(outputLevel <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "Failed to make a track out of the TRT segment!" << endreq;continue;}

	// get all TSOS 
	const DataVector<const Trk::MeasurementBase>* ttsos = trtSeg->measurementsOnTrack(); 
	
	if((int)ttsos->size()<10) { 
	  delete trtSeg; trtSeg = 0; 
	  continue; 
	} 
        m_segToTrackTool->addNewTrack(trtSeg);

      }else{
        if(outputLevel <= MSG::DEBUG)  msg() << MSG::DEBUG << "Found segment with few TRT ROTs " << (*trackTRT) << endreq;
      }
    }
  }

  m_finalTracks = m_segToTrackTool->resolveTracks();

  m_nBckTrk = m_segToTrackTool->GetnTRTTrk();

  //Update the total counters
  m_nTrtSegTotal += m_nTrtSeg; m_nTrtSegGoodTotal += m_nTrtSegGood; 
  m_nUsedSegTotal +=m_nUsedSeg; m_nBckTrkTotal += m_nBckTrk;

  m_segToTrackTool->resetAssoTool();

  if(outputLevel <= MSG::DEBUG)  msg() << MSG::DEBUG << "Saving tracks in container " << endreq;
  
  //  Attach resolved tracks to the trigger element.
  if ( HLT::OK !=  attachFeature(outputTE, m_finalTracks, "TRTStandaloneTracks") ) {
    msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
    
    delete m_finalTracks;
    return HLT::NAV_ERROR;
  }

  m_nTRTTracks = m_finalTracks->size();
  if(outputLevel <= MSG::DEBUG){
    msg() << MSG::DEBUG << "Container recorded in StoreGate." << endreq;
    msg() << MSG::DEBUG << "REGTEST: Container size :" << m_nTRTTracks << endreq;
  }    

  if (outputLevel <= MSG::VERBOSE){
    for (int it=0; it<m_nTRTTracks; it++){
      msg() << MSG::VERBOSE << *(m_finalTracks->at(it)) << endreq;
    }
  }

  //Print common event information
  //
  if(m_outputlevel<=0){
    m_nprint=1; msg() << MSG::DEBUG << (*this) << endreq;
  }

  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::TRT_TrigStandaloneTrackFinder::hltFinalize()
{
  m_nprint=2; msg()<<MSG::INFO<<(*this)<<endreq;

  msg() << MSG::INFO << "REGTEST Finalizing with " << m_nTrtSegTotal << " segments and "
	<< m_nBckTrkTotal << " tracks. Invoked " <<  m_ntimesInvoked << " times."
	<< endreq;
  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream&  InDet::TRT_TrigStandaloneTrackFinder::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out); return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrigStandaloneTrackFinder::dumptools( MsgStream& out ) const
{

  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrigStandaloneTrackFinder::dumpevent( MsgStream& out ) const
{
  int ns = m_nTrtSeg; int nsg = m_nTrtSegGood;
  int nt = m_nBckTrk; int nu = m_nUsedSeg;
  if(m_nprint > 1){ns = m_nTrtSegTotal; nsg = m_nTrtSegGoodTotal; nt = m_nBckTrkTotal; nu = m_nUsedSegTotal;}

  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  out<<"|  Investigated "
     <<std::setw(7)<<ns<<" TRT Segments and found "
     <<std::setw(7)<<nsg<<" good TRT Segments that returned "
     <<std::setw(7)<<nt<<" standalone TRT tracks while "
     <<std::setw(7)<<nu<<" had already been assigned Si extensions."
     <<std::endl;
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrigStandaloneTrackFinder::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::TRT_TrigStandaloneTrackFinder& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_TrigStandaloneTrackFinder& se)
{
  return se.dump(sl); 
}   
