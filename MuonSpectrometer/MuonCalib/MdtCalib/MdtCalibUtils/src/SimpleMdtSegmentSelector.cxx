/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/SimpleMdtSegmentSelector.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>

namespace MuonCalib {

  SimpleMdtSegmentSelector::SimpleMdtSegmentSelector(double chi2_max, 
						     unsigned int hits_min)
    : m_chi2_max(chi2_max),m_hits_min(hits_min)
  {
    m_printLevel = 0;

    MsgStream log(Athena::getMessageSvc(),"SimpleMdtSegmentSelector");
    log<<MSG::INFO<<"SimpleMdtSegmentSelector::SimpleMdtSegmentSelector" << endmsg;
    log<<MSG::INFO<<"New SimpleMdtSegmentSelector : " << endmsg;
    log<<MSG::INFO<<"       chi2 cut    " << m_chi2_max << endmsg;
    log<<MSG::INFO<<"       hits cut    " << m_hits_min << endmsg;
  }


  bool SimpleMdtSegmentSelector::select(const MuonCalibSegment& seg) const {
    MsgStream log(Athena::getMessageSvc(),"SimpleMdtSegmentSelector");
    if (log.level()<=MSG::VERBOSE){
      log<<MSG::VERBOSE<<"SimpleMdtSegmentSelector::select" << endmsg;
      log<<MSG::VERBOSE<<"Segment: chi2 " <<  seg.chi2() << " mdtHits " << seg.mdtHitsOnTrack() << " tan " << seg.direction().y()/seg.direction().z() << endmsg;
    }

    // test if segments satisfies the selectors selection criteria

    // test chi2
    if( seg.chi2() > m_chi2_max ) return false;

    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"segment passed chi2 cut " << seg.chi2() << endmsg;
 
    // test total numbers of hits on segment
    if( seg.mdtHitsOnTrack() < m_hits_min ) return false;

    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"segment passed mdtHits cut " << seg.mdtHitsOnTrack() << endmsg;

    double tanphi = seg.direction().y()/seg.direction().z();
    //if( std::abs(tanphi) > 1. ) return false;
  
    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"segment passed angular cut " << tanphi << endmsg;

    return true;
  }

}
