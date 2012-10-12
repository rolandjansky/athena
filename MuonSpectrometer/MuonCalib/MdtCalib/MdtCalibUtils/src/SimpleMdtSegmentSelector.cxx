/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/SimpleMdtSegmentSelector.h"

#include "MuonCalibEventBase/MuonCalibSegment.h"

#include <iostream>

namespace MuonCalib {

  SimpleMdtSegmentSelector::SimpleMdtSegmentSelector(double chi2_max, 
						     unsigned int hits_min)
    : m_chi2_max(chi2_max),m_hits_min(hits_min)
  {
    m_printLevel = 0;
  
    std::cout << "SimpleMdtSegmentSelector::SimpleMdtSegmentSelector" << std::endl;
    std::cout << "New SimpleMdtSegmentSelector : " << std::endl;
    std::cout << "       chi2 cut    " << m_chi2_max << std::endl;
    std::cout << "       hits cut    " << m_hits_min << std::endl;
  }


  bool SimpleMdtSegmentSelector::select(const MuonCalibSegment& seg) const
  {
    if( m_printLevel >= 1){
      std::cout << "SimpleMdtSegmentSelector::select" << std::endl;
      std::cout << "Segment: chi2 " <<  seg.chi2() << " mdtHits " << seg.mdtHitsOnTrack() 
		<< " tan " << seg.direction().y()/seg.direction().z() << std::endl;
    }

    // test if segments satisfies the selectors selection criteria

    // test chi2
    if( seg.chi2() > m_chi2_max ) return false;

    if( m_printLevel >= 2)
      std::cout << "segment passed chi2 cut " << seg.chi2() << std::endl;
 
    // test total numbers of hits on segment
    if( seg.mdtHitsOnTrack() < m_hits_min ) return false;

    if( m_printLevel >= 2)
      std::cout << "segment passed mdtHits cut " 
		<< seg.mdtHitsOnTrack() << std::endl;

    double tanphi = seg.direction().y()/seg.direction().z();
    //if( std::abs(tanphi) > 1. ) return false;
  
    if( m_printLevel >= 2)
      std::cout << "segment passed angular cut " 
		<< tanphi << std::endl;

    return true;
  }

}
