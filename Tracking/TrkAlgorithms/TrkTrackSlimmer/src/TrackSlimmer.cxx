/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSlimmer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkTrackSlimmer/TrackSlimmer.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"

//================ Constructor =================================================

Trk::TrackSlimmer::TrackSlimmer(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_slimTool("Trk::TrkTrackSlimmingTool/TrkTrackSlimmingTool"),
  m_ptCut(0.0),
  m_numSlimmedTracks(0),
  m_numOriginalTracks(0)
{
  
  m_trackLocation.push_back("ConvertedMooreTracks");
  m_trackLocation.push_back("ConvertedMuonboyTracks");
  m_slimmedTracksLocation.push_back("SlimmedMooreTracks");
  m_slimmedTracksLocation.push_back("SlimmedMuonboyTracks");
  
  //  template for property decalration
  declareProperty("TrackSlimmingTool", m_slimTool);
  declareProperty("TrackLocation", m_trackLocation);
  declareProperty("SlimmedTrackLocation", m_slimmedTracksLocation);
  declareProperty("PtCut", m_ptCut);
}

//================ Destructor =================================================

Trk::TrackSlimmer::~TrackSlimmer()
{}


//================ Initialisation =================================================

StatusCode Trk::TrackSlimmer::initialize()
{
  // Code entered here will be executed once at program start.

  msg(MSG::INFO) << name() << " initialize()" << endmsg;

  if ( m_slimTool.retrieve().isFailure() ) {
    msg(MSG::ERROR) << "Failed to retrieve TrkTrackSlimmingTool tool "<< m_slimTool << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_slimTool << endmsg;
  }

  msg(MSG::INFO) << "initialize() successful in " << name() << endmsg;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::TrackSlimmer::finalize()
{
  msg(MSG::INFO) << "Produced "<< m_numSlimmedTracks<<" slimmed tracks from "<<m_numOriginalTracks<<" original tracks"<<endmsg;
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::TrackSlimmer::execute()
{
  // Code entered here will be executed once per event
  int numTracksFailingPtCut=0;


  using namespace std;
  vector<string>::const_iterator trackLoc         = m_trackLocation.begin();
  vector<string>::const_iterator slimmedTrackLoc  = m_slimmedTracksLocation.begin();
  vector<string>::const_iterator trackLocEnd      = m_trackLocation.end();
  for (;trackLoc!=trackLocEnd;++trackLoc, ++slimmedTrackLoc)
    {
      const TrackCollection* container;
      StatusCode sc = evtStore()->retrieve(container, *trackLoc);
      if (sc.isSuccess())
        {
	  
	  //create container for slimmed tracks
	  TrackCollection* slimmedTracks = new TrackCollection;
	  
	  //loop through tracks, slimming them as you go.
	  TrackCollection::const_iterator it    = container->begin();
	  TrackCollection::const_iterator itEnd = container->end();
	  for (;it!=itEnd;it++)
            {
	      const DataVector< const Trk::TrackParameters > * params = (**it).trackParameters();
	      if ( params!=0 && params->size()>0 && params->front()->pT()>m_ptCut )
                {
		  Track* slimmed = m_slimTool->slim(**it);
		  slimmedTracks->push_back(slimmed);
            //m_log<<MSG::INFO<<"Original Track"<<(**it)<<endmsg;
            //m_log<<MSG::INFO<<"Slimmed Track"<<(*slimmed)<<endmsg;
                } else {
		  if (params!=0 && params->size()>0) numTracksFailingPtCut++;
                }
            }

            sc = evtStore()->record(slimmedTracks, *slimmedTrackLoc, false);
	    
	    // info for stats in finalise
            m_numSlimmedTracks+=slimmedTracks->size();
            m_numOriginalTracks+=container->size();

            if (sc.isFailure())
	      msg(MSG::ERROR) << "Could not record tracks" << endmsg;
            else
	      if (msgLvl(MSG::VERBOSE)) 
                msg(MSG::VERBOSE) << "Saved "<<slimmedTracks->size()<<" slimmed tracks (to "<<*slimmedTrackLoc
		                  <<"), from input ("<<*trackLoc<<") of "<<container->size()<<" original tracks (which makes "
			          << m_numSlimmedTracks<<"/"<<m_numOriginalTracks<<" so far). "
				  << numTracksFailingPtCut<<" tracks failed pT cut of "<<m_ptCut<<endmsg; 
        }
    }
  return StatusCode::SUCCESS;
}

//============================================================================================
