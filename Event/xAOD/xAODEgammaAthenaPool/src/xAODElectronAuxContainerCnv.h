// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronAuxContainerCnv.h 614830 2014-09-03 09:41:57Z krasznaa $
#ifndef XAODEGAMMAATHENAPOOL_XAODELECTRONAUXCONTAINERCNV_H
#define XAODEGAMMAATHENAPOOL_XAODELECTRONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODElectronAuxContainerCnv_v1.h"
#include "xAODElectronAuxContainerCnv_v2.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::ElectronAuxContainer,
                                     xAODElectronAuxContainerCnv_v2,
                                     xAODElectronAuxContainerCnv_v1 >
   xAODElectronAuxContainerCnv;


#endif // XAODEGAMMAATHENAPOOL_XAODELECTRONAUXCONTAINERCNV_H
