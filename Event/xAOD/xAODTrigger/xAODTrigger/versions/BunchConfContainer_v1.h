// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfContainer_v1.h 583374 2014-02-14 15:52:54Z krasznaa $
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

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::BunchConfContainer_v1, 1244568624, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_VERSIONS_BUNCHCONFCONTAINER_V1_H
