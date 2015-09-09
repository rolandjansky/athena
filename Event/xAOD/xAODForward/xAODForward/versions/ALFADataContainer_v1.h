// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFADataContainer_v1.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_VERSIONS_ALFADATACONTAINER_V1_H
#define XAODFORWARD_VERSIONS_ALFADATACONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODForward/versions/ALFAData_v1.h"

namespace xAOD {
   typedef DataVector< ALFAData_v1 > ALFADataContainer_v1;
}

#endif //XAODFORWARD_VERSIONS_ALFADATACONTAINER_V1_H
