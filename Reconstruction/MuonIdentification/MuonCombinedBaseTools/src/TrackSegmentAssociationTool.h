/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKSEGMENTASSOCIATIONTOOL_H
#define TRACKSEGMENTASSOCIATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonCombinedToolInterfaces/IMuonTrackToSegmentAssociationTool.h"
/** @class TrackSegmentAssociationTool

    TrackSegmentAssociationTool matches muon segments to a track

*/


namespace MuonCombined {

    class TrackSegmentAssociationTool : public AthAlgTool, virtual public IMuonTrackToSegmentAssociationTool {
    public:
        TrackSegmentAssociationTool(const std::string&, const std::string&, const IInterface*);

        ~TrackSegmentAssociationTool() = default;

        StatusCode initialize() override;

        /** Returns a list of segments that match with the input Muon. */
         bool associatedSegments(const Trk::Track& track, const xAOD::MuonSegmentContainer* segments,
                                std::vector<ElementLink<xAOD::MuonSegmentContainer> >& assocSegmentVec) const override;

        
        bool associatedSegments(const xAOD::Muon& muon, const xAOD::MuonSegmentContainer* segments,
                                std::vector<ElementLink<xAOD::MuonSegmentContainer> >& assocSegmentVec) const override;

      

    private:
        ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                              "Handle to the service providing the IMuonEDMHelperSvc interface"};
        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "MuonEDMPrinterTool", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
    };
}  // namespace Muon

#endif
