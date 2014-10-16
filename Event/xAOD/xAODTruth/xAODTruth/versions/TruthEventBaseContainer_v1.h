// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEventBaseContainer_v1.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_VERSIONS_TRUTHEVENTBASECONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHEVENTBASECONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTruth/versions/TruthEventBase_v1.h"

namespace xAOD {
   /// Alias
   typedef DataVector< TruthEventBase_v1 > TruthEventBaseContainer_v1;
}

#endif // XAODTRUTH_VERSIONS_TRUTHEVENTBASECONTAINER_V1_H
