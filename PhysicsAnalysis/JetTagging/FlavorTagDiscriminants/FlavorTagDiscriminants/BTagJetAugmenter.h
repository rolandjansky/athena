/*
  Copyright (C) 2002-2018,2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_JET_AUGMENTER_HH
#define BTAG_JET_AUGMENTER_HH

#include "FlavorTagDiscriminants/EDMSchemaEnums.h"

// ATLAS things
#include "xAODJet/Jet.h"
#include "xAODEventInfo/EventInfo.h"

class BTagJetAugmenter
{
public:
  typedef FlavorTagDiscriminants::EDMSchema EDMSchema;
  BTagJetAugmenter(EDMSchema schema = EDMSchema::WINTER_2018);
  void augment(const xAOD::Jet &jet);
  void augment(const xAOD::Jet &jet, const xAOD::Jet &uncalibrated_jet);
private:
  typedef SG::AuxElement AE;

  AE::Decorator<double> pt_uncalib;
  AE::Decorator<double> eta_uncalib;
  AE::Decorator<double> abs_eta_uncalib;

  AE::ConstAccessor<std::vector<float> > ip2d_weightBOfTracks;
  AE::Decorator<int> ip2d_nTrks;
  AE::ConstAccessor<double> ip2d_pu;
  AE::ConstAccessor<double> ip2d_pc;
  AE::ConstAccessor<double> ip2d_pb;
  AE::Decorator<char> ip2d_isDefaults;
  AE::Decorator<double> ip2d_cu;
  AE::Decorator<double> ip2d_bu;
  AE::Decorator<double> ip2d_bc;

  AE::ConstAccessor<std::vector<float> > ip3d_weightBOfTracks;
  AE::Decorator<int> ip3d_nTrks;
  AE::ConstAccessor<double> ip3d_pu;
  AE::ConstAccessor<double> ip3d_pc;
  AE::ConstAccessor<double> ip3d_pb;
  AE::Decorator<char> ip3d_isDefaults;
  AE::Decorator<double> ip3d_cu;
  AE::Decorator<double> ip3d_bu;
  AE::Decorator<double> ip3d_bc;

  AE::ConstAccessor<float> jf_deltaEta;
  AE::ConstAccessor<float> jf_deltaPhi;
  AE::ConstAccessor<std::vector<float> > jf_fittedPosition;
  AE::ConstAccessor<std::vector<ElementLink<xAOD::BTagVertexContainer> > > jf_vertices;
  AE::ConstAccessor<int> jf_nVtx;
  AE::ConstAccessor<int> jf_nSingleTracks;
  AE::Decorator<char> jf_isDefaults;
  AE::Decorator<float> jf_deltaR;

  AE::ConstAccessor<std::vector<ElementLink<xAOD::VertexContainer> > > sv1_vertices;
  AE::Decorator<int> sv1_nVtx;
  AE::Decorator<char> sv1_isDefaults;

  AE::ConstAccessor<std::vector<ElementLink<xAOD::TrackParticleContainer> > > jet_track_links;
  AE::Decorator<char> secondaryVtx_isDefaults;
  AE::Decorator<int> secondaryVtx_nTrks;
  AE::Decorator<double> secondaryVtx_m;
  AE::Decorator<double> secondaryVtx_E;
  AE::Decorator<double> secondaryVtx_EFrac;
  AE::Decorator<float> secondaryVtx_L3d;
  AE::Decorator<float> secondaryVtx_Lxy;
  AE::Decorator<double> secondaryVtx_min_trk_flightDirRelEta;
  AE::Decorator<double> secondaryVtx_max_trk_flightDirRelEta;
  AE::Decorator<double> secondaryVtx_avg_trk_flightDirRelEta;
  AE::Decorator<double> min_trk_flightDirRelEta;
  AE::Decorator<double> max_trk_flightDirRelEta;
  AE::Decorator<double> avg_trk_flightDirRelEta;

  AE::ConstAccessor<float> smt_mu_pt;
  AE::Decorator<char> smt_isDefaults;

  AE::ConstAccessor<char> rnnip_pbIsValid;
  AE::Decorator<char> rnnip_isDefaults;
};

#endif
