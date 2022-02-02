/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_STRIPCLUSTERAUXCONTAINER_H
#define XAODINDETMEASUREMENT_STRIPCLUSTERAUXCONTAINER_H

// Local include(s):
#include "xAODInDetMeasurement/versions/StripClusterAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current strip cluster auxiliary container
   ///
   typedef StripClusterAuxContainer_v1 StripClusterAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::StripClusterAuxContainer, 1187146189, 1 )

#endif // XAODINDETMEASUREMENT_STRIPCLUSTERAUXCONTAINER_H
