/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAmbiguitySolver/TrkAmbiguityScore.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityScoreProcessorTool.h"
#include "StoreGate/StoreGateSvc.h"

Trk::TrkAmbiguityScore::TrkAmbiguityScore(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm (name, pSvcLocator),
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
Trk::TrkAmbiguityScore::execute()
{
  std::vector<SG::ReadHandle<TrackCollection>> handles = m_originTracksKey.makeHandles();
  size_t totalsize = 0;
  for (SG::ReadHandle<TrackCollection>& trackColHandle : handles) {
     if (!trackColHandle.isValid())    
       msg(MSG::WARNING) << "Could not retrieve tracks from "<< trackColHandle.key() << endmsg;
     totalsize += trackColHandle->size();
  }
  
  std::vector<const Track*> originTracks;
  originTracks.reserve(totalsize);
  for (SG::ReadHandle<TrackCollection>& trackColHandle : handles) {
    for(const Track* trk: *trackColHandle )
      originTracks.push_back(trk);
  }
  
  std::unique_ptr<std::multimap<const Track*, float>> scoredTracks(new std::multimap<const Track*, float>);
  if (m_scoreTool.isEnabled()){
    m_scoreTool->process(&originTracks, scoredTracks.get());
  }
  else{
    for(const Track* trk: originTracks ){
      scoredTracks->insert( std::pair<Track*, float>(new Track(*trk), 0)); //TODO: logpT 
    }
  }
  
  SG::WriteHandle<std::multimap<const Track*, float>> scoredTracksHandle(m_scoredTracksKey);
  ATH_CHECK(scoredTracksHandle.record(std::move(scoredTracks)));
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------


StatusCode
Trk::TrkAmbiguityScore::finalize()
{

  return StatusCode::SUCCESS;
}
