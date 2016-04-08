/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDAnalysis/src/D3PDMCTruthClassifier.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2012
 * @brief Work around MCTruthClassifier brain-damage.
 *
 * This avoids having to specify the container names during configuration.
 */


#include "D3PDMCTruthClassifier.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "AthenaKernel/errorcheck.h"

#include "Particle/TrackParticle.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODJet/Jet.h"
#include "VxVertex/VxTrackAtVertex.h"

#include <vector>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
D3PDMCTruthClassifier::D3PDMCTruthClassifier (const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
  : MCTruthClassifier (type, name, parent),
    m_sg ("StoreGateSvc", name)
{
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode D3PDMCTruthClassifier::initialize()
{
  CHECK( MCTruthClassifier::initialize() );
  CHECK( m_sg.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Run the classifier for an electron.
 * @param el The input electron.
 */
std::pair<MCTruthPartClassifier::ParticleType,
          MCTruthPartClassifier::ParticleOrigin>
D3PDMCTruthClassifier::particleTruthClassifier (const xAOD::Electron* el)
{
  // Try to find the name of the truth map container,
  // given the name of the TrackParticleContainer.
  bool got_names = false;
  std::string tp_truth_name, tp_name;
  if (el->trackParticleLink().isValid()) {
    tp_name = el->trackParticleLink().dataID();
    got_names = get_tptruth_name (tp_name, tp_truth_name);
  }

  // If we found the truth map container, diddle the properties
  // for the base tool (remembering the original values).
  std::string old_tp_truth_name, old_tp_name;
  if (got_names) {
    getProperty1 ("TrackParticleTruthCollection", old_tp_truth_name);
    getProperty1 ("TrackParticleContainerName", old_tp_name);
    setProperty1 ("TrackParticleTruthCollection", tp_truth_name);
    setProperty1 ("TrackParticleContainerName", tp_name);
  }

  // Call the base tool.
  std::pair<MCTruthPartClassifier::ParticleType,
            MCTruthPartClassifier::ParticleOrigin> ret = 
    MCTruthClassifier::particleTruthClassifier (el);

  // Restore original properties, if we changed them.
  if (got_names) {
    setProperty1 ("TrackParticleTruthCollection", old_tp_truth_name);
    setProperty1 ("TrackParticleContainerName", old_tp_name);
  }

  return ret;
}


/**
 * @brief Run the classifier for a photon.
 * @param el The input photon.
 */
std::pair<MCTruthPartClassifier::ParticleType,
          MCTruthPartClassifier::ParticleOrigin>
D3PDMCTruthClassifier::particleTruthClassifier (const xAOD::Photon* ph)
{
  // Try to find the name of the truth map container,
  // given the name of the TrackParticleContainer.
  bool got_names = false;
  std::string tp_truth_name, tp_name;
  const xAOD::Vertex* vx = ph->vertex();
  if (vx) {
    const xAOD::Vertex::TrackParticleLinks_t links = vx->trackParticleLinks();
    for (auto trklink : links) {
      if (trklink.isValid()) {
        tp_name = trklink.dataID();
        got_names = get_tptruth_name (tp_name, tp_truth_name);
        if (got_names) break;
      }
    }
  }

  // If we found the truth map container, diddle the properties
  // for the base tool (remembering the original values).
  std::string old_tp_truth_name, old_tp_name;
  if (got_names) {
    getProperty1 ("TrackParticleTruthCollection", old_tp_truth_name);
    getProperty1 ("TrackParticleContainerName", old_tp_name);
    setProperty1 ("TrackParticleTruthCollection", tp_truth_name);
    setProperty1 ("TrackParticleContainerName", tp_name);
  }

  // Call the base tool.
  std::pair<MCTruthPartClassifier::ParticleType,
            MCTruthPartClassifier::ParticleOrigin> ret = 
    MCTruthClassifier::particleTruthClassifier (ph);

  // Restore original properties, if we changed them.
  if (got_names) {
    setProperty1 ("TrackParticleTruthCollection", old_tp_truth_name);
    setProperty1 ("TrackParticleContainerName", old_tp_name);
  }

  return ret;
}


/**
 * This function is here to avoid a problem on the mc12 validation samples.
 */
std::pair<MCTruthPartClassifier::ParticleType,
          MCTruthPartClassifier::ParticleOrigin>
D3PDMCTruthClassifier::particleTruthClassifier (const xAOD::Muon* mu)
{
  // Avoid using forward muons that have a tracklet associated to them:
  if( mu->muonType() == xAOD::Muon::SiliconAssociatedForwardMuon ) {
    return std::make_pair( MCTruthPartClassifier::Unknown, MCTruthPartClassifier::NonDefined );
  }

  // For all other muons just use the base class's function:
  return MCTruthClassifier::particleTruthClassifier( mu );
}


/**
 * @brief Try to find a TrackParticleTruthCollection pointing at TP_NAME.
 * @param tp_name The name of a TrackParticleContainer.
 * @param tptruth_name[out] The name of a TrackParticleTruthCollection
 *                          pointing at it.
 * @returns True if we found a good truth collection.
 */
bool D3PDMCTruthClassifier::get_tptruth_name (const std::string& tp_name,
                                              std::string& tptruth_name)
{
  // Check all TrackParticleTruthCollections to try to find one
  // pointing at the desired TrackParticle container.
  std::vector<std::string> keys;
  m_sg->keys<TrackParticleTruthCollection> (keys);

  for (size_t i = 0; i < keys.size(); i++) {
    const TrackParticleTruthCollection* tptc = 0;
    if (m_sg->retrieve (tptc, keys[i]).isFailure() || !tptc)
    {
      MsgStream mlog(  msgSvc() , name() );
      mlog << MSG::WARNING
           << "Can't retrieve TrackParticleTruthCollection/"
           << keys[i] << endreq;
      continue;
    }
    tptruth_name = keys[i];

    std::string target_tp_name;
    if (tptc->trackParticleContainerLink().isDefault()) {
      // Older files didn't have this link.  Hardcode this case.
      target_tp_name = "TrackParticleCandidate";
    }
    else if (!tptc->trackParticleContainerLink().isValid())
      continue;
    else
      target_tp_name = tptc->trackParticleContainerLink().dataID();

    if (tp_name == target_tp_name)
      return true;
  }
  return false;
}


/**
 * @brief Helper to retrieve the value of a Gaudi property.
 * @param pname The name of the property.
 * param value[out] The value of the property.
 */
void D3PDMCTruthClassifier::getProperty1 (const std::string& pname,
                                          std::string& value)
{
  StatusCode sc = getProperty (pname, value);
  if (sc.isFailure())
    REPORT_ERROR (sc) << "Can't get property " << pname << "\n";
}


/**
 * @brief Helper to set the value of a Gaudi property.
 * @param pname The name of the property.
 * param value The new value of the property.
 */
void D3PDMCTruthClassifier::setProperty1 (const std::string& pname,
                                          const std::string& value)
{
  StatusCode sc = setProperty (pname, value);
  if (sc.isFailure())
    REPORT_ERROR (sc) << "Can't set property " << pname << "\n";
}


} // namespace D3PD
