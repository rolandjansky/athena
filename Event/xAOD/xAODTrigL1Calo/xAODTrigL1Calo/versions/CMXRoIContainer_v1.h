// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXRoIContainer_v1.h 681007 2015-07-07 13:34:12Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXROICONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXROICONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXRoI_v1.h"

namespace xAOD {
   /// Define the CPMHits as a simple DataVector
   typedef DataVector< xAOD::CMXRoI_v1 > CMXRoIContainer_v1;
}

#endif // XAODTRIGL1CALO_VERSIONS_CMXROICONTAINER_V1_H
  
