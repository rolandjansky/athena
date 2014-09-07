// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonContainer_v1.h 615077 2014-09-04 13:42:41Z schaffer $
#ifndef XAODEGAMMA_VERSIONS_PHOTONCONTAINER_V1_H
#define XAODEGAMMA_VERSIONS_PHOTONCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODEgamma/versions/Photon_v1.h"
#include "xAODEgamma/versions/EgammaContainer_v1.h"

// Declare the inheritance of Photon_v1:
DATAVECTOR_BASE( xAOD::Photon_v1, xAOD::Egamma_v1 );

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::Photon_v1 > PhotonContainer_v1;
}

#endif // XAODEGAMMA_VERSIONS_PHOTONCONTAINER_V1_H
