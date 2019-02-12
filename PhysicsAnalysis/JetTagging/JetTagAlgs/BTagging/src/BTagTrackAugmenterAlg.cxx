/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "BTagging/BTagTrackAugmenterAlg.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// slightly ugly that we include both of these: ideally we'd just
// add the methods we need to the extrapolator
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkSurfaces/PerigeeSurface.h"


namespace Analysis {

  BTagTrackAugmenterAlg::BTagTrackAugmenterAlg(
    const std::string& name, ISvcLocator* loc):
    AthAlgorithm(name, loc),
    m_extrapolator("Trk::Extrapolator"),
    m_dec_d0("DUMMY"),
    m_dec_z0("DUMMY"),
    m_dec_d0_sigma("DUMMY"),
    m_dec_z0_sigma("DUMMY"),
    m_dec_track_pos("DUMMYv"),
    m_dec_track_mom("DUMMYv"),
    m_prefix("btag_"),
    m_primaryVertexContainer("PrimaryVertices")
  {
    declareProperty( "TrackToVertexIPEstimator", m_track_to_vx );
    declareProperty("Extrapolator", m_extrapolator);
    declareProperty("prefix", m_prefix);
    declareProperty("primaryVertexContainer", m_primaryVertexContainer);
  }
  StatusCode BTagTrackAugmenterAlg::initialize() {
    if (m_track_to_vx.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_track_to_vx);
      return StatusCode::FAILURE;
    }
    if (m_extrapolator.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_extrapolator);
      return StatusCode::FAILURE;
    }

    // name the decorations
    typedef SG::AuxElement::Decorator< float > AEF;
    m_dec_d0 = AEF(m_prefix + "ip_d0");
    m_dec_z0 = AEF(m_prefix + "ip_z0");
    m_dec_d0_sigma = AEF(m_prefix + "ip_d0_sigma");
    m_dec_z0_sigma = AEF(m_prefix + "ip_z0_sigma");
    m_dec_track_pos = Vector3DDecorator(m_prefix + "track_displacement");
    m_dec_track_mom = Vector3DDecorator(m_prefix + "track_momentum");

    return StatusCode::SUCCESS;
  }

  StatusCode BTagTrackAugmenterAlg::finalize() {
    return StatusCode::SUCCESS;
  }

  StatusCode BTagTrackAugmenterAlg::execute() {
    const xAOD::Vertex* primary = getPrimaryVertex();

    if (!primary) {
      ATH_MSG_FATAL("No primary vertex found");
      return StatusCode::FAILURE;
    }

    Trk::PerigeeSurface primary_surface(primary->position());

    const xAOD::TrackParticleContainer* tracks = nullptr;
    CHECK( evtStore()->retrieve(tracks, "InDetTrackParticles") );

    // now decorate the tracks
    for (const auto* track: *tracks) {
      std::unique_ptr<const Trk::ImpactParametersAndSigma> ip(
        m_track_to_vx->estimate(track, primary));
      m_dec_d0(*track) = ip->IPd0;
      m_dec_z0(*track) = ip->IPz0SinTheta;
      m_dec_d0_sigma(*track) = ip->sigmad0;
      m_dec_z0_sigma(*track) = ip->sigmaz0SinTheta;

      // some other parameters we have go get directly from the
      // extrapolator. This is more or less copied from:
      // https://goo.gl/iWLv5T
      std::unique_ptr<const Trk::TrackParameters> extrap_pars(
        m_extrapolator->extrapolate(*track, primary_surface));
      const Amg::Vector3D& track_pos = extrap_pars->position();
      const Amg::Vector3D& vertex_pos = primary->position();
      m_dec_track_pos.set(*track, track_pos - vertex_pos);
      m_dec_track_mom.set(*track, extrap_pars->momentum());
    }

    return StatusCode::SUCCESS;
  }

  const xAOD::Vertex* BTagTrackAugmenterAlg::getPrimaryVertex() const {

    const xAOD::VertexContainer *vertices = 0;
    auto sc = evtStore()->retrieve(vertices, m_primaryVertexContainer);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("No primary vertex found in " + m_primaryVertexContainer);
      return nullptr;
    }
    xAOD::VertexContainer::const_iterator vtx_itr = vertices->begin();
    xAOD::VertexContainer::const_iterator vtx_end = vertices->end();
    for (; vtx_itr != vtx_end; ++vtx_itr) {
      if ((*vtx_itr)->vertexType() == xAOD::VxType::PriVtx) {
        return *vtx_itr;
      }
    }
    // this is taken from BTagTool, should be the beam spot if nothing
    // else exists.
    return vertices->front();
  }

  //___________________________________________________________________
  // utility class

  Vector3DDecorator::Vector3DDecorator(const std::string& name):
    m_dec(name)
  {}
  void Vector3DDecorator::set(const SG::AuxElement& el,
                              const Amg::Vector3D& vec) const {
    std::vector<float> out_vec(vec.data(), vec.data() + vec.size());
    m_dec(el) = out_vec;
  }

}
