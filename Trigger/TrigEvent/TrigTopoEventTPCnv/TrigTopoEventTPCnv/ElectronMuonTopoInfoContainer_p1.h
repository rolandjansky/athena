/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTopoEventTPCnv
 * @Class  : ElectronMuonTopoInfoContainer_p3
 *
 * @brief persistent partner for ElectronMuonTopoInfoContainer
 *
 **********************************************************************************/
#ifndef TRIGTOPOEVENTTPCNV_ELECTRONMUONTOPOINFOCONTAINER_P1_H
#define TRIGTOPOEVENTTPCNV_ELECTRONMUONTOPOINFOCONTAINER_P1_H

#include "TrigTopoEventTPCnv/ElectronMuonTopoInfo_p1.h"
  
class ElectronMuonTopoInfoContainer_p1 : public std::vector<ElectronMuonTopoInfo_p1> { };
    
#endif
