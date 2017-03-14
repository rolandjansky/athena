/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValDecoratorAlg.cxx
 * implementation file for class of same name
 * @author shaun roe
 * @date 25 January 2015
 **/

#include "InDetPhysValMonitoring/InDetPhysValDecoratorAlg.h"
#include "ParameterErrDecoratorTool.h"
#include "InDetPhysHitDecoratorTool.h"
#include "TruthClassDecoratorTool.h"
#include "InDetPhysValTruthDecoratorTool.h"
// containers
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"


namespace { // utility functions used here
  // get truth particle associated with a track particle
  const xAOD::TruthParticle*
  getTruthPtr(const xAOD::TrackParticle& trackParticle) {
    typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
    const xAOD::TruthParticle* result(nullptr);
    // 0. is there any truth?
    if (trackParticle.isAvailable<ElementTruthLink_t>("truthParticleLink")) {
      // 1. ..then get link
      const ElementTruthLink_t ptruthContainer = trackParticle.auxdata<ElementTruthLink_t>("truthParticleLink");
      if (ptruthContainer.isValid()) {
        result = *ptruthContainer;
      }
    }
    return result;
  }
}


InDetPhysValDecoratorAlg::InDetPhysValDecoratorAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(
    name, pSvcLocator),
  m_truthDecoratorTool("InDetPhysValTruthDecoratorTool"),
  m_hitDecoratorTool("InDetPhysHitDecoratorTool"),
  m_errDecoratorTool("ParameterErrDecoratorTool"),
  m_truthClassDecoratorTool("TruthClassDecoratorTool") {
  declareProperty("TrackParticleContainerName", m_trkParticleName = "InDetTrackParticles");
  declareProperty("TruthParticleContainerName", m_truthParticleName = "TruthParticles");

  declareProperty("InDetPhysValTruthDecoratorTool", m_truthDecoratorTool);
  declareProperty("InDetPhysHitDecoratorTool", m_hitDecoratorTool);
  declareProperty("ParameterErrDecoratorTool", m_errDecoratorTool);
  declareProperty("TruthClassDecoratorTool", m_truthClassDecoratorTool);
}

InDetPhysValDecoratorAlg::~InDetPhysValDecoratorAlg() {
  // nop;
}

StatusCode
InDetPhysValDecoratorAlg::initialize() {
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(AthAlgorithm::initialize());
  ATH_CHECK(m_truthDecoratorTool.retrieve());
  ATH_CHECK(m_hitDecoratorTool.retrieve());
  ATH_CHECK(m_errDecoratorTool.retrieve());
  ATH_CHECK(m_truthClassDecoratorTool.retrieve());
  return StatusCode::SUCCESS;
}

// retrieve containers and decorate
StatusCode
InDetPhysValDecoratorAlg::execute() {
  // retrieve trackParticle container
  auto ptracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);

  if ((!ptracks)) {
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("Trackparticle container was retrieved");
  //
  // Loop over reconstructed tracks
  const unsigned int nTracks(ptracks->size());
  // const unsigned int nTruth(ptruth->size());
  //
  unsigned int num_truthmatch_match(0);
  for (const auto& thisTrack: *ptracks) {
    const xAOD::TruthParticle* associatedTruth = getTruthPtr(*thisTrack); // get the associated truth
    // decorate the track here, if necessary <insert code>
    // running hit decorator on slimmed tracks crashes
    bool successfulTrackDecoration = m_hitDecoratorTool->decorateTrack(*thisTrack, "");
    // if (not successfulTrackDecoration) ATH_MSG_DEBUG ("Could not get hit info for the track particle.");
    successfulTrackDecoration = m_errDecoratorTool->decorateTrack(*thisTrack, "");
    if (not successfulTrackDecoration) {
      ATH_MSG_WARNING("Could not get err info for the track particle.");
    }
    //
    if (associatedTruth) {
      ++num_truthmatch_match;
      /**
       * Decorate the particles with information
       **/
      // bool successfulDecoration = m_truthDecoratorTool->decorateTruth(*associatedTruth,"");
      // if (not successfulDecoration) ATH_MSG_WARNING ("Could not retrieve some information for the truth particle.");
      // successfulDecoration = m_truthClassDecoratorTool->decorateTruth(*associatedTruth,"");
      // if (not successfulDecoration) ATH_MSG_WARNING ("The truth particle could not be assigned a type");
    }
  }
  const xAOD::TruthParticleContainer* truthParticles =
    (!m_truthParticleName.empty() ? getContainer<xAOD::TruthParticleContainer>(m_truthParticleName) : nullptr);
  if (truthParticles) {
    for (const auto& thisTruth: *truthParticles) {
      bool successfulDecoration = m_truthDecoratorTool->decorateTruth(*thisTruth, "");
      if (not successfulDecoration) {
        ATH_MSG_DEBUG("Could not retrieve some information for the truth particle.");
      }
    }
  }

  ATH_MSG_VERBOSE(nTracks << " tracks were retrieved; " << num_truthmatch_match << " had associated truth.");
  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysValDecoratorAlg::finalize() {
  return StatusCode::SUCCESS;
}
