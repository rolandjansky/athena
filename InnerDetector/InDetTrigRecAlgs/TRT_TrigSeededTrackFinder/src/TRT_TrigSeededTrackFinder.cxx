/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_TrigSeededTrackFinder
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 05/03/2007 IGB
///////////////////////////////////////////////////////////////////

#include "TRT_TrigSeededTrackFinder/TRT_TrigSeededTrackFinder.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/TrackCollection.h"

#include "IRegionSelector/IRegSelSvc.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "EventPrimitives/EventPrimitives.h"

using namespace std;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrigSeededTrackFinder::TRT_TrigSeededTrackFinder (const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
    m_regionSelector("RegSelSvc", name),
    m_etaHalfWidth(0.1),
    m_phiHalfWidth(0.1),
    m_doFullScan(false),
    m_nprint(0),
    m_trackmaker("InDet::TRT_SeededTrackFinderTool"),
    m_fitterTool("Trk::KalmanFitter/InDetTrigTrackFitter"),
    m_trtExtension("InDet::TRT_TrackExtensionTool_xk")
{
  m_doRefit = false ; //Do a final careful refit of tracks
  m_doExtension = false ; //Find the track TRT extension
  m_saveTRT = true ; //Save stand-alone TRT segments
  
  declareProperty("RegionSelectorTool"         ,m_regionSelector   ); //RegionSelector tool
  declareProperty("EtaHalfWidth"               ,m_etaHalfWidth     ); //Eta half-width of RoI
  declareProperty("PhiHalfWidth"               ,m_phiHalfWidth     ); //Phi half-width of RoI
  declareProperty("doFullScan"                 ,m_doFullScan       ); // Enables full scan mode
  declareProperty("TrackTool"                  ,m_trackmaker       ); //Back tracking tool
  declareProperty("RefitterTool"               ,m_fitterTool       ); //Track refit tool
  declareProperty("TrackExtensionTool"         ,m_trtExtension     ); //TRT track extension tool
  declareProperty("FinalRefit"                 ,m_doRefit          ); //Do a final careful refit of tracks
  declareProperty("TrtExtension"               ,m_doExtension      ); //Find the TRT extension of the track
  declareProperty("OutputSegments"             ,m_saveTRT          ); //Save stand-alone TRT segments
  declareMonitoredVariable("numTrtSeg"         ,m_nTrtSeg);
  declareMonitoredVariable("numTrtSegGood"     ,m_nTrtSegGood);
  declareMonitoredVariable("numBckTrk"         ,m_nBckTrk);
  declareMonitoredVariable("numBckTrkTrt"      ,m_nBckTrkTrt);
  declareMonitoredVariable("numBckTrkSi"       ,m_nBckTrkSi);
  
  m_timerRegSel = 0;
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::TRT_TrigSeededTrackFinder::hltInitialize() {

  msg() << MSG::INFO << "Initializing TRT_TrigSeededTrackFinder" << endreq;

   //Get the TRT seeded track maker tool
  //
  if(m_trackmaker.retrieve().isFailure()) {
    msg() << MSG::FATAL << "Could not get " << m_trackmaker << endreq; 
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }
  else{
    msg() << MSG::INFO << "Got track maker tool " << m_trackmaker << endreq;
  }

  //Get the refitting tool
  //
  if(m_doRefit){
    if(m_fitterTool.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Could not get " << m_fitterTool << endreq; 
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      msg() << MSG::INFO << "Got refitting tool " << m_fitterTool << endreq;
    }
  }
  
  // Get tool for track extension to TRT
  //
  if(m_doExtension){
    if( m_trtExtension.retrieve().isFailure()) {
      msg() << MSG::FATAL<< "Could not get " << m_trtExtension << endreq; 
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else {
      msg() << MSG::INFO << "Retrieved tool " << m_trtExtension << endreq;
    }
  }
  
  if(!m_doFullScan){
    // Retrieving Region Selector Tool
    if ( m_regionSelector.retrieve().isFailure() ) {
      msg() << MSG::FATAL
	    << "Unable to retrieve RegionSelector tool "
	    << m_regionSelector.type() << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
  }
  else{
    msg() << MSG::INFO
	  << "RegionSelector not initialized due to FullScan mode. " << endreq;
  }
  
  // Initialize timers:
  m_timerRegSel = addTimer("RegSel");
  
  // Get output print level
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg() << MSG::DEBUG << (*this) << endreq;
  }

  //Global counters. See the include file for definitions
  m_nTrtSegTotal = 0; m_nTrtSegGoodTotal = 0; m_nBckTrkTotal = 0; m_nBckTrkSiTotal = 0; m_nBckTrkTrtTotal = 0;

  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::TRT_TrigSeededTrackFinder::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {

  int outputLevel = msgLvl();

  //Counters. See the include file for definitions
  m_nTrtSeg = 0; m_nTrtSegGood = 0; m_nBckTrk = 0; m_nBckTrkSi = 0; m_nBckTrkTrt = 0;

  ///Retrieve segments from HLT navigation
  if ( HLT::OK != getFeature(outputTE, m_Segments) ) {
    msg() << MSG::ERROR << " Input TRT segments collection could not be found " << endreq;

    return HLT::NAV_ERROR;
  }

  if(m_Segments->size()==0){
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Input TRT segments collection has 0 size. Algorithm not executed!" << endreq;
    
    return HLT::OK; 
  }

  m_nTrtSeg = int(m_Segments->size());
  if(outputLevel <= MSG::DEBUG)  msg() << MSG::DEBUG << "REGTEST: TRT track container size " << m_nTrtSeg << endreq;

  // Get RoiDescriptor
  const TrigRoiDescriptor* roi;
  if(!m_doFullScan){ //roi descriptor not needed for FullScan
    if ( ( HLT::OK != getFeature(outputTE, roi) ) || !roi ) {
      msg() << MSG::WARNING << "Can't get RoI" << endreq;
      return HLT::NAV_ERROR;
    }
    
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "REGTEST:" << *roi << endreq;
      msg() << MSG::DEBUG << "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth <<endreq;
    }
    m_RoIEta = roi->eta();
    m_RoIPhi = roi->phi();
  }

  std::vector<IdentifierHash> listOfSCTIds; 
  std::vector<IdentifierHash> listOfPixIds; 
  
  if(!m_doFullScan){
    if(doTiming()) m_timerRegSel->start();
    
    //    const double zmax= 168;

    // SCT hash id's:
    m_regionSelector->DetHashIDList(SCT,
				    *roi, 
				    listOfSCTIds );
    
    m_nDetElSCT = listOfSCTIds.size();
    if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Number of SCT detector elements:" << m_nDetElSCT << endreq;
 
    // pixels hash id's:
    m_regionSelector->DetHashIDList( PIXEL,
				     *roi, 
				     listOfPixIds);
    m_nDetElPixel = listOfPixIds.size();
    if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Number of Pixel detector elements:" << m_nDetElPixel << endreq;
    
    if(doTiming()) m_timerRegSel->stop();
  }
  
  ///Initialize the TRT seeded track tool's new event
  if(!m_doFullScan){
    m_trackmaker->newRegion(listOfPixIds,listOfSCTIds); //RoI-based reconstruction
  }
  else{
    m_trackmaker->newEvent(); // FullScan mode
  }

  if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Begin looping over all TRT segments in the event" << endreq;
  
  Trk::SegmentCollection::const_iterator iseg = m_Segments->begin();
  Trk::SegmentCollection::const_iterator isegEnd = m_Segments->end();

  TrackCollection* tempTracks = new TrackCollection;           //Temporary track collection per event
  TrackCollection* outTracks = new TrackCollection;            //Tracks to be finally output

  for(; iseg != isegEnd; ++ iseg) {
    ///Get the track segment
    const Trk::TrackSegment *trackTRT = dynamic_cast<const Trk::TrackSegment*>(*iseg);
    if(!trackTRT){
      msg() << MSG::ERROR << "No pointer to segment !" << endreq;
      continue;
    }
    else{
    
      ///Get the number of the TRT track segment ROTs
      if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Number Of ROTs " << (trackTRT->numberOfMeasurementBases()) << endreq;
      if(trackTRT->numberOfMeasurementBases()>9){  //Ask for at least 10 TRT hits in order to process
        m_nTrtSegGood++;
        std::list<Trk::Track*> trackSi = m_trackmaker->getTrack(*trackTRT); //Get the possible Si extensions
      
	if(trackSi.size()==0){
	  if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "No Si track candidates associated to the TRT track " << endreq;
          if(m_saveTRT && trackTRT->numberOfMeasurementBases()>15){	
	    ///Transform the original TRT segment into a track	
	    Trk::Track* trtSeg = 0;trtSeg = segToTrack(*trackTRT);	
	    if(!trtSeg){ if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to make a track out of the TRT segment!" << endreq;continue;}
	    m_nBckTrk++; m_nBckTrkTrt++;
	    outTracks->push_back(trtSeg);
	  }    
	  continue;
	}
	else{
	  ///Add the track to the list of tracks in the event
	  if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: Found " << (trackSi.size()) << " Si tracks associated to the TRT track " << endreq;

	  ///Merge the resolved Si extensions with the original TRT track segment
	  std::list<Trk::Track*>::const_iterator itt    = trackSi.begin();
	  std::list<Trk::Track*>::const_iterator ittEnd = trackSi.end();
	  
	  for (; itt != ittEnd ; ++itt){

	    tempTracks->push_back((*itt));

	    const DataVector<const Trk::TrackStateOnSurface>* temptsos = (*itt)->trackStateOnSurfaces();
	    if(!temptsos) continue;
		  
	    Trk::Track* globalTrackNew = 0;
	    
	    if(int(temptsos->size())>=4 && m_doExtension){
	      std::vector<const Trk::MeasurementBase*>& tn = m_trtExtension->extendTrack(*(*itt));
	      
	      if(tn.size()) {

		globalTrackNew = mergeExtension(**itt,tn);
		std::vector<const Trk::MeasurementBase*>::const_iterator iv, ive=tn.end();
		for(iv=tn.begin(); iv!=ive; ++iv) delete (*iv);
	      }
	      else globalTrackNew = mergeSegments(**itt,*trackTRT);
	    }
		
	    if(int(temptsos->size())<4 || !m_doExtension) globalTrackNew = mergeSegments(**itt,*trackTRT);

	    if(!globalTrackNew){
	      if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to merge TRT+Si track segment!Return the original TRT segment only" << endreq;
	      
	      if(m_saveTRT && trackTRT->numberOfMeasurementBases()>15){
	  	Trk::Track* trtSeg = 0;trtSeg = segToTrack(*trackTRT);
		if(!trtSeg){ if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to make a track out of the  TRT segment!" << endreq;continue;}
		m_nBckTrk++; m_nBckTrkTrt++;
		outTracks->push_back(trtSeg);
	      }
	    }
	    else {
	      if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Save merged TRT+Si track segment!" << endreq;
	      m_nBckTrk++; m_nBckTrkSi++;
	      outTracks->push_back(globalTrackNew);
	    }
	  }
	}
      }
      else{
	if(outputLevel <= MSG::DEBUG)  msg() << MSG::DEBUG << "Found segment with few TRT ROTs " << (*trackTRT) << endreq;
      }
    }
  }
  delete tempTracks;

  //Update the total counters
  m_nTrtSegTotal += m_nTrtSeg; m_nTrtSegGoodTotal += m_nTrtSegGood; 
  m_nBckTrkTotal += m_nBckTrk; m_nBckTrkSiTotal += m_nBckTrkSi; m_nBckTrkTrtTotal += m_nBckTrkTrt; 

  if(outputLevel <= MSG::DEBUG)  msg() << MSG::DEBUG << "Saving tracks in container " << endreq;

  //  Attach resolved tracks to the trigger element.
  if ( HLT::OK !=  attachFeature(outputTE, outTracks, "TRTSeededTracks") ) {
    msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;

    delete outTracks;
    return HLT::NAV_ERROR;
  }

  if(outputLevel <= MSG::DEBUG){
    msg() << MSG::DEBUG << "Container recorded in StoreGate." << endreq;
    msg() << MSG::DEBUG << "REGTEST: Container size :" << outTracks->size() << endreq;
  }    

  //Print common event information
  if(m_outputlevel<=0){
    m_nprint=1; msg() << MSG::DEBUG << (*this) << endreq;
  }

  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::TRT_TrigSeededTrackFinder::hltFinalize() {

  m_nprint=2; msg()<<MSG::INFO<<(*this)<<endreq;

  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream&  InDet::TRT_TrigSeededTrackFinder::dump( MsgStream& out ) const {

  out<<std::endl;
  if(m_nprint)  return dumpevent(out); return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::TRT_TrigSeededTrackFinder::dumptools( MsgStream& out ) const {

  int n = 65-m_trackmaker.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n = 65-m_fitterTool.type().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n = 65-m_trtExtension.type().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  out<<"| Tool for TRT seeded track finding | "<<m_trackmaker             <<s1<<std::endl;
  out<<"| Tool for final track refitting | "<<m_fitterTool.type() <<s2<<std::endl;
  out<<"| Tool for TRT trac extension | "<<m_trtExtension.type() <<s3<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::TRT_TrigSeededTrackFinder::dumpevent( MsgStream& out ) const {

  int ns = m_nTrtSeg; int nsg = m_nTrtSegGood;
  int nt = m_nBckTrk; int nsi = m_nBckTrkSi; int ntrt = m_nBckTrkTrt;
  
  if(m_nprint > 1){ns = m_nTrtSegTotal; nsg = m_nTrtSegGoodTotal; nt = m_nBckTrkTotal; nsi = m_nBckTrkSiTotal; ntrt = m_nBckTrkTrtTotal;}

  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  out<<"|  Investigated "
     <<std::setw(7)<<ns<<" TRT Segments and found "
     <<std::setw(7)<<nsg<<" good TRT Segments that returned "
     <<std::setw(7)<<nt<<" back tracks of which "
     <<std::setw(7)<<nsi<<" have Si extensions and "
     <<std::setw(7)<<ntrt<<" have only a TRT component                      |"
     <<std::endl;
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////
std::ostream& InDet::TRT_TrigSeededTrackFinder::dump( std::ostream& out ) const {

  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::operator    << 
(MsgStream& sl,const InDet::TRT_TrigSeededTrackFinder& se) { 

  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////
std::ostream& InDet::operator << 
(std::ostream& sl,const InDet::TRT_TrigSeededTrackFinder& se) {

  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Merge a Si extension and a TRT segment.Refit at the end
///////////////////////////////////////////////////////////////////
Trk::Track* InDet::TRT_TrigSeededTrackFinder::mergeSegments(const Trk::Track& tT, const Trk::TrackSegment& tS) {

  int outputLevel = msgLvl();
  
  // TSOS from the track
  const DataVector<const Trk::TrackStateOnSurface>* stsos = tT.trackStateOnSurfaces();
  // fitQuality from track
  const Trk::FitQuality* fq = tT.fitQuality()->clone();
  // output datavector of TSOS
  DataVector<const Trk::TrackStateOnSurface>*       ntsos = new DataVector<const Trk::TrackStateOnSurface>;
  
  int siHits = 0;
  // copy track Si states into track
  DataVector<const Trk::TrackStateOnSurface>::const_iterator p_stsos;
  for(p_stsos=stsos->begin();p_stsos!=stsos->end();++p_stsos){
    ntsos->push_back( (*p_stsos)->clone() );
    if ((*p_stsos)->type(Trk::TrackStateOnSurface::Measurement)) siHits++;
  }
  
  // loop over segment
  for(int it=0; it<int(tS.numberOfMeasurementBases()); it++){
    //test if it is a pseudo measurement
    if ( dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tS.measurement(it)) ) {
      if (siHits<4) {
	if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Too few Si hits.Will keep pseudomeasurement..." << endreq;
	//Get the track segment information and build the initial track parameters
	const AmgVector(5)& p = tS.localParameters();
	AmgSymMatrix(5)* cov = new AmgSymMatrix(5)(tS.localCovariance());
	//Create the track parameter on surface. the dynamic_cast in principle should not be used any more. 
	//const Trk::StraightLineSurface*       surf = dynamic_cast<const Trk::StraightLineSurface*>(&(tS.associatedSurface()));
	//const Trk::TrackParameters* segPar = surf->createParameters<5,Trk::Charged>(p(0),p(1),p(2),p(3),p(4),cov);
	const Trk::TrackParameters* segPar = tS.associatedSurface().createTrackParameters(p[Trk::loc1],
											  p[Trk::loc2],
											  p[Trk::phi],
											  p[Trk::theta],
											  p[Trk::qOverP],cov);
	if(!segPar){
	  if(outputLevel <= MSG::DEBUG) msg() << MSG::WARNING << "Could not get TRT Segment Parameters for pseudomeasurement TSOS.Dropping pseudomeasurement..." << endreq;
	  continue;
	}
	std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
	typePattern.set(Trk::TrackStateOnSurface::Measurement); 
	const Trk::TrackStateOnSurface* seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),segPar,0,0,typePattern);
	
	ntsos->push_back(seg_tsos);
      }
    } 
    else {

      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
      typePattern.set(Trk::TrackStateOnSurface::Measurement); 
      const Trk::TrackStateOnSurface* seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),0,0,0,typePattern);
      
      ntsos->push_back(seg_tsos);
    }
  }  
  
  ///Construct the new track
  Trk::TrackInfo trkinfo(tT.info());
  trkinfo.setPatternRecognitionInfo(Trk::TrackInfo::TRTSeededTrackFinder);
  Trk::Track* newTrack = new Trk::Track(trkinfo, ntsos, fq);
  
  //Careful refitting at the end
  if(m_doRefit){ 
    Trk::Track* fitTrack = m_fitterTool->fit(*newTrack,false,Trk::pion);
    delete newTrack;
    if(!fitTrack){if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Refit of TRT+Si track segment failed!" << endreq;return 0;}
    
    //Protect for tracks that have no really defined locz and theta parameters
    const Trk::Perigee* perTrack = fitTrack->perigeeParameters();
    const AmgSymMatrix(5)* CM = perTrack ? perTrack->covariance() : NULL;
    if(perTrack && CM){
      if((sqrt((*CM)(1,1))==0.)||(sqrt((*CM)(3,3))==0.)){delete fitTrack; return 0;}
    }
    return fitTrack;
  }
  
  return newTrack;
}

///////////////////////////////////////////////////////////////////
// Transform a TRT segment to track
///////////////////////////////////////////////////////////////////
Trk::Track* InDet::TRT_TrigSeededTrackFinder::segToTrack(const Trk::TrackSegment& tS) {

  int outputLevel = msgLvl();
  
  if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Transforming the TRT segment into a track..." << endreq;
  
  DataVector<const Trk::TrackStateOnSurface>* ntsos = new DataVector<const Trk::TrackStateOnSurface>;
  
  //Get the track segment information and build the initial track parameters
  const AmgVector(5)& p = tS.localParameters();
  AmgSymMatrix(5)* cov = new AmgSymMatrix(5)(tS.localCovariance());
  //Create the track parameter on surface. the dynamic_cast in principle should not be used any more. 
  //const Trk::StraightLineSurface*       surf = dynamic_cast<const Trk::StraightLineSurface*>(&(tS.associatedSurface()));
  //const Trk::TrackParameters* segPar = surf->createParameters<5,Trk::Charged>(p(0),p(1),p(2),p(3),p(4),cov);
  const Trk::TrackParameters* segPar = tS.associatedSurface().createTrackParameters(p[Trk::loc1],
										    p[Trk::loc2],
										    p[Trk::phi],
										    p[Trk::theta],
										    p[Trk::qOverP],cov);
  if(segPar){
    if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Initial TRT Segment Parameters for refitting " << (*segPar) << endreq;
  }
  else{
    if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Could not get initial TRT segment parameters! " << endreq;
    return 0;
  }

  for(int it=0; it<int(tS.numberOfMeasurementBases()); it++){
    // on first measurement add parameters
    const Trk::TrackStateOnSurface* seg_tsos = 0;
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
    typePattern.set(Trk::TrackStateOnSurface::Measurement); 
    
    if(it==0) 
      seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),segPar,0,0,typePattern); 
    else
      seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),0,0,0,typePattern);
    ntsos->push_back(seg_tsos);
  }  
  
  ///Construct the new track
  Trk::TrackInfo trkinfo;
  trkinfo.setPatternRecognitionInfo(Trk::TrackInfo::TRTSeededTrackFinder);
  Trk::Track* newTrack = new Trk::Track(trkinfo, ntsos, 0);
  
  ///Careful refitting of the TRT stand alone track
  if(m_doRefit){
    Trk::Track* fitTrack = m_fitterTool->fit(*newTrack,false,Trk::pion);
    delete newTrack; // cleanup
    if(!fitTrack){if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Refit of TRT track segment failed!" << endreq;return 0;}
    
    //Protect for tracks that have no really defined locz and theta parameters
    const Trk::Perigee* perTrack = fitTrack->perigeeParameters();
    const AmgSymMatrix(5)* CM = perTrack ? perTrack->covariance() : NULL;
    if(perTrack && CM){
      if((sqrt((*CM)(1,1))==0.)||(sqrt((*CM)(3,3))==0.)){delete fitTrack; return 0;}
    }
    return fitTrack;
  }
  
  return newTrack;
}

///////////////////////////////////////////////////////////////////
// Merge a Si extension and a TRT track extension.Refit at the end
///////////////////////////////////////////////////////////////////
Trk::Track* InDet::TRT_TrigSeededTrackFinder::mergeExtension(const Trk::Track& tT, std::vector<const Trk::MeasurementBase*>& tS) {
  
  int outputLevel = msgLvl();
  
  // TSOS from the track
  const DataVector<const Trk::TrackStateOnSurface>* stsos = tT.trackStateOnSurfaces();
  // fitQuality from track
  const Trk::FitQuality* fq = tT.fitQuality()->clone();
  // output datavector of TSOS
  DataVector<const Trk::TrackStateOnSurface>*       ntsos = new DataVector<const Trk::TrackStateOnSurface>;
  
  int siHits = 0;
  // copy track Si states into track
  DataVector<const Trk::TrackStateOnSurface>::const_iterator p_stsos;
  for(p_stsos=stsos->begin();p_stsos!=stsos->end();++p_stsos){
    ntsos->push_back( (*p_stsos)->clone() );
    if ((*p_stsos)->type(Trk::TrackStateOnSurface::Measurement)) siHits++;
  }
  
  // loop over TRT track extension
  for(int it=0; it<int(tS.size()); it++){
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
    typePattern.set(Trk::TrackStateOnSurface::Measurement); 
    const Trk::TrackStateOnSurface* seg_tsos = new Trk::TrackStateOnSurface(tS[it]->clone(),0,0,0,typePattern);
    
    ntsos->push_back(seg_tsos);
  }  
  
  ///Construct the new track
  Trk::TrackInfo trkinfo(tT.info());
  trkinfo.setPatternRecognitionInfo(Trk::TrackInfo::TRTSeededTrackFinder);
  Trk::Track* newTrack = new Trk::Track(trkinfo, ntsos, fq);
  
  //Careful refitting at the end
  if(m_doRefit){ 
    Trk::Track* fitTrack = m_fitterTool->fit(*newTrack,false,Trk::pion);
    delete newTrack;
    if(!fitTrack){if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Refit of TRT+Si track segment failed!" << endreq;return 0;}
    
    //Protect for tracks that have no really defined locz and theta parameters
    const Trk::Perigee* perTrack = fitTrack->perigeeParameters();
    const AmgSymMatrix(5)* CM = perTrack ? perTrack->covariance() : NULL;
    if(perTrack && CM){
      if((sqrt((*CM)(1,1))==0.)||(sqrt((*CM)(3,3))==0.)){delete fitTrack; return 0;}
    }
    return fitTrack;
  }
  
  return newTrack;
}

