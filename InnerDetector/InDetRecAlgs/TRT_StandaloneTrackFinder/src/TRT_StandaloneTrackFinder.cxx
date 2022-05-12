/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_StandaloneTrackFinder
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 09/28/2007 Thomas Koffas
// update by Markus Elsing
///////////////////////////////////////////////////////////////////

#include "TRT_StandaloneTrackFinder/TRT_StandaloneTrackFinder.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"

///Track events
#include "TrkSegment/TrackSegment.h"

#include "GaudiKernel/SystemOfUnits.h"
using Gaudi::Units::GeV;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_StandaloneTrackFinder::TRT_StandaloneTrackFinder
(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
  
  m_minNumDriftCircles = 15                                   ;       //Minimum number of drift circles for TRT segment tracks
  m_minPt              = 1.0 * GeV                            ;       //pt cut    
  m_matEffects         = 0                                    ;

  declareProperty("MinNumDriftCircles"         ,m_minNumDriftCircles); //Minimum number of drift circles for TRT segment tracks
  declareProperty("MinPt"                      ,m_minPt             ); //Minimum Pt in preselection
  declareProperty("MaterialEffects"            ,m_matEffects        ); //Particle hypothesis during track fitting
  declareProperty("OldTransitionLogic"         ,m_oldLogic = true   ); //use old transition logic for hits 
}

InDet::TRT_StandaloneTrackFinder::~TRT_StandaloneTrackFinder()
= default;

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_StandaloneTrackFinder::initialize()
{

  StatusCode  sc;
  m_total = {};

  ATH_MSG_DEBUG ("Initializing TRT_StandaloneTrackFinder");

  ATH_CHECK(m_segToTrackTool.retrieve());

  ATH_CHECK(m_Segments.initialize());
  ATH_CHECK(m_prdToTrackMap.initialize(!m_prdToTrackMap.key().empty()));
  ATH_CHECK(m_finalTracks.initialize());

  // Get output print level
  //
  ATH_MSG_DEBUG ( (*this) );
  return sc;

}

/////////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
StatusCode InDet::TRT_StandaloneTrackFinder::execute(const EventContext& ctx) const
{

  Counter_t counter {};
  ITRT_SegmentToTrackTool::EventData event_data;

  ///Retrieve segments from StoreGate
  //

  SG::ReadHandle<Trk::SegmentCollection > segments(m_Segments, ctx);
  if(!segments.isValid()){
      ATH_MSG_FATAL ("No segment with name " << m_Segments.key() << " found in StoreGate!");
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<Trk::PRDtoTrackMap > prd_to_track_map;
  const Trk::PRDtoTrackMap *prd_to_track_map_cptr = nullptr;
  if (!m_prdToTrackMap.key().empty()) {
    prd_to_track_map=SG::ReadHandle<Trk::PRDtoTrackMap >(m_prdToTrackMap, ctx);
    if(!prd_to_track_map.isValid()){
      ATH_MSG_FATAL ("Failed to get " << m_prdToTrackMap.key() << ".");
      return StatusCode::FAILURE;
    }
    prd_to_track_map_cptr = prd_to_track_map.cptr();
  }

  // statistics...
  counter[kNTrtSeg] = int(segments->size());
  ATH_MSG_DEBUG ("TRT track container size " << counter[kNTrtSeg]);
  ATH_MSG_DEBUG ("Begin looping over all TRT segments in the event");
  for(const Trk::Segment *base_segment: *segments) {

    // Get the track segment
    const Trk::TrackSegment *trackTRT = dynamic_cast<const Trk::TrackSegment*>(base_segment);

    if(!trackTRT) {

      ATH_MSG_WARNING ("No pointer to segment, should not happen !");
      continue;

    } else {

      ATH_MSG_DEBUG ("--> start evaluating new segment");

      // start with pt preselection, get the segment parameters
      const Amg::VectorX& p = trackTRT->localParameters();
      if ( fabs(sin(p(3))/p(4)) < m_minPt*0.9 ) {
	// Statistics...
	counter[kNRejectedSeg]++;
	ATH_MSG_DEBUG ("Segment pt = " << fabs(sin(p(3))/p(4)) << " , fails pre-cut, drop it !");
	continue;
      }

      // Check if segment has already been assigned to a BackTrack
      if(m_segToTrackTool->segIsUsed(*trackTRT, prd_to_track_map_cptr)) {
      	// Statistics...
	counter[kNUsedSeg]++;
	ATH_MSG_DEBUG ("Segment excluded by BackTrack, drop it !");
	continue;
      }

      // Get the number of the TRT track segment ROTs
      int  nROTs = trackTRT->numberOfMeasurementBases();
      ATH_MSG_DEBUG ("Number Of ROTs " << nROTs);

      // requited number of drift circles
      int  minDriftCircles = m_minNumDriftCircles; 

      bool is_toLower = false; // to handle special case 

      // Cases where the min number of required TRT drift circles drops to 10
      if(nROTs <= minDriftCircles && m_oldLogic) {
	ATH_MSG_DEBUG ("Few DCs, can we recover ?");

	is_toLower = m_segToTrackTool->toLower(*trackTRT);
	if (is_toLower) {
	  ATH_MSG_DEBUG ("We recovered this one, let's try...");
	}
      }

      if((nROTs < m_minNumDriftCircles) && !is_toLower) {
	// statistics...
	counter[kNRejectedSeg]++;
	ATH_MSG_DEBUG ("Segment fails number of DC requirements, reject it");
      }
      else {
	// statistics
        counter[kNTrtSegGood]++;
	ATH_MSG_DEBUG ("Segment considered for further processing, enter into list");

	// Transform the original TRT segment into a track
	Trk::Track* trtSeg = m_segToTrackTool->segToTrack(ctx, *trackTRT);
	if(!trtSeg){
	  // Statistics...
	  counter[kNSegFailed]++;
	  ATH_MSG_DEBUG ("Failed to make a track out of the TRT segment!");
	  continue;
	}

	// get all TSOS
	const DataVector<const Trk::MeasurementBase>* ttsos = trtSeg->measurementsOnTrack();
	if((int)ttsos->size()<10) {
	  // Statistics...
	  counter[kNSegFailed]++;
	  ATH_MSG_DEBUG ("Too few ROTs on track, reject !");
          delete trtSeg;
          continue;
        }
	// add the track to list
        m_segToTrackTool->addNewTrack(trtSeg,event_data);
      }
    }
  }

  // now resolve tracks
  ATH_MSG_DEBUG ("Creating track container ");

  std::unique_ptr<TrackCollection> final_tracks(m_segToTrackTool->resolveTracks(prd_to_track_map.cptr(),event_data));
  if (!final_tracks || SG::WriteHandle<TrackCollection>(m_finalTracks,ctx).record(std::move(final_tracks)).isFailure()) {
      ATH_MSG_WARNING ("Could not save the reconstructed TRT seeded Si tracks!");
      return StatusCode::FAILURE;
  }

  // Update the total counters
  counter[kNTrkScoreZeroTotal] = event_data.m_counter[ITRT_SegmentToTrackTool::EventData::knTrkScoreZero];
  counter[kNTrkSegUsedTotal]   = event_data.m_counter[ITRT_SegmentToTrackTool::EventData::knTrkSegUsed];
  counter[kNTRTTrkTotal]       = event_data.m_counter[ITRT_SegmentToTrackTool::EventData::knTRTTrk];
  {
     std::lock_guard<std::mutex> lock(m_statMutex);
     for (int idx=0; idx< kNCounter; ++idx) {
        m_total[idx]+=counter[idx];
     }
  }

  // Print common event information
  
 ATH_MSG_DEBUG( counter  << std::endl );
  

   
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_StandaloneTrackFinder::finalize() 
{
  ATH_MSG_INFO( m_total  << std::endl );   
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream&  InDet::TRT_StandaloneTrackFinder::dump( MsgStream& out ) const
{
  // out<<std::endl;
  
  return dumpContainerNames(out);
}


MsgStream& InDet::TRT_StandaloneTrackFinder::dumpevent(MsgStream &out, const InDet::TRT_StandaloneTrackFinder::Counter_t& counter) {
  out<<"| counters |" << counter << std::endl; 
  return out;
}



///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_StandaloneTrackFinder::dumpContainerNames( MsgStream& out ) const
{
  const std::string::size_type max_width =65;
  
  out<<std::endl
     <<"|----------------------------------------------------------------------"
     <<"-------------------|"<<std::endl;
  out<<"| Location of input tracks          | "<<m_Segments.key()       << std::setw( max_width-m_Segments.key().size())<<  " " << "|"    <<std::endl;
  out<<"| Location of output tracks         | "<<m_finalTracks.key()    << std::setw( max_width-m_finalTracks.key().size()) << " " << "|" <<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|";
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator<<( MsgStream& out,  const InDet::TRT_StandaloneTrackFinder::Counter_t &counter) 
{   
out<<std::endl 
     <<"|-------------------------------------------------------------------|" <<std::endl;
     out<<"|  Investigated  |" <<std::endl;
     out<<"| "<<std::setw(7)<<counter[InDet::TRT_StandaloneTrackFinder::kNTrtSeg]            <<" input TRT segments to be investigated" <<std::endl;
     out<<"| "<<std::setw(7)<<counter[InDet::TRT_StandaloneTrackFinder::kNUsedSeg]           <<" TRT segments excluded at input (by BackTracking tracks)" <<std::endl;
     out<<"| "<<std::setw(7)<<counter[InDet::TRT_StandaloneTrackFinder::kNRejectedSeg]       <<" segments rejected in selection at input" <<std::endl;
     out<<"|-------------------------------------------------------------------|" <<std::endl;
     out<<"| "<<std::setw(7)<<counter[InDet::TRT_StandaloneTrackFinder::kNTrtSegGood]        <<" input TRT segments after cuts" <<std::endl;
     out<<"| "<<std::setw(7)<<counter[InDet::TRT_StandaloneTrackFinder::kNSegFailed]         <<" segments failing to translate to a track (including refit)" <<std::endl;
     out<<"| "<<std::setw(7)<<counter[InDet::TRT_StandaloneTrackFinder::kNTrkScoreZeroTotal] <<" tracks rejected by score zero" <<std::endl;
     out<<"| "<<std::setw(7)<<counter[InDet::TRT_StandaloneTrackFinder::kNTrkSegUsedTotal]   <<" excluded segments by other TRT segment" <<std::endl;
     out<<"| "<<std::setw(7)<<counter[InDet::TRT_StandaloneTrackFinder::kNTRTTrkTotal]       <<" TRT-only tracks on output" <<std::endl;
     out<<"|-------------------------------------------------------------------|";
  return out;
}


///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& operator<<(MsgStream& sl, const InDet::TRT_StandaloneTrackFinder& se) 
{ 
  return se.dump(sl); 
}

   
