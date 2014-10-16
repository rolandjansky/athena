// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEventContainer_v1.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_VERSIONS_TRUTHEVENTCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHEVENTCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTruth/versions/TruthEvent_v1.h"

// Declare the base class of TruthEvent_v1 for DataVector
DATAVECTOR_BASE( xAOD::TruthEvent_v1, xAOD::TruthEventBase_v1 );

namespace xAOD {
   /// Alias
   typedef DataVector< TruthEvent_v1 > TruthEventContainer_v1;
}

#endif // XAODTRUTH_VERSIONS_TRUTHEVENTCONTAINER_V1_H
