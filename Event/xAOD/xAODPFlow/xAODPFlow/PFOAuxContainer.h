// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFOAuxContainer.h 744541 2016-05-03 15:55:30Z krasznaa $
#ifndef XAODPFLOW_PFOAUXCONTAINER_H
#define XAODPFLOW_PFOAUXCONTAINER_H

// Local include(s):
#include "xAODPFlow/versions/PFOAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current pfo auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::PFOContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef PFOAuxContainer_v1 PFOAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PFOAuxContainer , 1222686486 , 1 )

#endif // XAODPFLOW_PFOAUXCONTAINER_H
