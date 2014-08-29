// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHPILEUPEVENTCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHPILEUPEVENTCONTAINER_V1_H

#include "AthContainers/DataVector.h"
#include "xAODTruth/versions/TruthPileupEvent_v1.h"


namespace xAOD {
   typedef DataVector< TruthPileupEvent_v1 > TruthPileupEventContainer_v1;
}


// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthPileupEventContainer_v1 , 1141476193 , 1 )


#endif
