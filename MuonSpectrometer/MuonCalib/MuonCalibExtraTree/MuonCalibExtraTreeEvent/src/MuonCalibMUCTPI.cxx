/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibMUCTPI.h"

#include <iostream>

namespace MuonCalib{
  
  MuonCalibMUCTPI::MuonCalibMUCTPI( int roiWord, int bcID, int sysID, int subsysID, int sectorID, int thresholdNumber, int thresholdValue, int roINumber, int overlap, int firstCandidate, int sectorOverflow, int padOverflow, float phi, float eta ) :
    m_roiWord(roiWord), m_bcID(bcID), m_sysID(sysID), m_subsysID(subsysID), m_sectorID(sectorID), m_thresholdNumber(thresholdNumber), m_thresholdValue(thresholdValue), m_roINumber(roINumber), m_overlap(overlap), m_firstCandidate(firstCandidate), m_sectorOverflow(sectorOverflow), m_padOverflow(padOverflow), m_phi(phi), m_eta(eta)
  {
  }
  
}//namespace MuonCalib
