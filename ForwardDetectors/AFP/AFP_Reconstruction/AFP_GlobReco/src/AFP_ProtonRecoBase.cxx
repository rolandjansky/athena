/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_GlobReco/AFP_ProtonRecoBase.h"

AFP_ProtonRecoBase::AFP_ProtonRecoBase (const std::string &type, const std::string &name, const IInterface *parent)
 : base_class(type, name, parent)
{
  ATH_MSG_DEBUG("in AFP_ProtonRecoBase constructor");
}


StatusCode AFP_ProtonRecoBase::doProtonReco(std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer, const EventContext& ctx) const 
{
  
  SG::ReadHandle<xAOD::AFPTrackContainer> trackContainer( m_trackContainerKey, ctx );
  if(!trackContainer.isValid())
  {
    // this is allowed, there might be no AFP data in the input
    return StatusCode::SUCCESS;
  }
  
  const double trackDistanceRadiusSq = m_trackDistance*m_trackDistance;

  // Select tracks in near station
  std::vector<const xAOD::AFPTrack*> trackNearContainer;
  const int nearId = m_side + 1;
  std::copy_if(trackContainer->begin(), trackContainer->end(), std::back_inserter(trackNearContainer),
      [&nearId](auto track) { return track->stationID() == nearId; });

  // Select tracks in far station
  std::vector<const xAOD::AFPTrack*> trackFarContainer;
  const int farId = 3 * m_side;
  std::copy_if(trackContainer->begin(), trackContainer->end(), std::back_inserter(trackFarContainer),
      [&farId](auto track) { return track->stationID() == farId; });

  ATH_MSG_DEBUG("trackNearContainer size: " << trackNearContainer.size()<<", side "<<m_side);
  ATH_MSG_DEBUG("trackFarContainer  size: " << trackFarContainer.size()<<", side "<<m_side);

  // Loop over both containers
  for (const xAOD::AFPTrack* trackFar : trackFarContainer) {
    bool foundMatchingTrack = false;

    for (const xAOD::AFPTrack* trackNear : trackNearContainer) {
      // Apply cuts
      const double dx = trackFar->xLocal() - trackNear->xLocal();
      const double dy = trackFar->yLocal() - trackNear->yLocal();
      const double r2 = dx*dx + dy*dy;

      if (r2 > trackDistanceRadiusSq) {
        ATH_MSG_DEBUG(
            "Tracks too far away from each other (xNear, yNear; xFar, yFar; distance) [mm]: "
            << trackNear->xLocal() << ", " << trackNear->yLocal() << "; "
            << trackFar->xLocal()  << ", " << trackFar->yLocal()  << "; " << r2);

        continue;
      }

      // Reconstruct proton and add it to the container
      xAOD::AFPProton * proton = reco(trackNear, trackFar, outputContainer);

      if (!proton)
        continue;

      foundMatchingTrack = true;

      // Create link to tracks
      linkTracksToProton(trackNear, trackContainer, proton);
      linkTracksToProton(trackFar, trackContainer, proton);
    }

    // Reconstuct proton using only FAR station if
    // no matching track on NEAR station was found
    if (m_allowSingleStationReco and !foundMatchingTrack) {
      // Apply cuts
      // none

      xAOD::AFPProton * proton = reco(trackFar, outputContainer);

      if (!proton)
        continue;

      linkTracksToProton(trackFar, trackContainer, proton);
    }
  }
  
  return StatusCode::SUCCESS;
}


xAOD::AFPProton * AFP_ProtonRecoBase::createProton(const Momentum& momentum, const Measurement& my_measAFP, const int algID, std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer) const 
{

  // Return nullptr if any of momentum components is not a number
  if ( std::any_of(begin(momentum), end(momentum), [](auto& el) { return !isfinite(el); }) )
    return nullptr;

  const auto [px, py, pz] = momentum;

  auto * proton = outputContainer->push_back(std::make_unique<xAOD::AFPProton>());
  
  // Set proton properties
  constexpr double protonMass = 0.938; // in GeV
  
  proton->setPxPyPzE(px, py, pz, sqrt(px*px + py*py + pz*pz + protonMass*protonMass));
  proton->setChi2(chi2(px, py, pz, my_measAFP));
  proton->setSide(m_side);
  proton->setMethodID(algID);
  
  ATH_MSG_DEBUG("Reconstructed proton (px, py, pz): " << proton->px() << ", " << proton->py() << ", " << proton->pz()<<", chi2 "<<proton->chi2()<<", side "<<proton->side());

  return proton;
}


void AFP_ProtonRecoBase::linkTracksToProton
  (const xAOD::AFPTrack* track, SG::ReadHandle<xAOD::AFPTrackContainer>& trackContainer, xAOD::AFPProton * proton) const {

  ElementLink<xAOD::AFPTrackContainer> trackLink;

  trackLink.toContainedElement(*trackContainer, track);
  proton->addAFPTrackLink(trackLink);
}