// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODL2StandAloneMuonAuxContainerCnv.h 592615 2014-04-11 14:36:58Z krasznaa $
#ifndef XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONAUXCONTAINERCNV_H
#define XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODL2StandAloneMuonAuxContainerCnv_v1.h"


typedef T_AthenaPoolAuxContainerCnv< xAOD::L2StandAloneMuonAuxContainer,
                                     xAODL2StandAloneMuonAuxContainerCnv_v1 >
   xAODL2StandAloneMuonAuxContainerCnv;


#endif // XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONAUXCONTAINERCNV_H
