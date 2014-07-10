// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAuxContainer.h 569109 2013-11-07 14:59:42Z krasznaa $
#ifndef XAODBTAGGING_BTAGGINGAUXCONTAINER_H
#define XAODBTAGGING_BTAGGINGAUXCONTAINER_H

// Local include(s):
#include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current BTagging auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::BTaggingContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef BTaggingAuxContainer_v1 BTaggingAuxContainer;
}

#endif // XAODBTAGGING_BTAGGINGAUXCONTAINER_H
