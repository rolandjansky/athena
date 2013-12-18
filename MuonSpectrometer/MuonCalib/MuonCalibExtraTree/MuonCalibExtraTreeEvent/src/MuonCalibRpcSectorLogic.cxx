/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibRpcSectorLogic.h"

#include <iostream>

namespace MuonCalib{
  
  MuonCalibRpcSectorLogic::MuonCalibRpcSectorLogic( int sectorId, int fel1Id, int bcid, int errorCode, int crc, int hasMoreThan2TriggerCand, 
						    int slIndex, float triggerRates, int counters,
						    int slIndex2, int rowinBcid, int padid, int ptid, int roi, int outerPlane, 
						    int overlapPhi, int overlapEta, int triggerBcid, int isInput ) : 
    m_sectorId(sectorId), m_fel1Id(fel1Id), m_bcid(bcid), m_errorCode(errorCode), m_crc(crc), m_hasMoreThan2TriggerCand(hasMoreThan2TriggerCand), m_slIndex(slIndex), m_triggerRates(triggerRates), m_counters(counters), m_slIndex2(slIndex2), m_rowinBcid(rowinBcid), m_padid(padid), m_ptid(ptid), m_roi(roi), m_outerPlane(outerPlane), m_overlapPhi(overlapPhi), m_overlapEta(overlapEta), m_triggerBcid(triggerBcid), m_isInput(isInput)
    
  {
  }
  
}//namespace MuonCalib
