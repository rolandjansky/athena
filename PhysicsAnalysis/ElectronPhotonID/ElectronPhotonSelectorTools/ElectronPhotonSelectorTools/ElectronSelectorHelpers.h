/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ELECTRONSELECTORHELPERS__
#define __ELECTRONSELECTORHELPERS__

// Atlas includes
#include "xAODEgamma/ElectronFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include <cstddef>

#include "EgammaAnalysisInterfaces/IEGammaAmbiguityTool.h"

namespace ElectronSelectorHelpers{
  ///@brief return the number of Pixel hits plus dead sensors in the track particle
  std::size_t numberOfPixelHitsAndDeadSensors(const xAOD::TrackParticle *tp);

  ///@brief return the number of SCT hits plus dead sensors in the track particle
  std::size_t numberOfSCTHitsAndDeadSensors(const xAOD::TrackParticle *tp);

  ///@brief return the number of Silicon hits plus dead sensors in the track particle
  std::size_t numberOfSiliconHitsAndDeadSensors(const xAOD::TrackParticle *tp);

  ///@brief return true if effective number of BL hits + outliers is at least one
  //        (that is, if innermost layer is masked off, look at the next
  //        to innermost layer. If both are off, then return true)
  bool passBLayerRequirement(const xAOD::TrackParticle *tp);

  ///@brief return true if the ambiguity type is one of several that are stored in a bitmask
  bool passAmbiguity(xAOD::AmbiguityTool::AmbiguityType type, const uint8_t criterion);

  struct ROOT6_NamespaceAutoloadHook{};
}

#endif
