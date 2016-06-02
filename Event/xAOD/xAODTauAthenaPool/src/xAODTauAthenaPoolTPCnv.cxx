/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauAthenaPoolTPCnv.cxx 749546 2016-05-25 01:31:32Z griffith $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/versions/TauJetContainer_v1.h"
#include "xAODTau/versions/TauJetContainer_v2.h"
#include "xAODTau/versions/TauJetContainer_v3.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/versions/TauJetAuxContainer_v1.h"
#include "xAODTau/versions/TauJetAuxContainer_v2.h"
#include "xAODTau/versions/TauJetAuxContainer_v3.h"

// Local include(s):
#include "xAODTauJetContainerCnv_v1.h"
#include "xAODTauJetAuxContainerCnv_v1.h"
#include "xAODTauJetContainerCnv_v2.h"
#include "xAODTauJetAuxContainerCnv_v2.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODTauJetContainerCnv_v1,
                       xAOD::TauJetContainer,
                       xAOD::TauJetContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODTauJetAuxContainerCnv_v1,
                       xAOD::TauJetAuxContainer,
                       xAOD::TauJetAuxContainer_v1,
                       Athena::TPCnvVers::Old )

// DECLARE_NAMED_TPCNV_FACTORY( xAODTauJetContainerCnv_v2,
// 			     xAODTauJetContainerCnv_v2,
// 			     xAOD::TauJetContainer,
// 			     xAOD::TauJetContainer_v2,
// 			     Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODTauJetContainerCnv_v2,
		       DataVector<xAOD::TauJet_v3>,
		       DataVector<xAOD::TauJet_v2>,
		       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODTauJetAuxContainerCnv_v2,
                       xAOD::TauJetAuxContainer,
                       xAOD::TauJetAuxContainer_v2,
                       Athena::TPCnvVers::Old )
