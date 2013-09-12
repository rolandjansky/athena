/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTopoEventTPCnv
 * @Class  : ElectronMuonTopoInfoContainerCnv_p1
 *
 * @brief transient persistent converter for ElectronMuonTopoInfoContainer
 *
 * author - Olga Igonkina
 **********************************************************************************/
#ifndef TRIGTOPOEVENTTPCNV_ELECTRONMUONTOPOINFOCONTAINER_CNV_P1_H
#define TRIGTOPOEVENTTPCNV_ELECTRONMUONTOPOINFOCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoContainer_p1.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoCnv_p1.h"

typedef T_AthenaPoolTPCnvVector<ElectronMuonTopoInfoContainer,ElectronMuonTopoInfoContainer_p1, ElectronMuonTopoInfoCnv_p1> ElectronMuonTopoInfoContainerCnv_p1;

#endif
