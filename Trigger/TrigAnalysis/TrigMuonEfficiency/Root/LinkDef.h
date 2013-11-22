/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigMuonEfficiencyLinkDef_h__
#define __TrigMuonEfficiencyLinkDef_h__

#include "TrigMuonEfficiency/MuonHypothesis.h"
#include "TrigMuonEfficiency/LeptonTriggerSF.h"
#include "TrigMuonEfficiency/HSG3TrigLeptonSFTool.h"
#include "TrigMuonEfficiency/MuonTriggerMatching.h" 
#include "TrigMuonEfficiency/ElectronTriggerMatching.h" 
#include "TrigMuonEfficiency/TriggerNavigationVariables.h" 

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class MuonHypothesis+;
#pragma link C++ class LeptonTriggerSF+;
#pragma link C++ class HSG3TrigLeptonSFTool+;

#pragma link C++ class MuonTriggerMatching+; 
#pragma link C++ class ElectronTriggerMatching+; 
#pragma link C++ class TriggerNavigationVariables+;

#endif

#endif
