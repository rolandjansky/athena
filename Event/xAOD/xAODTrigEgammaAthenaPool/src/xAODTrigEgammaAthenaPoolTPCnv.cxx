/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEgammaAthenaPoolTPCnv.cxx 705790 2015-11-04 13:09:23Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODTrigEgamma/ElectronTrigAuxContainer.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v1.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v2.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v3.h"
#include "xAODTrigEgamma/PhotonTrigAuxContainer.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v1.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v2.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v3.h"

// Local include(s):
#include "xAODElectronTrigAuxContainerOfflineCnv_v1.h"
#include "xAODElectronTrigAuxContainerOfflineCnv_v2.h"
#include "xAODElectronTrigAuxContainerOfflineCnv_v3.h"
#include "xAODPhotonTrigAuxContainerOfflineCnv_v1.h"
#include "xAODPhotonTrigAuxContainerOfflineCnv_v2.h"
#include "xAODPhotonTrigAuxContainerOfflineCnv_v3.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODElectronTrigAuxContainerOfflineCnv_v1,
                       xAOD::ElectronTrigAuxContainer,
                       xAOD::ElectronAuxContainer_v1,
                       Athena::TPCnvVers::Old )
DECLARE_TPCNV_FACTORY( xAODElectronTrigAuxContainerOfflineCnv_v2,
                       xAOD::ElectronTrigAuxContainer,
                       xAOD::ElectronAuxContainer_v2,
                       Athena::TPCnvVers::Old )
DECLARE_TPCNV_FACTORY( xAODElectronTrigAuxContainerOfflineCnv_v3,
                       xAOD::ElectronTrigAuxContainer,
                       xAOD::ElectronAuxContainer_v3,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODPhotonTrigAuxContainerOfflineCnv_v1,
                       xAOD::PhotonTrigAuxContainer,
                       xAOD::PhotonAuxContainer_v1,
                       Athena::TPCnvVers::Old )
DECLARE_TPCNV_FACTORY( xAODPhotonTrigAuxContainerOfflineCnv_v2,
                       xAOD::PhotonTrigAuxContainer,
                       xAOD::PhotonAuxContainer_v2,
                       Athena::TPCnvVers::Old )
DECLARE_TPCNV_FACTORY( xAODPhotonTrigAuxContainerOfflineCnv_v3,
                       xAOD::PhotonTrigAuxContainer,
                       xAOD::PhotonAuxContainer_v3,
                       Athena::TPCnvVers::Old )
