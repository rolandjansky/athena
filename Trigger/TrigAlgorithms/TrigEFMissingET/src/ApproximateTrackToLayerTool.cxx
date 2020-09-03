/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEFMissingET/ApproximateTrackToLayerTool.h"

ApproximateTrackToLayerTool::ApproximateTrackToLayerTool(const std::string &name) : asg::AsgTool(name)
{
}

IExtendTrackToLayerTool::TrackExtension ApproximateTrackToLayerTool::extendTrack(
    const EventContext &,
    const xAOD::TrackParticle &track) const
{
  // The approximation doesn't affect track etas at all
  double eta = track.eta();
  double sinDPhi = (m_trackExtrapolationLinear.value() +
                    m_trackExtrapolationQuadratic.value() * eta*eta +
                    m_trackExtrapolationQuartic.value() * eta*eta*eta*eta) /
                   (track.pt() * track.charge());

  if (std::abs(sinDPhi) > 1.0)
    // The track was extrapolated outside of acceptance
    return TrackExtension{};
  else
    return TrackExtension{track.eta(), track.phi() - std::asin(sinDPhi)};
}
