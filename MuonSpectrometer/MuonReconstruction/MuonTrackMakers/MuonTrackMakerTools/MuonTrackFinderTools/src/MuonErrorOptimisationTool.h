/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONERROROPTIMISATIONTOOL_H
#define MUONERROROPTIMISATIONTOOL_H


// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecToolInterfaces/IMuonRefitTool.h"
#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"

namespace Muon {
  class MuonEDMPrinterTool;
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
}

namespace Trk {
  class Track;
  class ITrackSummaryHelperTool;
}

namespace Muon {

  class MuonErrorOptimisationTool : virtual public IMuonErrorOptimisationTool, public AthAlgTool { 
  public:

    /** Constructor with parameters: */
    MuonErrorOptimisationTool( const std::string& ty,const std::string& na,const IInterface* pa);

    /** Destructor: */
    virtual ~MuonErrorOptimisationTool(); 

    virtual StatusCode  initialize();
    virtual StatusCode  finalize();

    /** optimise the error strategy used for the track */
    Trk::Track* optimiseErrors( const Trk::Track& track ) const;

  protected:


    ToolHandle<MuonEDMPrinterTool>  m_printer; //<! helper to nicely print out tracks
    ToolHandle<MuonEDMHelperTool>   m_helper; //<! muon EDM helper
    ToolHandle<MuonIdHelperTool>    m_idHelper; //<! muon id helper
    ToolHandle<Trk::ITrackSummaryHelperTool>    m_trackSummaryTool; //<! muon id helper
    ToolHandle<IMuonRefitTool>      m_refitTool;

    double                                       m_chi2NdofCutRefit;
    double                                       m_lowPtThreshold;
    mutable unsigned int                         m_nrefitAll;
    mutable unsigned int                         m_nrefitAllLowPt;
    mutable unsigned int                         m_nrefitOk;
    mutable unsigned int                         m_nrefit;
    mutable unsigned int                         m_nrefitLowPt;
    mutable unsigned int                         m_nrefitPrecise;
    mutable unsigned int                         m_nrefitPreciseLowPt;
    mutable unsigned int                         m_nbetterPreciseFit;
    mutable unsigned int                         m_nbetterFit;

    mutable IMuonRefitTool::Settings m_refitSettings;
  }; 
}

#endif 
