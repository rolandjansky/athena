/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_TRACK_AUGMENTER_ALG_HH
#define BTAGGING_TRACK_AUGMENTER_ALG_HH

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/AuxElement.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "StoreGate/WriteDecorHandle.h"

namespace Analysis {


  class BTagTrackAugmenterAlg: public AthAlgorithm {
  public:
    BTagTrackAugmenterAlg(const std::string& name,
                          ISvcLocator* pSvcLocator );

    StatusCode initialize() override final;
    StatusCode execute() override final;

  private:
    const xAOD::Vertex* getPrimaryVertex( const xAOD::VertexContainer& ) const;

  private:
    ToolHandle< Trk::ITrackToVertexIPEstimator > m_track_to_vx {this,"TrackToVertexIPEstimator","Trk::TrackToVertexIPEstimator",""};
    ToolHandle< Trk::IExtrapolator >  m_extrapolator {this,"Extrapolator","Trk::Extrapolator",""};

    // Input Containers
    SG::ReadHandleKey< xAOD::TrackParticleContainer > m_TrackContainerKey {this,"TrackContainer","InDetTrackParticles","Key for the input track collection"};
    SG::ReadHandleKey< xAOD::VertexContainer > m_VertexContainerKey {this,"PrimaryVertexContainer","PrimaryVertices","Key for the input vertex collection"};

    // Decorators for tracks
    // Decorator keys will be modified at run-time to conform to the correct container name
    // For the run-time update to work, the decoration key name properties must start with a period (".")
    Gaudi::Property< std::string > m_prefix{this,"prefix","btagIp_",""};

    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_d0 {this, "d0", "d0", "d0 of tracks"};
    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_z0 {this, "z0SinTheta", "z0SinTheta", "z0SinTheta of tracks"};
    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_d0_sigma {this, "d0Uncertainty", "d0Uncertainty", "d0Uncertainty of tracks"};
    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_z0_sigma {this, "z0SinThetaUncertainty", "z0SinThetaUncertainty", "z0SinThetaUncertainty of tracks"};

    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_track_pos {this, "trackDisplacement","trackDisplacement","trackDisplacement of tracks" };
    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_track_mom {this, "trackMomentum","trackMomentum","trackMomentum of tracks" };

    SG::WriteDecorHandleKey< xAOD::TrackParticleContainer > m_dec_invalid {
      this, "invalidIp", "invalidIp", "flag for invalid impact parameter"
    };
  };

}

#endif
