/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCTExtensionAlg_H
#define SCTExtensionAlg_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTracking/TrackParticleContainer.h"

class SCTExtensionAlg:public AthReentrantAlgorithm {
 public:
  SCTExtensionAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
 private:
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_pixelTracklets{this, "pixelTracklets", "InDetPixelTrackParticles"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracks{this, "tracks", "InDetTrackParticles"};
};

#endif
