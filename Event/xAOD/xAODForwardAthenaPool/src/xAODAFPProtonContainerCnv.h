// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   xAODAFPProtonContainerCnv.h
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 *
 * @brief  Declaration of the xAODAFPProtonContainerCnv class.
 *
 */

#ifndef XAODFORWARDATHENAPOOL_XAODAFPPROTONCONTAINERCNV_H
#define XAODFORWARDATHENAPOOL_XAODAFPPROTONCONTAINERCNV_H

// POOL include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

// EDM include(s):
#include "xAODForward/AFPProtonContainer.h"

/// Type of the POOL converter for xAOD::AFPProtonContainer
typedef T_AthenaPoolxAODCnv< xAOD::AFPProtonContainer >
   xAODAFPProtonContainerCnv;

#endif // XAODFORWARDATHENAPOOL_XAODAFPPROTONCONTAINERCNV_H
