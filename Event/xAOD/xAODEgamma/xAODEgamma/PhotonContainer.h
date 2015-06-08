// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonContainer.h 652975 2015-03-10 14:14:13Z christos $
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
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PhotonContainer , 1105575213 , 1 )

#endif // XAODEGAMMA_PHOTONCONTAINER_H
