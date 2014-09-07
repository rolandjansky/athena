// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonAuxContainer.h 614853 2014-09-03 11:40:46Z krasznaa $
#ifndef XAODEGAMMA_PHOTONAUXCONTAINER_H
#define XAODEGAMMA_PHOTONAUXCONTAINER_H

// Local include(s):
#include "xAODEgamma/versions/PhotonAuxContainer_v2.h"

namespace xAOD {
   /// Definition of the current photon auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::PhotonContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef PhotonAuxContainer_v2 PhotonAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PhotonAuxContainer, 1250139444, 1 )

#endif // XAODEGAMMA_PHOTONAUXCONTAINER_H
