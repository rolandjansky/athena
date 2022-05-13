/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONSEGMENETMERGERALG_H
#define MUONCOMBINEDALGS_MUONSEGMENETMERGERALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedEvent/MuonTagToSegMap.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"

/**
 * @brief The MuonSegContainerMergerAlg merges all MuonSegments assocaited with the combined muon tags into a single Trk::Segment container.
 *        The container is further converted into a xAOD::MuonSegmentContainer downstream and finally dumped to disk. In order to avoid an
 * unnecssary coping of all muon tags in memory, the MuonTagToSegMap is saved into the storegate which allows for a mapping of the transient
 * segments associated with the segment container to the persistent ones in the MuonCreatorAlg downstream
 */

class MuonSegContainerMergerAlg : public AthReentrantAlgorithm {
public:
    MuonSegContainerMergerAlg(const std::string& name, ISvcLocator* pSvcLocator);

    ~MuonSegContainerMergerAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

private:
    /// Name of all MuonCandidate tags (Main / LRT / EMEO)
    SG::ReadHandleKeyArray<MuonCandidateCollection> m_muonCandidateKeys{this, "MuonCandidateMaps", {"MuonCandidates"}, "Muon candidates"};
    /// List of all InDetCandidate tags MuidCo, MuGirl from  all muon reconstruction streams
    SG::ReadHandleKeyArray<MuonCombined::InDetCandidateToTagMap> m_tagMaps{
        this, "TagMaps", {"muidcoTagMap", "stacoTagMap", "muGirlTagMap", "caloTagMap", "segmentTagMap"}, "ID candidate to tag maps"};

    /// Optionally segments from containers that are not associated with any segment candidate are dumped into
    /// a seperate container
    SG::ReadHandleKeyArray<Trk::SegmentCollection> m_inputSegContainerName{
        this, "InputSegmentContainers", {"TrackMuonSegments"}, "Container names of muon segments not used in muon reconstruction"};

    /// Name of the bulk segment output container
    SG::WriteHandleKey<Trk::SegmentCollection> m_segTrkContainerName{this, "TrackSegmentContainerName", "TrkMuonSegments",
                                                                     "Track segments"};
    SG::WriteHandleKey<MuonCombined::MuonTagToSegMap> m_assocMapKey{this, "AssocMapKey", "MuonTagSegMap"};

    /// Name of the unassociated segment container name
    SG::WriteHandleKey<Trk::SegmentCollection> m_unassocTrkContainerName{this, "UnassociatedSegmentKey", "UnAssocMuonTrkSegments"};

    ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiguityProcessor{this, "AmbiguityProcessor", ""};

    Gaudi::Property<bool> m_saveUnassocSegs{this, "saveUnusedSegments", true,
                                            "Dumps segments that were not fed into muon reconstruction into a second container"};
    Gaudi::Property<bool> m_solveAmbi{this, "solveAmbiguities", true, "Solves the ambiguities between unassociated segments"};
};

#endif
