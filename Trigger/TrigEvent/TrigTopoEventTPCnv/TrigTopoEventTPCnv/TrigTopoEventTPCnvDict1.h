/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTOPOEVENTTPCNV_TRIGTOPOEVENTTPCNVDICT1_H
#define TRIGTOPOEVENTTPCNV_TRIGTOPOEVENTTPCNVDICT1_H

/* ElectronMuonTopoInfo T/P classes: persistent version 1*/
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoContainer_p1.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoContainerCnv_p1.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfo_p1.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoCnv_p1.h"

//dummy instances of all objects using std::vector<TPObjRef> for the dictionary
struct dummy_TrigTopoEventTPCnvDict1
{
  std::vector<ElectronMuonTopoInfo_p1>                     m_dummyElectronMuonTopoInfo_p1;
};

#endif
