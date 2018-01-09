/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthTrackCreation.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TruthTrackCreation.h"
// Trk includes
#include "TrkTruthTrackInterfaces/IPRD_TruthTrajectoryBuilder.h"
#include "TrkTruthTrackInterfaces/IPRD_TruthTrajectorySelector.h"
#include "TrkTruthTrackInterfaces/ITruthTrackBuilder.h"
#include "TrkTruthTrackInterfaces/PRD_TruthTrajectory.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
// HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/SimpleVector.h"

//================ Constructor =================================================

Trk::TruthTrackCreation::TruthTrackCreation(const std::string& name, ISvcLocator* pSvcLocator)
    :
    AthAlgorithm(name,pSvcLocator),
    m_prdTruthTrajectoryBuilder("Trk::PRD_TruthTrajectoryBuilder/InDetPRD_TruthTrajectoryBuilder"),
    m_truthTrackBuilder("Trk::TruthTrackBuilder/InDetTruthTrackBuilder"),
    m_assoTool(""),
    m_trackSummaryTool("")
{
    // Trk Truth Tools
  declareProperty("PRD_TruthTrajectoryBuilder",   m_prdTruthTrajectoryBuilder, "Truth Trajectory Builder Tool");
  declareProperty("TruthTrackBuilder",            m_truthTrackBuilder, "Truth Track Builder Tool");
  declareProperty("PRD_TruthTrajectorySelectors", m_prdTruthTrajectorySelectors, "PRD truth trajectory selectors");
    // Trk Tools
  declareProperty("TrackSelectors",               m_trackSelectors, "Track selectors for a posteriori track selection");
  declareProperty("AssoTool",                     m_assoTool, "Association tool for PRDs");
  declareProperty("TrackSummaryTool",             m_trackSummaryTool, "Summary tool for completing the track with its summary info");
}

//================ Destructor =================================================

Trk::TruthTrackCreation::~TruthTrackCreation()
    {}


//================ Initialisation =================================================

StatusCode Trk::TruthTrackCreation::initialize()
{
    // trajectory creator
    if (m_prdTruthTrajectoryBuilder.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_prdTruthTrajectoryBuilder << ". Aborting ...");
        return StatusCode::FAILURE;
    }
    // truth track builder
    if (m_truthTrackBuilder.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_truthTrackBuilder << ". Aborting ...");
        return StatusCode::FAILURE;
    }
    // PRD truth trajectory selectors
    if (m_prdTruthTrajectorySelectors.size() && m_prdTruthTrajectorySelectors.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_prdTruthTrajectorySelectors << ". Aborting ...");
        return StatusCode::FAILURE;        
    }
    // association tool if configured
    if (!m_assoTool.empty() && m_assoTool.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_assoTool << ". Aborting ...");
        return StatusCode::FAILURE;
    }
    // track summary tool if configured
    if (!m_trackSummaryTool.empty() && m_trackSummaryTool.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_trackSummaryTool << ". Aborting ...");
        return StatusCode::FAILURE;
    }
    // track selector if configured
    if (!m_trackSelectors.empty() && m_trackSelectors.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_trackSelectors << ". Aborting ...");
        return StatusCode::FAILURE;
    }

    ATH_CHECK( m_outputTrackCollectionName.initialize() );
    ATH_CHECK( m_skippedTrackCollectionName.initialize() );
 
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::TruthTrackCreation::finalize()
{
  // Code entered here will be executed once at the end of the program run.
    return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::TruthTrackCreation::execute()
{
    if (!m_assoTool.empty()) m_assoTool->reset();
    // create the track collection
    std::unique_ptr<TrackCollection> outputTrackCollection = std::make_unique<TrackCollection>();
    std::unique_ptr<TrackCollection> skippedTrackCollection = std::make_unique<TrackCollection>();
    SG::WriteHandle<TrackCollection> outputTrackCollectionHandle(m_outputTrackCollectionName);
    SG::WriteHandle<TrackCollection> skippedTrackCollectionHandle(m_skippedTrackCollectionName);
 

    // set up the PRD trajectory builder
    if ( m_prdTruthTrajectoryBuilder->refreshEvent().isFailure() ){
        ATH_MSG_INFO("Could not refresh the PRD truth trajectory builder. No truth track creation.");
        return StatusCode::SUCCESS;
    }

    // ----------------------------------- main loop ------------------------------------------------------------------
    // get the PRD truth trajectories
    const std::map< const HepMC::GenParticle*, PRD_TruthTrajectory >& truthTraj =
        m_prdTruthTrajectoryBuilder->truthTrajectories();
    // some screen output
    ATH_MSG_VERBOSE("PRD_TruthTrajectoryBuilder delivered " << truthTraj.size() << " PRD truth trajectories, starting track creation.");
    // loop over truth trajectories and create track
    std::map< const HepMC::GenParticle*, PRD_TruthTrajectory >::const_iterator ttIter  = truthTraj.begin();
    std::map< const HepMC::GenParticle*, PRD_TruthTrajectory >::const_iterator ttIterE = truthTraj.end();
    for ( ; ttIter != ttIterE; ++ttIter){
        // run through the selector chain
        if (m_prdTruthTrajectorySelectors.size()){
            ToolHandleArray<Trk::IPRD_TruthTrajectorySelector>::const_iterator prdTTSelIter  = m_prdTruthTrajectorySelectors.begin();
            ToolHandleArray<Trk::IPRD_TruthTrajectorySelector>::const_iterator prdTTSelIterE = m_prdTruthTrajectorySelectors.end();
            bool passed = true;
            for ( ; prdTTSelIter != prdTTSelIterE && passed; ++prdTTSelIter ){
                if (!(*prdTTSelIter)->pass((*ttIter).second)){
                    ATH_MSG_VERBOSE("PRD truth trajectory did not pass the selection cuts. Skipping ... ");
                    passed = false;
                }
            }
            // the consequence of the selection loop
            if (!passed) continue;
        }
        // create the truth track
        Trk::Track* truthTrack = m_truthTrackBuilder->createTrack(ttIter->second);
        if (!truthTrack){
            ATH_MSG_VERBOSE("Track creation for PRD truth trajectory with size " << (*ttIter).second.prds.size() << " failed. Skipping ...");
            continue;
        } 
        ATH_MSG_VERBOSE("Track creation for PRD truth trajectory with size " << (*ttIter).second.prds.size() << " successful.");
        // If configured : update the track summary
        if (!m_trackSummaryTool.empty()){
            ATH_MSG_VERBOSE("Updating the TrackSummary.");
            m_trackSummaryTool->updateTrack(*truthTrack);
        }
        // If configured : check with the TrackSelectors
        bool passed = !(m_trackSelectors.size());
        if ( m_trackSelectors.size() ) {
            ToolHandleArray<Trk::ITrackSelectorTool>::iterator tsIter  = m_trackSelectors.begin();
            ToolHandleArray<Trk::ITrackSelectorTool>::iterator tsIterE = m_trackSelectors.end();
            for ( ; ( tsIter != tsIterE && !passed ); ++tsIter){
                passed = (*tsIter)->decision(*truthTrack);
            }
        } 
        // now check the result
        if (passed){
            // everything good
            ATH_MSG_VERBOSE("Track created and pushed into the output track collection.");
            if (!m_assoTool.empty() && m_assoTool->addPRDs(*truthTrack).isFailure()) ATH_MSG_WARNING("Failed to add PRDs to map");
            outputTrackCollection->push_back(truthTrack);       
        } else {
            ATH_MSG_VERBOSE("Track did not pass the track selection. Putting it into skipped track collection.");
            skippedTrackCollection->push_back(truthTrack);
        }
    }

    
    // If configured : update the track summary
    if (!m_trackSummaryTool.empty()) {
        TrackCollection::iterator rf,rfe=outputTrackCollection->end();
        for (rf=outputTrackCollection->begin();rf!=rfe; ++rf) {
            ATH_MSG_VERBOSE("Updating the TrackSummary with shared hits.");
            m_trackSummaryTool->updateTrack(**rf);
        }
    }

    // ----------------------------------- record & cleanup ---------------------------------------------------------------    

    ATH_CHECK(outputTrackCollectionHandle.record(std::move(outputTrackCollection)));

    ATH_MSG_VERBOSE("Truth TrackCollection with name " << m_outputTrackCollectionName << " and size " <<  outputTrackCollection->size() << " recorded.");

    ATH_CHECK(skippedTrackCollectionHandle.record(std::move(skippedTrackCollection)));
 
    ATH_MSG_VERBOSE("Truth TrackCollection with name " << m_skippedTrackCollectionName << " and size " <<  skippedTrackCollection->size() << " recorded.");

    // job done
    return StatusCode::SUCCESS;
}

//============================================================================================
