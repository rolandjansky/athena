/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
Trk::TrkAmbiguityScore::initialize()
{
  ATH_MSG_INFO( "TrkAmbiguityScore::initialize(). " );

  ATH_CHECK(m_scoreTool.retrieve( DisableTool{m_scoreTool.empty()} ));

  ATH_CHECK(m_scoredTracksKey.initialize());
  ATH_CHECK(m_originTracksKey.initialize());

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguityScore::execute(const EventContext& ctx) const
{
  std::vector<SG::ReadHandle<TrackCollection>> handles = m_originTracksKey.makeHandles(ctx);
  size_t totalsize = 0;
  for (SG::ReadHandle<TrackCollection>& trackColHandle : handles) {
     if (!trackColHandle.isValid())
       msg(MSG::WARNING) << "Could not retrieve tracks from "<< trackColHandle.key() << endmsg;
     totalsize += trackColHandle->size();
  }

  std::vector<const Track*> originTracks;
  originTracks.reserve(totalsize);
  for (SG::ReadHandle<TrackCollection>& trackColHandle : handles) {
    for(const Track* trk: *trackColHandle ) {
       if (std::find(originTracks.begin(), originTracks.end(),trk) == originTracks.end()) {
          originTracks.push_back(trk);
       }
    }
  }

  std::unique_ptr<TracksScores> scoredTracks(new TracksScores);
  if (m_scoreTool.isEnabled()){
    m_scoreTool->process(&originTracks, scoredTracks.get());
  }
  else{
    scoredTracks->reserve(originTracks.size());
    for(const Track* trk: originTracks ){
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
