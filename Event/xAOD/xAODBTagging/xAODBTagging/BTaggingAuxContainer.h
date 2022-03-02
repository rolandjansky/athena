// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAuxContainer.h 631475 2014-11-27 11:53:05Z filthaut $
#ifndef XAODBTAGGING_BTAGGINGAUXCONTAINER_H
#define XAODBTAGGING_BTAGGINGAUXCONTAINER_H

// Local include(s):
#include "xAODBTagging/versions/BTaggingAuxContainer_v2.h"

namespace xAOD {
   /// Definition of the current BTagging auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::BTaggingContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef BTaggingAuxContainer_v2 BTaggingAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BTaggingAuxContainer, 1243532533, 1 )

#endif // XAODBTAGGING_BTAGGINGAUXCONTAINER_H
