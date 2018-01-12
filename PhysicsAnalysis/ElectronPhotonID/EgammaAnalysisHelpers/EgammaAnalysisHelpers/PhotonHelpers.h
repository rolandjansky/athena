/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __PHOTONHELPERS__
#define __PHOTONHELPERS__

// Atlas includes
#include "xAODEgamma/PhotonFwd.h"
#include <cstddef>

namespace PhotonHelpers{
  ///@brief Helper to ease the implemmantation of the pass Quality requirements
  bool passOQquality(const xAOD::Photon *ph);
  ///@brief Helpers to ease the implementation of the pass Quality requirements
  bool passOQqualityDelayed(const xAOD::Photon *ph);
}

#endif
