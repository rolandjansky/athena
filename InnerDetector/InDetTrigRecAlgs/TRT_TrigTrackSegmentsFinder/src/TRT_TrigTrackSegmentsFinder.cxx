/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//
// filename: TRT_TrigTrackSegmentsFinder.cxx
//
// author: Iwona Grabowska-Bold, Sep 2006
//         igrabows@mail.cern.ch
//
// Description:  Trigger version of the TRT_TrackSegmentsFinder
//               (see original package documentation).
//
// **************************************************************************

#include "TRT_TrigTrackSegmentsFinder/TRT_TrigTrackSegmentsFinder.h"
#include "TrkSegment/SegmentCollection.h"

//Trigger stuff
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrigTrackSegmentsFinder::TRT_TrigTrackSegmentsFinder
(const std::string& name,ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator),
  m_segmentsMakerTool("InDet::TRT_TrackSegmentsMaker_ATLxk/InDetTrigTRT_TrackSegmentsMaker"),
  m_regionSelector("RegSelSvc", name),
  m_etaHalfWidth(0.1),
  m_phiHalfWidth(0.1),
  m_doFullScan(false)
{
  declareProperty("SegmentsMakerTool", m_segmentsMakerTool);
  declareProperty("RegionSelector", m_regionSelector);
  declareProperty("EtaHalfWidth",  m_etaHalfWidth);
  declareProperty("PhiHalfWidth",  m_phiHalfWidth);
  declareProperty("doFullScan",    m_doFullScan);
  declareMonitoredVariable("RoIEta",m_RoIEta);
  declareMonitoredVariable("RoIPhi",m_RoIPhi);
  declareMonitoredVariable("numTRTSeg",m_nTRTSegs);

  m_timerRegSel = m_timerFind = 0;
  m_timerMainLoop = m_timerSegMaker = 0;
}

//----------------------------------
//          beginRun method:
//----------------------------------------------------------------------------
HLT::ErrorCode InDet::TRT_TrigTrackSegmentsFinder::hltBeginRun() {

  msg() << MSG::INFO << "TRT_TrigTrackSegmentsFinder::beginRun()" << endreq;

  return HLT::OK;
}
//----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::TRT_TrigTrackSegmentsFinder::hltInitialize() {
  
    if(m_segmentsMakerTool.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_segmentsMakerTool<< endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " << m_segmentsMakerTool  << endreq;
    }
    
    // Get output print level
    if(msgLvl()<=MSG::DEBUG) {
      m_nprint=0; msg()<<MSG::DEBUG<<(*this)<<endreq;
    }
    m_nsegmentsTotal = 0;
    
    if(!m_doFullScan){
      // Region selector tool:
      if ( m_regionSelector.retrieve().isFailure() ) {
	msg() << MSG::FATAL
	      << m_regionSelector.propertyName()
	      << " : Unable to retrieve RegionSelector tool "
	      << m_regionSelector.type() << endreq;
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
    }
    else{
      msg() << MSG::INFO
            << "RegionSelector tool not needed for FullScan" << endreq;
    }

    // Initialize timers:
    m_timerRegSel = addTimer("RegSel");
    m_timerFind = addTimer("Find");
    m_timerSegMaker = addTimer("SegMaker");
    m_timerMainLoop = addTimer("MainLoop");

    return HLT::OK;
  }
///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::TRT_TrigTrackSegmentsFinder::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
 

  //----------------------------------------------------------------------
  // Trigger specific part: navigate throw the trigger element to get the
  // ROI center. Use the region selector in order to retrieve the list of
  // detector elements that describe the region of the detector to be 
  // reconstructed.
  //----------------------------------------------------------------------


  if(!m_doFullScan){

    // Get RoiDescriptor
    const TrigRoiDescriptor* roi;
    if ( ( HLT::OK != getFeature(outputTE, roi) ) || !roi ) {
      msg() << MSG::WARNING << "Can't get RoI" << endreq;
      return HLT::NAV_ERROR;
    }
  
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "REGTEST:" << *roi << endreq;
    }

    m_RoIPhi = roi->phi();
    m_RoIEta = roi->eta();


    // TRT hash id's:
    if(doTiming()) m_timerRegSel->start();
    std::vector<IdentifierHash> listOfTRTIds; 
    m_regionSelector->DetHashIDList(TRT, 
				    *roi, 
				    listOfTRTIds );
    if(doTiming()) m_timerRegSel->stop();

    if(doTiming()) m_timerSegMaker->resume();
    m_segmentsMakerTool->newRegion(listOfTRTIds);
    if(doTiming()) m_timerSegMaker->stop();
  } 
  else{
    if(doTiming()) m_timerSegMaker->resume();
    m_segmentsMakerTool->newEvent();
    if(doTiming()) m_timerSegMaker->stop();
  }

  if(doTiming()) m_timerFind->resume();
  m_segmentsMakerTool->find    (); 
  if(doTiming()) m_timerFind->stop();

  if(doTiming()) m_timerMainLoop->resume();

  Trk::Segment* segment = 0;
  m_nsegments           = 0;

  // Loop through all segments and reconsrtucted segments collection preparation

  Trk::SegmentCollection* foundSegments  = new Trk::SegmentCollection;

  while((segment = m_segmentsMakerTool->next())) {
    ++m_nsegments; foundSegments->push_back(segment);
  }

  m_nsegmentsTotal+= m_nsegments;
  m_segmentsMakerTool->endEvent();

  if(doTiming()) m_timerMainLoop->stop();

  //  Attach TRT segments to the trigger element.

  if ( HLT::OK !=  attachFeature(outputTE, foundSegments, "TRTSegm") ) {
    msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
    
    return HLT::NAV_ERROR;
  }

 // Print common event information
  //
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST: Stored " << foundSegments->size() 
	<< " TRT segments in SG." << endreq;
  }


  m_nTRTSegs = foundSegments->size();

  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::TRT_TrigTrackSegmentsFinder::hltFinalize() {

  m_nprint=2; msg()<<MSG::INFO<<(*this)<<endreq;

  return HLT::OK;
}

//----------------------------------
//          endRun method:
//----------------------------------------------------------------------------
HLT::ErrorCode InDet::TRT_TrigTrackSegmentsFinder::hltEndRun() {
  
  msg() << MSG::INFO << "TRT_TrigTrackSegmentsFinder::endRun()" << endreq;
  
  return HLT::OK;
}
  
///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::TRT_TrigTrackSegmentsFinder& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_TrigTrackSegmentsFinder& se)
{
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrigTrackSegmentsFinder::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out); return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrigTrackSegmentsFinder::dumptools( MsgStream& out ) const
{
  int n = 65-m_segmentsMakerTool.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  out<<"| Tool for TRT track segments finding             | "<<m_segmentsMakerTool.type()<<s1
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrigTrackSegmentsFinder::dumpevent( MsgStream& out ) const
{
  int n = m_nsegments; if(m_nprint > 1) n = m_nsegmentsTotal;
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
    out<<"| TRT track segments found                        |"
     <<std::setw(7)<<n
     <<"                                       |"
     <<std::endl;
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrigTrackSegmentsFinder::dump( std::ostream& out ) const
{
  return out;
}





