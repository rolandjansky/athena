// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODL2StandAloneMuonContainerCnv.h 592615 2014-04-11 14:36:58Z krasznaa $
#ifndef XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONCONTAINERCNV_H
#define XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

// EDM include(s):
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODL2StandAloneMuonContainerCnv_v1.h"


/// Type definition for the converter's base
typedef T_AthenaPoolxAODCnv< xAOD::L2StandAloneMuonContainer,
                             xAODL2StandAloneMuonContainerCnv_v1 >
   xAODL2StandAloneMuonContainerCnv;


#endif // XAODTRIGMUONATHENAPOOL_XAODL2STANDALONEMUONCONTAINERCNV_H
