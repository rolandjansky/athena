// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfContainer_v1.h 630856 2014-11-25 11:22:49Z gwatts $
#ifndef XAODTRIGGER_VERSIONS_BUNCHCONFCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_BUNCHCONFCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/BunchConf_v1.h"

namespace xAOD {
   /// Define the bunch configuration container as a simple DataVector
   typedef DataVector< BunchConf_v1 > BunchConfContainer_v1;
}

#endif // XAODTRIGGER_VERSIONS_BUNCHCONFCONTAINER_V1_H
