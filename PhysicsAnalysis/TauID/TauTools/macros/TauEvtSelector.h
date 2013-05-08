/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUSELECT_H
#define TAUSELECT_H

#include <vector>

class TauEvtSelector{
public:	
bool isGoodLBRun00153565(int LBnumber);
bool isGoodBCIDRun00153565(int BCID);
bool JetsGood(std::vector<int> jet_isGood);
bool PassedTrigger(bool trigger);
bool hasGoodVertex(std::vector<int> vxp_trk_n);
bool hasDiTau(std::vector<float> tau_phi, std::vector<float> tau_Et);
TauEvtSelector();	
~TauEvtSelector();	
  
private:	
  
};
#endif
