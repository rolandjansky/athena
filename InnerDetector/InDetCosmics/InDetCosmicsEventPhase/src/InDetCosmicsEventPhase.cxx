/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetCosmicsEventPhase/InDetCosmicsEventPhase.h"

#include "TrkTrack/Track.h"
#include "TrkSegment/SegmentCollection.h"

#include "TrkTrack/TrackCollection.h"
//#include "TrkParameters/Perigee.h"
//#include "TrkParameters/AtaStraightLine.h"
//#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "GaudiKernel/ListItem.h"

#include "CommissionEvent/ComTime.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "TRT_ConditionsData/RtRelation.h"
#include "TRT_ConditionsData/BasicRtRelation.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "InDetIdentifier/TRT_ID.h"


//#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

/////////////////////////////////////////////////////////////////////////////
//using namespace std;

namespace InDet
{
  InDetCosmicsEventPhase::InDetCosmicsEventPhase(const std::string& name, ISvcLocator* pSvcLocator) : 
    AthAlgorithm(name, pSvcLocator),
    m_inputtracksnames(0),
    m_trtconddbsvc("TRT_CalDbSvc",name),
    m_eventPhaseTool()
  {
    m_inputtracksnames.push_back("Tracks");
    
    declareProperty("InputTracksNames" ,m_inputtracksnames);
    declareProperty("InputSegmentsName",m_inputsegmentsname="Segments");
    declareProperty("TRTCalDbSvc"      , m_trtconddbsvc);
    declareProperty("TrackSummaryTool" , m_trackSumTool);
    declareProperty("EventPhaseTool"   , m_eventPhaseTool);
  }
  
  StatusCode InDetCosmicsEventPhase::initialize()
  {
    ATH_MSG_INFO( "initialize()" );
  
    StatusCode sc= m_trtconddbsvc.retrieve();
    if(sc.isFailure())
      {
	ATH_MSG_FATAL("Failed to retrieve TRT Calibration DB Service!");
	return sc;
      }
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode InDetCosmicsEventPhase::beginRun()
  {
    m_event=0;
    //m_eventPhaseTool->beginRun();  // To access conditions, move this
    return StatusCode::SUCCESS;
  }
  
  StatusCode InDetCosmicsEventPhase::execute() 
  {
    ++m_event;
    ATH_MSG_DEBUG ("execute() event: " << m_event);
    m_phase=0;    
    if(m_event==1) m_eventPhaseTool->beginRun(); //to here

    const Trk::Track* selected=0;
    int maxTRT=-1;

// Sasa Nov 30

    const TRT_ID *TRTHelper; 
    StatusCode sc = detStore()->retrieve(TRTHelper, "TRT_ID");
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << "Unable to retrieve TRT ID Helper." << endmsg;
      return sc;
    }

    std::vector<double> EPforTracksWithBarrelHits;

    std::vector<std::string>::const_iterator trackColIt  = m_inputtracksnames.begin();
    std::vector<std::string>::const_iterator trackColEnd = m_inputtracksnames.end();
    for( ; trackColIt != trackColEnd; ++trackColIt)
      {
	const TrackCollection *trtbarreltracks=0;
	StatusCode sc = evtStore()->retrieve(trtbarreltracks,*trackColIt);
	if (sc.isFailure())
	  {
	    ATH_MSG_FATAL( "Could not retrieve input tracks " << *trackColIt );
	    return sc;
	  }
	ATH_MSG_DEBUG ( *trackColIt << " retrieved from StoreGate");
	
	//TrackCollection::const_iterator trackit    = trtbarreltracks->begin();
	TrackCollection::const_iterator trackIt  = trtbarreltracks->begin();
	TrackCollection::const_iterator trackEnd = trtbarreltracks->end();
	for(; trackIt != trackEnd; ++trackIt)
	  {

// Sasa

          int nTRTbarrelHits = 0;
          Trk::Track const * track = *trackIt;
          for (Trk::TrackStateOnSurface const* state : *track->trackStateOnSurfaces()) {

            Trk::MeasurementBase const * mesb = state->measurementOnTrack();
            if (!mesb || !state->type(Trk::TrackStateOnSurface::Measurement) ) continue;
            InDet::TRT_DriftCircleOnTrack const * trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const *>(mesb);
            if( !trtcirc ) continue;
            Identifier const & id = trtcirc->identify();
            if (abs(TRTHelper->barrel_ec(id))!=1) continue;
            nTRTbarrelHits++;
          }

          if (nTRTbarrelHits>=10) { // require at least 10 barrel TRT hits
            double tmpEP = m_eventPhaseTool->findPhase(track);
            if (tmpEP!=0.) EPforTracksWithBarrelHits.push_back(tmpEP);
          }
	    
	    if( nTRTbarrelHits > maxTRT )
	      {
		maxTRT=nTRTbarrelHits;
		selected=*trackIt; // track with max N of barrel hits
	      }
	  }
      }

    m_phase = 0.;
    
    if ( 0 && selected ) { // used to be default in cosmic to use only one track
	ATH_MSG_DEBUG ( "Using track with " << maxTRT << " TRT Hits" );
	m_phase = m_eventPhaseTool->findPhase(selected);
    }

// Sasa Dec 2009: use average instead of result for one track
    if (EPforTracksWithBarrelHits.size()) {
      for (unsigned int i=0; i<EPforTracksWithBarrelHits.size(); i++) m_phase += EPforTracksWithBarrelHits[i];
      m_phase /= (double) (EPforTracksWithBarrelHits.size());
    }
	
    ATH_MSG_DEBUG ( "Event Phase - " << m_phase );

/*
    else
      if (0) { // Sasa Dec 30 - skip segments

	//check for endcap segments
	
	// input TrackSegment Collection
	const Trk::SegmentCollection *inputSegments   = 0;
	StatusCode sc;
	sc = evtStore()->retrieve(inputSegments, m_inputsegmentsname);
	if (sc.isFailure())
	  {
	    ATH_MSG_FATAL( "Could not retrieve input segments " << m_inputsegmentsname );
	    return sc;
	  }
	ATH_MSG_DEBUG("Segments " << m_inputsegmentsname << " retrieved from StoreGate");
	
	Trk::SegmentCollection::const_iterator iseg    = inputSegments->begin();
	Trk::SegmentCollection::const_iterator iseg2   = inputSegments->begin();
	Trk::SegmentCollection::const_iterator isegEnd = inputSegments->end();
	for(; iseg != isegEnd; ++ iseg) 
	  {
	    if( (*iseg)->numberOfMeasurementBases() > (*iseg2)->numberOfMeasurementBases() )
	      {
		iseg2=iseg;
	      }
	  }
	if( iseg2 != isegEnd )
	  {
	    m_phase = m_eventPhaseTool->findPhase(*iseg2);
	  }
      }
  */  
    storePhase();
    
    ATH_MSG_DEBUG("exit InDetCosmicsEventPhase");
    return StatusCode::SUCCESS;
  }
  


  StatusCode InDetCosmicsEventPhase::finalize() {
    
    ATH_MSG_INFO( "finalize()" );
    return StatusCode::SUCCESS;
  }
  
  
  void InDetCosmicsEventPhase::storePhase()
  {
    
    ComTime *ctime=new ComTime(m_phase,m_phase);
    ATH_MSG_DEBUG("Recording phase...  " << m_phase);
    StatusCode sc = evtStore()->record(ctime,"TRT_Phase");
    if (sc.isFailure())
      {
	ATH_MSG_FATAL( "TRT_Phase could not be recorded in StoreGate !" );
      }
  }   
} 
