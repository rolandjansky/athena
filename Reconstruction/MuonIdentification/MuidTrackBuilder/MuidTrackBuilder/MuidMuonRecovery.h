/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuidMuonRecovery.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Muon
{
    class MuonEDMHelperTool;
    class MuonEDMPrinterTool;
    class MuonIdHelperTool;
}
namespace Trk
{
    class IExtrapolator;
    class IResidualPullCalculator;
}

namespace Rec
{
    class ICombinedMuonTrackBuilder;    
    
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
    const Trk::Track*	recoverableMatch (const Trk::Track&	indetTrack, 
					  const Trk::Track&	spectrometerTrack) const;

 private:
    // helpers, managers, tools
    ToolHandle<Trk::IExtrapolator>                      m_extrapolator; 
    ToolHandle<Muon::MuonEDMHelperTool>                 m_helper;   //<! multipurpose helper tool
    ToolHandle<Muon::MuonIdHelperTool>                  m_idHelper; //<! tool to assist with Identifiers
    ToolHandle<Muon::MuonEDMPrinterTool>                m_printer;  //<! tool to print EDM objects
    ToolHandle<Trk::IResidualPullCalculator>            m_residualCalculator;
    ToolHandle<ICombinedMuonTrackBuilder>		m_trackBuilder;

    // configurable cuts and tolerances
    double						m_minP;
    double						m_minPt;
    double                                              m_pullCut;
    
    // counters
    mutable unsigned int 				m_recoveryAttempts;
    mutable unsigned int 				m_recoveryFitFailure;
    mutable unsigned int 				m_recoverySuccess;


};

}	// end of namespace

#endif // MUIDCOMBINEDTOOLS_MUIDMUONRECOVERY_H


