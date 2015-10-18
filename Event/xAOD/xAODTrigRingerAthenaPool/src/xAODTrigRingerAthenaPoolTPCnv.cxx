/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRNNOutputAuxContainer.h"
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v1.h"

// Local include(s):
#include "xAODTrigRNNOutputAuxContainerCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODTrigRNNOutputAuxContainerCnv_v1,
                       xAOD::TrigRNNOutputAuxContainer,
                       xAOD::TrigRNNOutputAuxContainer_v1,
                       Athena::TPCnvVers::Old )
