/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODEgammaAthenaPoolTPCnv.cxx 619884 2014-10-03 15:00:40Z christos $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v1.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v1.h"

// Local include(s):
#include "xAODElectronAuxContainerCnv_v1.h"
#include "xAODPhotonAuxContainerCnv_v1.h"
#include "xAODElectronAuxContainerCnv_v2.h"
#include "xAODPhotonAuxContainerCnv_v2.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODElectronAuxContainerCnv_v1,
                       xAOD::ElectronAuxContainer,
                       xAOD::ElectronAuxContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODPhotonAuxContainerCnv_v1,
                       xAOD::PhotonAuxContainer,
                       xAOD::PhotonAuxContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODElectronAuxContainerCnv_v2,
                       xAOD::ElectronAuxContainer,
                       xAOD::ElectronAuxContainer_v2,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODPhotonAuxContainerCnv_v2,
                       xAOD::PhotonAuxContainer,
                       xAOD::PhotonAuxContainer_v2,
                       Athena::TPCnvVers::Old )
