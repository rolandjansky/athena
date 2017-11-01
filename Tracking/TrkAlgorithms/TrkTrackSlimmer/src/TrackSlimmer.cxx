/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSlimmer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkTrackSlimmer/TrackSlimmer.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "AthContainers/ConstDataVector.h"

//================ Constructor =================================================

Trk::TrackSlimmer::TrackSlimmer(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_slimTool("Trk::TrkTrackSlimmingTool/TrkTrackSlimmingTool"),
    m_ptCut(0.0),
    m_trackLocation{"ConvertedMooreTracks"},
    m_slimmedTracks{"SlimmedMooreTracks"},
    m_numSlimmedTracks(0),
    m_numOriginalTracks(0)
{
    //  template for property decalration
    declareProperty("TrackSlimmingTool", m_slimTool);
    declareProperty("TrackLocation", m_trackLocation);
    declareProperty("SlimmedTrackLocation", m_slimmedTracks);
    declareProperty("PtCut", m_ptCut);
    declareProperty("OnlySetPersistificationHints", m_setPersistificationHints, "Only set persistification hints in each track state on surface");
}

//================ Destructor =================================================

Trk::TrackSlimmer::~TrackSlimmer()
{}


//================ Initialisation =================================================

StatusCode Trk::TrackSlimmer::initialize()
{
    // Code entered here will be executed once at program start.

    ATH_MSG_INFO(name() << " initialize()" );

    if ( m_slimTool.retrieve().isFailure() ) {
        ATH_MSG_ERROR("Failed to retrieve TrkTrackSlimmingTool tool "<< m_slimTool );
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("Retrieved tool " << m_slimTool );
    }
    ATH_CHECK(m_trackLocation.initialize());
    ATH_CHECK(m_slimmedTracks.initialize());

    ATH_MSG_INFO("initialize() successful in " << name() );
    bool error = m_trackLocation.size() != m_slimmedTracks.size();
    if(error){
       ATH_MSG_ERROR("Inconsistent number of inputs and output in " << name() );
       ATH_MSG_ERROR("Inputs " << m_trackLocation.size() << " outputs " << m_slimmedTracks.size() );
    }
    if(msgLvl(MSG::DEBUG) || error){
       auto level = error  ? MSG::ERROR : MSG::DEBUG;
       for(auto &key : m_trackLocation) msg(level) << "TrackLocation " << key.key() << endmsg;
       for(auto &key : m_slimmedTracks) msg(level) << "SlimmedTrackLocation " << key.key() << endmsg;
    }
    if(error) return StatusCode::FAILURE;
    
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::TrackSlimmer::finalize()
{
    ATH_MSG_INFO("Produced "<< m_numSlimmedTracks<<" slimmed tracks from "<<m_numOriginalTracks<<" original tracks");
    // Code entered here will be executed once at the end of the program run.
    return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::TrackSlimmer::execute()
{
    // Code entered here will be executed once per event
    int numTracksFailingPtCut=0;


    using namespace std;
    auto InputHandles = m_trackLocation.makeHandles();
    auto OutputHandles = m_slimmedTracks.makeHandles();
    auto InputIt  = InputHandles.begin();
    auto InputE   = InputHandles.end();
    auto OutputIt = OutputHandles.begin();

    for(; InputIt!= InputE ; ++InputIt, ++OutputIt){
      auto &trackLocation = *InputIt;
      auto &slimmedTracksHandle = *OutputIt;
      if (trackLocation.isValid())
        {
          //create container for slimmed tracks
          std::unique_ptr<ConstDataVector<TrackCollection> >slimmedTracks;
          if ( m_setPersistificationHints) {
            if (m_ptCut>0) {
              slimmedTracks = std::make_unique<ConstDataVector<TrackCollection> >(SG::VIEW_ELEMENTS);
            }
          }
          else {
            slimmedTracks = std::make_unique<ConstDataVector<TrackCollection> >();
          }

          //loop through tracks, slimming them as you go.
          TrackCollection::const_iterator it    = trackLocation->begin();
          TrackCollection::const_iterator itEnd = trackLocation->end();
          for (; it!=itEnd; it++)
            { 
	      const DataVector< const Trk::TrackParameters > * params = (**it).trackParameters();
	      if ( params!=0 && params->size()>0 && params->front()->pT()>m_ptCut )
                {
		  Track* slimmed = m_slimTool->slim(**it);
                  if (m_setPersistificationHints) {
                    if (slimmed) {
                      ATH_MSG_FATAL("Track slimmer is configured to expect the slimming tool to only set persistification hints"
                                    " rather than creating slimmed tracks. But the tool created a track.");
                      delete slimmed;
                    }
                    if (slimmedTracks) {
                      slimmedTracks->push_back(*it);
                    }
                  }
                  else {
                    if (!slimmed) {
                      ATH_MSG_ERROR("Track slimmer is configured to expect the slimming tool to create slimmed tracks. "
                                    " But the tool returned a nullptr for the one track.");
                    }
                    else {
                      slimmedTracks->push_back(slimmed);
                    }
                  }
                  //m_log<<MSG::INFO<<"Original Track"<<(**it)<<endmsg;
                  //m_log<<MSG::INFO<<"Slimmed Track"<<(*slimmed)<<endmsg;
                } else {
                if (params!=0 && params->size()>0) numTracksFailingPtCut++;
              }
            }

          if (slimmedTracks) {
            // info for stats in finalise
            m_numSlimmedTracks+=slimmedTracks->size();
            m_numOriginalTracks+=trackLocation->size();
            slimmedTracksHandle.put (std::move (slimmedTracks));

            ATH_MSG_VERBOSE( "Saved "<<slimmedTracksHandle->size()<<" slimmed tracks (to "<<slimmedTracksHandle.name()
                             <<"), from input ("<<trackLocation.name()<<") of "<<trackLocation->size()<<" original tracks (which makes "
                             << m_numSlimmedTracks<<"/"<<m_numOriginalTracks<<" so far). "
                             << numTracksFailingPtCut<<" tracks failed pT cut of "<<m_ptCut );

          }
        }
    }

    return StatusCode::SUCCESS;
}

//============================================================================================
