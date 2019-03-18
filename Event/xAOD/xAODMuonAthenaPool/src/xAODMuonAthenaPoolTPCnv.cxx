/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id:  $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/versions/MuonAuxContainer_v1.h"
#include "xAODMuon/versions/MuonAuxContainer_v2.h"
#include "xAODMuon/versions/MuonAuxContainer_v3.h"
#include "xAODMuon/versions/MuonAuxContainer_v4.h"


// Local include(s):
#include "xAODMuonAuxContainerCnv_v1.h"
#include "xAODMuonAuxContainerCnv_v2.h"
#include "xAODMuonAuxContainerCnv_v3.h"
#include "xAODMuonAuxContainerCnv_v4.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODMuonAuxContainerCnv_v1,
                       xAOD::MuonAuxContainer,
                       xAOD::MuonAuxContainer_v1,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODMuonAuxContainerCnv_v2,
                       xAOD::MuonAuxContainer,
                       xAOD::MuonAuxContainer_v2,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODMuonAuxContainerCnv_v3,
                       xAOD::MuonAuxContainer,
                       xAOD::MuonAuxContainer_v3,
                       Athena::TPCnvVers::Old )

DECLARE_TPCNV_FACTORY( xAODMuonAuxContainerCnv_v4,
                       xAOD::MuonAuxContainer,
                       xAOD::MuonAuxContainer_v4,
                       Athena::TPCnvVers::Old )
