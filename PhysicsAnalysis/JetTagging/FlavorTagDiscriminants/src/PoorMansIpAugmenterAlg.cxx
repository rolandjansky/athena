/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "FlavorTagDiscriminants/PoorMansIpAugmenterAlg.h"

#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadDecorHandle.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "xAODTracking/TrackParticlexAODHelpers.h"

// NOTE: would be nice to include this to access track parameters, but
// it's not in all builds.
//
// #include "TrkEventPrimitives/ParamDefs.h"

namespace Pmt {
  // copied from
  // https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/Tracking/TrkEvent/TrkEventPrimitives/TrkEventPrimitives/ParamDefs.h,
  // which isn't included in some builds.
  enum ParamDefs {
    // Track defining parameters
    d0 = 0,
    z0 = 1,
    phi0 = 2,
    theta = 3,
    qOverP = 4,
    // Cartesian
    x = 0,
    y = 1,
    z = 2,
  };

  // I'm pretty sure I can get x, y, z, beamspot perigee points from
  // x = -sin(phi) * d0
  // y = cos(phi) * d0
  // z = z0
  Eigen::Vector3d getPosition(
    const xAOD::TrackParticle& trk) {
    double d0 = trk.d0();
    double phi = trk.phi();
    return Amg::Vector3D(
      d0 * -std::sin(phi),
      d0 *  std::cos(phi),
      trk.z0());
  }

  // this one is going to be harder, there's some lead here:
  // https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/Tracking/TrkVertexFitter/TrkVertexFitterUtils/src/TrackToVertexIPEstimator.cxx
  double getSigmaD0(const xAOD::TrackParticle& trk,
                    const Eigen::Matrix2d& vtxCov) {

    // start with the track part
    double trackComponent = trk.definingParametersCovMatrixDiagVec().at(
      Pmt::d0);

    // now do the vertex part
    double phi = trk.phi();
    // The sign seems inverted below, but maybe that doesn't
    // matter. I'm just following TrackToVertexIPEstimator...
    Eigen::Vector2d vtxJacobian(-std::sin(phi), std::cos(phi));
    double vertexComponent = vtxJacobian.transpose()*vtxCov*vtxJacobian;

    return std::sqrt(trackComponent + vertexComponent);

  }

  double getSigmaD0(const xAOD::TrackParticle& trk,
                    const xAOD::Vertex& vtx) {

    // first two elements of the vertex covariance is xy
    Eigen::Matrix2d vtxCov = vtx.covariancePosition().block<2,2>(0,0);
    return getSigmaD0(trk, vtxCov);

  }

  double getSigmaD0WithRespectToBeamspot(const xAOD::TrackParticle& trk,
                                         const xAOD::EventInfo& evt) {
    Eigen::Matrix2d bsCov;
    // based on what I read in the beamspot code [1] and some code
    // that copies it to the EventInfo [2] I'm pretty sure the
    // beamPosSigmaX and beamPosSigmaY need to be squared in the
    // covariance matrix, whereas beamPosSigmaXY does not.
    //
    // [1]: https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/InnerDetector/InDetConditions/BeamSpotConditionsData/src/BeamSpotData.cxx
    // [2]: https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/Simulation/BeamEffects/src/BeamSpotFixerAlg.cxx#0068
    bsCov(0, 0) = std::pow(evt.beamPosSigmaX(),2);
    bsCov(0, 1) = evt.beamPosSigmaXY();
    bsCov(1, 0) = evt.beamPosSigmaXY();
    bsCov(1, 1) = std::pow(evt.beamPosSigmaY(),2);
    return getSigmaD0(trk, bsCov);
  }


  double getSigmaZ0SinTheta(const xAOD::TrackParticle& trk,
                            double vxZCov) {

    // first do the track part
    const auto& fullCov = trk.definingParametersCovMatrix();
    Eigen::Matrix2d trkCov;
    trkCov(0,0)=fullCov(Pmt::z0,    Pmt::z0);
    trkCov(0,1)=fullCov(Pmt::z0,    Pmt::theta);
    trkCov(1,0)=fullCov(Pmt::theta, Pmt::z0);
    trkCov(1,1)=fullCov(Pmt::theta, Pmt::theta);
    double theta = trk.theta();
    Eigen::Vector2d trkJacobian(
      std::sin(theta),
      trk.z0()*std::cos(theta));
    double trackComponent = trkJacobian.transpose()*trkCov*trkJacobian;

    // now do the vertex part
    double vertexComponent = std::sin(theta)*vxZCov*std::sin(theta);

    return std::sqrt(trackComponent + vertexComponent);

  }
  double getSigmaZ0SinTheta(const xAOD::TrackParticle& trk,
                            const xAOD::Vertex& vtx) {
    double vxZCov = vtx.covariancePosition()(Pmt::z,Pmt::z);
    return getSigmaZ0SinTheta(trk, vxZCov);
  }

}

namespace FlavorTagDiscriminants {

  PoorMansIpAugmenterAlg::PoorMansIpAugmenterAlg(
    const std::string& name, ISvcLocator* loc )
    : AthReentrantAlgorithm(name, loc) {}

  StatusCode PoorMansIpAugmenterAlg::initialize() {
    ATH_MSG_INFO( "Inizializing " << name() << "... " );

    // Initialize Container keys
    ATH_MSG_DEBUG( "Inizializing containers:"        );
    ATH_MSG_DEBUG( "    ** " << m_TrackContainerKey  );
    ATH_MSG_DEBUG( "    ** " << m_VertexContainerKey );
    ATH_MSG_DEBUG( "    ** " << m_eventInfoKey );

    ATH_CHECK( m_TrackContainerKey.initialize() );
    ATH_CHECK( m_eventInfoKey.initialize() );

    bool use_vertices = !m_VertexContainerKey.empty();
    ATH_CHECK( m_VertexContainerKey.initialize(use_vertices) );

    // Prepare decorators
    m_dec_d0_sigma = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_d0_sigma.key();
    m_dec_z0_sigma = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_z0_sigma.key();

    m_dec_track_pos = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_track_pos.key();
    m_dec_track_mom = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_track_mom.key();

    // Initialize decorators
    ATH_MSG_DEBUG( "Inizializing decorators:"  );
    ATH_MSG_DEBUG( "    ** " << m_dec_d0_sigma );
    ATH_MSG_DEBUG( "    ** " << m_dec_z0_sigma );
    ATH_MSG_DEBUG( "    ** " << m_dec_track_pos );
    ATH_MSG_DEBUG( "    ** " << m_dec_track_mom );

    CHECK( m_dec_d0_sigma.initialize() );
    CHECK( m_dec_z0_sigma.initialize() );
    CHECK( m_dec_track_pos.initialize() );
    CHECK( m_dec_track_mom.initialize() );

    ATH_MSG_DEBUG(
      "Accessors for beamspot:" <<
      "    ** " << m_beam_sigma_x <<
      "    ** " << m_beam_sigma_y <<
      "    ** " << m_beam_sigma_z <<
      "    ** " << m_beam_cov_xy <<
      "");

    CHECK( m_beam_sigma_x.initialize() );
    CHECK( m_beam_sigma_y.initialize() );
    CHECK( m_beam_sigma_z.initialize() );
    CHECK( m_beam_cov_xy.initialize() );

    return StatusCode::SUCCESS;
  }

  StatusCode PoorMansIpAugmenterAlg::execute(const EventContext& ctx) const {
    ATH_MSG_DEBUG( "Executing " << name() << "... " );

    SG::ReadHandle<xAOD::EventInfo> event_info(m_eventInfoKey, ctx);
    CHECK( event_info.isValid() );

    const xAOD::Vertex* primary = nullptr;
    if (!m_VertexContainerKey.empty()) {
      SG::ReadHandle<xAOD::VertexContainer> verteces(
        m_VertexContainerKey,ctx );
      CHECK( verteces.isValid() );
      primary = getPrimaryVertex( *verteces );
      if ( primary == nullptr ) {
        ATH_MSG_FATAL("No primary vertex found");
        return StatusCode::FAILURE;
      }
    }

    SG::ReadHandle<xAOD::TrackParticleContainer> tracks(
      m_TrackContainerKey,ctx);
    CHECK( tracks.isValid() );
    ATH_MSG_DEBUG( "Retrieved " << tracks->size() << " input tracks..." );

    using TPC = xAOD::TrackParticleContainer;

    SG::WriteDecorHandle<TPC, float> decor_d0_sigma(m_dec_d0_sigma, ctx);
    SG::WriteDecorHandle<TPC, float> decor_z0_sigma(m_dec_z0_sigma, ctx);

    SG::WriteDecorHandle<TPC,std::vector<float>> decor_track_pos(
      m_dec_track_pos, ctx);
    SG::WriteDecorHandle<TPC,std::vector<float>> decor_track_mom(
      m_dec_track_mom, ctx);

    SG::ReadDecorHandle<xAOD::EventInfo,float> beam_sigma_x(
      m_beam_sigma_x, ctx);
    SG::ReadDecorHandle<xAOD::EventInfo,float> beam_sigma_y(
      m_beam_sigma_y, ctx);
    SG::ReadDecorHandle<xAOD::EventInfo,float> beam_sigma_z(
      m_beam_sigma_z, ctx);
    SG::ReadDecorHandle<xAOD::EventInfo,float> beam_cov_xy(
      m_beam_cov_xy, ctx);

    // now decorate the tracks
    for (const xAOD::TrackParticle *trk: *tracks) {

      const xAOD::EventInfo& evt = *event_info;
      float d0_sigma2 = trk->definingParametersCovMatrixDiagVec().at(Pmt::d0);
      float bs_d0_sigma2 = xAOD::TrackingHelpers::d0UncertaintyBeamSpot2(
        trk->phi(),
        beam_sigma_x(evt),
        beam_sigma_y(evt),
        beam_cov_xy(evt));
      float full_d0_sigma = std::sqrt(d0_sigma2 + bs_d0_sigma2);
      ATH_MSG_DEBUG("track    d0Uncertainty: " << std::sqrt(d0_sigma2));
      ATH_MSG_DEBUG("beamspot d0Uncertainty: " << std::sqrt(bs_d0_sigma2));
      ATH_MSG_DEBUG("combined d0Uncertainty: " << full_d0_sigma);

      decor_d0_sigma(*trk) = full_d0_sigma;
      if (primary) {
        decor_z0_sigma(*trk) = Pmt::getSigmaZ0SinTheta(*trk, *primary);
      } else {
        // if we don't have a primary we take the beamspot z
        // uncertainty
        double vxZCov = std::pow(beam_sigma_z(evt), 2);
        decor_z0_sigma(*trk) = Pmt::getSigmaZ0SinTheta(*trk, vxZCov);
      }

      // the primary vertex position is absolute, whereas the track
      // perigee parameters are all relative to the beamspot. The x
      // and y coordinates are zero so that the 2d impact parameter
      // has no idea about the primary vertex location.
      const Amg::Vector3D primary_relative_to_beamspot(
        0, 0,
        primary ? primary->position().z() - trk->vz() : 0);
      const Amg::Vector3D position = (
        Pmt::getPosition(*trk) - primary_relative_to_beamspot);

      auto trkp4 = trk->p4();
      const Amg::Vector3D momentum(trkp4.Px(), trkp4.Py(), trkp4.Pz());

      ATH_MSG_DEBUG(
        "track_displacement (x,y,z)= ("
        << position.x() << ", " << position.y() << ", " << position.z()
        << ")");
      ATH_MSG_DEBUG(
        "track_momentum (x,y,z)= ("
        << momentum.x() << ", " << momentum.y() << ", " << momentum.z()
        << ")");

      std::vector<float> out_vec_pos(
        position.data(), position.data() + position.size());
      std::vector<float> out_vec_mom(
        momentum.data(), momentum.data() + momentum.size());

      decor_track_pos (*trk) = out_vec_pos;
      decor_track_mom (*trk) = out_vec_mom;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode PoorMansIpAugmenterAlg::finalize() {
    return StatusCode::SUCCESS;
  }

  const xAOD::Vertex* PoorMansIpAugmenterAlg::getPrimaryVertex(
    const xAOD::VertexContainer& vertexCollection ) const {
    if ( vertexCollection.size() == 0 ) {
      ATH_MSG_DEBUG( "Input vertex collection has size 0!" );
      return nullptr;
    }

    for ( const xAOD::Vertex *vertex : vertexCollection ) {
      if ( vertex->vertexType() == xAOD::VxType::PriVtx ) {
        return vertex;
      }
    }

    // this is taken from BTagTool, should be the beam spot if nothing
    // else exists.
    return vertexCollection.front();
  }

}


