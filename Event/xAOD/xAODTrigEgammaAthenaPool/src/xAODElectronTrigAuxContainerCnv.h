// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronTrigAuxContainerCnv.h 705790 2015-11-04 13:09:23Z krasznaa $
#ifndef XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINERCNV_H
#define XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigEgamma/ElectronTrigAuxContainer.h"
#include "xAODElectronTrigAuxContainerOfflineCnv_v1.h"
#include "xAODElectronTrigAuxContainerOfflineCnv_v2.h"
#include "xAODElectronTrigAuxContainerOfflineCnv_v3.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::ElectronTrigAuxContainer,
                                     xAODElectronTrigAuxContainerOfflineCnv_v3,
                                     xAODElectronTrigAuxContainerOfflineCnv_v2,
                                     xAODElectronTrigAuxContainerOfflineCnv_v1 >
   xAODElectronTrigAuxContainerCnv;


#endif // XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINERCNV_H
