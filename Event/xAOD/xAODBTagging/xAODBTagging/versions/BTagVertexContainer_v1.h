// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTagVertexContainer_v1.h 569109 2013-11-07 14:59:42Z krasznaa $
#ifndef XAODBTAGGING_VERSIONS_BTAGVERTEXCONTAINER_V1_H
#define XAODBTAGGING_VERSIONS_BTAGVERTEXCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODBTagging/versions/BTagVertex_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::BTagVertex_v1 > BTagVertexContainer_v1;
}

#endif // XAODBTAGGING_VERSIONS_BTAGGINGCONTAINER_V1_H
