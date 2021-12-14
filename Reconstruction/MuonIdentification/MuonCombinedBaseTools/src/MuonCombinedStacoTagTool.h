/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCOMBINEDSTACOTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCOMBINEDSTACOTAGTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackCollection.h"

namespace MuonCombined {
    class InDetCandidate;
    class MuonCandidate;
    class InDetCandidateToTagMap;

    class MuonCombinedStacoTagTool : public AthAlgTool, virtual public IMuonCombinedTagTool {
    public:
        MuonCombinedStacoTagTool(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~MuonCombinedStacoTagTool() = default;

        virtual StatusCode initialize() override;

        /**IMuonCombinedTagTool interface: build combined  muons from a muon and a vector of indet candidates */
        virtual void combine(const MuonCandidate& muonCandidate, const std::vector<const InDetCandidate*>& indetCandidates,
                             InDetCandidateToTagMap& tagMap, TrackCollection* combTracks, TrackCollection* METracks,
                             const EventContext& ctx) const override;

    private:
        std::unique_ptr<Trk::Perigee> theCombIdMu(const Trk::Perigee& indetPerigee, const Trk::Perigee& extrPerigee,
                                                        double& chi2) const;

        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
        ToolHandle<MuonCombined::IMuonTrackTagTool> m_tagTool{this, "TagTool", ""};  // tool needs to be set up explicitly
        ToolHandle<Trk::IExtrapolator> m_extrapolator{this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator"};
    };

}  // namespace MuonCombined

#endif  // MUIDCOMBINEDTOOLS_MUIDMUONRECOVERY_H
