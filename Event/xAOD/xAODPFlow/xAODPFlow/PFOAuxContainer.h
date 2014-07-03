// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFOAuxContainer.h 582152 2014-02-07 13:09:21Z krasznaa $
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

#endif // XAODPFLOW_PFOAUXCONTAINER_H
