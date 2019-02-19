/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef BTAG_TRACK_AUGMENTER_HH
#define BTAG_TRACK_AUGMENTER_HH

#include "FlavorTagDiscriminants/EDMSchemaEnums.h"

#include <vector>

#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODJet/Jet.h"

struct BTagSignedIP {
  double ip2d_signed_d0;
  double ip3d_signed_d0;
  double ip3d_signed_z0;
  double ip3d_signed_d0_significance;
  double ip3d_signed_z0_significance;
  double ip2d_grade;
  double ip3d_grade;
};

class BTagTrackAugmenter {
public:
  typedef FlavorTagDiscriminants::EDMSchema EDMSchema;
  BTagTrackAugmenter(EDMSchema schema = EDMSchema::WINTER_2018);
  void augment(const xAOD::TrackParticle &track, const xAOD::Jet &jet);

  // NOTE: this should be called in the derivations if possible,
  // since it adds information that we don't need to store and
  // should be independent of the jet
  void augment_with_grades(const xAOD::TrackParticle &track, const xAOD::Jet &jet);

  // NOTE: this _must_ be called outside the derivation framework,
  // and should only be called in cases where the decoration is read
  // imediately afterword: it is only valid for the jet it is
  // assigned to!
  //
  // Better advice: don't use this at all, use get_signed_ip() instead
  void augment_with_ip(const xAOD::TrackParticle &track, const xAOD::Jet &jet);
  double d0(const xAOD::TrackParticle &track) const;
  double d0Uncertainty(const xAOD::TrackParticle &track) const;
  double z0SinTheta(const xAOD::TrackParticle &track) const;

  BTagSignedIP get_signed_ip(const xAOD::TrackParticle &track, const xAOD::Jet &jet) const;
private:
  typedef SG::AuxElement AE;

  AE::ConstAccessor<float> m_ip_d0;
  AE::ConstAccessor<float> m_ip_z0;
  AE::ConstAccessor<float> m_ip_d0_sigma;
  AE::ConstAccessor<float> m_ip_z0_sigma;
  AE::ConstAccessor<std::vector<float> > m_track_displacement;
  AE::ConstAccessor<std::vector<float> > m_track_momentum;
  AE::ConstAccessor<std::vector<ElementLink<xAOD::TrackParticleContainer> > > m_ip2d_trackParticleLinks;
  AE::ConstAccessor<std::vector<ElementLink<xAOD::TrackParticleContainer> > > m_ip3d_trackParticleLinks;
  AE::ConstAccessor<std::vector<int> > m_ip2d_gradeOfTracks;
  AE::ConstAccessor<std::vector<int> > m_ip3d_gradeOfTracks;
  AE::Decorator<float> m_ip2d_signed_d0;
  AE::Decorator<float> m_ip3d_signed_d0;
  AE::Decorator<float> m_ip3d_signed_z0;
  AE::Decorator<float> m_ip3d_signed_d0_significance;
  AE::Decorator<float> m_ip3d_signed_z0_significance;
  AE::Decorator<int> m_ip2d_grade;
  AE::Decorator<int> m_ip3d_grade;
};

#endif
