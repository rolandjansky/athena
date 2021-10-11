/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOR_MANS_TRACK_AUGMENTER_ALG_HH
#define POOR_MANS_TRACK_AUGMENTER_ALG_HH

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/AuxElement.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "StoreGate/WriteDecorHandle.h"

namespace FlavorTagDiscriminants {


  class PoorMansIpAugmenterAlg: public AthReentrantAlgorithm {
  public:
    PoorMansIpAugmenterAlg(const std::string& name,
                          ISvcLocator* pSvcLocator );

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ) const override;
    virtual StatusCode finalize() override;


  private:
    const xAOD::Vertex* getPrimaryVertex( const xAOD::VertexContainer& ) const;

  private:

    // Input Containers
    SG::ReadHandleKey< xAOD::TrackParticleContainer > m_TrackContainerKey {
      this,"TrackContainer","InDetTrackParticles",
        "Key for the input track collection"};
    SG::ReadHandleKey< xAOD::VertexContainer > m_VertexContainerKey {
      this,"PrimaryVertexContainer","PrimaryVertices",
      "Key for the input vertex collection"};

    // Decorators for tracks
    Gaudi::Property< std::string > m_prefix{this,"prefix","poboyIp_",""};

    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_d0_sigma {
      this, "d0Uncertainty", "d0Uncertainty", "d0Uncertainty of tracks"};
    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_z0_sigma {
      this, "z0SinThetaUncertainty", "z0SinThetaUncertainty",
      "z0SinThetaUncertainty of tracks"};

    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_track_pos {
      this, "trackDisplacement","trackDisplacement",
      "trackDisplacement of tracks" };
    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_track_mom {
      this, "trackMomentum","trackMomentum","trackMomentum of tracks" };
  };

}

#endif
