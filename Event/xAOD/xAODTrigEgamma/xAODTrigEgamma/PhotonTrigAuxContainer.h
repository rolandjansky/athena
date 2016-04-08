// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonTrigAuxContainer.h 691901 2015-08-28 12:05:25Z krasznaa $
#ifndef XAODTRIGEGAMMA_PHOTONTRIGAUXCONTAINER_H
#define XAODTRIGEGAMMA_PHOTONTRIGAUXCONTAINER_H

// Local include(s):
#include "xAODTrigEgamma/versions/PhotonTrigAuxContainer_v1.h"

namespace xAOD {
   /// Definition of the current photon auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::PhotonContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef PhotonTrigAuxContainer_v1 PhotonTrigAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PhotonTrigAuxContainer, 1270619095, 1 )

#endif // XAODTRIGEGAMMA_PHOTONTRIGAUXCONTAINER_H
