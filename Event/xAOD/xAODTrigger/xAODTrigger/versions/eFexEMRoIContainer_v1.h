// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: eFexEMRoIContainer_v1.cxx  2019-07-01 13:04:25Z watsona $
#ifndef XAODTRIGGER_VERSIONS_EFEXEMROICONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_EFEXEMROICONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/eFexEMRoI_v1.h"

namespace xAOD {
   /// Declare the em/tau RoI container type
   typedef DataVector< xAOD::eFexEMRoI_v1 > eFexEMRoIContainer_v1;
}

#endif // XAODTRIGGER_VERSIONS_EFEXEMROICONTAINER_V1_H
