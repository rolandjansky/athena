// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetAuxContainerCnv.h 800296 2017-03-10 18:16:40Z griffith $
#ifndef XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_H
#define XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_H

// stystem includes
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTauJetAuxContainerCnv_v1.h"
#include "xAODTauJetAuxContainerCnv_v2.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::TauJetAuxContainer,
                                     xAODTauJetAuxContainerCnv_v2,
                                     xAODTauJetAuxContainerCnv_v1 >
   xAODTauJetAuxContainerCnv;


#endif // XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_H
