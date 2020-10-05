/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAmbiguitySolver/TrkAmbiguitySolver.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityScoreProcessorTool.h"

Trk::TrkAmbiguitySolver::TrkAmbiguitySolver(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm (name, pSvcLocator),
  m_scoredTracksKey(""),
  m_resolvedTracksKey("Tracks"),
  m_ambiTool("Trk::SimpleAmbiguityProcessorTool/TrkAmbiguityProcessor", this),
  m_trackInCount(0),
  m_trackOutCount(0)
{
  declareProperty("TrackInput"        , m_scoredTracksKey);
  declareProperty("TrackOutput"       , m_resolvedTracksKey);
  declareProperty("AmbiguityProcessor", m_ambiTool);
}

//--------------------------------------------------------------------------
Trk::TrkAmbiguitySolver::~TrkAmbiguitySolver(void)
{}

//-----------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguitySolver::initialize()
{
  ATH_CHECK(m_ambiTool.retrieve());

  ATH_CHECK(m_scoredTracksKey.initialize());
  ATH_CHECK(m_resolvedTracksKey.initialize());
  m_trackInCount=0;
  m_trackOutCount=0;
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguitySolver::execute(const EventContext& ctx) const
{
  ATH_MSG_VERBOSE ("TrkAmbiguitySolver::execute()");
  SG::ReadHandle<TracksScores> scoredTracksHandle(m_scoredTracksKey, ctx);
  if ( !scoredTracksHandle.isValid() )  ATH_MSG_ERROR("Could not read scoredTracks.");
  m_trackInCount += scoredTracksHandle->size();

  std::unique_ptr<TrackCollection> resolvedTracks;
  resolvedTracks.reset(m_ambiTool->process(scoredTracksHandle.cptr())); //note: take ownership and delete
  m_trackOutCount += resolvedTracks->size();

  SG::WriteHandle<TrackCollection> resolvedTracksHandle(m_resolvedTracksKey, ctx);
  ATH_CHECK(resolvedTracksHandle.record(std::move(resolvedTracks)));
  ATH_MSG_VERBOSE ("Saved "<<resolvedTracksHandle->size()<<" tracks");
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode
Trk::TrkAmbiguitySolver::finalize(){
  if (m_ambiTool.isEnabled()) {
      m_ambiTool->statistics();
  }
  ATH_MSG_INFO( "Finalizing with "<< m_trackInCount << " tracks input, and "<< m_trackOutCount<< " output");
  return StatusCode::SUCCESS;
}
