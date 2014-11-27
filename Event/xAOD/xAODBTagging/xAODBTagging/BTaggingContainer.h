// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingContainer.h 631475 2014-11-27 11:53:05Z filthaut $
#ifndef XAODBTAGGING_BTAGGINGCONTAINER_H
#define XAODBTAGGING_BTAGGINGCONTAINER_H

// Local include(s):
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/versions/BTaggingContainer_v1.h"

namespace xAOD {
   /// Definition of the current "BTagging container version"
   typedef BTaggingContainer_v1 BTaggingContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BTaggingContainer, 1264356158, 1 )

#endif // XAODBTAGGING_BTAGGINGCONTAINER_H
