/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyBFAccess.h"

#include "MboyCoreEvt/MuonboyBFAbstract.h"
 
MuonboyBFAbstract* MuonboyBFAccess::p_MuonboyBFAbstract = 0;
MuonboyBFAccess::MuonboyBFAccess(){}
 
MuonboyBFAccess::~MuonboyBFAccess(){}
 
// Get
MuonboyBFAbstract* MuonboyBFAccess::GetMuonboyBFAbstract(){return p_MuonboyBFAbstract;}
 
// Set
void MuonboyBFAccess::SetMuonboyBFAbstract(MuonboyBFAbstract* pMuonboyBFAbstract){p_MuonboyBFAbstract = pMuonboyBFAbstract;}
 
// Reset
void MuonboyBFAccess::ResetMuonboyBFAbstract(){p_MuonboyBFAbstract = 0;}

// Get field 
void MuonboyBFAccess::field(float* XYZ,float* BXYZ){p_MuonboyBFAbstract->field(XYZ,BXYZ);}

// Get field and gradient
void MuonboyBFAccess::fieldd(float* XYZ,float* BXYZ){p_MuonboyBFAbstract->fieldd(XYZ,BXYZ);}

// Get field 
void MuonboyBFAccess::field_tesla_cm(float* XYZ,float* BXYZ){p_MuonboyBFAbstract->field_tesla_cm(XYZ,BXYZ);}

// Get field and gradient
void MuonboyBFAccess::fieldGradient_tesla_cm(float* XYZ,float* BXYZ){p_MuonboyBFAbstract->fieldGradient_tesla_cm(XYZ,BXYZ);}
