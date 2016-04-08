/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibTools/SimplePatternSelectorTool.h"

#include "MdtCalibUtils/SimplePatternSelector.h"

namespace MuonCalib {

  SimplePatternSelectorTool::SimplePatternSelectorTool(const std::string& t, 
						       const std::string& n, 
						       const IInterface* p) :
    AthAlgTool(t,n,p)
  {
    declareInterface< IPatternSelectorTool >(this) ;

  }


  StatusCode SimplePatternSelectorTool::initialize()
  {
    // set maximum chi2 of segment
    double chi2_max = 1000.;
  
    // set minimum number of mdt segments
    unsigned int mdt_min = 1;

    // set minimum number of phi segments
    unsigned int phi_min = 1;

    // create an instance of SimplePatternSelector
    m_implementation = new SimplePatternSelector(chi2_max,mdt_min,phi_min);
  
    return StatusCode::SUCCESS;
  }

  StatusCode SimplePatternSelectorTool::finalize()
  {
    // delete pointer 
    delete m_implementation;

    return StatusCode::SUCCESS;
  }
 
}
