// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonContainer.h 615077 2014-09-04 13:42:41Z schaffer $
#ifndef XAODEGAMMA_PHOTONCONTAINER_H
#define XAODEGAMMA_PHOTONCONTAINER_H

// Local include(s):
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/versions/PhotonContainer_v1.h"

namespace xAOD {
   /// Definition of the current "photon container version"
   typedef PhotonContainer_v1 PhotonContainer;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::PhotonContainer , 1105575213 , 1 )
#endif // XAOD_STANDALONE

#endif // XAODEGAMMA_PHOTONCONTAINER_H
