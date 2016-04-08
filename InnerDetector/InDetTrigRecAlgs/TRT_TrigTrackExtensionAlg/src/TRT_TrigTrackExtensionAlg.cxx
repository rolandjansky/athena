/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackExtensionMap.h"
#include "TrkTrack/TrackCollection.h"
#include "TRT_TrigTrackExtensionAlg/TRT_TrigTrackExtensionAlg.h"
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrigTrackExtensionAlg::TRT_TrigTrackExtensionAlg
(const std::string& name,ISvcLocator* pSvcLocator) : 
  HLT::FexAlgo(name, pSvcLocator) ,
  m_trtExtension("InDet::TRT_TrackExtensionTool_xk/InDetTrigTRTExtensionTool")
{
  // TRT_TrigTrackExtensionAlg steering parameters
  declareProperty("TrackExtensionTool"    ,m_trtExtension );

  declareMonitoredVariable("numTracks", m_nTracks   );
  declareMonitoredVariable("numTracksExtended", m_nTracksExtended   );

}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::TRT_TrigTrackExtensionAlg::hltInitialize() {

  // Get tool for track ectension to TRT
  if( m_trtExtension.retrieve().isFailure()) {
    msg() << MSG::FATAL<< "Failed to retrieve tool " << m_trtExtension << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 
  else{
    msg() << MSG::INFO << "Retrieved tool " << m_trtExtension << endreq;
  }

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg()<<MSG::DEBUG<<(*this)<<endreq;
    m_nprint=1;
  }

  m_nTracksTotal         = 0;
  m_nTracksExtendedTotal = 0;

  return HLT::OK;
}
///////////////////////////////////////////////////////////////////
// Execute HLT Algorithm
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::TRT_TrigTrackExtensionAlg::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
 

  
  m_nTracks         = 0;
  m_nTracksExtended = 0;

  //----------------------------------------------------------------------
  // Trigger specific part: navigate throw the trigger element to get the
  // input track collection.
  //----------------------------------------------------------------------

  const TrackCollection* inputTracks;
  
  //if ( HLT::OK != getFeature(outputTE, inputTracks, "AmbigSolv") ) { //if AmbigSolv doesn't exist, it crashes!!!
  if ( HLT::OK != getFeature(outputTE, inputTracks) ) {
    msg() << MSG::ERROR << " Input track collection could not be found " << endreq;
    
    return HLT::NAV_ERROR;
  }
  
  if(inputTracks->size()==0){
    if(m_outputlevel <= 0)
      msg() << MSG::DEBUG << " Input track collection has 0 size. Algorithm not executed!" << endreq;
    
    return HLT::OK; 
  }

  if(m_outputlevel<=0) {
    m_nprint=1; 
    msg() << MSG::DEBUG << "REGTEST: Investigated " << std::setw(5)
	  << inputTracks->size() << " input tracks " << endreq;
  }

  m_trtExtension->newEvent();

  // Loop through all input track and output tracks collection production
  //
  TrackExtensionMap* extendedTracks    = new TrackExtensionMap;
  TrackCollection::const_iterator t,te;
  te = inputTracks->end();
  
  for (t=inputTracks->begin(); t!=te; ++t) {

    if(!(*t))
      continue;

    ++m_nTracks;
         
    std::vector<const Trk::MeasurementBase*>& tn = 
      m_trtExtension->extendTrack(*(*t));

    if(!tn.size())
      continue;

    extendedTracks->insert(std::make_pair((*t),tn));
    ++m_nTracksExtended; 

  }

  m_nTracksTotal        += m_nTracks        ;
  m_nTracksExtendedTotal+= m_nTracksExtended;

  //  Attach resolved tracks to the trigger element.
  std::string collKey = "ExtTracks"; 
  if ( HLT::OK !=  attachFeature(outputTE, extendedTracks, collKey) ) {
    msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
    
    return HLT::NAV_ERROR;
  }

  
  if(m_outputlevel <= 0){ 
    msg() << MSG::DEBUG << "REGTEST: Extended " << std::setw(9)
	  << extendedTracks->size() << " tracks in SG/"
	  << collKey << endreq; 
  }

  // Print common event information
  //
  //m_nprint=1;
  //if(m_outputlevel<=0) {msg()<<MSG::DEBUG<<(*this)<<endreq;}

  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::TRT_TrigTrackExtensionAlg::hltFinalize() {
  
  
  m_nprint=2; msg()<<MSG::INFO<<(*this)<<endreq;
  
  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::TRT_TrigTrackExtensionAlg& se) {
 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_TrigTrackExtensionAlg& se)
{
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrigTrackExtensionAlg::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpEvent(out); return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrigTrackExtensionAlg::dumpConditions( MsgStream& out ) const
{
  int n = 65-m_trtExtension.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  
  out<<"|----------------------------------------------------------------"
     <<"--------------------------------|"
     <<std::endl;
  out<<"| Tool for tracks extension to TRT                | " << m_trtExtension.type()    
     <<s1<<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"--------------------------------|"
     <<std::endl;
 return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrigTrackExtensionAlg::dumpEvent( MsgStream& out ) const
{

  int nt  = m_nTracks;
  int nte = m_nTracksExtended;
  if(m_nprint > 1) {nt = m_nTracksTotal; nte = m_nTracksExtendedTotal;}

  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  out<<"|  REGTEST Investigated "
     <<std::setw(7)<<nt <<" input tracks and extended    "
     <<std::setw(7)<<nte<<" tracks                      |"
     <<std::endl;
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrigTrackExtensionAlg::dump( std::ostream& out ) const
{
  return out;
}





