// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingContainer.h 569109 2013-11-07 14:59:42Z krasznaa $
#ifndef XAODBTAGGING_BTAGVERTEXCONTAINER_H
#define XAODBTAGGING_BTAGVERTEXCONTAINER_H

// Local include(s):
#include "xAODBTagging/BTagVertex.h"
#include "xAODBTagging/versions/BTagVertexContainer_v1.h"

namespace xAOD {
   /// Definition of the current "BTagging container version"
   typedef BTagVertexContainer_v1 BTagVertexContainer;
}

// Set up a CLID for the container for Athena
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BTagVertexContainer, 1158124735, 1 )

#endif // XAODBTAGGING_BTAGGINGCONTAINER_H
