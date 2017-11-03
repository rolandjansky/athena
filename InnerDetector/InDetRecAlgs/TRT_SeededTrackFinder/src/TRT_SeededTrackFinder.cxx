/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_SeededTrackFinder
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 02/20/2006 Thomas Koffas
// modified by Markus ELsing
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "TRT_SeededTrackFinder/TRT_SeededTrackFinder.h"
#include "TrkTrack/TrackInfo.h"
//#include "TrkParameters/MeasuredPerigee.h"
//#include "TrkParameters/MeasuredAtaStraightLine.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
///Needed for my scoring
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "CxxUtils/make_unique.h"
using namespace std;
//using CLHEP::HepVector;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_SeededTrackFinder::TRT_SeededTrackFinder
(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_nprint(0),
    m_trackmaker("InDet::TRT_SeededTrackFinderTool"),
    m_fitterTool("Trk::KalmanFitter/InDetTrackFitter"),
    m_trtExtension("InDet::TRT_TrackExtensionTool_xk"),
    m_Segments("TRTSegments"),
    m_outTracks("TRTSeededTracks"),
    m_iBeamCondSvc("BeamCondSvc",name)
{
  m_doRefit          = false                                ;       //Do a final careful refit of tracks
  m_doExtension      = false                                ;       //Find the track TRT extension
  m_rejectShortExten = false                                ;
  m_doStat           = false                                ;       //Statistics of final tracks
  m_saveTRT          = true                                 ;       //Save stand-alone TRT segments
  m_MaxSegNum        = 5000                                 ;

  declareProperty("TrackTool"                  ,m_trackmaker       ); //Back tracking tool
  declareProperty("RefitterTool"               ,m_fitterTool       ); //Track refit tool
  declareProperty("TrackExtensionTool"         ,m_trtExtension     ); //TRT track extension tool
  declareProperty("InputSegmentsLocation"      ,m_Segments         ); //Input track collection
  declareProperty("OutputTracksLocation"       ,m_outTracks        ); //Output track collection
  declareProperty("FinalRefit"                 ,m_doRefit          ); //Do a final careful refit of tracks
  declareProperty("TrtExtension"               ,m_doExtension      ); //Find the TRT extension of the track
  declareProperty("RejectShortExtension"       ,m_rejectShortExten ); //Reject short extensions
  declareProperty("FinalStatistics"            ,m_doStat           ); //Statistics of final tracks
  declareProperty("OutputSegments"             ,m_saveTRT          ); //Save stand-alone TRT segments
  declareProperty("MaxNumberSegments"          ,m_MaxSegNum        ); //Maximum Number of segments to be handled
  declareProperty("MinTRTonSegment"            ,m_minTRTonSegment = 10); //Minimum Number of Hits on segment
  declareProperty("MinTRTonly"                 ,m_minTRTonly      = 15); //Minimum Number of Hits on TRT only
  // --- selection cuts after SI extension
  declareProperty("SiExtensionCuts",   m_SiExtensionCuts    = false);
  declareProperty("minPt",             m_minPt              = 500.);
  declareProperty("maxEta",            m_maxEta             = 2.7);
  declareProperty("maxRPhiImp",        m_maxRPhiImp         = 10.);
  declareProperty("maxZImp",           m_maxZImp            = 250.);
  declareProperty("Extrapolator",      m_extrapolator);
  declareProperty("BeamPositionSvc",   m_iBeamCondSvc );
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_SeededTrackFinder::initialize()
{
  StatusCode  sc;

  msg(MSG::DEBUG) << "Initializing TRT_SeededTrackFinder" << endmsg;

  //Get the TRT seeded track maker tool
  //
  if(m_trackmaker.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not get " << m_trackmaker << endmsg; return StatusCode::FAILURE;
  }else{
    msg(MSG::INFO) << "Got track maker tool " << m_trackmaker << endmsg;
  }

  //Get the refitting tool
  //
  if(m_doRefit){
    if(m_fitterTool.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not get " << m_fitterTool << endmsg; return StatusCode::FAILURE;
    }else{
      msg(MSG::INFO) << "Got refitting tool " << m_fitterTool << endmsg;
    }
  }

  if (m_SiExtensionCuts) {
    // get extrapolator
    sc = m_extrapolator.retrieve();
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endmsg;
      return StatusCode::FAILURE;
    } else 
      msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg;

    // get beam spot service
    sc = m_iBeamCondSvc.retrieve();
    if (sc.isFailure()) {
      msg(MSG::INFO) << "Could not find BeamCondSvc." << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Get tool for track ectension to TRT
  //
  if(m_doExtension){
    if( m_trtExtension.retrieve().isFailure()) {
      msg(MSG::FATAL)<< "Could not get " << m_trtExtension << endmsg; return StatusCode::FAILURE;
    } 
    else {
      msg(MSG::INFO) << "Retrieved tool " << m_trtExtension << endmsg;
    }
  }

  // Get output print level
  //
  if(msgLvl(MSG::DEBUG)){m_nprint=0; msg(MSG::DEBUG) << (*this) << endmsg;}

  //Global counters. See the include file for definitions
  m_nTrtSegTotal      = 0;
  m_nTrtFailSelTotal  = 0;
  m_nTrtSegGoodTotal  = 0;
  m_nTrtLimitTotal    = 0;
  m_nTrtNoSiExtTotal  = 0;
  m_nExtCutTotal      = 0;      
  m_nBckTrkTrtTotal   = 0;
  m_nTrtExtCallsTotal = 0;
  m_nTrtExtTotal      = 0;
  m_nTrtExtBadTotal   = 0;
  m_nTrtExtFailTotal  = 0;
  m_nBckTrkSiTotal    = 0;
  m_nBckTrkTotal      = 0;
  
  return sc;

}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
StatusCode InDet::TRT_SeededTrackFinder::execute()
{

  //Counters. See the include file for definitions
  m_nTrtSeg      = 0;
  m_nTrtFailSel  = 0;
  m_nTrtSegGood  = 0;
  m_nTrtLimit    = 0;
  m_nTrtNoSiExt  = 0;
  m_nExtCut      = 0;
  m_nBckTrkTrt   = 0;
  m_nTrtExtCalls = 0;
  m_nTrtExt      = 0;
  m_nTrtExtBad   = 0;
  m_nTrtExtFail  = 0;
  m_nBckTrkSi    = 0;
  m_nBckTrk      = 0;

  // counter
  int nTrtSegCur = 0;

  // Retrieve segments from StoreGate
  if(!m_Segments.isValid()){
    ATH_MSG_FATAL ("No segment with name " << m_Segments.name() << " found in StoreGate!");
    return StatusCode::FAILURE;
  }else{
    ATH_MSG_DEBUG ("Found segments collection " << m_Segments.name() << " in StoreGate!");
  }


  // number of segments + statistics
  m_nTrtSeg = int(m_Segments->size());
  ATH_MSG_DEBUG ("TRT track container size " << m_nTrtSeg);
  if(m_nTrtSeg>m_MaxSegNum) {
    ATH_MSG_DEBUG ("TRT track container size huge; will process event partially if number of max segments reached !!!");
  }

  // Initialize the TRT seeded track tool's new event
  m_trackmaker  ->newEvent();
  m_trtExtension->newEvent();
  
//  TrackCollection* outTracks  = new TrackCollection;           //Tracks to be finally output
  m_outTracks = CxxUtils::make_unique<TrackCollection>();

  std::vector<Trk::Track*> tempTracks;                           //Temporary track collection 
  tempTracks.reserve(128);
  // loop over event
  ATH_MSG_DEBUG ("Begin looping over all TRT segments in the event");
  Trk::SegmentCollection::const_iterator iseg    = m_Segments->begin();
  Trk::SegmentCollection::const_iterator isegEnd = m_Segments->end();
  for(; iseg != isegEnd; ++ iseg) {

    // Get the track segment
    const Trk::TrackSegment *trackTRT = dynamic_cast<const Trk::TrackSegment*>(*iseg);
    if(!trackTRT){
      ATH_MSG_ERROR ("No pointer to segment !");
      continue;
    }else{

      // the segment finder is applying a TRT(eta) cut and a pt preselection, so we don't do that here  

      //Ask for at least 10 TRT hits in order to process
      if(trackTRT->numberOfMeasurementBases() < m_minTRTonSegment) {
	ATH_MSG_DEBUG ("TRT segment fails nTRT hit cut, reject.");

	// statistics
	m_nTrtFailSel++;

      }	else {

	// do we continue to process ?
	nTrtSegCur++;
	if(nTrtSegCur>=m_MaxSegNum) {
	  ATH_MSG_INFO ("====> Reached maximal number of segments in event, stop !!!");
	  // statistics
	  m_nTrtLimit++;
	  break;
	}

	// Get the number of the TRT track segment ROTs
	ATH_MSG_DEBUG ("=> New segment to process, number Of TRT ROTs : " << (trackTRT->numberOfMeasurementBases()));

	// statistics
        m_nTrtSegGood++;

	// ok, call track maker and get list of possible track candidates
        std::list<Trk::Track*> trackSi = m_trackmaker->getTrack(*trackTRT); //Get the possible Si extensions

        if (trackSi.size()==0) {
          ATH_MSG_DEBUG ("No Si track candidates associated to the TRT track ");

	  // statistics
	  m_nTrtNoSiExt++;

	  // obsolete backup of TRT only
          if(m_saveTRT && trackTRT->numberOfMeasurementBases() > m_minTRTonly){
            ///Transform the original TRT segment into a track
            Trk::Track* trtSeg = 0;trtSeg = segToTrack(*trackTRT);
            if(!trtSeg) {
	      ATH_MSG_DEBUG ("Failed to make a track out of the TRT segment!");
	      continue;
	    }
	    // statistics
            m_nBckTrk++; m_nBckTrkTrt++;
	    // add track to output list
	    m_outTracks->push_back(trtSeg);
          }
          continue;

        } else {

	  // Found useful extensions
          ATH_MSG_DEBUG ("Found " << (trackSi.size()) << " Si tracks associated to the TRT track ");

          // Merge the resolved Si extensions with the original TRT track segment
          std::list<Trk::Track*>::const_iterator itt    = trackSi.begin();
          std::list<Trk::Track*>::const_iterator ittEnd = trackSi.end();
          for (; itt != ittEnd ; ++itt){

	    tempTracks.push_back(*itt);

	    // get list of TSOS
            const DataVector<const Trk::TrackStateOnSurface>* temptsos = (*itt)->trackStateOnSurfaces();
            if (!temptsos) {
	      ATH_MSG_DEBUG ("Silicon extension empty ???");
	      // cleanup
	      //delete *itt;
	      continue;
	    }

	    // Add the track to the list of tracks in the event
	    ATH_MSG_DEBUG ("Silicon extension found has length of : " << temptsos->size());

	    // do we do a preselection ?
	    if (m_SiExtensionCuts) {

             // get parameters without errors
	      const Trk::TrackParameters* input = (*itt)->trackParameters()->front()->clone();

	      // cuts on parameters
	      if (fabs(input->pT()) < m_minPt) {
	      	ATH_MSG_DEBUG ("Track pt < "<<m_minPt<<", reject it");
	        // cleanup
	        // delete *itt;
	        delete input;
	        // statistics
	        m_nExtCut++;
	        continue;
	      } 
	      if (fabs(input->eta()) > m_maxEta) {
	        ATH_MSG_DEBUG ("Track eta > "<<m_maxEta<<", reject it");
	        // cleanup
	        // delete *itt;
	        delete input;
	        // statistics
	        m_nExtCut++;
	        continue;
	      }

	      // --- beam spot position
	      Amg::Vector3D beamSpotPosition(0,0,0);
	      if (m_iBeamCondSvc) beamSpotPosition = m_iBeamCondSvc->beamVtx().position();
	      // --- create surface
	      Trk::PerigeeSurface perigeeSurface(beamSpotPosition);

	      // uses perigee on track or extrapolates, no material in any case, we cut on impacts
	      const Trk::TrackParameters* parm = m_extrapolator->extrapolateDirectly(*input, perigeeSurface);
	      delete input;

	      const Trk::Perigee*extrapolatedPerigee = dynamic_cast<const Trk::Perigee*> (parm ); 
	      if (!extrapolatedPerigee) {
	        msg(MSG::WARNING) << "Extrapolation of perigee failed, this should never happen" << endmsg;
	        // cleanup
	        // delete *itt;
	        delete parm;
	        // statistics
	        m_nExtCut++;
	        continue;
	      }
	       
	      ATH_MSG_VERBOSE ("extrapolated perigee: "<<*extrapolatedPerigee);
	      
	      //if (fabs(extrapolatedPerigee->parameters()[Trk::d0]*extrapolatedPerigee->sinTheta()) > m_maxRPhiImp) {
	      if (fabs(extrapolatedPerigee->parameters()[Trk::d0]) > m_maxRPhiImp) {
	        ATH_MSG_DEBUG ("Track Rphi impact > "<<m_maxRPhiImp<<", reject it");
	        // cleanup
	        // delete *itt;
	        delete extrapolatedPerigee;
	        // statistics
	        m_nExtCut++;
	        continue;
	      }
	      // if (fabs(extrapolatedPerigee->parameters()[Trk::z0]*extrapolatedPerigee->sinTheta()) > m_maxZImp) {
	      if (fabs(extrapolatedPerigee->parameters()[Trk::z0]) > m_maxZImp) {
	        ATH_MSG_DEBUG ("Track Z impact > "<<m_maxZImp<<", reject it");
	        // cleanup
	        // delete *itt;
	        delete extrapolatedPerigee;
	        // statistics
	        m_nExtCut++;
	        continue;
	      }
	      delete extrapolatedPerigee;
	    }

	    // do re run a Track extension into TRT ?
	    Trk::Track* globalTrackNew = 0;
	    
	    // do we have 4 and extension is enabled ?
            if(int(temptsos->size())>=4 && m_doExtension){

	      // Add the track to the list of tracks in the event
	      ATH_MSG_DEBUG ("Try to improve TRT calling extension tool.");

	      // statistics
	      m_nTrtExtCalls++;

	      // call extension tool
              std::vector<const Trk::MeasurementBase*>& tn = m_trtExtension->extendTrack(*(*itt));

              if(!tn.size()) {

		// Fallback if extension failed
		ATH_MSG_DEBUG ("No new segment found, use input segment as fallback.");

		// statistics
		m_nTrtExtFail++;

		// merge Si with input track segments
		globalTrackNew = mergeSegments(**itt,*trackTRT);

	      } else if (!m_rejectShortExten || tn.size() >= trackTRT->numberOfMeasurementBases() ) {

 		// Use the extension to instead of the segment
		ATH_MSG_DEBUG ("Successful extension, number of TRT hits : " << tn.size() << " was : " << (trackTRT->numberOfMeasurementBases()));
 
		// merge the extension with the Si track
                globalTrackNew = mergeExtension(**itt,tn);

		// Add the track to the list of tracks in the event
		ATH_MSG_DEBUG ("Merged extension with Si segment");
		// statistics
		m_nTrtExt++;

		// clean up
		std::vector<const Trk::MeasurementBase*>::const_iterator iv, ive=tn.end();
                for(iv=tn.begin(); iv!=ive; ++iv) delete (*iv);

	      } else {

 		// Extension is shorter, let's fall back onto the original
		ATH_MSG_DEBUG ("Extension too short, number of TRT hits : " << tn.size() << " was : " << (trackTRT->numberOfMeasurementBases()) << ". Use Segement !");

		// merge segments
		globalTrackNew = mergeSegments(**itt,*trackTRT);

		// Add the track to the list of tracks in the event
		ATH_MSG_DEBUG ("Merged TRT segment with Si segment");

		// statistics
		m_nTrtExtBad++;

		// clean up
		std::vector<const Trk::MeasurementBase*>::const_iterator iv, ive=tn.end();
                for(iv=tn.begin(); iv!=ive; ++iv) delete (*iv);

	      }

	    } else {
	      // no extension tool, jsut add the two
	      ATH_MSG_DEBUG ("Do not try to extend Si track, merging it with input TRT.");

	      // merge segments
	      globalTrackNew = mergeSegments(**itt,*trackTRT);
	    }

	    // do we have an track candidate ?
            if(!globalTrackNew){
              ATH_MSG_DEBUG ("Failed to merge TRT+Si track segment !");

	      if(m_saveTRT && trackTRT->numberOfMeasurementBases() > m_minTRTonly) {
                Trk::Track* trtSeg = 0;trtSeg = segToTrack(*trackTRT);
                if(!trtSeg){
		  ATH_MSG_DEBUG ("Failed to make a track out of the  TRT segment!");
		  continue;
		}
		ATH_MSG_DEBUG ("Add TRT only to output list");

		// statistis
                m_nBckTrk++; m_nBckTrkTrt++;

		// add it to output list
		m_outTracks->push_back(trtSeg);
              }

	    } else {

              ATH_MSG_DEBUG ("Save merged TRT+Si track segment!");

	      // statistics
	      m_nBckTrk++; m_nBckTrkSi++;

	      // add it to output list
	      m_outTracks->push_back(globalTrackNew);
            }
	  }
        }
      }
    }
  }

  // further debugging of results
  if(m_doStat){
    Analyze(m_outTracks.ptr());
  }

  // Update the total counters
  m_nTrtSegTotal      += m_nTrtSeg;
  m_nTrtFailSelTotal  += m_nTrtFailSel;
  m_nTrtSegGoodTotal  += m_nTrtSegGood;
  m_nTrtLimitTotal    += m_nTrtLimit;
  m_nTrtNoSiExtTotal  += m_nTrtNoSiExt;
  m_nExtCutTotal      += m_nExtCut;
  m_nBckTrkTrtTotal   += m_nBckTrkTrt;
  m_nTrtExtCallsTotal += m_nTrtExtCalls;
  m_nTrtExtTotal      += m_nTrtExt;
  m_nTrtExtBadTotal   += m_nTrtExtBad;
  m_nTrtExtFailTotal  += m_nTrtExtFail;
  m_nBckTrkSiTotal    += m_nBckTrkSi;
  m_nBckTrkTotal      += m_nBckTrk;

  
  for (auto p : tempTracks){
    delete p;
  }
  m_trackmaker->endEvent();

  //Print common event information
  if(msgLvl(MSG::DEBUG)){
    m_nprint=1; msg(MSG::DEBUG) << (*this) << endmsg;
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_SeededTrackFinder::finalize()
{
  m_nprint=2; msg(MSG::INFO)<<(*this)<<endmsg;

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream&  InDet::TRT_SeededTrackFinder::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out);
  return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_SeededTrackFinder::dumptools( MsgStream& out ) const
{
  int n = 65-m_trackmaker.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 65-m_fitterTool.type().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 65-m_trtExtension.type().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 65-m_Segments.name().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  n     = 65-m_outTracks.name().size();
  std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");

  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
       <<std::endl;
  out<<"| Tool for TRT seeded track finding | "<<m_trackmaker.type()    <<s1<<std::endl;
  out<<"| Tool for final track refitting    | "<<m_fitterTool.type()    <<s2<<std::endl;
  out<<"| Tool for TRT trac extension       | "<<m_trtExtension.type()  <<s3<<std::endl;
  out<<"| Location of input tracks          | "<<m_Segments.name()      <<s4<<std::endl;
  out<<"| Location of output tracks         | "<<m_outTracks.name()     <<s5<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
      <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_SeededTrackFinder::dumpevent( MsgStream& out ) const
{
  int ns   = m_nTrtSeg;
  int nsf  = m_nTrtFailSel;
  int nsg  = m_nTrtSegGood;
  int nlim = m_nTrtLimit;
  int nnsi = m_nTrtNoSiExt;
  int nsic = m_nExtCut; 
  int ntrt = m_nBckTrkTrt;
  int next = m_nTrtExtCalls;
  int ngex = m_nTrtExt;
  int nbex = m_nTrtExtBad;
  int nfex = m_nTrtExtFail;
  int nsi  = m_nBckTrkSi;
  int nt   = m_nBckTrk;

  if(m_nprint > 1) {
    ns   = m_nTrtSegTotal;
    nsf  = m_nTrtFailSelTotal;
    nsg  = m_nTrtSegGoodTotal;
    nlim = m_nTrtLimitTotal;
    nnsi = m_nTrtNoSiExtTotal;
    nsic = m_nExtCutTotal; 
    ntrt = m_nBckTrkTrtTotal;
    next = m_nTrtExtCallsTotal;
    ngex = m_nTrtExtTotal;
    nbex = m_nTrtExtBadTotal;
    nfex = m_nTrtExtFailTotal;
    nsi  = m_nBckTrkSiTotal;
    nt   = m_nBckTrkTotal;
  }

  out<<"|-------------------------------------------------------------------"<<std::endl;
  out<<"|  Investigated :"<<std::endl
     <<"| "<<std::setw(7)<<ns  <<" TRT Segments on input"<<std::endl
     <<"| "<<std::setw(7)<<nsf <<" TRT Segments fail selection on input"<<std::endl
     <<"| "<<std::setw(7)<<nsg <<" TRT Segments after selection"<<std::endl;
  if (nlim>0) {
    out<<"|-------------------------------------------------------------------"<<std::endl;
    out<<"| "<<std::setw(7)<<nlim<<" TRT segments lost because of processing limit"<<std::endl;
  }
  out<<"|-------------------------------------------------------------------"<<std::endl;
  out<<"| "<<std::setw(7)<<nnsi<<" TRT segments without Si extension"<<std::endl;
  if (m_SiExtensionCuts) {
    out<<"| "<<std::setw(7)<<nsic<<" number of Si extension failing cuts"<<std::endl;
  }
  if (ntrt>0) {
    out<<"| "<<std::setw(7)<<ntrt <<" number ot TRT only tracks created"<<std::endl;
  }
  if (m_doExtension) {
    out<<"|-------------------------------------------------------------------"<<std::endl;
    out<<"| "<<std::setw(7)<<next<<" number of times TRT extension is called"<<std::endl
       <<"| "<<std::setw(7)<<ngex<<" number of good TRT extension"<<std::endl;
    if (m_rejectShortExten) {
      out<<"| "<<std::setw(7)<<nbex<<" number of bad TRT extension"<<std::endl;
    }
    out<<"| "<<std::setw(7)<<nfex<<" number of failed TRT extension and fallback"<<std::endl;
  }
  out<<"|-------------------------------------------------------------------"<<std::endl;
  out<<"| "<<std::setw(7)<<nsi <<" TRT+Si tracks created of output"<<std::endl;
  if (nt != nsi) {
    out<<"| "<<std::setw(7)<<nt  <<" total number of tracks on output"<<std::endl;
  }
  out<<"|-------------------------------------------------------------------";
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_SeededTrackFinder::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::TRT_SeededTrackFinder& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_SeededTrackFinder& se)
{
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Merge a Si extension and a TRT segment.Refit at the end
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::TRT_SeededTrackFinder::mergeSegments(const Trk::Track& tT, const Trk::TrackSegment& tS)
{
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
	ATH_MSG_DEBUG ("Too few Si hits.Will keep pseudomeasurement...");
	const Trk::TrackStateOnSurface* seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),0);
	ntsos->push_back(seg_tsos);
      }
    } else {
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Measurement);
      const Trk::TrackStateOnSurface* seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),0,0,0,typePattern);
      ntsos->push_back(seg_tsos);
    }
  }  

  ///Construct the new track
  Trk::TrackInfo info;
  info.setPatternRecognitionInfo(Trk::TrackInfo::TRTSeededTrackFinder);
  Trk::Track* newTrack = new Trk::Track(info, ntsos, fq);

  //Careful refitting at the end
  if(m_doRefit){ 
    Trk::Track* fitTrack = m_fitterTool->fit(*newTrack,false,Trk::pion);
    delete newTrack;
    if(!fitTrack){
      ATH_MSG_DEBUG ("Refit of TRT+Si track segment failed!");
      return 0;
    }
  
    //Protect for tracks that have no really defined locz and theta parameters
    //const Trk::MeasuredPerigee* perTrack=dynamic_cast<const Trk::MeasuredPerigee*>(fitTrack->perigeeParameters());

    const Trk::Perigee* perTrack=fitTrack->perigeeParameters();

    if(perTrack){
      //const Trk::CovarianceMatrix& CM = perTrack->localErrorMatrix().covariance();
      const AmgSymMatrix(5)* CM = perTrack->covariance();
      if((!CM)||(sqrt((*CM)(1,1))==0.)||(sqrt((*CM)(3,3))==0.)){delete fitTrack; return 0;}
    }
    return fitTrack;
  }

  return newTrack;
}

///////////////////////////////////////////////////////////////////
// Transform a TRT segment to track
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::TRT_SeededTrackFinder::segToTrack(const Trk::TrackSegment& tS)
{
  ATH_MSG_DEBUG ("Transforming the TRT segment into a track...");

  //Get the track segment information and build the initial track parameters
  const Trk::StraightLineSurface*       surf = dynamic_cast<const Trk::StraightLineSurface*>(&(tS.associatedSurface()));
  if (!surf) {
    throw std::logic_error("Unhandled surface.");
  }
  const AmgVector(5)&                         p = tS.localParameters();
  AmgSymMatrix(5)*  ep = new AmgSymMatrix(5)(tS.localCovariance());
  
  DataVector<const Trk::TrackStateOnSurface>* ntsos = new DataVector<const Trk::TrackStateOnSurface>;

  std::unique_ptr<const Trk::TrackParameters> segPar( surf->createParameters<5,Trk::Charged>(p(0),p(1),p(2),p(3),p(4),ep) );
  if(segPar){
    if(msgLvl(MSG::DEBUG)) {msg(MSG::DEBUG) << "Initial TRT Segment Parameters for refitting " << (*segPar) << endmsg;}
  }else{
    if(msgLvl(MSG::DEBUG)) {msg(MSG::DEBUG) << "Could not get initial TRT segment parameters! " << endmsg;}
    return 0;
  }

  for(int it=0; it<int(tS.numberOfMeasurementBases()); it++){
    // on first measurement add parameters
    const Trk::TrackStateOnSurface* seg_tsos = 0;
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Measurement);
    if(it==0) 
      seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),segPar.release(),0,0,typePattern);
    else
      seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),0,0,0,typePattern);
    ntsos->push_back(seg_tsos);
  }  

  // Construct the new track
  //  Trk::Track* newTrack = new Trk::Track(Trk::Track::TRT_SeededTrackFinder, ntsos, 0);

  Trk::TrackInfo info;
  info.setPatternRecognitionInfo(Trk::TrackInfo::TRTSeededTrackFinder);
  Trk::Track* newTrack = new Trk::Track(info, ntsos, 0);
  
  // Careful refitting of the TRT stand alone track
  if(m_doRefit){
    Trk::Track* fitTrack = m_fitterTool->fit(*newTrack,false,Trk::pion);
    delete newTrack; // cleanup
    if(!fitTrack){
      ATH_MSG_DEBUG ("Refit of TRT track segment failed!");
      return 0;
    }
  
    //Protect for tracks that have no really defined locz and theta parameters
    const Trk::Perigee* perTrack=fitTrack->perigeeParameters();
    if(perTrack){
      const AmgSymMatrix(5)* CM = perTrack->covariance();
      if((!CM)||(sqrt((*CM)(1,1))==0.)||(sqrt((*CM)(3,3))==0.)){delete fitTrack; return 0;}
    }
    return fitTrack;
  }
 
  return newTrack;
}

///////////////////////////////////////////////////////////////////
// Merge a Si extension and a TRT track extension.Refit at the end
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::TRT_SeededTrackFinder::
mergeExtension(const Trk::Track& tT, std::vector<const Trk::MeasurementBase*>& tS)
{
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

  Trk::TrackInfo info;
  info.setPatternRecognitionInfo(Trk::TrackInfo::TRTSeededTrackFinder);
  Trk::Track* newTrack = new Trk::Track(info, ntsos, fq);

  //Careful refitting at the end
  if(m_doRefit){ 
    Trk::Track* fitTrack = m_fitterTool->fit(*newTrack,false,Trk::pion);
    delete newTrack;
    if(!fitTrack){
      ATH_MSG_DEBUG ("Refit of TRT+Si track segment failed!");
      return 0;
    }
  
    //Protect for tracks that have no really defined locz and theta parameters
    const Trk::Perigee* perTrack=fitTrack->perigeeParameters();
    if(perTrack){
      const AmgSymMatrix(5)* CM = perTrack->covariance();
      if((!CM)||(sqrt((*CM)(1,1))==0.)||(sqrt((*CM)(3,3))==0.)){delete fitTrack; return 0;}
    }
    return fitTrack;
  }

  return newTrack;
}

///////////////////////////////////////////////////////////////////
// Analysis of tracks
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededTrackFinder::Analyze(TrackCollection* tC)
{

  if(msgLvl(MSG::DEBUG)) {msg(MSG::DEBUG) << "Analyzing tracks..." << endmsg;}

  if(msgLvl(MSG::DEBUG)) {msg(MSG::DEBUG) << "Number of back tracks " << (tC->size()) << endl;}
  int tc = 0; //Track counter
  int nsct1, nsct2, nsct3, nsct4; //SCT layer counters
  int nsctTot1, nsctTot2, nsctTot3, nsctTot4; //SCT layer counters
  nsctTot1=nsctTot2=nsctTot3=nsctTot4=0;
  int npix1, npix2, npix3; //Pixel layer counters
  int npixTot1, npixTot2, npixTot3; //Pixel layer counters
  npixTot1=npixTot2=npixTot3=0;
  int nhits, nholes, noutl;
  ///Loop over tracks in track collection
  TrackCollection::const_iterator r    = tC->begin();
  TrackCollection::const_iterator re = tC->end();
  for (; r != re ; ++r){
    tc++; nsct1=nsct2=nsct3=nsct4=0; npix1=npix2=npix3=0; nhits=nholes=noutl=0;
    const DataVector<const Trk::TrackStateOnSurface>* newtsos = (*r)->trackStateOnSurfaces();
    if(!newtsos) continue;
    DataVector<const Trk::TrackStateOnSurface>::const_iterator itp, itpe=newtsos->end();
    for(itp=newtsos->begin(); itp!=itpe; itp++){
      ///Concentrate on the Si component of the track
      const InDet::SiClusterOnTrack* clus = dynamic_cast<const InDet::SiClusterOnTrack*>((*itp)->measurementOnTrack());
      if(clus && ((*itp)->type(Trk::TrackStateOnSurface::Measurement))){  //Count the number of hits used in the track
	double rc = clus->globalPosition().perp();
        if((40.<=rc)&&(rc<80.)){npix1++;}    //1st pixel layer
        if((80.<=rc)&&(rc<100.)){npix2++;}   //2nd pixel layer
        if((100.<=rc)&&(rc<150.)){npix3++;}  //3rd pixel layer
        if((280.<=rc)&&(rc<340.)){nsct1++;}  //1st SCT layer
        if((340.<=rc)&&(rc<390.)){nsct2++;}  //2nd SCT layer
        if((390.<=rc)&&(rc<460.)){nsct3++;}  //3rd SCT layer
        if((460.<=rc)&&(rc<550.)){nsct4++;}  //4th SCT layer
        nhits++;
      }
      if(clus && ((*itp)->type(Trk::TrackStateOnSurface::Outlier))){  //Count the total number of outliers per track
        noutl++;
      }
      if(clus && ((*itp)->type(Trk::TrackStateOnSurface::Hole))){     //Count the total number of holes per track
        nholes++;
      }
    }
    nsctTot1+=nsct1; nsctTot2+=nsct2; nsctTot3+=nsct3; nsctTot4+=nsct4;
    npixTot1+=npix1; npixTot2+=npix2; npixTot3+=npix3;
    //cout << "HITS IN TRACK " << nhits << " OUTLIERS " << noutl << " HOLES " << nholes << endl;
  }
  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG)<<"Total hits on 1st SCT: "<<nsctTot1<<" 2nd SCT: "<<nsctTot2<<" 3rd SCT: "<<nsctTot3<<" 4th SCT: "<<nsctTot4<<endmsg;
    msg(MSG::DEBUG)<<"Total hits on 1st Pixel: "<<npixTot1<<" 2nd Pixel: "<<npixTot2<<" 3rd Pixel: "<<npixTot3<<endmsg;
  }

}

