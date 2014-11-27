// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterAuxContainer.h 631625 2014-11-27 17:24:55Z gwatts $
#ifndef XAODTRIGCALO_TRIGEMCLUSTERAUXCONTAINER_H
#define XAODTRIGCALO_TRIGEMCLUSTERAUXCONTAINER_H

// Local include(s):
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v1.h"

namespace xAOD {
  /// Define the latest version of the trigger EM cluster auxiliary container
  typedef TrigEMClusterAuxContainer_v1 TrigEMClusterAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigEMClusterAuxContainer, 1111649561, 1 )

#endif // XAODTRIGCALO_TRIGEMCLUSTERAUXCONTAINER_H
