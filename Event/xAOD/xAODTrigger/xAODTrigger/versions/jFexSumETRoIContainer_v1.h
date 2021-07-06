/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_JFEXSUMETROICONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_JFEXSUMETROICONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/jFexSumETRoI_v1.h"

 namespace xAOD {
    // Declare the Jet TOB container type
       typedef DataVector< xAOD::jFexSumETRoI_v1 > jFexSumETRoIContainer_v1;
       }

#endif // XAODTRIGGER_VERSIONS_JFEXSUMETROICONTAINER_V1_H
