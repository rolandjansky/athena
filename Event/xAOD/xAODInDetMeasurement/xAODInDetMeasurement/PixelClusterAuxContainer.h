/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_PIXELCLUSTERAUXCONTAINER_H
#define XAODINDETMEASUREMENT_PIXELCLUSTERAUXCONTAINER_H

// Local include(s):
#include "xAODInDetMeasurement/versions/PixelClusterAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current pixel cluster auxiliary container
   ///
   typedef PixelClusterAuxContainer_v1 PixelClusterAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PixelClusterAuxContainer, 1279452163, 1 )

#endif // XAODINDETMEASUREMENT_PIXELCLUSTERAUXCONTAINER_H
