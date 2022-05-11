/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONINDETSYSTEMEXTENSIONALG_H
#define MUONCOMBINEDALGS_MUONINDETSYSTEMEXTENSIONALG_H

#include <map>
#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedEvent/CombinedFitTag.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/HoughDataPerSec.h"
#include "MuonRecToolInterfaces/IMuonSystemExtensionTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonStationIndex/MuonStationIndex.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

/// The MuonInDetToMuonSystemExtensionAlg processes further the InDetCandidates created by the MuonCombinedInDetCandidateAlg.
/// The idea is that the latter is only selecting the Id tracks based on the outcome of the Id selection tool and
/// whether a CaloExtension could be successfully attached. This information is full sufficient to run
/// the MuonCombinedAlg and the MuonCalo algorithms, but not the SegmentTagging or muGirl. InDetCandidates, not
/// combined to a full muon track by MuidCo (STACO generates to much noise) are copied and then equipped with the
/// muonSystem extension.
class MuonInDetToMuonSystemExtensionAlg : public AthReentrantAlgorithm {
public:
    MuonInDetToMuonSystemExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~MuonInDetToMuonSystemExtensionAlg() = default;

    StatusCode initialize() override;
    StatusCode execute(const EventContext& ctx) const override;

private:
    SG::ReadHandleKey<InDetCandidateCollection> m_inputCandidate{this, "InputInDetCandidates", "InDetCandidates"};

    /// Collection of InDet candidates written for MuGirl and MuonSegmentTag
    SG::WriteHandleKey<InDetCandidateCollection> m_bulkInDetCandKey{this, "WriteInDetCandidates", "InDetCandidatesSystemExtened"};
    /// Collection of InDet candidates written for MuGirlStau
    SG::WriteHandleKey<InDetCandidateCollection> m_stauInDetCandKey{this, "WriteStauCandidates", ""};

    /// Use the hough data to find sectors in the speectrometer traversed by a muon.
    SG::ReadHandleKey<Muon::HoughDataPerSectorVec> m_houghDataPerSectorVecKey{this, "LayerHoughKey", "HoughDataPerSectorVec",
                                                                              "HoughDataPerSectorVec key"};
    /// Let's exploit the segments to get rid of as much tracks as possible
    SG::ReadHandleKey<Trk::SegmentCollection> m_segmentKey{this, "MuonSegmentLocation", "TrackMuonSegments",
                                                           "name of muon segment container"};

    PublicToolHandle<Muon::IMuonSegmentSelectionTool> m_segmentSelector{this, "MuonSegmentSelectionTool",
                                                                        "Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool"};

    /// The system extension tool extrapolates the candidate through the spectrometer
    ToolHandle<Muon::IMuonSystemExtensionTool> m_muonSystemExtensionTool{this, "MuonSystemExtensionTool", "", "Muon system extension tool"};
    /// That is the collection of MuidCo candidates
    SG::ReadHandleKey<MuonCombined::InDetCandidateToTagMap> m_combTagMap{this, "CombinedTagMap", "muidcoTagMap", "Combined muon tag map"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                          "Handle to the service providing the IMuonEDMHelperSvc interface"};
    /// Helper struct to cache the MuidCo track and it's associated segments while keeping
    /// the association InDetCandidate <-- > muidCo Tag
    struct MuidCoCache {
        const MuonCombined::InDetCandidate* id_trk{nullptr};
        const MuonCombined::CombinedFitTag* cmb_trk{nullptr};
        MuidCoCache(const MuonCombined::InDetCandidate* _id, const MuonCombined::CombinedFitTag* _cb) : id_trk{_id}, cmb_trk{_cb} {}
    };
    using MuidCoVector = std::vector<MuidCoCache>;

    struct InDetCandidateCache {
        /// Collection of the InDet candidates to consider
        const InDetCandidateCollection* input_candidates{nullptr};

        /// Inner detector tracks to exclude
        std::set<const MuonCombined::InDetCandidate*> excluded_trks;
        /// Combined fit tag map
        MuidCoVector tag_map;
        /// List of segments that were successfully combined to a muon
        std::set<const Muon::MuonSegment*> combined_segs{};

        /// Output container for the StoreGate
        std::unique_ptr<InDetCandidateCollection> outputContainer = std::make_unique<InDetCandidateCollection>(SG::OWN_ELEMENTS);

        /// Hit sector map
        std::map<Muon::MuonStationIndex::DetectorRegionIndex, std::set<int>> hit_sectors;

        std::map<int, std::vector<double>> eta_seeds{};

        /// Map of hits that were successfully combined to a muon segment
        /// Key is the hash made out of region & layer index
        std::map<unsigned int, std::set<Identifier>> consumed_hits{};

        /// Muon segments that could be potentially used for MuGirl / MuTagIMO
        std::vector<const Muon::MuonSegment*> candidate_segments{};
    };

    /// Restrict the extrapolation of ID tracks to sectors in the MS where at least a hit is recorded
    Gaudi::Property<bool> m_restrictExtension{this, "UseOnlyHitSectors", false};
    /// The hough maxima always contain at least 2 hits. If a muon track passes through a chamber one expects more than that.
    /// Apply a minimum cuts on the hits in the hough maximum to reject background --> needs to be further tuned
    Gaudi::Property<int> m_houghMin{this, "minHoughHits", 4, "Minimum number of hits required for a hough maximum to be accepted"};

    Gaudi::Property<int> m_segmentQuality{this, "segQuality", 1, "Quality cut on the segment."};

    /// Minimum pt threshold of the IdCandidate to be extrapolated through the spectrometer
    Gaudi::Property<float> m_extThreshold{this, "ExtensionPtThreshold", 2500};
    /// Shall SAF tracks  be equiped with a muon system extension used by MuGirl later
    Gaudi::Property<bool> m_extendSAF{this, "ExtendSAF", false};
    /// Shall ordinary ID tracks  be equiped with a muon system extension used by MuGirl later
    Gaudi::Property<bool> m_extendBulk{this, "ExtendBulk", true};

    const Muon::MuonSectorMapping m_sector_mapping{};

    /// Select the MuidCo candidates and put the associated id tracks on a black list
    StatusCode selectCandidates(const EventContext& ctx, InDetCandidateCache& cache) const;
    /// Find the sectors in the MS with muon signals
    StatusCode findHitSectors(const EventContext& ctx, InDetCandidateCache& output_cache) const;
    /// Select good segments
    StatusCode findSegments(const EventContext& ctx, InDetCandidateCache& output_cache) const;

    /// Searches for segments within theta < 0.2 && sharing the same sector
    bool hasMatchingSegment(const MuonCombined::InDetCandidate& id_cand, const InDetCandidateCache& cache) const;
    /// Searches for hough seeds within theta < 0.2 && sharing the same sector
    bool hasMatchingSeed(const MuonCombined::InDetCandidate& id_cand, const InDetCandidateCache& cache) const;

    /// Create the InDetCandidaes with system extensions
    StatusCode create(const EventContext& ctx, InDetCandidateCache& cache) const;

    StatusCode createStaus(const EventContext& ctx, const InDetCandidateCollection* ext_candidates, const MuidCoVector& tag_map) const;
    /// Mask the hits of the corresponding muon segment as used.
    void maskHits(const Muon::MuonSegment* muon_seg, InDetCandidateCache& output_cache) const;
};

#endif
