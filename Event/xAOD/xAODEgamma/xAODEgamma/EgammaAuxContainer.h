// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id $
#ifndef XAODEGAMMA_EGAMMAAUXCONTAINER_H
#define XAODEGAMMA_EGAMMAAUXCONTAINER_H

// Local include(s):
#include "xAODEgamma/versions/EgammaAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current egamma auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::EgammaContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef EgammaAuxContainer_v1 EgammaAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EgammaAuxContainer, 1239597298 , 1 )

#endif // XAODEGAMMA_EGAMMAAUXCONTAINER_H
