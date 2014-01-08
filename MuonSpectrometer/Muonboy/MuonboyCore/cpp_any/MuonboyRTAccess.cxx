/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyRTAccess.h"
 
MuonboyRTAbstract* MuonboyRTAccess::p_MuonboyRTAbstract = 0;
MuonboyRTAccess::MuonboyRTAccess(){}
 
MuonboyRTAccess::~MuonboyRTAccess(){}
 
MuonboyRTAbstract* MuonboyRTAccess::GetMuonboyRTAbstract(){return p_MuonboyRTAbstract;}
 
void MuonboyRTAccess::SetMuonboyRTAbstract(MuonboyRTAbstract* pMuonboyRTAbstract){p_MuonboyRTAbstract = pMuonboyRTAbstract;}
 
void MuonboyRTAccess::ResetMuonboyRTAbstract(){p_MuonboyRTAbstract = 0;}

