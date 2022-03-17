/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRACKTOSEGMENTTOOL_H
#define MUON_MUONTRACKTOSEGMENTTOOL_H

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonStationIntersectCond/MuonIntersectGeoData.h"

namespace Trk {
    class Track;
    class MeasurementBase;
}  // namespace Trk

namespace Muon {
    class MuonSegment;
}

namespace Muon {

    /**
        @class MuonTrackToSegmentTool

        Tool to combine two segments. It appoximates the combined segment by a straight line.
        The tool assumes that this is a valid assumption.

        For more details look at the mainpage of this package.
    */
    class MuonTrackToSegmentTool : virtual public IMuonTrackToSegmentTool, public AthAlgTool {
    public:
        using MeasVec = std::vector<const Trk::MeasurementBase*>;       
   
        /** default AlgTool constructor */
        MuonTrackToSegmentTool(const std::string&, const std::string&, const IInterface*);

        /** destructor */
        ~MuonTrackToSegmentTool() = default;

        /** initialize method, method taken from bass-class AlgTool */
        StatusCode initialize();

        /** @brief convert track to segment */
        MuonSegment* convert(const EventContext& ctx, const Trk::Track& track) const;

    private:
        /** @brief calculate holes */
        std::vector<Identifier> calculateHoles(const EventContext& ctx, const Identifier& chid, const Trk::TrackParameters& pars,
                                               const MeasVec& measurements) const;

       
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc{
            this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
            "Handle to the service providing the IMuonEDMHelperSvc interface"};  //<! multipurpose helper tool

        PublicToolHandle<MuonEDMPrinterTool> m_printer{this, "EDMPrinter", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
                                                       "helper to nicely print out tracks"};
        ToolHandle<Trk::IPropagator> m_propagator{this, "Propagator", "Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"};
      
        SG::ReadCondHandleKey<Muon::MuonIntersectGeoData> m_chamberGeoKey{this, "ChamberGeoKey", "MuonStationIntersects", "Pointer to hole search service"};
   
    };

}  // namespace Muon

#endif
