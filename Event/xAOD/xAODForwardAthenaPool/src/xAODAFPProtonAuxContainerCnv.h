// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   xAODAFPProtonAuxContainerCnv.h
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 *
 * @brief  Declaration of the xAODAFPProtonAuxContainerCnv class.
 *
 */

#ifndef XAODFORWARDATHENAPOOL_AFPPROTONAUXCONTAINERCNV_H
#define XAODFORWARDATHENAPOOL_AFPPROTONAUXCONTAINERCNV_H

// POOL include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODForward/AFPProtonAuxContainer.h"

/// Type of the POOL converter for xAOD::AFPProtonAuxContainer
typedef T_AthenaPoolAuxContainerCnv< xAOD::AFPProtonAuxContainer >
   xAODAFPProtonAuxContainerCnv;

#endif // XAODFORWARDATHENAPOOL_AFPPROTONAUXCONTAINERCNV_H
