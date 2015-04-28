// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSumsContainer_v2.h 643054 2015-01-30 13:55:12Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMETSUMSCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_JEMETSUMSCONTAINER_V2_H

// EDM include(s):
#include "AthContainers/DataVector.h"
// Local include(s):
#include "xAODTrigL1Calo/versions/JEMEtSums_v2.h"

namespace xAOD {
   /// Define the JEMEtSums as a simple DataVector
   typedef DataVector< xAOD::JEMEtSums_v2 > JEMEtSumsContainer_v2;
}

#endif // XAODTRIGL1CALO_VERSIONS_JEMETSUMSCONTAINER_V2_H
  
