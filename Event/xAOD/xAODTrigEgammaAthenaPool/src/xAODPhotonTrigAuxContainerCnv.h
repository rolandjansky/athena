// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonTrigAuxContainerCnv.h 705790 2015-11-04 13:09:23Z krasznaa $
#ifndef XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINERCNV_H
#define XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigEgamma/PhotonTrigAuxContainer.h"
#include "xAODPhotonTrigAuxContainerOfflineCnv_v1.h"
#include "xAODPhotonTrigAuxContainerOfflineCnv_v2.h"
#include "xAODPhotonTrigAuxContainerOfflineCnv_v3.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::PhotonTrigAuxContainer,
                                     xAODPhotonTrigAuxContainerOfflineCnv_v3,
                                     xAODPhotonTrigAuxContainerOfflineCnv_v2,
                                     xAODPhotonTrigAuxContainerOfflineCnv_v1 >
   xAODPhotonTrigAuxContainerCnv;


#endif // XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINERCNV_H
