/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibTools/SimpleMdtSegmentSelectorTool.h"

#include "MdtCalibUtils/SimpleMdtSegmentSelector.h"

namespace MuonCalib {

  SimpleMdtSegmentSelectorTool::SimpleMdtSegmentSelectorTool(const std::string& t, 
							     const std::string& n, 
							     const IInterface* p) :
    AthAlgTool(t,n,p)
  {
    declareInterface< IMdtSegmentSelectorTool >(this) ;

    // chi2 and #hits cuts
    m_minHits      = 6;
    m_chi2         = 10.;

    declareProperty("MinHitsOntrack", m_minHits);
    declareProperty("MaxChi2"   , m_chi2);
  }


  StatusCode SimpleMdtSegmentSelectorTool::initialize()
  {
    // Create instance of type SimpleMdtSegmentSelector
    m_implementation = new SimpleMdtSegmentSelector(m_chi2,m_minHits);
  
    return StatusCode::SUCCESS;
  }

  StatusCode SimpleMdtSegmentSelectorTool::finalize()
  {
    // delete instance
    delete m_implementation;
  
    return StatusCode::SUCCESS;
  }
 
}
