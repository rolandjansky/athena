// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronTrigAuxContainerOfflineCnv_v3.h 705790 2015-11-04 13:09:23Z krasznaa $
#ifndef XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINEROFFLINECNV_V3_H
#define XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINEROFFLINECNV_V3_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv.h"

// EDM include(s):
#include "xAODEgamma/versions/ElectronAuxContainer_v3.h"
#include "xAODTrigEgamma/ElectronTrigAuxContainer.h"

/// Converter class used to read xAOD::ElectronAuxContainer_v3
///
/// Since we switched from using the offline xAOD::ElectronAuxContainer
/// type to using a trigger specific xAOD::ElectronTrigAuxContainer type
/// at the end of 2015, we now need to provide means of translating the offline
/// objects into the trigger ones automatically, to be able to read old
/// RAW files correctly.
///
typedef T_AuxContainerCopyTPCnv< xAOD::ElectronTrigAuxContainer,
                                 xAOD::ElectronAuxContainer_v3 >
  xAODElectronTrigAuxContainerOfflineCnv_v3;


#endif // XAODTRIGEGAMMAATHENAPOOL_XAODELECTRONTRIGAUXCONTAINEROFFLINECNV_V3_H
