// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterContainer.h 631625 2014-11-27 17:24:55Z gwatts $
#ifndef XAODTRIGCALO_TRIGCALOCLUSTERCONTAINER_H
#define XAODTRIGCALO_TRIGCALOCLUSTERCONTAINER_H

// Local include(s):
#include "xAODTrigCalo/TrigCaloCluster.h"
#include "xAODTrigCalo/versions/TrigCaloClusterContainer_v1.h"

namespace xAOD {
  /// Define the latest version of the trigger calorimeter cluster container
  typedef TrigCaloClusterContainer_v1 TrigCaloClusterContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigCaloClusterContainer, 1153319721, 1 )

#endif // XAODTRIGCALO_TRIGCALOCLUSTERCONTAINER_H
