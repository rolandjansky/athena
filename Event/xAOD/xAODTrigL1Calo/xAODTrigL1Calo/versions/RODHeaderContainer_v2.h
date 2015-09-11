// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeaderContainer_v2.h 694350 2015-09-11 12:02:19Z krasznaa $
#ifndef XAODTRIGL1CALO_VERSIONS_RODHEADERCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_RODHEADERCONTAINER_V2_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/RODHeader_v2.h"

namespace xAOD {
   /// Define the RODHeader as a simple DataVector
   typedef DataVector< xAOD::RODHeader_v2 > RODHeaderContainer_v2;
}

#endif // XAODTRIGL1CALO_VERSIONS_RODHEADERCONTAINER_V2_H
  
