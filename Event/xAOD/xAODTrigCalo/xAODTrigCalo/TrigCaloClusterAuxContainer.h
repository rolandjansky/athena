// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterAuxContainer.h 631625 2014-11-27 17:24:55Z gwatts $
#ifndef XAODTRIGCALO_TRIGCALOCLUSTERAUXCONTAINER_H
#define XAODTRIGCALO_TRIGCALOCLUSTERAUXCONTAINER_H

// Local include(s):
#include "xAODTrigCalo/versions/TrigCaloClusterAuxContainer_v1.h"

namespace xAOD {
  /// Define the latest version of the trigger calorimeter cluster auxiliary container
  typedef TrigCaloClusterAuxContainer_v1 TrigCaloClusterAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigCaloClusterAuxContainer, 1130546378, 1 )

#endif // XAODTRIGCALO_TRIGCALOCLUSTERAUXCONTAINER_H
