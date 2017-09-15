/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackValidation/InDetSplittedTracksCreator.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "InDetTrackSplitterTool/IInDetTrackSplitterTool.h"




/////////////////////////////////////////////////////////////////////////////
InDet::InDetSplittedTracksCreator::InDetSplittedTracksCreator(const std::string& name, ISvcLocator* pSvcLocator) :
AthReentrantAlgorithm(name, pSvcLocator),
// m_sgSvc(0),
m_TrackCol("Tracks"),
m_trackSplitterTool()

{
  // Properties go here
  declareProperty("TrackCollection"         , m_TrackCol);
  declareProperty("OutputTrackCollection"   , m_OutputTrackCol);
  declareProperty("TrackSplitterTool"       , m_trackSplitterTool);
  declareProperty("makeSiOnlyTracks"        , m_makeSiOnlyTracks=false);
  declareProperty("takeUpperSegment"        , m_takeUpperSegment=true);
  declareProperty("takeLowerSegment"        , m_takeLowerSegment=true);
}

InDet::InDetSplittedTracksCreator::~InDetSplittedTracksCreator() {}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode InDet::InDetSplittedTracksCreator::initialize(){

   ATH_MSG_DEBUG("InDetSplittedTracksCreator initialize()");

   if (m_trackSplitterTool.empty()) {
      ATH_MSG_DEBUG("No track splitter tool configured");
   } else if (m_trackSplitterTool.retrieve().isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)<< "Could not retrieve InDetTrackSplitterTool" << endmsg; 
   } else{ 
      ATH_MSG_VERBOSE ("Retrieved tool InDetTrackSplitterTool");
   }
   ATH_CHECK( m_TrackCol.initialize() );
   ATH_CHECK( m_OutputTrackCol.initialize() );
   return StatusCode::SUCCESS;
}   
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode InDet::InDetSplittedTracksCreator::execute_r(const EventContext& ctx) const {
   ATH_MSG_DEBUG("InDetSplittedTracksCreator execute()");
   ATH_MSG_DEBUG("Input Track Collection: " << m_TrackCol);

   //return if no input collection is found
   SG::ReadHandle<TrackCollection> trks(m_TrackCol,ctx);
   if (!trks.isValid()) {
       ATH_MSG_ERROR(" Could not retrieve the track collection " <<m_TrackCol.key() );
       return StatusCode::SUCCESS;
   }

   // @In athenaMT the algorithm should not be scheduled if the splitted tracks exist already or they are overwritten
   // TrackCollection* splittedTracks=0;
   // if ((not evtStore()->contains<TrackCollection>(m_OutputTrackCol)) or
   //     (evtStore()->retrieve(splittedTracks,m_OutputTrackCol)).isFailure()) {
   //   splittedTracks = new TrackCollection;
   // } else {
   //   ATH_MSG_WARNING("Output split track collection already exists: " << m_OutputTrackCol.key() << ". Quitting.");
   //   return StatusCode::SUCCESS;
   // }

    SG::WriteHandle<TrackCollection> splittedTracks(m_OutputTrackCol,ctx);
    if (splittedTracks.record( std::make_unique<TrackCollection>() ).isFailure()){
      ATH_MSG_ERROR("Failed to record output collection " << m_OutputTrackCol.key());
      return StatusCode::FAILURE;
    }


   //loop over tracks and split them in upper and lower part
   TrackCollection::const_iterator it = trks->begin();
   TrackCollection::const_iterator itE = trks->end();
   for (; it != itE; ++it){
     ATH_MSG_VERBOSE("Splitting Track");
     std::pair<Trk::Track*, Trk::Track*> splitTracks =  std::pair<Trk::Track*, Trk::Track*> (0,0);
     splitTracks = m_trackSplitterTool->splitInUpperLowerTrack(**it,m_makeSiOnlyTracks);
     //only save them if both track fits succeeded       
     if(splitTracks.first && splitTracks.second){
       if (m_takeUpperSegment) {
	 ATH_MSG_DEBUG("** InDetSplittedTracksCreator ** Accept upper segment ");
	 splittedTracks->push_back( splitTracks.first );
       }
       if (m_takeLowerSegment) {
	 ATH_MSG_DEBUG("** InDetSplittedTracksCreator ** Accept lower segment ");
	 splittedTracks->push_back( splitTracks.second );
       }
     } else {
       delete splitTracks.first; delete splitTracks.second;
     }
       
   }
   ATH_MSG_DEBUG("** InDetSplittedTracksCreator ** SplittedTracks->size() " << splittedTracks->size() << "  stored in " << m_OutputTrackCol.key() );
   

   return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode InDet::InDetSplittedTracksCreator::finalize() {
    ATH_MSG_VERBOSE("InDetSplittedTracksCreator finalize()");
    return StatusCode::SUCCESS;
}
