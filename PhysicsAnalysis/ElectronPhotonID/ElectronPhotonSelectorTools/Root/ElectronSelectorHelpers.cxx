/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <stdexcept>

std::size_t
ElectronSelectorHelpers::numberOfPixelHitsAndDeadSensors(
  const xAOD::TrackParticle& tp)
{

  uint8_t nPixHits(0);
  uint8_t nPixDead(0);

  bool allFound = true;
  allFound = allFound && tp.summaryValue(nPixHits, xAOD::numberOfPixelHits);
  allFound =
    allFound && tp.summaryValue(nPixDead, xAOD::numberOfPixelDeadSensors);
  if (!allFound) {
    throw std::runtime_error(
      "numberOfPixelHitsAndDeadSensors : Failed to load variables");
  }

  return nPixHits + nPixDead;
}

std::size_t
ElectronSelectorHelpers::numberOfSCTHitsAndDeadSensors(
  const xAOD::TrackParticle& tp)
{
  uint8_t nSCTHits(0);
  uint8_t nSCTDead(0);

  bool allFound = true;
  allFound = allFound && tp.summaryValue(nSCTHits, xAOD::numberOfSCTHits);
  allFound =
    allFound && tp.summaryValue(nSCTDead, xAOD::numberOfSCTDeadSensors);
  if (!allFound) {
    throw std::runtime_error(
      "numberOfSCTHitsAndDeadSensors : Failed to load variables");
  }

  return nSCTHits + nSCTDead;
}

std::size_t
ElectronSelectorHelpers::numberOfSiliconHitsAndDeadSensors(
  const xAOD::TrackParticle& tp)
{
  return numberOfPixelHitsAndDeadSensors(tp) +
         numberOfSCTHitsAndDeadSensors(tp);
}

bool
ElectronSelectorHelpers::passBLayerRequirement(const xAOD::TrackParticle& tp)
{

  uint8_t expectInnermostLayer(0);
  uint8_t nInnermostLayerHits(0);
  uint8_t nInnermostLayerOutliers(0);
  uint8_t expectNextToInnermostLayer(0);
  uint8_t nNextToInnermostLayerHits(0);
  uint8_t nNextToInnermostLayerOutliers(0);

  bool allFound = true;
  allFound =
    allFound && tp.summaryValue(expectInnermostLayer, xAOD::expectBLayerHit);
  allFound =
    allFound && tp.summaryValue(nInnermostLayerHits, xAOD::numberOfBLayerHits);
  allFound = allFound && tp.summaryValue(nInnermostLayerOutliers,
                                         xAOD::numberOfBLayerOutliers);
  allFound =
    allFound && tp.summaryValue(expectNextToInnermostLayer,
                                xAOD::expectNextToInnermostPixelLayerHit);
  allFound =
    allFound && tp.summaryValue(nNextToInnermostLayerHits,
                                xAOD::numberOfNextToInnermostPixelLayerHits);
  allFound = allFound &&
             tp.summaryValue(nNextToInnermostLayerOutliers,
                             xAOD::numberOfNextToInnermostPixelLayerOutliers);
  if (!allFound) {
    throw std::runtime_error(
      "passBLayerRequirement : Failed to load variables");
  }

  // If innermost and next to innermost layer are both masked off, then the
  // selectors should PASS the BL cut, so the default here is true
  bool passBLReq = true;

  if (expectInnermostLayer) {
    passBLReq = (nInnermostLayerHits + nInnermostLayerOutliers > 0);
  } else if (expectNextToInnermostLayer) {
    passBLReq = (nNextToInnermostLayerHits + nNextToInnermostLayerOutliers > 0);
  }

  return passBLReq;
}

bool
ElectronSelectorHelpers::passAmbiguity(xAOD::AmbiguityTool::AmbiguityType type,
                                       const uint16_t criterion)
{

  // helper to check if ambiguity type is one of several that are stored in a
  // bitmask
  //
  return (criterion & 0x1 << type);
}
