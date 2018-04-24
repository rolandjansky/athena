/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXCPTOBCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXCPTOBCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::CMXCPTobContainer>
  xAODCMXCPTobContainerCnv;
  
#endif   
