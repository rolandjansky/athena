/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/BTagHighLevelAugmenterAlg.h"

#include <cmath>
#include <cstddef>
#include <regex>

#include "xAODJet/Jet.h"
#include "StoreGate/WriteDecorHandle.h"

#include "TVector3.h"

namespace {
  // grab names based on configuration
  std::string negString(FlavorTagDiscriminants::FlipTagConfig f) {
    switch(f) {
    case FlipTagConfig::STANDARD: return "";
    case FlipTagConfig::FLIP_SIGN: // intentional fall-through
    case FlipTagConfig::NEGATIVE_IP_ONLY: return "Neg";
    default: throw std::logic_error("undefined flip config");
    }
    return "";
  }
  std::string flipString(FlavorTagDiscriminants::FlipTagConfig f) {
    using namespace FlavorTagDiscriminants;
    switch(f) {
    case FlipTagConfig::STANDARD: return "";
    case FlipTagConfig::FLIP_SIGN: // intentional fall-through
    case FlipTagConfig::NEGATIVE_IP_ONLY: return "Flip";
    default: throw std::logic_error("undefined flip config");
    }
    return "";
  }

  // the taggers
  std::string ip2(FlavorTagDiscriminants::FlipTagConfig f) {
    return "IP2D" + negString(f);
  }
  std::string ip3(FlavorTagDiscriminants::FlipTagConfig f) {
    return "IP3D" + negString(f);
  }
  std::string rnn(FlavorTagDiscriminants::FlipTagConfig f) {
    using namespace FlavorTagDiscriminants;
    switch(f) {
    case FlipTagConfig::STANDARD: return "rnnip";
    case FlipTagConfig::FLIP_SIGN: // intentional fall-through
    case FlipTagConfig::NEGATIVE_IP_ONLY: return "rnnipflip";
    default: throw std::logic_error("undefined flip config");
    }
    return "rnnip";
  }
  std::string jf(FlavorTagDiscriminants::FlipTagConfig f) {
    return "JetFitter" + flipString(f);
  }
  std::string sv(FlavorTagDiscriminants::FlipTagConfig f) {
    return "SV1" + flipString(f);
  }
  std::string jfSvNew(FlavorTagDiscriminants::FlipTagConfig f) {
    return "JetFitterSecondaryVertex" + flipString(f);
  }

}

namespace Analysis {
  StatusCode BTagHighLevelAugmenterAlg::initialize() {
    ATH_MSG_INFO( "Inizializing " << name() << "... " );
    
    // Initialize Container keys
    ATH_MSG_DEBUG( "Inizializing containers:"        );
    ATH_MSG_DEBUG( "    ** " << m_BTagCollectionName  );

    ATH_CHECK( m_BTagCollectionName.initialize() );

    FlipTagConfig flipTagConf = FlavorTagDiscriminants::flipTagConfigFromString(m_flipTagConfig);
    m_dec_pt_uncalib = m_BTagCollectionName.key() + ".pt_btagJes";
    m_dec_eta_uncalib = m_BTagCollectionName.key() + ".eta_btagJes";
    m_dec_abs_eta_uncalib = m_BTagCollectionName.key() + ".absEta_btagJes";
    m_acc_ip2d_weightBOfTracks = ip2(flipTagConf) + "_weightBofTracks";
    m_dec_ip2d_nTrks = m_BTagCollectionName.key() + "." + ip2(flipTagConf) + "_nTrks";
    m_acc_ip2d_pu = ip2(flipTagConf) + "_pu";
    m_acc_ip2d_pc = ip2(flipTagConf) + "_pc";
    m_acc_ip2d_pb = ip2(flipTagConf) + "_pb";
    m_dec_ip2d_isDefaults = m_BTagCollectionName.key() + "." + ip2(flipTagConf) + "_isDefaults";
    m_dec_ip2d_cu = m_BTagCollectionName.key() + "." + ip2(flipTagConf) + "_cu";
    m_dec_ip2d_bu = m_BTagCollectionName.key() + "." + ip2(flipTagConf) + "_bu";
    m_dec_ip2d_bc = m_BTagCollectionName.key() + "." + ip2(flipTagConf) + "_bc";
    m_acc_ip3d_pu = ip3(flipTagConf) + "_pu";
    m_acc_ip3d_pc = ip3(flipTagConf) + "_pc";
    m_acc_ip3d_pb = ip3(flipTagConf) + "_pb";
    m_acc_ip3d_weightBOfTracks = ip3(flipTagConf) + "_weightBofTracks";
    m_dec_ip3d_nTrks = m_BTagCollectionName.key() + "." + ip3(flipTagConf) + "_nTrks";
    m_dec_ip3d_isDefaults = m_BTagCollectionName.key() + "." + ip3(flipTagConf) + "_isDefaults";
    m_dec_ip3d_cu = m_BTagCollectionName.key() + "." + ip3(flipTagConf) + "_cu";
    m_dec_ip3d_bu = m_BTagCollectionName.key() + "." + ip3(flipTagConf) + "_bu";
    m_dec_ip3d_bc = m_BTagCollectionName.key() + "." + ip3(flipTagConf) + "_bc";
    m_acc_jf_deltaEta = jf(flipTagConf) + "_deltaeta";
    m_acc_jf_deltaPhi = jf(flipTagConf) + "_deltaphi";
    m_acc_jf_fittedPosition = jf(flipTagConf) + "_fittedPosition";
    m_acc_jf_vertices = jf(flipTagConf) + "_JFvertices";
    m_acc_jf_nVtx = jf(flipTagConf) + "_nVTX";
    m_acc_jf_nSingleTracks = jf(flipTagConf) + "_nSingleTracks";
    m_dec_jf_isDefaults = m_BTagCollectionName.key() + "." + jf(flipTagConf) + "_isDefaults";
    m_dec_jf_deltaR = m_BTagCollectionName.key() + "." + jf(flipTagConf) + "_deltaR";
    m_acc_sv1_vertices = sv(flipTagConf) + "_vertices";
    m_dec_sv1_isDefaults = m_BTagCollectionName.key() + "." + sv(flipTagConf) + "_isDefaults";
    m_dec_secondaryVtx_isDefaults = m_BTagCollectionName.key() + "." + jfSvNew(flipTagConf) + "_isDefaults";
    m_dec_secondaryVtx_nTrks = m_BTagCollectionName.key() + "." + jfSvNew(flipTagConf) + "_nTracks";
    m_dec_secondaryVtx_m = m_BTagCollectionName.key() + "." + jfSvNew(flipTagConf) + "_mass";
    m_dec_secondaryVtx_E = m_BTagCollectionName.key() + "." + jfSvNew(flipTagConf) + "_energy";
    m_dec_secondaryVtx_EFrac = m_BTagCollectionName.key() + "." + jfSvNew(flipTagConf) + "_energyFraction";
    m_dec_secondaryVtx_L3d = m_BTagCollectionName.key() + "." + jfSvNew(flipTagConf) + "_displacement3d";
    m_dec_secondaryVtx_Lxy = m_BTagCollectionName.key() + "." + jfSvNew(flipTagConf) + "_displacement2d";
    m_dec_secondaryVtx_min_trk_flightDirRelEta = m_BTagCollectionName.key() + "." + jfSvNew(flipTagConf) + "_minimumTrackRelativeEta";
    m_dec_secondaryVtx_max_trk_flightDirRelEta = m_BTagCollectionName.key() + "." + jfSvNew(flipTagConf) + "_maximumTrackRelativeEta";
    m_dec_secondaryVtx_avg_trk_flightDirRelEta = m_BTagCollectionName.key() + "." + jfSvNew(flipTagConf) + "_averageTrackRelativeEta";
    m_dec_min_trk_flightDirRelEta = m_BTagCollectionName.key() + ".minimumTrackRelativeEta" + flipString(flipTagConf);
    m_dec_max_trk_flightDirRelEta = m_BTagCollectionName.key() + ".maximumTrackRelativeEta" + flipString(flipTagConf);
    m_dec_avg_trk_flightDirRelEta = m_BTagCollectionName.key() + ".averageTrackRelativeEta" + flipString(flipTagConf);
    m_dec_smt_isDefaults = m_BTagCollectionName.key() + ".SMT_isDefaults";
    m_acc_smt_mu_pt = "SMT_mu_pt";
    m_dec_rnnip_isDefaults = m_BTagCollectionName.key() + "." + rnn(flipTagConf) + "_isDefaults";

    CHECK( m_dec_pt_uncalib.initialize() );
    CHECK( m_dec_eta_uncalib.initialize() );
    CHECK( m_dec_abs_eta_uncalib.initialize() );

    CHECK( m_dec_ip2d_nTrks.initialize() );
    CHECK( m_dec_ip2d_isDefaults.initialize() );
    CHECK( m_dec_ip2d_cu.initialize() );
    CHECK( m_dec_ip2d_bu.initialize() );
    CHECK( m_dec_ip2d_bc.initialize() );

    CHECK( m_dec_ip3d_nTrks.initialize() );
    CHECK( m_dec_ip3d_isDefaults.initialize() );
    CHECK( m_dec_ip3d_cu.initialize() );
    CHECK( m_dec_ip3d_bu.initialize() );
    CHECK( m_dec_ip3d_bc.initialize() );

    CHECK( m_dec_jf_isDefaults.initialize() );
    CHECK( m_dec_jf_deltaR.initialize() );

    CHECK( m_dec_sv1_isDefaults.initialize() );

    CHECK( m_dec_secondaryVtx_isDefaults.initialize() );
    CHECK( m_dec_secondaryVtx_nTrks.initialize() );
    CHECK( m_dec_secondaryVtx_m.initialize() );
    CHECK( m_dec_secondaryVtx_E.initialize() );
    CHECK( m_dec_secondaryVtx_EFrac.initialize() );
    CHECK( m_dec_secondaryVtx_L3d.initialize() );
    CHECK( m_dec_secondaryVtx_Lxy.initialize() );
    CHECK( m_dec_secondaryVtx_min_trk_flightDirRelEta.initialize() );
    CHECK( m_dec_secondaryVtx_max_trk_flightDirRelEta.initialize() );
    CHECK( m_dec_secondaryVtx_avg_trk_flightDirRelEta.initialize() );
    CHECK( m_dec_min_trk_flightDirRelEta.initialize() );
    CHECK( m_dec_max_trk_flightDirRelEta.initialize() );
    CHECK( m_dec_avg_trk_flightDirRelEta.initialize() );

    CHECK( m_dec_smt_isDefaults.initialize() );

    CHECK( m_dec_rnnip_isDefaults.initialize() );

    return StatusCode::SUCCESS;
  }

  StatusCode BTagHighLevelAugmenterAlg::execute() {
    ATH_MSG_DEBUG( "Executing " << name() << "... " );
  
    SG::ReadHandle< xAOD::BTaggingContainer > h_bTagContainer( m_BTagCollectionName);
    CHECK( h_bTagContainer.isValid() );
    const xAOD::BTaggingContainer* btags = h_bTagContainer.get();
    ATH_MSG_DEBUG( "Retrieved " << btags->size() << " input btagging..." );

    for (const xAOD::BTagging * btag : *btags) {
      //ATH_MSG_INFO( "ip2d_pu with ConstAccess " << m_ip2d_pu(*btag));
      //ATH_MSG_INFO( "ip2d_pu with ReadDecor " << ip2d_pu(*btag));
      augment(*btag);
    }
    
    return StatusCode::SUCCESS;
  }

 
BTagHighLevelAugmenterAlg::BTagHighLevelAugmenterAlg( const std::string& name, ISvcLocator* loc )
    : AthAlgorithm(name, loc),
    m_flipTagConfig("STANDARD"),
    m_acc_jet_track_links("BTagTrackToJetAssociation")
{
  declareProperty("FlipTagConfig", m_flipTagConfig);
  declareProperty("BTagTrackToJetAssociatorName", m_acc_jet_track_links);
}


void BTagHighLevelAugmenterAlg::augment(const xAOD::BTagging& btag, const xAOD::Jet &uncalibrated_jet) {

  SG::WriteDecorHandle< xAOD::BTaggingContainer, float > pt_uncalib( m_dec_pt_uncalib );
  SG::WriteDecorHandle< xAOD::BTaggingContainer, float > eta_uncalib( m_dec_eta_uncalib );
  SG::WriteDecorHandle< xAOD::BTaggingContainer, float > abs_eta_uncalib( m_dec_abs_eta_uncalib );
  pt_uncalib(btag) = uncalibrated_jet.pt();
  eta_uncalib(btag) = uncalibrated_jet.eta();
  abs_eta_uncalib(btag) = std::abs(uncalibrated_jet.eta());

  // pass off to calibrated jet function
  augment(btag);
}

void BTagHighLevelAugmenterAlg::augmentJfDr(const xAOD::BTagging& btag) {
  SG::WriteDecorHandle< xAOD::BTaggingContainer, float > jf_deltaR( m_dec_jf_deltaR );
  AE::ConstAccessor<float> jf_deltaEta(m_acc_jf_deltaEta);
  AE::ConstAccessor<float> jf_deltaPhi(m_acc_jf_deltaPhi);
  if (jfIsDefaults(btag)) {
    jf_deltaR(btag) = NAN;
  } else {
    jf_deltaR(btag) = std::hypot(jf_deltaEta(btag), jf_deltaPhi(btag));
  }
}
void BTagHighLevelAugmenterAlg::augmentIpRatios(const xAOD::BTagging& btag) {

  SG::WriteDecorHandle< xAOD::BTaggingContainer, double > ip2d_cu( m_dec_ip2d_cu );
  SG::WriteDecorHandle< xAOD::BTaggingContainer, double > ip2d_bu( m_dec_ip2d_bu );
  SG::WriteDecorHandle< xAOD::BTaggingContainer, double > ip2d_bc( m_dec_ip2d_bc );
  AE::ConstAccessor<double> ip2d_pu(m_acc_ip2d_pu);
  AE::ConstAccessor<double> ip2d_pb(m_acc_ip2d_pb);
  AE::ConstAccessor<double> ip2d_pc(m_acc_ip2d_pc);
  ip2d_cu(btag) = std::log(ip2d_pc(btag) / ip2d_pu(btag));
  ip2d_bu(btag) = std::log(ip2d_pb(btag) / ip2d_pu(btag));
  ip2d_bc(btag) = std::log(ip2d_pb(btag) / ip2d_pc(btag));

  SG::WriteDecorHandle< xAOD::BTaggingContainer, double > ip3d_cu( m_dec_ip3d_cu );
  SG::WriteDecorHandle< xAOD::BTaggingContainer, double > ip3d_bu( m_dec_ip3d_bu );
  SG::WriteDecorHandle< xAOD::BTaggingContainer, double > ip3d_bc( m_dec_ip3d_bc );

  AE::ConstAccessor<double> ip3d_pu(m_acc_ip3d_pu);
  AE::ConstAccessor<double> ip3d_pb(m_acc_ip3d_pb);
  AE::ConstAccessor<double> ip3d_pc(m_acc_ip3d_pc);  
  ip3d_cu(btag) = std::log(ip3d_pc(btag) / ip3d_pu(btag));
  ip3d_bu(btag) = std::log(ip3d_pb(btag) / ip3d_pu(btag));
  ip3d_bc(btag) = std::log(ip3d_pb(btag) / ip3d_pc(btag));

}

void BTagHighLevelAugmenterAlg::augment(const xAOD::BTagging &btag) {
  
  SG::WriteDecorHandle< xAOD::BTaggingContainer, char > ip2d_isDefaults( m_dec_ip2d_isDefaults );
  AE::ConstAccessor<std::vector<float> > ip2d_weightBOfTracks(m_acc_ip2d_weightBOfTracks);
  if (ip2d_weightBOfTracks(btag).size() > 0) {
    ip2d_isDefaults(btag) = 0;
  } else {
    ip2d_isDefaults(btag) = 1;
  }
  SG::WriteDecorHandle< xAOD::BTaggingContainer, int > ip2d_nTrks( m_dec_ip2d_nTrks );
  ip2d_nTrks(btag) = ip2d_weightBOfTracks(btag).size();

  SG::WriteDecorHandle< xAOD::BTaggingContainer, char > ip3d_isDefaults( m_dec_ip3d_isDefaults );
  AE::ConstAccessor<std::vector<float> > ip3d_weightBOfTracks(m_acc_ip3d_weightBOfTracks);
  if (ip3d_weightBOfTracks(btag).size() > 0) {
    ip3d_isDefaults(btag) = 0;
  } else {
    ip3d_isDefaults(btag) = 1;
  }

  SG::WriteDecorHandle< xAOD::BTaggingContainer, int > ip3d_nTrks( m_dec_ip3d_nTrks );
  ip3d_nTrks(btag) = ip3d_weightBOfTracks(btag).size();
  augmentIpRatios(btag);

  SG::WriteDecorHandle< xAOD::BTaggingContainer, char > jf_isDefaults( m_dec_jf_isDefaults );
  jf_isDefaults(btag) = jfIsDefaults(btag);
  augmentJfDr(btag);

  AE::ConstAccessor<std::vector<ElementLink<xAOD::VertexContainer> > > sv1_vertices(m_acc_sv1_vertices);
  SG::WriteDecorHandle< xAOD::BTaggingContainer, char > sv1_isDefaults( m_dec_sv1_isDefaults );
  if (sv1_vertices(btag).size() > 0) {
    sv1_isDefaults(btag) = 0;
  } else {
    sv1_isDefaults(btag) = 1;
  }

  TVector3 flightDir(NAN, NAN, NAN);
  float jf_phi = NAN;
  float jf_theta = NAN;

  int secondary_jf_vtx_index = -1;
  int secondaryVtx_track_number = -1;
  double secondaryVtx_track_flightDirRelEta_total = NAN;
  float min_jf_vtx_L3d = NAN;

  SG::WriteDecorHandle< xAOD::BTaggingContainer, char > secondaryVtx_isDefaults( m_dec_secondaryVtx_isDefaults );
  AE::ConstAccessor<std::vector<float> > jf_fittedPosition(m_acc_jf_fittedPosition);
  AE::ConstAccessor<std::vector<ElementLink<xAOD::BTagVertexContainer> > > jf_vertices(m_acc_jf_vertices);
  if (jf_fittedPosition(btag).size() > 4) {
    jf_phi = jf_fittedPosition(btag).at(3);
    jf_theta = jf_fittedPosition(btag).at(4);
    flightDir.SetMagThetaPhi(1, jf_theta, jf_phi);

    for (std::size_t jf_vtx_index = 0; jf_vtx_index < jf_vertices(btag).size() && jf_vtx_index < jf_fittedPosition(btag).size() - 5; jf_vtx_index++) {
      float jf_vtx_L3d = jf_fittedPosition(btag).at(jf_vtx_index + 5);
      if (jf_vtx_L3d > 0 && (jf_vtx_L3d < min_jf_vtx_L3d || std::isnan(min_jf_vtx_L3d))) {
        secondary_jf_vtx_index = jf_vtx_index;
        min_jf_vtx_L3d = jf_vtx_L3d;
      }
    }

    if (secondary_jf_vtx_index >= 0) {
      secondaryVtx_track_number = 0;
      secondaryVtx_track_flightDirRelEta_total = 0;
      secondaryVtx_isDefaults(btag) = 0;
    } else {
      secondaryVtx_isDefaults(btag) = 1;
    }
  } else {
    secondaryVtx_isDefaults(btag) = 1;
  }
  SG::WriteDecorHandle< xAOD::BTaggingContainer, float > secondaryVtx_L3d( m_dec_secondaryVtx_L3d );
  SG::WriteDecorHandle< xAOD::BTaggingContainer, float > secondaryVtx_Lxy( m_dec_secondaryVtx_Lxy );
  secondaryVtx_L3d(btag) = min_jf_vtx_L3d;
  secondaryVtx_Lxy(btag) = min_jf_vtx_L3d * sinf(jf_theta);

  const float track_mass = 139.57; // Why?

  unsigned track_number = 0;
  double track_E_total = 0;
  double min_track_flightDirRelEta = NAN;
  double max_track_flightDirRelEta = NAN;
  double track_flightDirRelEta_total = 0;

  TLorentzVector secondaryVtx_4momentum_total;
  double secondaryVtx_min_track_flightDirRelEta = NAN;
  double secondaryVtx_max_track_flightDirRelEta = NAN;

  AE::ConstAccessor<std::vector<ElementLink<xAOD::TrackParticleContainer> > > jet_track_links(m_acc_jet_track_links);
  for (const auto &jet_track_link : jet_track_links(btag)) {
    const xAOD::TrackParticle &track_particle = **jet_track_link;

    uint8_t n_pixel_hits;
    uint8_t n_sct_hits;
    bool rc = true;
    rc &= track_particle.summaryValue(n_pixel_hits, xAOD::numberOfPixelHits);
    rc &= track_particle.summaryValue(n_sct_hits, xAOD::numberOfSCTHits);
    if (!rc) throw std::runtime_error(
      "track summary values are missing, can't compute b-tagging variables");
    if (n_pixel_hits + n_sct_hits < 2) continue;

    track_number++;
    track_E_total += track_particle.e();

    TVector3 track_flightDirRelVect = track_particle.p4().Vect();
    track_flightDirRelVect.SetTheta(track_flightDirRelVect.Angle(flightDir));

    double track_flightDirRelEta = NAN;
    if (track_flightDirRelVect.Perp() != 0) {
      track_flightDirRelEta = track_flightDirRelVect.PseudoRapidity();
    }

    track_flightDirRelEta_total += track_flightDirRelEta;
    if (track_flightDirRelEta < min_track_flightDirRelEta || std::isnan(min_track_flightDirRelEta)) {
      min_track_flightDirRelEta = track_flightDirRelEta;
    }
    if (track_flightDirRelEta > max_track_flightDirRelEta || std::isnan(max_track_flightDirRelEta)) {
      max_track_flightDirRelEta = track_flightDirRelEta;
    }
    if (secondary_jf_vtx_index >= 0) {
      for (const ElementLink<xAOD::TrackParticleContainer> vertex_track_particle : (**jf_vertices(btag).at(secondary_jf_vtx_index)).track_links()) {
        if (*vertex_track_particle == &track_particle) {
          secondaryVtx_track_number++;
          TLorentzVector track_fourVector;
          track_fourVector.SetVectM(track_particle.p4().Vect(), track_mass);
          secondaryVtx_4momentum_total += track_fourVector;
          secondaryVtx_track_flightDirRelEta_total += track_flightDirRelEta;
          if (track_flightDirRelEta < secondaryVtx_min_track_flightDirRelEta || std::isnan(secondaryVtx_min_track_flightDirRelEta)) {
            secondaryVtx_min_track_flightDirRelEta = track_flightDirRelEta;
          }
          if (track_flightDirRelEta > secondaryVtx_max_track_flightDirRelEta || std::isnan(secondaryVtx_max_track_flightDirRelEta)) {
            secondaryVtx_max_track_flightDirRelEta = track_flightDirRelEta;
          }
        }
      }
    }
  }

  SG::WriteDecorHandle< xAOD::BTaggingContainer, int > secondaryVtx_nTrks( m_dec_secondaryVtx_nTrks );
  secondaryVtx_nTrks(btag) = secondaryVtx_track_number;

  // Here begins a few blocks where the decoration depends on the
  // schema. I would like to move a number of values that were
  // previously stored as double to float.
  {
    double m = NAN;
    double E = NAN;
    double EFrac = NAN;
    if (secondaryVtx_track_number >= 0) {
      m = secondaryVtx_4momentum_total.M();
      E = secondaryVtx_4momentum_total.E();
      EFrac = secondaryVtx_4momentum_total.E() / track_E_total;
    }
    SG::WriteDecorHandle< xAOD::BTaggingContainer, float > secondaryVtx_m( m_dec_secondaryVtx_m );
    SG::WriteDecorHandle< xAOD::BTaggingContainer, float > secondaryVtx_E( m_dec_secondaryVtx_E );
    SG::WriteDecorHandle< xAOD::BTaggingContainer, float > secondaryVtx_EFrac( m_dec_secondaryVtx_EFrac );
    secondaryVtx_m(btag) = m;
    secondaryVtx_E(btag) = E;
    secondaryVtx_EFrac(btag) = EFrac;
  }
  {
    double min = secondaryVtx_min_track_flightDirRelEta;
    double max = secondaryVtx_max_track_flightDirRelEta;
    double avg = secondaryVtx_track_flightDirRelEta_total / secondaryVtx_track_number;
    SG::WriteDecorHandle< xAOD::BTaggingContainer, float > secondaryVtx_min_trk_flightDirRelEta( m_dec_secondaryVtx_min_trk_flightDirRelEta );
    SG::WriteDecorHandle< xAOD::BTaggingContainer, float > secondaryVtx_max_trk_flightDirRelEta( m_dec_secondaryVtx_max_trk_flightDirRelEta );
    SG::WriteDecorHandle< xAOD::BTaggingContainer, float > secondaryVtx_avg_trk_flightDirRelEta( m_dec_secondaryVtx_avg_trk_flightDirRelEta );
    secondaryVtx_min_trk_flightDirRelEta(btag) = min;
    secondaryVtx_max_trk_flightDirRelEta(btag) = max;
    secondaryVtx_avg_trk_flightDirRelEta(btag) = avg;
  }
  {
    double min = min_track_flightDirRelEta;
    double max = max_track_flightDirRelEta;
    double avg = track_flightDirRelEta_total / track_number;
    SG::WriteDecorHandle< xAOD::BTaggingContainer, float > min_trk_flightDirRelEta( m_dec_min_trk_flightDirRelEta );
    SG::WriteDecorHandle< xAOD::BTaggingContainer, float > max_trk_flightDirRelEta( m_dec_max_trk_flightDirRelEta );
    SG::WriteDecorHandle< xAOD::BTaggingContainer, float > avg_trk_flightDirRelEta( m_dec_avg_trk_flightDirRelEta );
    min_trk_flightDirRelEta(btag) = min;
    max_trk_flightDirRelEta(btag) = max;
    avg_trk_flightDirRelEta(btag) = avg;
  }

  SG::WriteDecorHandle< xAOD::BTaggingContainer, char > smt_isDefaults( m_dec_smt_isDefaults );
  AE::ConstAccessor<float> smt_mu_pt(m_acc_smt_mu_pt);
  if (smt_mu_pt(btag) > 0) {
    smt_isDefaults(btag) = 0;
  }  else {
    smt_isDefaults(btag) = 1;
  }
  SG::WriteDecorHandle< xAOD::BTaggingContainer, char > rnnip_isDefaults( m_dec_rnnip_isDefaults );
  rnnip_isDefaults(btag) = 0;

}


bool BTagHighLevelAugmenterAlg::jfIsDefaults(const xAOD::BTagging& btag) {
  AE::ConstAccessor<std::vector<ElementLink<xAOD::BTagVertexContainer> > > jf_vertices(m_acc_jf_vertices);
  AE::ConstAccessor<int> jf_nVtx(m_acc_jf_nVtx);
  AE::ConstAccessor<int> jf_nSingleTracks(m_acc_jf_nSingleTracks);
  return !(jf_vertices(btag).size() > 0 && (jf_nVtx(btag) > 0 || jf_nSingleTracks(btag) > 0));
}

}