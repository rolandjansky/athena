/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_JET_AUGMENTER_HH
#define BTAG_JET_AUGMENTER_HH

#include "FlavorTagDiscriminants/FlipTagEnums.h"

// ATLAS things
#include "xAODBTagging/BTagging.h"
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
  void augmentJfDr(const xAOD::BTagging &btag) const;
  void augmentIpRatios(const xAOD::BTagging &btag) const;
  void augmentBtagJes(const xAOD::BTagging &target,
                      const xAOD::BTagging &uncalib) const;
  void augment(const xAOD::BTagging &btag) const;
  void augment(const xAOD::BTagging &btag,
               const xAOD::BTagging &uncalibrated_btag) const;

  std::set<std::string> getDecoratorKeys() const;
  std::set<std::string> getAuxInputKeys() const;

private:
  bool jfIsDefaults(const xAOD::BTagging &btag) const;
  double augmentDmeson(int secondaryVtx_track_number, float secondaryVtx_charge, std::vector<TLorentzVector> secondaryVtx_4momentum_vector, std::vector<float> secondaryVtx_charge_vector) const;

  float safelog_prob(float p_up, float p_down) const;

  typedef SG::AuxElement AE;

  AE::ConstAccessor<ElementLink<xAOD::JetContainer>> m_jetLink;

  AE::Decorator<float> m_pt_uncalib;
  AE::Decorator<float> m_eta_uncalib;
  AE::Decorator<float> m_abs_eta_uncalib;

  AE::ConstAccessor<std::vector<float> > m_ip2d_weightBOfTracks;
  AE::Decorator<int> m_ip2d_nTrks;
  AE::ConstAccessor<float> m_ip2d_pu;
  AE::ConstAccessor<float> m_ip2d_pc;
  AE::ConstAccessor<float> m_ip2d_pb;
  AE::Decorator<char> m_ip2d_isDefaults;
  AE::Decorator<float> m_ip2d_cu;
  AE::Decorator<float> m_ip2d_bu;
  AE::Decorator<float> m_ip2d_bc;

  AE::ConstAccessor<std::vector<float> > m_ip3d_weightBOfTracks;
  AE::Decorator<int> m_ip3d_nTrks;
  AE::ConstAccessor<float> m_ip3d_pu;
  AE::ConstAccessor<float> m_ip3d_pc;
  AE::ConstAccessor<float> m_ip3d_pb;
  AE::Decorator<char> m_ip3d_isDefaults;
  AE::Decorator<float> m_ip3d_cu;
  AE::Decorator<float> m_ip3d_bu;
  AE::Decorator<float> m_ip3d_bc;

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

  // Add new variable
  AE::Decorator<float> m_secondaryVtx_DmesonMass;
  AE::Decorator<bool> m_secondaryVtx_isDmesonRecon;

  AE::ConstAccessor<char> m_rnnip_pbIsValid;
  AE::Decorator<char> m_rnnip_isDefaults;

};

#endif
