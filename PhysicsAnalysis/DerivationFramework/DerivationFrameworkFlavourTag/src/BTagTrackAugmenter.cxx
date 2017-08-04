/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagTrackAugmenter.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "xAODJet/JetContainer.h"

// slightly ugly that we include both of these: ideally we'd just
// add the methods we need to the extrapolator
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkSurfaces/PerigeeSurface.h"

#include <set>

const std::string VX_INDEX_KEY = "BTaggingVertexIndex";

Vector3DComponentDecorator::Vector3DComponentDecorator(const std::string& prefix):
  m_x(prefix + "x"), m_y(prefix + "y"), m_z(prefix + "z")
{
}
void Vector3DComponentDecorator::set(const SG::AuxElement& el,
                                     const Amg::Vector3D& vec) const {
  m_x(el) = vec.x();
  m_y(el) = vec.y();
  m_z(el) = vec.z();
}

Vector3DDecorator::Vector3DDecorator(const std::string& name):
  m_dec(name)
{}
void Vector3DDecorator::set(const SG::AuxElement& el,
                            const Amg::Vector3D& vec) const {
  std::vector<float> out_vec(vec.data(), vec.data() + vec.size());
  m_dec(el) = out_vec;
}


BTagTrackAugmenter::BTagTrackAugmenter(
  const std::string& name, ISvcLocator* loc):
  AthAlgorithm(name, loc),
  m_extrapolator("Trk::Extrapolator"),
  m_ghost_accessor("GhostTrack"),
  m_dec_z0_raw("btag_z0"),
  m_dec_d0("btag_ip_d0"),
  m_dec_z0("btag_ip_z0"),
  m_dec_phi("btag_ip_phi"),
  // m_dec_theta("btag_ip_theta"),
  m_dec_d0_sigma("btag_ip_d0_sigma"),
  m_dec_z0_sigma("btag_ip_z0_sigma"),
  m_dec_track_pos("btag_track_displacement"),
  m_dec_track_mom("btag_track_momentum")
{
  declareProperty("JetCollectionName", m_jet_collection);
  declareProperty( "TrackToVertexIPEstimator", m_track_to_vx );
  declareProperty( "TrackAssociator",
                   m_track_associator = "BTagTrackToJetAssociator");
  declareProperty("Extrapolator", m_extrapolator);
  declareProperty("SaveTrackVectors", m_save_track_vectors = false);
}

StatusCode BTagTrackAugmenter::initialize() {

  if (m_track_to_vx.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_track_to_vx);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode BTagTrackAugmenter::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode BTagTrackAugmenter::execute() {
  typedef ElementLink<xAOD::TrackParticleContainer> TrackLink;
  typedef std::vector<TrackLink> TrackLinks;

  // get the primary vertex
  const xAOD::VertexContainer *vertices = 0;
  CHECK( evtStore()->retrieve(vertices, "PrimaryVertices") );
  int* pv_index = 0;
  auto sc = evtStore()->retrieve(pv_index, VX_INDEX_KEY);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("could not find " + VX_INDEX_KEY + " in file");
    return StatusCode::FAILURE;
  }
  const xAOD::Vertex *primary = vertices->at(*pv_index);
  Trk::PerigeeSurface primary_surface(primary->position());

  // loop jets (we don't worry about cleaning bad jets etc, we just
  // want to avoid adding extra parameters to every track)
  const xAOD::JetContainer *jets = 0;
  CHECK( evtStore()->retrieve(jets, m_jet_collection) );
  for (const auto* jet: *jets) {

    // grab all possible tracks
    // start with the track associator
    const xAOD::BTagging *bjet = jet->btagging();
    if (!bjet) {
      ATH_MSG_WARNING("no btagging object found for " + m_jet_collection +
                      " skipping");
      return StatusCode::SUCCESS;
    }
    TrackLinks assoc_tracks = bjet->auxdata<TrackLinks>(m_track_associator);
    std::set<const xAOD::TrackParticle*> tracks;
    for (const auto& track_link: assoc_tracks) {
      const xAOD::TrackParticle* track = *track_link;
      tracks.insert(track);
    }
    // then use ghost association
    if (m_ghost_accessor.isAvailable(*jet)) {
      for (const auto& ghost: m_ghost_accessor(*jet)) {
        const auto* track = dynamic_cast<const xAOD::TrackParticle*>(*ghost);
        if (!track) throw std::runtime_error("this isn't a track particle");
        tracks.insert(track);
      }
    }

    // now decorate the tracks
    for (const auto* track: tracks) {
      m_dec_z0_raw(*track) = track->z0() + track->vz() - primary->z();
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
      m_dec_phi(*track) = extrap_pars->momentum().phi();
      if (m_save_track_vectors) {
        const Amg::Vector3D& track_pos = extrap_pars->position();
        const Amg::Vector3D& vertex_pos = primary->position();
        m_dec_track_pos.set(*track, track_pos - vertex_pos);
        m_dec_track_mom.set(*track, extrap_pars->momentum());
      }
    }
  }

  return StatusCode::SUCCESS;
}
