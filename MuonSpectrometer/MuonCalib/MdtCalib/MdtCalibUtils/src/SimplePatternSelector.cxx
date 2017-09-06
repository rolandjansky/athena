/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/SimplePatternSelector.h"

#include "MuonCalibEventBase/MuonCalibPattern.h"

#include <iostream>

namespace MuonCalib {

  SimplePatternSelector::SimplePatternSelector(double chi2_max, 
					       unsigned int mdtSeg_min,
					       unsigned int /*phiSeg_min*/)
  
    : m_chi2_max(chi2_max),m_mdtSeg_min(mdtSeg_min)
  {
    m_printLevel = 0;
    if( m_printLevel >= 1 ){
      std::cout << "SimplePatternSelector::SimplePatternSelector" << std::endl;
      std::cout << "New SimplePatternSelector : " << std::endl;
      std::cout << "       chi2 cut    " << m_chi2_max << std::endl;
      std::cout << "       muon seg cut " << m_mdtSeg_min << std::endl;
    }
  }


  bool SimplePatternSelector::select(const MuonCalibPattern& pattern) const
  {
    if( m_printLevel >= 1)
      std::cout << "SimplePatternSelector::select" << std::endl;

    // test if pattern satisfies the selectors selection criteria

    // test chi2
    if( pattern.chi2() > m_chi2_max ) return false;

    if( m_printLevel >= 2)
      std::cout << "pattern passed chi2 cut " << pattern.chi2() << std::endl;

    // test total numbers of mdt segments on pattern
    if( pattern.muonSegments() < m_mdtSeg_min ) return false;

    if( m_printLevel >= 2)
      std::cout << "pattern passed muon segments cut " 
		<< pattern.muonSegments() << std::endl;


    return true;
  }

}
