/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECMUONCOMBINEDTIMINGTOOL_H
#define RECMUONCOMBINEDTIMINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTimingTool.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkToolInterfaces/ITrackTimingTool.h"
#include "xAODMuon/Muon.h"

namespace Rec {
class IMuonPropertiesTool;

/** @class MuonCombinedTimingTool

    Tool to coordinate time-of-flight calculation for various types
    of muons. It analyses the type of measurements contributing to
    the muon and calls the appropriate detector tools.

    @author  Wolfgang Liebig <->
*/

class MuonCombinedTimingTool : virtual public IMuonCombinedTimingTool, public AthAlgTool {
  public:
    MuonCombinedTimingTool(const std::string&, const std::string&, const IInterface*);

    /** default destructor */
    virtual ~MuonCombinedTimingTool()=default;

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();

    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const xAOD::Muon& muon) const;

  private:
    //! time calculation in subdetector responsibility: inner detector
    ToolHandle<Trk::ITrackTimingTool> m_trtTimingTool{
        this,
        "InDetTimingTool",
        "InDet::TRT_TrackTimingTool/TRT_TrackTimingTool",
    };
    //! time calculation in subdetector responsibility: calorimeter
    ToolHandle<Trk::ITrackTimingTool> m_caloTimingTool{
        this,
        "CaloTimingTool",
        "Rec::TrackInCaloTimingTool/TrackInCaloTimingTool",
    };
    //! time calculation in subdetector responsibility: muon spectrometer (T0 fit) for MDT and CSC
    ToolHandle<Trk::ITrackTimingTool> m_mdtSegmentTimingTool{
        this,
        "MDT_TimingTool",
        "Muon::SegmentTimingTool/SegmentTimingTool",
    };
    //! time calculation in subdetector responsibility: muon spectrometer (RPC timing)
    ToolHandle<Trk::ITrackTimingTool> m_rpcTimingTool{
        this,
        "RPC_TimingTool",
        "",
    };

    virtual const Trk::SegmentCollection* getSegmentContainer(const xAOD::Muon& muon) const;
};
}  // namespace Rec

#endif
