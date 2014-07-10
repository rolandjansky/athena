// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingContainer_v1.h 569109 2013-11-07 14:59:42Z krasznaa $
#ifndef XAODBTAGGING_VERSIONS_BTAGGINGCONTAINER_V1_H
#define XAODBTAGGING_VERSIONS_BTAGGINGCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODBTagging/versions/BTagging_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::BTagging_v1 > BTaggingContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::BTaggingContainer_v1, 1264356158, 1 )
#endif // XAOD_STANDALONE

#endif // XAODBTAGGING_VERSIONS_BTAGGINGCONTAINER_V1_H
