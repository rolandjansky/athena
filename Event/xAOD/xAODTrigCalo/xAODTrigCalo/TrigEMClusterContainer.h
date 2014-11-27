// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterContainer.h 631117 2014-11-26 08:48:22Z gwatts $
#ifndef XAODTRIGCALO_TRIGEMCLUSTERCONTAINER_H
#define XAODTRIGCALO_TRIGEMCLUSTERCONTAINER_H

// Local include(s):
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/versions/TrigEMClusterContainer_v1.h"

namespace xAOD {
  /// Define the latest version of the trigger EM cluster container
  typedef TrigEMClusterContainer_v1 TrigEMClusterContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
#include "xAODCore/BaseInfo.h"
CLASS_DEF( xAOD::TrigEMClusterContainer, 1264979038, 1 )

#endif // XAODTRIGCALO_TRIGEMCLUSTERCONTAINER_H
