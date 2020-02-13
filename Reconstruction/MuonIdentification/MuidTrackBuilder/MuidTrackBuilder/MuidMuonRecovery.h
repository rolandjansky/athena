/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidMuonRecovery
//  AlgTool performing MS hit reallocation for a likely spectrometer-indet 
//  match which has given combined fit problems.
//  Extrapolates indet track to MS.
//  Returns a combined track with full track fit.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCOMBINEDTOOLS_MUIDMUONRECOVERY_H
#define MUIDCOMBINEDTOOLS_MUIDMUONRECOVERY_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuidInterfaces/IMuidMuonRecovery.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>


namespace Rec
{
    
class MuidMuonRecovery: public AthAlgTool,
  virtual public IMuidMuonRecovery
{

public:
    MuidMuonRecovery	(const std::string&	type, 
			 const std::string&	name,
			 const IInterface*	parent);
    ~MuidMuonRecovery(void); // destructor
  
    StatusCode		initialize();
    StatusCode		finalize();
    
    /**IMuidMuonRecovery interface:
       algorithmic code for recovering muon spectrometer using the inner detector track */    
    Trk::Track*	recoverableMatch (const Trk::Track&	indetTrack, 
				  const Trk::Track&	spectrometerTrack) const;

 private:
    // helpers, managers, tools
    ToolHandle<Trk::IExtrapolator> m_extrapolator {this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator", "Extrapolator tool"};
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };   //<! multipurpose helper tool
    ServiceHandle<Muon::IMuonIdHelperSvc> m_muonIdHelperSvc{this, "idHelper", 
      "Muon::MuonIdHelperSvc/MuonIdHelperSvc", "Handle to the service providing the IMuonIdHelperSvc interface"}; //<! tool to assist with Identifiers
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer {this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool", "Tool to print EDM objects"};  //<! tool to print EDM objects
    ToolHandle<Trk::IResidualPullCalculator> m_residualCalculator {this, "TrackBuilder", "Trk::ResidualPullCalculator/ResidualPullCalculator", "Residual calculator tool"};
    ToolHandle<ICombinedMuonTrackBuilder> m_trackBuilder {this, "TrackBuilder", "Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder", "Track builder tool"};

    // configurable cuts and tolerances
    double						m_minP;
    double						m_minPt;
    double                                              m_pullCut;
    
    // counters
    mutable std::atomic<unsigned int> m_recoveryAttempts;
    mutable std::atomic<unsigned int> m_recoveryFitFailure;
    mutable std::atomic<unsigned int> m_recoverySuccess;


};

}	// end of namespace

#endif // MUIDCOMBINEDTOOLS_MUIDMUONRECOVERY_H


