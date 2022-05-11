/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKSEGMENTASSOCIATIONTOOL_H
#define TRACKSEGMENTASSOCIATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonTrackToSegmentAssociationTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkSegment/SegmentCollection.h"
/** @class TrackSegmentAssociationTool

    TrackSegmentAssociationTool matches muon segments to a track

*/

namespace MuonCombined {

    class TrackSegmentAssociationTool : public AthAlgTool, virtual public IMuonTrackToSegmentAssociationTool {
    public:
        TrackSegmentAssociationTool(const std::string&, const std::string&, const IInterface*);

        ~TrackSegmentAssociationTool() = default;

        StatusCode initialize() override;
        bool associatedSegments(const Trk::Track& muon, const Trk::SegmentCollection* segments,
                                std::vector<const Muon::MuonSegment*>& assocSegmentVec) const override;

    private:
        ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                              "Handle to the service providing the IMuonEDMHelperSvc interface"};
        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "MuonEDMPrinterTool", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    };
}  // namespace MuonCombined

#endif
