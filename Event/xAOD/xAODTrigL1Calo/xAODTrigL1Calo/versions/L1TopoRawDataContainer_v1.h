// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoRawDataContainer_v1.h 652824 2015-03-09 23:24:32Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_L1TOPORAWDATACONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_L1TOPORAWDATACONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/L1TopoRawData_v1.h"

namespace xAOD {
   /// Define the CMMCPHits as a simple DataVector
   typedef DataVector< xAOD::L1TopoRawData_v1 > L1TopoRawDataContainer_v1;
}

#endif // XAODTRIGL1CALO_VERSIONS_L1TOPORAWDATACONTAINER_V1_H
