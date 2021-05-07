/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "MuonTruthAlgs/MuonDetailedTrackTruthMaker.h"

#include <iterator>

#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

//================================================================
MuonDetailedTrackTruthMaker::MuonDetailedTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name, pSvcLocator), m_truthTool("Trk::DetailedTrackTruthBuilder") {
    declareProperty("TruthTool", m_truthTool);

    // Inputs
    declareProperty("TrackCollectionNames",
                    m_trackCollectionNames = {"MuonSpectrometerTracks", "MooreTracks", "ConvertedMBoyMuonSpectroOnlyTracks",
                                              "ConvertedMBoyTracks", "MuidExtrapolatedTracks", "ExtrapolatedMuonSpectrometerTracks",
                                              "Combined_Tracks", "CombinedFitMuonTracks", "ConvertedMuIdCBTracks",
                                              "ConvertedMuIdExtrTracks", "ConvertedStacoTracks", "MuGirlRefittedTracks"});
    declareProperty("PRD_TruthNames", m_PRD_TruthNames);
}

// Initialize method
// -----------------------------------------------------------------------------------------------------
StatusCode MuonDetailedTrackTruthMaker::initialize() {
    ATH_MSG_DEBUG("MuonDetailedTrackTruthMaker::initialize()");

    if (m_PRD_TruthNames.empty()) {
        ATH_MSG_FATAL("No PRD names have been parsed");
        return StatusCode::FAILURE;
    }

    //----------------
    ATH_CHECK(m_truthTool.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_truthTool);

    m_detailedTrackTruthNames.reserve(m_trackCollectionNames.size());
    for (const std::string &it : m_trackCollectionNames) m_detailedTrackTruthNames.push_back(it + "Truth");

    msg(MSG::INFO) << " processing: ";
    std::vector<std::string>::const_iterator ikeyt = m_detailedTrackTruthNames.begin();
    for (std::vector<std::string>::const_iterator ikey = m_trackCollectionNames.begin(); ikey != m_trackCollectionNames.end();
         ikey++, ++ikeyt) {
        msg(MSG::INFO) << "  " << *ikey << "  " << *ikeyt;
    }
    msg(MSG::INFO) << endmsg;

    //----------------
    return StatusCode::SUCCESS;
}
// -----------------------------------------------------------------------------------------------------
StatusCode MuonDetailedTrackTruthMaker::execute() {
    ATH_MSG_DEBUG("MuonDetailedTrackTruthMaker::execute()");

    StatusCode sc;

    //----------------------------------------------------------------
    // Retrieve prep raw data truth
    std::vector<const PRD_MultiTruthCollection *> prdCollectionVector;
    for (std::vector<std::string>::const_iterator ikey = m_PRD_TruthNames.begin(); ikey != m_PRD_TruthNames.end(); ikey++) {
        prdCollectionVector.push_back(0);
        sc = evtStore()->retrieve(*prdCollectionVector.rbegin(), *ikey);
        if (!sc.isSuccess()) {
            ATH_MSG_WARNING("PRD_MultiTruthCollection " << *ikey << " NOT found");
        } else {
            ATH_MSG_DEBUG("Got PRD_MultiTruthCollection " << *ikey);
        }
    }

    //----------------------------------------------------------------
    // Retrieve track collections
    unsigned int i = 0;
    std::vector<std::string>::const_iterator it = m_trackCollectionNames.begin(), itEnd = m_trackCollectionNames.end();
    for (; it != itEnd; ++it, ++i) {
        const TrackCollection *tracks = 0;
        if (evtStore()->contains<TrackCollection>(*it) && evtStore()->retrieve(tracks, *it).isSuccess()) {
            ATH_MSG_DEBUG("Got TrackCollection " << *it);
        } else {
            ATH_MSG_VERBOSE("TrackCollection " << *it << " NOT found");
            continue;
        }

        //----------------------------------------------------------------
        // Produce and store the output.

        DetailedTrackTruthCollection *dttc = new DetailedTrackTruthCollection(tracks);
        m_truthTool->buildDetailedTrackTruth(dttc, *tracks, prdCollectionVector);

        sc = evtStore()->record(dttc, m_detailedTrackTruthNames[i], false);
        if (sc.isFailure()) {
            ATH_MSG_WARNING("DetailedTrackTruthCollection '" << m_detailedTrackTruthNames[i] << "' could not be registered in StoreGate !");
            return StatusCode::SUCCESS;
        } else {
            ATH_MSG_DEBUG("DetailedTrackTruthCollection '" << m_detailedTrackTruthNames[i]
                                                           << "' is registered in StoreGate, size=" << dttc->size());
        }
    }
    return StatusCode::SUCCESS;
}

//================================================================
// EOF
