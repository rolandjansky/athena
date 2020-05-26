/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/SimplePatternSelector.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>

namespace MuonCalib {

  SimplePatternSelector::SimplePatternSelector(double chi2_max, 
					       unsigned int mdtSeg_min,
					       unsigned int /*phiSeg_min*/)
  
    : m_chi2_max(chi2_max),m_mdtSeg_min(mdtSeg_min)
  {
    m_printLevel = 0;
    MsgStream log(Athena::getMessageSvc(),"SimplePatternSelector");
    if(log.level()<=MSG::VERBOSE) {
      log<<MSG::VERBOSE<<"SimplePatternSelector::SimplePatternSelector" << endmsg;
      log<<MSG::VERBOSE<<"New SimplePatternSelector : " << endmsg;
      log<<MSG::VERBOSE<<"       chi2 cut    " << m_chi2_max << endmsg;
      log<<MSG::VERBOSE<<"       muon seg cut " << m_mdtSeg_min << endmsg;
    }
  }


  bool SimplePatternSelector::select(const MuonCalibPattern& pattern) const {
    MsgStream log(Athena::getMessageSvc(),"SimplePatternSelector");
    if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE<<"SimplePatternSelector::select" << endmsg;

    // test if pattern satisfies the selectors selection criteria

    // test chi2
    if( pattern.chi2() > m_chi2_max ) return false;

    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"pattern passed chi2 cut " << pattern.chi2() << endmsg;

    // test total numbers of mdt segments on pattern
    if( pattern.muonSegments() < m_mdtSeg_min ) return false;

    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"pattern passed muon segments cut " << pattern.muonSegments() << endmsg;


    return true;
  }

}
