/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONAMBITRACKSELECTIONTOOL_H
#define MUON_MUONAMBITRACKSELECTIONTOOL_H

#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"

namespace Trk {
class Track;
}


namespace Muon {

/** @class MuonAmbiTrackSelectionTool
    This tool cross checks the hits on a track with the hits already stored. Shared hits
    are allowed to some extent und under certain conditions
    (e.g. the track score is high enough), additional shared hits are removed if necessary.
    This tool does not delete any tracks. If the client is going to discard tracks entered into
    this tool, the tracks have to be deleted by this client.

    @author  Niels van Eldik <niels.van.eldik@cern.ch>
*/

class MuonAmbiTrackSelectionTool : virtual public Trk::IAmbiTrackSelectionTool, public AthAlgTool {
  public:
    MuonAmbiTrackSelectionTool(const std::string &, const std::string &, const IInterface *);

    /** default destructor */
    virtual ~MuonAmbiTrackSelectionTool(){};

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;

    virtual std::tuple<Trk::Track *, bool> getCleanedOutTrack(const Trk::Track *track, const Trk::TrackScore score,
                                                              Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                                                              Trk::PRDtoTrackMap &prd_to_track_map) const override;

  private:
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer{
        this,
        "Printer",
        "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
    };
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };

    /** maximum hit overlap fraction between two track, if higher track will be rejected*/
    double m_maxOverlapFraction;

    /** flag to keep partial overlaps */
    bool m_keepPartial;

    /** flag to keep overlaps which share more than one presicion layer */
    bool m_keepMoreThanOne;
};
}  // namespace Muon

#endif
