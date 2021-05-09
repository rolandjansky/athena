/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDETAILEDTRACKTRUTHMAKER_H
#define MUONDETAILEDTRACKTRUTHMAKER_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKeyArray.h"
#include "TrkToolInterfaces/IDetailedTrackTruthBuilder.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

/**
 * This algorithm produces track truth data using Muon PRD truth collections.
 * Calls a DetailedTrackTruthBuilder tool that does the actual job.
 *
 * @author Andrei Gaponenko <agaponenko@lbl.gov>
 */

class MuonDetailedTrackTruthMaker : public AthReentrantAlgorithm {
public:
    MuonDetailedTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &ctx) const override;

private:
    // PRD truth maps in any order
    SG::ReadHandleKeyArray<PRD_MultiTruthCollection> m_PRD_TruthNames{this, "PRD_TruthNames", {}, "PRD truth"};

    // Track input
    SG::ReadHandleKeyArray<TrackCollection> m_trackCollectionNames{
        this, "TrackCollectionNames", {"MuonSpectrometerTracks"}, "track collections"};

    // DetailedTrackTruthCollection output
    SG::WriteHandleKeyArray<DetailedTrackTruthCollection> m_detailedTrackTruthNames{
        this, "DetailedTrackTruthNames", {""}, "output collections"};

    ToolHandle<Trk::IDetailedTrackTruthBuilder> m_truthTool{this, "TruthTool", "Trk::DetailedTrackTruthBuilder"};
};

#endif /*MUONDETAILEDTRACKTRUTHMAKER_H*/
