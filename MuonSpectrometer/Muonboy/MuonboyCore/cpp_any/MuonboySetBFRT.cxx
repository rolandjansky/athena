/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboySetBFRT.h"

#include "MuonboyCore/MuonboyBFAccess.h"
#include "MuonboyCore/MuonboyRTAccess.h"

MuonboySetBFRT::MuonboySetBFRT(){}

MuonboySetBFRT::~MuonboySetBFRT(){}

void MuonboySetBFRT::initialize(){}

void MuonboySetBFRT::finalize(){}


void MuonboySetBFRT::Set(
     MuonboyBFAbstract*       pMuonboyBFAbstract,
     MuonboyRTAbstract*       pMuonboyRTAbstract
){
     
 MuonboyBFAccess::SetMuonboyBFAbstract(pMuonboyBFAbstract);
 MuonboyRTAccess::SetMuonboyRTAbstract(pMuonboyRTAbstract);

}
