// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingContainer_v1.h 631475 2014-11-27 11:53:05Z filthaut $
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

#endif // XAODBTAGGING_VERSIONS_BTAGGINGCONTAINER_V1_H
