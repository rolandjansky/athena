// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterContainer_v1.h 592214 2014-04-09 19:27:48Z krasznaa $
#ifndef XAODTRIGCALO_VERSIONS_TRIGEMCLUSTERCONTAINER_V1_H
#define XAODTRIGCALO_VERSIONS_TRIGEMCLUSTERCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigCalo/versions/TrigEMCluster_v1.h"

namespace xAOD {
   /// Define the trigger EM cluster container as a simple DataVector
   typedef DataVector< TrigEMCluster_v1 > TrigEMClusterContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigEMClusterContainer_v1, 1264979038, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTRIGCALO_VERSIONS_TRIGEMCLUSTERCONTAINER_V1_H
