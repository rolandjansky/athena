// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElementContainer_v2.h 642630 2015-01-29 10:32:43Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JETELEMENTCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_JETELEMENTCONTAINER_V2_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JetElement_v2.h"

namespace xAOD {
   /// Define the JetElement as a simple DataVector
   typedef DataVector< xAOD::JetElement_v2 > JetElementContainer_v2;
}
#endif // XAODTRIGL1CALO_VERSIONS_JETELEMENTCONTAINER_V2_H
  
