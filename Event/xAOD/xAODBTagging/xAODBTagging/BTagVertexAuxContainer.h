// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAuxContainer.h 569109 2013-11-07 14:59:42Z krasznaa $
#ifndef XAODBTAGGING_BTAGVERTEXGAUXCONTAINER_H
#define XAODBTAGGING_BTAGVERTEXGAUXCONTAINER_H

// Local include(s):
#include "xAODBTagging/versions/BTagVertexAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current BTagging auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::BTaggingContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef BTagVertexAuxContainer_v1 BTagVertexAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BTagVertexAuxContainer, 1083866246, 1 )

#endif // XAODBTAGGING_BTAGGINGAUXCONTAINER_H
