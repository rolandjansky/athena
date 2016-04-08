// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LumiBlockRangeContainer_v1.h 647501 2015-02-17 14:22:33Z krasznaa $
#ifndef XAODLUMINOSITY_VERSIONS_LUMIBLOCKRANGECONTAINER_V1_H
#define XAODLUMINOSITY_VERSIONS_LUMIBLOCKRANGECONTAINER_V1_H

// Core EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODLuminosity/versions/LumiBlockRange_v1.h"

namespace xAOD {

   /// The container is simply a type definition
   typedef DataVector< LumiBlockRange_v1 > LumiBlockRangeContainer_v1;

} // namespace xAOD

#endif // XAODLUMINOSITY_VERSIONS_LUMIBLOCKRANGECONTAINER_V1_H
