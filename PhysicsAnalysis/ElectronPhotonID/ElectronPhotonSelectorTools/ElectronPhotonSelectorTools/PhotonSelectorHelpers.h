/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __PHOTONSELECTORHELPERS__
#define __PHOTONSELECTORHELPERS__

// Atlas includes
#include "xAODEgamma/Photon.h"
#include <cstddef>

namespace PhotonSelectorHelpers{
  ///@brief return the number of Pixel hits plus dead sensors in the track particle
  bool passOQquality(const xAOD::Photon *ph);
  bool passOQqualitydelayed(const xAOD::Photon *ph);
  
}

#endif
