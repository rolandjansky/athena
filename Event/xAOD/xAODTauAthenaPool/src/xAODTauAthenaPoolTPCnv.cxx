/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauAthenaPoolTPCnv.cxx 632169 2014-11-29 15:13:34Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/versions/TauJetContainer_v1.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/versions/TauJetAuxContainer_v1.h"

// Local include(s):
#include "xAODTauJetContainerCnv_v1.h"
#include "xAODTauJetAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODTauJetContainerCnv_v1,
                       xAOD::TauJetContainer,
                       xAOD::TauJetContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODTauJetAuxContainerCnv_v1,
                       xAOD::TauJetAuxContainer,
                       xAOD::TauJetAuxContainer_v1,
                       Athena::TPCnvVers::Old )
