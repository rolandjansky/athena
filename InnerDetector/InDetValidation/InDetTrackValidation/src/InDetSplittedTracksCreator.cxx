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
AthAlgorithm(name, pSvcLocator),
// m_sgSvc(0),
m_TrackCol("Tracks"),
m_trackSplitterTool()

{
  // Properties go here
  declareProperty("TrackCollection"         , m_TrackCol);
  declareProperty("OutputTrackCollection"   , m_OutputTrackCol);
  declareProperty("TrackSplitterTool"       , m_trackSplitterTool);
  declareProperty("makeSiOnlyTracks"        , m_makeSiOnlyTracks=false);
}

InDet::InDetSplittedTracksCreator::~InDetSplittedTracksCreator() {}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode InDet::InDetSplittedTracksCreator::initialize(){

   ATH_MSG_DEBUG("InDetSplittedTracksCreator initialize()");

   if (m_trackSplitterTool.empty()) {
      ATH_MSG_DEBUG("No track splitter tool configured");
   } else if (m_trackSplitterTool.retrieve().isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)<< "Could not retrieve InDetTrackSplitterTool" << endreq; 
   } else{ 
      ATH_MSG_VERBOSE ("Retrieved tool InDetTrackSplitterTool");
   }

   return StatusCode::SUCCESS;
}   
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode InDet::InDetSplittedTracksCreator::execute() {
   ATH_MSG_DEBUG("InDetSplittedTracksCreator execute()");

   //return if no input collection is found
   const TrackCollection* trks = 0;
   if (evtStore()->contains<TrackCollection>(m_TrackCol) &&
        (evtStore()->retrieve(trks,m_TrackCol)).isFailure()){
      ATH_MSG_ERROR(" Could not retrieve the track collection" <<m_TrackCol);
      return StatusCode::SUCCESS;
   }

   TrackCollection* splittedTracks=0;
   if (evtStore()->contains<TrackCollection>(m_OutputTrackCol) &&
        (evtStore()->retrieve(splittedTracks,m_OutputTrackCol)).isFailure()){
   } else {
     splittedTracks = new TrackCollection;
   }

   //loop over tracks and split them in upper and lower part
   TrackCollection::const_iterator it = trks->begin();
   TrackCollection::const_iterator itE = trks->end();
   for (; it != itE; ++it){
     
     std::pair<Trk::Track*, Trk::Track*> splitTracks =  std::pair<Trk::Track*, Trk::Track*> (0,0);
     if (m_makeSiOnlyTracks){
       splitTracks = m_trackSplitterTool->splitInUpperLowerTrack(**it,true); //use Si only hits
     }
     else {
       splitTracks = m_trackSplitterTool->splitInUpperLowerTrack(**it);
     }
     //only save them if both track fits succeeded       
     if(splitTracks.first && splitTracks.second){
       splittedTracks->push_back( splitTracks.first );
       splittedTracks->push_back( splitTracks.second );
     } else {
       delete splitTracks.first; delete splitTracks.second;
     }
       
   }
   ATH_MSG_DEBUG("SplittedTracks->size() " <<splittedTracks->size());
   
   if(evtStore()->record(splittedTracks, m_OutputTrackCol,false).isFailure() ){
     ATH_MSG_ERROR("Could not save the "<<m_OutputTrackCol);
   }

   return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode InDet::InDetSplittedTracksCreator::finalize() {
    ATH_MSG_INFO("InDetSplittedTracksCreator finalize()");
    return StatusCode::SUCCESS;
}

void InDet::InDetSplittedTracksCreator::copyTrackCollection(const TrackCollection* inputTrackCol,  TrackCollection* outputTrackCol) { 
  if(inputTrackCol && inputTrackCol->size()) {
    ATH_MSG_VERBOSE("Size of track collection " << inputTrackCol->size());
    // loop over tracks
    TrackCollection::const_iterator rf,rfe=inputTrackCol->end();
    for(rf=inputTrackCol->begin();rf!=rfe; ++rf){
      // add track into output
      Trk::Track* newTrack = new Trk::Track(**rf);
      outputTrackCol->push_back(newTrack);
    }
  }
}

void InDet::InDetSplittedTracksCreator::copyTrackCollection(const TrackCollection* inputTrackCol_upper,const TrackCollection* inputTrackCol_lower,   TrackCollection* outputTrackCol) { 
  if(inputTrackCol_upper && inputTrackCol_upper->size() && inputTrackCol_lower && inputTrackCol_lower->size() &&  inputTrackCol_upper->size() == inputTrackCol_lower->size()) {
    ATH_MSG_VERBOSE ("Size of track collection upper" << inputTrackCol_upper->size());
    ATH_MSG_VERBOSE ("Size of track collection lower" << inputTrackCol_lower->size());
    
    // loop over tracks
    TrackCollection::const_iterator rfU,rfeU=inputTrackCol_upper->end();
    TrackCollection::const_iterator rfL,rfeL=inputTrackCol_lower->end();
    
    for(rfU=inputTrackCol_upper->begin(),rfL=inputTrackCol_lower->begin();rfU!=rfeU; ++rfU,++rfL){
      
//     for(rfL=inputTrackCol_lower->begin();rfL!=rfeL; ++rfL){
      // add track into output
      Trk::Track* newTrack_U = new Trk::Track(**rfU);
      Trk::Track* newTrack_L = new Trk::Track(**rfL);
      outputTrackCol->push_back(newTrack_U);
      outputTrackCol->push_back(newTrack_L);
      
    }
  }
}
