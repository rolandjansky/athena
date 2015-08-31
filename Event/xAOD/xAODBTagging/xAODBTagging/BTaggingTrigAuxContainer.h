// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingTrigAuxContainer.h 688384 2015-08-10 11:21:34Z kleney $
#ifndef XAODBTAGGING_BTAGGINGTRIGAUXCONTAINER_H
#define XAODBTAGGING_BTAGGINGTRIGAUXCONTAINER_H

// Local include(s):
#include "xAODBTagging/versions/BTaggingTrigAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current BTagging auxiliary container for trigger
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::BTaggingContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef BTaggingTrigAuxContainer_v1 BTaggingTrigAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BTaggingTrigAuxContainer, 1090666328, 1 ) 

#endif // XAODBTAGGING_BTAGGINGTRIGAUXCONTAINER_H
