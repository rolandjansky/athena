// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronAuxContainer.h 619882 2014-10-03 14:57:46Z christos $
#ifndef XAODEGAMMA_ELECTRONAUXCONTAINER_H
#define XAODEGAMMA_ELECTRONAUXCONTAINER_H

// Local include(s):
#include "xAODEgamma/versions/ElectronAuxContainer_v3.h"

namespace xAOD {
   /// Definition of the current electron auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::ElectronContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef ElectronAuxContainer_v3 ElectronAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::ElectronAuxContainer, 1115737378, 1 )

#endif // XAODEGAMMA_ELECTRONAUXCONTAINER_H
