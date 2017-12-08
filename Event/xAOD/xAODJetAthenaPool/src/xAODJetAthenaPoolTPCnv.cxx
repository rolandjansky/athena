/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetAthenaPoolTPCnv.cxx 797270 2017-02-15 00:03:55Z khoo $

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODJet/JetTrigAuxContainer.h"
#include "xAODJet/versions/JetTrigAuxContainer_v1.h"

// Local include(s):
#include "xAODJetTrigAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODJetTrigAuxContainerCnv_v1,
                       xAOD::JetTrigAuxContainer,
                       xAOD::JetTrigAuxContainer_v1,
                       Athena::TPCnvVers::Old )
