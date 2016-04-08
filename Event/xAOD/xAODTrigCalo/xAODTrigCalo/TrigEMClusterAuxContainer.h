// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterAuxContainer.h 639154 2015-01-13 14:35:02Z wigleswt $
#ifndef XAODTRIGCALO_TRIGEMCLUSTERAUXCONTAINER_H
#define XAODTRIGCALO_TRIGEMCLUSTERAUXCONTAINER_H

// Local include(s):
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v2.h"

namespace xAOD {
  /// Define the latest version of the trigger EM cluster auxiliary container
  typedef TrigEMClusterAuxContainer_v2 TrigEMClusterAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigEMClusterAuxContainer, 1111649561, 1 )

#endif // XAODTRIGCALO_TRIGEMCLUSTERAUXCONTAINER_H
