/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAmbiguitySolver/TrkAmbiguityScore.h"
#include "TrkToolInterfaces/ITrackAmbiguityScoreProcessorTool.h"

Trk::TrkAmbiguityScore::TrkAmbiguityScore(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm (name, pSvcLocator),
  m_originTracksKey{""},
  m_scoredTracksKey("Tracks"),
  m_scoreTool("",this)
{
  declareProperty("TrackInput"        , m_originTracksKey);
  declareProperty("TrackOutput"       , m_scoredTracksKey);
  declareProperty("AmbiguityScoreProcessor", m_scoreTool);
}

//--------------------------------------------------------------------------
Trk::TrkAmbiguityScore::~TrkAmbiguityScore(void)
{}

//-----------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguityScore::initialize(){
  ATH_MSG_VERBOSE( "TrkAmbiguityScore::initialize(). " );
  ATH_CHECK(m_scoreTool.retrieve( DisableTool{m_scoreTool.empty()} ));
  ATH_CHECK(m_scoredTracksKey.initialize());
  ATH_CHECK(m_originTracksKey.initialize());
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguityScore::execute(const EventContext& ctx) const{
  std::vector<SG::ReadHandle<TrackCollection>> handles = m_originTracksKey.makeHandles(ctx);
  ATH_MSG_DEBUG(handles.size() << " handles are requested");
  if (handles.size() > 1){
    ATH_MSG_WARNING("More than one collection has been requested. Only the first collection is taken");
  }
  auto & theTrackCollectionHandle = handles.at(0);
  if (not theTrackCollectionHandle.isValid()){
    ATH_MSG_FATAL( "Could not retrieve tracks from "<< theTrackCollectionHandle.key() );
    return StatusCode::FAILURE;
  }
  const auto & theTrackCollection = *theTrackCollectionHandle;
  std::unique_ptr<TracksScores> scoredTracks(new TracksScores);
  if (m_scoreTool.isEnabled()){
    m_scoreTool->process(theTrackCollection, scoredTracks.get());
  } else {
    scoredTracks->reserve(theTrackCollection.size());
    for(const Track* trk: theTrackCollection ){
      scoredTracks->push_back( std::pair<const Track*, float>(trk, 0));//TODO: logpT
    }
  }

  SG::WriteHandle<TracksScores> scoredTracksHandle(m_scoredTracksKey, ctx);
  ATH_CHECK(scoredTracksHandle.record(std::move(scoredTracks)));
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------


StatusCode
Trk::TrkAmbiguityScore::finalize()
{
  if (m_scoreTool.isEnabled()) {
     m_scoreTool->statistics();
  }
  return StatusCode::SUCCESS;
}
