/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/versions/EmTauRoIContainer_v1.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/versions/JetRoIContainer_v1.h"
#include "xAODTrigger/EmTauRoIAuxContainer.h"
#include "xAODTrigger/versions/EmTauRoIAuxContainer_v1.h"
#include "xAODTrigger/JetRoIAuxContainer.h"
#include "xAODTrigger/versions/JetRoIAuxContainer_v1.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v1.h"

// Local include(s):
#include "xAODEmTauRoIContainerCnv_v1.h"
#include "xAODJetRoIContainerCnv_v1.h"
#include "xAODEmTauRoIAuxContainerCnv_v1.h"
#include "xAODJetRoIAuxContainerCnv_v1.h"
#include "xAODEnergySumRoIAuxInfoCnv_v1.h"
#include "xAODEnergySumRoICnv_v1.h"
#include "xAODTrigCompositeAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODEmTauRoIContainerCnv_v1,
                       xAOD::EmTauRoIContainer,
                       xAOD::EmTauRoIContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODJetRoIContainerCnv_v1,
                       xAOD::JetRoIContainer,
                       xAOD::JetRoIContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODEmTauRoIAuxContainerCnv_v1,
                       xAOD::EmTauRoIAuxContainer,
                       xAOD::EmTauRoIAuxContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODJetRoIAuxContainerCnv_v1,
                       xAOD::JetRoIAuxContainer,
                       xAOD::JetRoIAuxContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODEnergySumRoIAuxInfoCnv_v1,
                       xAOD::EnergySumRoIAuxInfo,
                       xAOD::EnergySumRoIAuxInfo_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODEnergySumRoICnv_v1,
                       xAOD::EnergySumRoI,
                       xAOD::EnergySumRoI_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODTrigCompositeAuxContainerCnv_v1,
                       xAOD::TrigCompositeAuxContainer,
                       xAOD::TrigCompositeAuxContainer_v1,
                       Athena::TPCnvVers::Old )
