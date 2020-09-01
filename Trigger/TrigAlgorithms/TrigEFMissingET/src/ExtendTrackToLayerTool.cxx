/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEFMissingET/ExtendTrackToLayerTool.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"

ExtendTrackToLayerTool::ExtendTrackToLayerTool(const std::string &name) : asg::AsgTool(name)
{
}

StatusCode ExtendTrackToLayerTool::initialize()
{
  ATH_MSG_DEBUG("Initialize " << name());
  CHECK(m_extensionTool.retrieve());

  if (m_caloLayerNames.value().size() == 0)
  {
    ATH_MSG_ERROR("No layers requested, tool would not do anything!");
    return StatusCode::FAILURE;
  }

  std::map<std::string, CaloSampling::CaloSample> samplings;
  for (unsigned int ii = 0; ii < CaloSampling::getNumberOfSamplings(); ++ii)
    samplings[CaloSampling::getSamplingName(ii)] = static_cast<CaloSampling::CaloSample>(ii);
  m_caloLayers.reserve(m_caloLayerNames.value().size());
  for (const std::string &name : m_caloLayerNames.value())
  {
    auto itr = samplings.find(name);
    if (itr == samplings.end())
    {
      ATH_MSG_ERROR("Unknown sampling '" << name << "' requested!");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

IExtendTrackToLayerTool::TrackExtension ExtendTrackToLayerTool::extendTrack(
    const EventContext &ctx,
    const xAOD::TrackParticle &track) const
{
  std::unique_ptr<Trk::CaloExtension> extension = m_extensionTool->caloExtension(
      ctx, track);
  if (!extension)
    // This means that track wasn't extrapolated into the calorimeter
    return TrackExtension{};
  // Get the eta-phi entry positions of the intersections of this track with the
  // calorimeter
  CaloExtensionHelpers::EtaPhiHashLookupVector result;
  CaloExtensionHelpers::entryEtaPhiHashLookupVector(*extension, result);
  // Go through the layers we requested in order
  bool isCrossed;
  double eta;
  double phi;
  for (CaloSampling::CaloSample layer : m_caloLayers)
  {
    std::tie(isCrossed, eta, phi) = result[layer];
    // Return the first that is crossed
    if (isCrossed)
      return TrackExtension{eta, phi};
  }
  // If we got here, it crossed no layer that we're interested in, return an
  // invalid extension
  return TrackExtension{};
}
