/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_JET_AUGMENTER_HH
#define BTAG_JET_AUGMENTER_HH

#include "FlavorTagDiscriminants/FlipTagEnums.h"
#include "FlavorTagDiscriminants/ftagfloat_t.h"

// ATLAS things
#include "xAODJet/Jet.h"
#include "xAODEventInfo/EventInfo.h"

class BTagJetAugmenter
{
public:
  typedef FlavorTagDiscriminants::FlipTagConfig FlipTagConfig;
  BTagJetAugmenter(std::string associator = "BTagTrackToJetAssociator",
                   FlipTagConfig flip = FlipTagConfig::STANDARD);
  ~BTagJetAugmenter();
  BTagJetAugmenter(BTagJetAugmenter&&);
  void augmentJfDr(const xAOD::BTagging &btag);
  void augmentIpRatios(const xAOD::BTagging &btag);
  void augmentBtagJes(const xAOD::Jet &target, const xAOD::Jet &uncalib);
  void augment(const xAOD::Jet &jet);
  void augment(const xAOD::Jet &jet, const xAOD::Jet &uncalibrated_jet);

  std::vector<std::string> getDecoratorKeys() const;

private:
  bool jfIsDefaults(const xAOD::BTagging &btag);

  float safelog_prob(float p_up, float p_down);
  
  typedef SG::AuxElement AE;

  AE::Decorator<float> m_pt_uncalib;
  AE::Decorator<float> m_eta_uncalib;
  AE::Decorator<float> m_abs_eta_uncalib;

  AE::ConstAccessor<std::vector<float> > m_ip2d_weightBOfTracks;
  AE::Decorator<int> m_ip2d_nTrks;
  AE::ConstAccessor<ftagfloat_t> m_ip2d_pu;
  AE::ConstAccessor<ftagfloat_t> m_ip2d_pc;
  AE::ConstAccessor<ftagfloat_t> m_ip2d_pb;
  AE::Decorator<char> m_ip2d_isDefaults;
  AE::Decorator<ftagfloat_t> m_ip2d_cu;
  AE::Decorator<ftagfloat_t> m_ip2d_bu;
  AE::Decorator<ftagfloat_t> m_ip2d_bc;

  AE::ConstAccessor<std::vector<float> > m_ip3d_weightBOfTracks;
  AE::Decorator<int> m_ip3d_nTrks;
  AE::ConstAccessor<ftagfloat_t> m_ip3d_pu;
  AE::ConstAccessor<ftagfloat_t> m_ip3d_pc;
  AE::ConstAccessor<ftagfloat_t> m_ip3d_pb;
  AE::Decorator<char> m_ip3d_isDefaults;
  AE::Decorator<ftagfloat_t> m_ip3d_cu;
  AE::Decorator<ftagfloat_t> m_ip3d_bu;
  AE::Decorator<ftagfloat_t> m_ip3d_bc;

  AE::ConstAccessor<float> m_jf_deltaEta;
  AE::ConstAccessor<float> m_jf_deltaPhi;
  AE::ConstAccessor<std::vector<float> > m_jf_fittedPosition;
  AE::ConstAccessor<std::vector<ElementLink<xAOD::BTagVertexContainer> > > m_jf_vertices;
  AE::ConstAccessor<int> m_jf_nVtx;
  AE::ConstAccessor<int> m_jf_nSingleTracks;
  AE::Decorator<char> m_jf_isDefaults;
  AE::Decorator<float> m_jf_deltaR;

  AE::ConstAccessor<std::vector<ElementLink<xAOD::VertexContainer> > > m_sv1_vertices;
  AE::Decorator<int> m_sv1_nVtx;
  AE::Decorator<char> m_sv1_isDefaults;

  AE::ConstAccessor<std::vector<ElementLink<xAOD::TrackParticleContainer> > > m_jet_track_links;
  AE::Decorator<char> m_secondaryVtx_isDefaults;
  AE::Decorator<int> m_secondaryVtx_nTrks;
  AE::Decorator<float> m_secondaryVtx_m;
  AE::Decorator<float> m_secondaryVtx_E;
  AE::Decorator<float> m_secondaryVtx_EFrac;
  AE::Decorator<float> m_secondaryVtx_L3d;
  AE::Decorator<float> m_secondaryVtx_Lxy;
  AE::Decorator<float> m_secondaryVtx_min_trk_flightDirRelEta;
  AE::Decorator<float> m_secondaryVtx_max_trk_flightDirRelEta;
  AE::Decorator<float> m_secondaryVtx_avg_trk_flightDirRelEta;
  AE::Decorator<float> m_min_trk_flightDirRelEta;
  AE::Decorator<float> m_max_trk_flightDirRelEta;
  AE::Decorator<float> m_avg_trk_flightDirRelEta;

  AE::ConstAccessor<char> m_rnnip_pbIsValid;
  AE::Decorator<char> m_rnnip_isDefaults;

};

#endif
