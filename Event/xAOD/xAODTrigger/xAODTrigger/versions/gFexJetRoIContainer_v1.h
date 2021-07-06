// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_GFEXJETROICONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_GFEXJETROICONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/gFexJetRoI_v1.h"

namespace xAOD {
   /// Declare the Jet TOB container type
   typedef DataVector< xAOD::gFexJetRoI_v1 > gFexJetRoIContainer_v1;
}

#endif // XAODTRIGGER_VERSIONS_GFEXJETROICONTAINER_V1_H
