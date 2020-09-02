/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONERROROPTIMISATIONTOOL_H
#define MUONERROROPTIMISATIONTOOL_H

#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonRefitTool.h"
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"

namespace Muon {

  class MuonErrorOptimisationTool : virtual public IMuonErrorOptimisationTool, public AthAlgTool { 
  public:

    /** Constructor with parameters: */
    MuonErrorOptimisationTool( const std::string& ty,const std::string& na,const IInterface* pa);

    /** Destructor: */
    virtual ~MuonErrorOptimisationTool() = default; 

    virtual StatusCode  initialize() override;
    virtual StatusCode  finalize() override;

    /** optimise the error strategy used for the track */
    virtual std::unique_ptr<Trk::Track> optimiseErrors( Trk::Track* track ) const override;

  private:
    ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", "Handle to the service providing the IMuonEDMHelperSvc interface" };

    ToolHandle<MuonEDMPrinterTool> m_printer {this, "EDMPrinter", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool", "helper to nicely print out tracks"};
    ToolHandle<Trk::ITrackSummaryHelperTool> m_trackSummaryTool {this, "TrackSummaryTool", "Muon::MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool"};
    ToolHandle<IMuonRefitTool> m_refitTool {this, "RefitTool", "Muon::MuonRefitTool/MuonRefitTool"};

    Gaudi::Property<double>                      m_chi2NdofCutRefit {this, "Chi2NDofCutRefit", 5.};
    Gaudi::Property<double>                      m_lowPtThreshold   {this, "LowPtThreshold", 5000.};
    mutable std::atomic_uint                     m_nrefitAll{0};
    mutable std::atomic_uint                     m_nrefitAllLowPt{0};
    mutable std::atomic_uint                     m_nrefitOk{0};
    mutable std::atomic_uint                     m_nrefit{0};
    mutable std::atomic_uint                     m_nrefitLowPt{0};
    mutable std::atomic_uint                     m_nrefitPrecise{0};
    mutable std::atomic_uint                     m_nrefitPreciseLowPt{0};
    mutable std::atomic_uint                     m_nbetterPreciseFit{0};
    mutable std::atomic_uint                     m_nbetterFit{0};

    IMuonRefitTool::Settings m_refitSettings;
  }; 
}

#endif 
