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
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

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
      this,"trackContainer","InDetTrackParticles",
        "Key for the input track collection"};
    SG::ReadHandleKey< xAOD::VertexContainer > m_VertexContainerKey {
      this,"primaryVertexContainer","",
      "Key for the input vertex collection, (empty to use beamspot)"};

    SG::ReadHandleKey< xAOD::EventInfo > m_eventInfoKey {
      this, "eventInfo", "EventInfo", "Key for EventInfo"};

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

    // accessors for beam spot uncertainty
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_beam_sigma_x {
      this, "beamspotSigmaX", "EventInfo.beamPosSigmaX",
      "Beam spot position sigma in X"
    };
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_beam_sigma_y {
      this, "beamspotSigmaY", "EventInfo.beamPosSigmaY",
      "Beam spot position sigma in Y"
    };
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_beam_sigma_z {
      this, "beamspotSigmaZ", "EventInfo.beamPosSigmaZ",
      "Beam spot position sigma in Z"
    };
    // note that this last entry is a covariance: the units are mm^2,
    // whereas the above have units of mm
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_beam_cov_xy {
      this, "beamspotCovarianceXY", "EventInfo.beamPosSigmaXY",
      "Beam spot covariance in XY"
    };
  };

}

#endif
