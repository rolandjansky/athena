// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonTrigAuxContainerOfflineCnv_v2.h 705790 2015-11-04 13:09:23Z krasznaa $
#ifndef XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINEROFFLINECNV_V2_H
#define XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINEROFFLINECNV_V2_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv.h"

// EDM include(s):
#include "xAODEgamma/versions/PhotonAuxContainer_v2.h"
#include "xAODTrigEgamma/PhotonTrigAuxContainer.h"

/// Converter class used to read xAOD::PhotonAuxContainer_v2
///
/// Since we switched from using the offline xAOD::PhotonAuxContainer
/// type to using a trigger specific xAOD::PhotonTrigAuxContainer type
/// at the end of 2015, we now need to provide means of translating the offline
/// objects into the trigger ones automatically, to be able to read old
/// RAW files correctly.
///
typedef T_AuxContainerCopyTPCnv< xAOD::PhotonTrigAuxContainer,
                                 xAOD::PhotonAuxContainer_v2 >
  xAODPhotonTrigAuxContainerOfflineCnv_v2;


#endif // XAODTRIGEGAMMAATHENAPOOL_XAODPHOTONTRIGAUXCONTAINEROFFLINECNV_V2_H
