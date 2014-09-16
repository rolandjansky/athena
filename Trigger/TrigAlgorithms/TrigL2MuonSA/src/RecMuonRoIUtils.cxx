/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/RecMuonRoIUtils.h"

bool TrigL2MuonSA::RecMuonRoIUtils::isBarrel(const LVL1::RecMuonRoI* p_roi) {
   return (p_roi->sysID()==0) ? true : false; 
}
bool TrigL2MuonSA::RecMuonRoIUtils::isLowPt(const LVL1::RecMuonRoI* p_roi)  { 
   return (p_roi->getThresholdNumber() <4) ? true : false;
}
bool TrigL2MuonSA::RecMuonRoIUtils::isHighPt(const LVL1::RecMuonRoI* p_roi) {
   return (p_roi->getThresholdNumber()>=4) ? true : false;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
