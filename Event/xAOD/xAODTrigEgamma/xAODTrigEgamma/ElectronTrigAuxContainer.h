// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronTrigAuxContainer.h 691901 2015-08-28 12:05:25Z krasznaa $
#ifndef XAODTRIGEGAMMA_ELECTRONTRIGAUXCONTAINER_H
#define XAODTRIGEGAMMA_ELECTRONTRIGAUXCONTAINER_H

// Local include(s):
#include "xAODTrigEgamma/versions/ElectronTrigAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current electron auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::ElectronContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef ElectronTrigAuxContainer_v1 ElectronTrigAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::ElectronTrigAuxContainer, 1338372933, 1 )

#endif // XAODTRIGEGAMMA_ELECTRONTRIGAUXCONTAINER_H
