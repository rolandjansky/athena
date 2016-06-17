// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPassBitsContainer_v1.h 702738 2015-10-23 16:00:51Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGPASSBITSCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_TRIGPASSBITSCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/TrigPassBits_v1.h"

namespace xAOD {
   /// Define the trig pass bits container as a simple DataVector
   typedef DataVector< TrigPassBits_v1 > TrigPassBitsContainer_v1;
}

#endif // XAODTRIGGER_VERSIONS_TRIGPASSBITSCONTAINER_V1_H
