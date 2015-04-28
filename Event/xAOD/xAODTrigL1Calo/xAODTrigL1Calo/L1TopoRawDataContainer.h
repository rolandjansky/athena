/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_L1TOPORAWDATACONTAINER_H
#define XAODTRIGL1CALO_L1TOPORAWDATACONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/L1TopoRawData.h"
#include "xAODTrigL1Calo/versions/L1TopoRawDataContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the L1TopoRawData container
   typedef L1TopoRawDataContainer_v1 L1TopoRawDataContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::L1TopoRawDataContainer , 1185434818 , 1 )

#endif // XAODTRIGL1CALO_L1TOPORAWDATACONTAINER_H
