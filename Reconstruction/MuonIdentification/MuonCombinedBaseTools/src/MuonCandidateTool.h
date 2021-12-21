/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCANDIDATETOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCANDIDATETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuonCombinedToolInterfaces/IMuonCandidateTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "MuonCombinedToolInterfaces/IMuonTrackToSegmentAssociationTool.h"


namespace MuonCombined {

    class MuonCandidateTool : public AthAlgTool, virtual public IMuonCandidateTool {
    public:
        MuonCandidateTool(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~MuonCandidateTool() = default;

        virtual StatusCode initialize() override;

        /**IMuonCandidateTool interface: build a MuonCandidateCollection from a TrackCollection of spectrometer tracks */
        virtual void create(const xAOD::TrackParticleContainer& tracks, MuonCandidateCollection& outputCollection,
                            TrackCollection& outputTracks, const EventContext& ctx) const override;

    private:
        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
        ToolHandle<Rec::ICombinedMuonTrackBuilder> m_trackBuilder{this, "TrackBuilder", "", ""};
        ToolHandle<Muon::IMuonTrackExtrapolationTool> m_trackExtrapolationTool{this, "TrackExtrapolationTool",
                                                                               "ExtrapolateMuonToIPTool/ExtrapolateMuonToIPTool"};
        ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiguityProcessor{this, "AmbiguityProcessor",
                                                                           "Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"};
        ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool{this, "TrackSummaryTool", "MuonTrackSummaryTool"};

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};

        /// Retrieve the segment container to perform the segment association offline
        SG::ReadHandleKey<xAOD::MuonSegmentContainer> m_segmentKey{this, "SegmentContainer", "" };
        PublicToolHandle<MuonCombined::IMuonTrackToSegmentAssociationTool> m_trackSegmentAssociationTool{
            this, "TrackSegmentAssociationTool", "MuonCombined::TrackSegmentAssociationTool/TrackSegmentAssociationTool"};


        Gaudi::Property<unsigned int> m_extrapolationStrategy{this, "ExtrapolationStrategy", 0};

        Gaudi::Property<bool> m_comissioning{this, "Comissioning", false,
                                             "Flag deciding whether the candidate belongs to the comissioning."};
    };

}  // namespace MuonCombined

#endif
