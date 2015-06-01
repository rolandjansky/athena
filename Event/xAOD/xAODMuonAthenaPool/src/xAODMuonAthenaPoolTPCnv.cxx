/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:  $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/versions/MuonAuxContainer_v1.h"


// Local include(s):
#include "xAODMuonAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODMuonAuxContainerCnv_v1,
                       xAOD::MuonAuxContainer,
                       xAOD::MuonAuxContainer_v1,
                       Athena::TPCnvVers::Old )

