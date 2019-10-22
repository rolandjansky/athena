/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONERROROPTIMISATIONTOOL_H
#define MUONERROROPTIMISATIONTOOL_H


// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonRefitTool.h"
#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IMuonRefitTool.h"
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"

#include "CxxUtils/checker_macros.h"
ATLAS_CHECK_FILE_THREAD_SAFETY;

namespace Muon {

  class MuonErrorOptimisationTool : virtual public IMuonErrorOptimisationTool, public AthAlgTool { 
  public:

    /** Constructor with parameters: */
    MuonErrorOptimisationTool( const std::string& ty,const std::string& na,const IInterface* pa);

    /** Destructor: */
    virtual ~MuonErrorOptimisationTool() = default; 

    virtual StatusCode  initialize();
    virtual StatusCode  finalize();

    /** optimise the error strategy used for the track */
    Trk::Track* optimiseErrors( const Trk::Track& track ) const;

  protected:


    ToolHandle<MuonEDMPrinterTool>  m_printer; //<! helper to nicely print out tracks
    ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" }; //<! muon EDM helper
    ToolHandle<MuonIdHelperTool>    m_idHelper; //<! muon id helper
    ToolHandle<Trk::ITrackSummaryHelperTool>    m_trackSummaryTool; //<! muon id helper
    ToolHandle<IMuonRefitTool>      m_refitTool;

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
