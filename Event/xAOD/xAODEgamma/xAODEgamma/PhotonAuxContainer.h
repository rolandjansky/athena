// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonAuxContainer.h 619882 2014-10-03 14:57:46Z christos $
#ifndef XAODEGAMMA_PHOTONAUXCONTAINER_H
#define XAODEGAMMA_PHOTONAUXCONTAINER_H

// Local include(s):
#include "xAODEgamma/versions/PhotonAuxContainer_v3.h"

namespace xAOD {
   /// Definition of the current photon auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::PhotonContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef PhotonAuxContainer_v3 PhotonAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PhotonAuxContainer, 1250139444, 1 )

#endif // XAODEGAMMA_PHOTONAUXCONTAINER_H
