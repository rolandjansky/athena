/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidMuonRecovery
//  AlgTool performing MS hit reallocation for a likely spectrometer-indet
//  match which has given combined fit problems.
//  Extrapolates indet track to MS.
//  Returns a combined track with full track fit.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCOMBINEDTOOLS_MUIDMUONRECOVERY_H
#define MUIDCOMBINEDTOOLS_MUIDMUONRECOVERY_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuidInterfaces/IMuidMuonRecovery.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"


namespace Rec {


class MuidMuonRecovery : public AthAlgTool, virtual public IMuidMuonRecovery {
  public:
    MuidMuonRecovery(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuidMuonRecovery() = default;

    StatusCode initialize();
    StatusCode finalize();

    /** IMuidMuonRecovery interface:
        algorithmic code for recovering muon spectrometer using the inner detector track */
    Trk::Track* recoverableMatch(const Trk::Track& indetTrack, const Trk::Track& spectrometerTrack) const;

  private:
    // helpers, managers, tools
    ToolHandle<Trk::IExtrapolator> m_extrapolator{
        this,
        "Extrapolator",
        "Trk::Extrapolator/AtlasExtrapolator",
        "Extrapolator tool",
    };

    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{
        this,
        "edmHelper",
        "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
        "Handle to the service providing the IMuonEDMHelperSvc interface",
    };  //<! multipurpose helper tool

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer{
        this,
        "Printer",
        "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
        "Tool to print EDM objects",
    };  //<! tool to print EDM objects

    ToolHandle<Trk::IResidualPullCalculator> m_residualCalculator{
        this,
        "TrackBuilder",
        "Trk::ResidualPullCalculator/ResidualPullCalculator",
        "Residual calculator tool",
    };

    ToolHandle<ICombinedMuonTrackBuilder> m_trackBuilder{
        this,
        "TrackBuilder",
        "Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder",
        "Track builder tool",
    };

    // configurable cuts and tolerances
    double m_minP;
    double m_minPt;
    double m_pullCut;

    // counters
    mutable std::atomic<unsigned int> m_recoveryAttempts;
    mutable std::atomic<unsigned int> m_recoveryFitFailure;
    mutable std::atomic<unsigned int> m_recoverySuccess;

};  // end of class MuidMuonRecovery


}  // end of namespace Rec

#endif  // MUIDCOMBINEDTOOLS_MUIDMUONRECOVERY_H
