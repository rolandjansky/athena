// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonAuxContainerCnv.h 614855 2014-09-03 11:42:57Z krasznaa $
#ifndef XAODEGAMMAATHENAPOOL_XAODPHOTONAUXCONTAINERCNV_H
#define XAODEGAMMAATHENAPOOL_XAODPHOTONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODPhotonAuxContainerCnv_v1.h"
#include "xAODPhotonAuxContainerCnv_v2.h"

// EDM include(s):
#include "xAODEgamma/PhotonAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::PhotonAuxContainer,
                                     xAODPhotonAuxContainerCnv_v2,
                                     xAODPhotonAuxContainerCnv_v1 >

   xAODPhotonAuxContainerCnv;


#endif // XAODEGAMMAATHENAPOOL_XAODPHOTONAUXCONTAINERCNV_H
