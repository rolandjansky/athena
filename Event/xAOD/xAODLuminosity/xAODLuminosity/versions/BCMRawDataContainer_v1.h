// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BCMRawDataContainer_v1.h 652111 2015-03-06 10:13:22Z krasznaa $
#ifndef XAODLUMINOSITY_VERSIONS_BCMRAWDATACONTAINER_V1_H
#define XAODLUMINOSITY_VERSIONS_BCMRAWDATACONTAINER_V1_H

// Core EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODLuminosity/versions/BCMRawData_v1.h"

namespace xAOD {

   /// The container is simply a type definition
   typedef DataVector< BCMRawData_v1 > BCMRawDataContainer_v1;

} // namespace xAOD

#endif // XAODLUMINOSITY_VERSIONS_BCMRAWDATACONTAINER_V1_H
