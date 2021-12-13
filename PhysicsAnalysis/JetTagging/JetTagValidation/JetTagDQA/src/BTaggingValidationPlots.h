/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGDQA_BTagPLOTS_H
#define JETTAGDQA_BTagPLOTS_H
    
#include "xAODBase/IParticle.h"
#include "TrkValHistUtils/PlotBase.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBTagging/BTagging.h" 
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODJet/Jet.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
//#include "GAFlavourLabel.h"
#include "xAODEventInfo/EventInfo.h"

#include "InDetTrackSystematicsTools/InDetTrackTruthOriginDefs.h"

    
namespace JetTagDQA{
 
  class BTaggingValidationPlots:public PlotBase {
    public:
      // constructor
      BTaggingValidationPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);

      // fill methods
      void fillJetKinVars(const xAOD::Jet* jet, const int& truth_label, const bool& onZprime, const xAOD::EventInfo* event);
      void fillDiscriminantVariables(const xAOD::BTagging* btag, const xAOD::Jet* jet, const double& jet_Lxy, const int& truth_label, const bool& has_muon, const bool& onZprime, std::map<std::string, int>& nJetsThatPassedWPCuts, const xAOD::EventInfo* event);
      void fillMultiplicities(const unsigned int& nJets, const unsigned int& nTracks, const int& nPrimVtx, const unsigned int& nTracksPrimVtx, const unsigned int& nJetsWithMuon, const unsigned int& nJetsWithSV, std::map<std::string, int>& nJetsThatPassedWPCuts, const xAOD::EventInfo* event);
      void fillPVVariables(const double& PV_x, const double& PV_y, const double& PV_z, const xAOD::EventInfo* event);
      void fillOther(const xAOD::Jet* jet, const xAOD::BTagging* btag, bool& contains_muon, double& jet_Lxy, const int& truth_label, const xAOD::EventInfo* event); 
      void fillTrackVariables(const xAOD::Jet* jet, const xAOD::BTagging* btag, const xAOD::Vertex *myVertex, std::map<const xAOD::TrackParticle*, int> track_truth_associations, const bool& contains_muon, const int& truth_label, int& num_HF_tracks_in_jet, const xAOD::EventInfo* event); 
      void fillSVVariables(const xAOD::BTagging* btag, std::map<const xAOD::TrackParticle*, int> track_truth_associations, const bool& contains_muon, const int& truth_label, const int& num_HF_tracks_in_jet, bool& contains_SV, const xAOD::EventInfo* event); 

      void bookNJetsThatPassedWPCutsHistos();
      void initializeNJetsThatPassedWPCutsMap(std::map<std::string, int>& nJetsThatPassedWPCuts);
      void updateNJetsThatPassedWPCutsMap(std::map<std::string, int>& nJetsThatPassedWPCuts, const double& discr_IP3D, const double& discr_IP2D, const double& discr_RNNIP, const double& discr_SV1, const double& discr_IP3DSV1, const double& discr_JetFitter, const double& discr_MV2c10, const double& discr_DL1, const double& discr_DL1d, const double& discr_DL1r);
      void fillNJetsThatPassedWPCutsHistos(std::map<std::string, int>& nJetsThatPassedWPCuts, const xAOD::EventInfo* event);

      void setTaggerInfos();    
      void bookEffHistos();

      // Reco only information
      std::string m_sParticleType;

      // multiplicities
      TH1* m_nJets = nullptr;
      TH1* m_nTracks = nullptr;
      TH1* m_nPrimVtx = nullptr;
      TH1* m_nTracksPrimVtx = nullptr;
      TH1* m_nJetsWithMuon = nullptr;
      TH1* m_nJetsWithSV = nullptr;
      TH1* m_fracJetsWithMuon = nullptr;
      TH1* m_fracJetsWithSV = nullptr;

      // PV variables
      TH1* m_PV_x = nullptr;
      TH1* m_PV_y = nullptr;
      TH1* m_PV_z = nullptr;

      // jet kinematic variables
      TH1* m_jet_e = nullptr;
      TH1* m_jet_e_Zprime = nullptr;
      TH1* m_jet_pt = nullptr;
      TH1* m_jet_pt_Zprime = nullptr;
      TH1* m_jet_eta = nullptr;
      TH1* m_jet_phi = nullptr;

      // muon vars
      TH1* m_muon_pT_frac = nullptr;

      // truth info
      TH1* m_truthLabel = nullptr;

      TH1* m_jet_pt_b = nullptr;
      TH1* m_jet_pt_c = nullptr;
      TH1* m_jet_pt_l = nullptr;
      TH1* m_jet_pt_Zprime_b = nullptr;
      TH1* m_jet_pt_Zprime_c = nullptr;
      TH1* m_jet_pt_Zprime_l = nullptr;
      TH1* m_jet_eta_b = nullptr;
      TH1* m_jet_eta_c = nullptr;
      TH1* m_jet_eta_l = nullptr;


      // SV1 related vars
      TH1* m_SV1_numSVs_incl = nullptr;
      TH1* m_SV1_masssvx_incl = nullptr;
      TH1* m_SV1_N2Tpair_incl = nullptr;
      TH1* m_SV1_efracsvx_incl = nullptr;
      TH1* m_SV1_deltaR_incl = nullptr;
      TH1* m_SV1_significance3d_incl = nullptr;
      TH1* m_SV1_energyTrkInJet_incl = nullptr;
      TH1* m_SV1_NGTinSvx_incl = nullptr;
      TH1* m_SV1_Lxy_incl = nullptr;
      TH1* m_SV1_purity_incl = nullptr;
      TH1* m_SV1_numSVs_b = nullptr;
      TH1* m_SV1_masssvx_b = nullptr;
      TH1* m_SV1_N2Tpair_b = nullptr;
      TH1* m_SV1_efracsvx_b = nullptr;
      TH1* m_SV1_deltaR_b = nullptr;
      TH1* m_SV1_significance3d_b = nullptr;
      TH1* m_SV1_energyTrkInJet_b = nullptr;
      TH1* m_SV1_NGTinSvx_b = nullptr;
      TH1* m_SV1_Lxy_b = nullptr;
      TH1* m_SV1_purity_b = nullptr;
      TH1* m_SV1_numSVs_c = nullptr;
      TH1* m_SV1_masssvx_c = nullptr;
      TH1* m_SV1_N2Tpair_c = nullptr;
      TH1* m_SV1_efracsvx_c = nullptr;
      TH1* m_SV1_deltaR_c = nullptr;
      TH1* m_SV1_significance3d_c = nullptr;
      TH1* m_SV1_energyTrkInJet_c = nullptr;
      TH1* m_SV1_NGTinSvx_c = nullptr;
      TH1* m_SV1_Lxy_c = nullptr;
      TH1* m_SV1_purity_c = nullptr;
      TH1* m_SV1_numSVs_l = nullptr;
      TH1* m_SV1_masssvx_l = nullptr;
      TH1* m_SV1_N2Tpair_l = nullptr;
      TH1* m_SV1_efracsvx_l = nullptr;
      TH1* m_SV1_deltaR_l = nullptr;
      TH1* m_SV1_significance3d_l = nullptr;
      TH1* m_SV1_energyTrkInJet_l = nullptr;
      TH1* m_SV1_NGTinSvx_l = nullptr;
      TH1* m_SV1_Lxy_l = nullptr;
      TH1* m_SV1_purity_l = nullptr;
      TH1* m_SV1_numSVs_muon = nullptr;
      TH1* m_SV1_masssvx_muon = nullptr;
      TH1* m_SV1_N2Tpair_muon = nullptr;
      TH1* m_SV1_efracsvx_muon = nullptr;
      TH1* m_SV1_deltaR_muon = nullptr;
      TH1* m_SV1_significance3d_muon = nullptr;
      TH1* m_SV1_energyTrkInJet_muon = nullptr;
      TH1* m_SV1_NGTinSvx_muon = nullptr;
      TH1* m_SV1_Lxy_muon = nullptr;
      TH1* m_SV1_purity_muon = nullptr;

      TH1* m_SV1_fracHFTracksInJet_incl = nullptr;
      TH1* m_SV1_fracHFTracksInJet_b = nullptr;
      TH1* m_SV1_fracHFTracksInJet_c = nullptr;
      TH1* m_SV1_fracHFTracksInJet_l = nullptr;
      TH1* m_SV1_fracHFTracksInJet_muon = nullptr;
      
      TH1* m_SV1_fracTracks_fromB_incl = nullptr;
      TH1* m_SV1_fracTracks_fromB_b = nullptr;
      TH1* m_SV1_fracTracks_fromB_c = nullptr;
      TH1* m_SV1_fracTracks_fromB_l = nullptr;
      TH1* m_SV1_fracTracks_fromB_muon = nullptr;
      TH1* m_SV1_fracTracks_fromC_incl = nullptr;
      TH1* m_SV1_fracTracks_fromC_b = nullptr;
      TH1* m_SV1_fracTracks_fromC_c = nullptr;
      TH1* m_SV1_fracTracks_fromC_l = nullptr;
      TH1* m_SV1_fracTracks_fromC_muon = nullptr;
      TH1* m_SV1_fracTracks_fromFragmentation_incl = nullptr;
      TH1* m_SV1_fracTracks_fromFragmentation_b = nullptr;
      TH1* m_SV1_fracTracks_fromFragmentation_c = nullptr;
      TH1* m_SV1_fracTracks_fromFragmentation_l = nullptr;
      TH1* m_SV1_fracTracks_fromFragmentation_muon = nullptr;
      TH1* m_SV1_fracTracks_fromSecondaries_incl = nullptr;
      TH1* m_SV1_fracTracks_fromSecondaries_b = nullptr;
      TH1* m_SV1_fracTracks_fromSecondaries_c = nullptr;
      TH1* m_SV1_fracTracks_fromSecondaries_l = nullptr;
      TH1* m_SV1_fracTracks_fromSecondaries_muon = nullptr;
      TH1* m_SV1_fracTracks_fromPileup_incl = nullptr;
      TH1* m_SV1_fracTracks_fromPileup_b = nullptr;
      TH1* m_SV1_fracTracks_fromPileup_c = nullptr;
      TH1* m_SV1_fracTracks_fromPileup_l = nullptr;
      TH1* m_SV1_fracTracks_fromPileup_muon = nullptr;
      TH1* m_SV1_fracTracks_fromFake_incl = nullptr;
      TH1* m_SV1_fracTracks_fromFake_b = nullptr;
      TH1* m_SV1_fracTracks_fromFake_c = nullptr;
      TH1* m_SV1_fracTracks_fromFake_l = nullptr;
      TH1* m_SV1_fracTracks_fromFake_muon = nullptr;

      // these are only filled for detail level above 10
      TH1* m_SV1_fracTracks_Secondaries_KshortDecay_incl = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_KshortDecay_b = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_KshortDecay_c = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_KshortDecay_u = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_KshortDecay_muon = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_LambdaDecay_incl = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_LambdaDecay_b = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_LambdaDecay_c = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_LambdaDecay_u = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_LambdaDecay_muon = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_GammaConversion_incl = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_GammaConversion_b = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_GammaConversion_c = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_GammaConversion_u = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_GammaConversion_muon = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_OtherDecay_incl = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_OtherDecay_b = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_OtherDecay_c = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_OtherDecay_u = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_OtherDecay_muon = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_HadronicInteraction_incl = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_HadronicInteraction_b = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_HadronicInteraction_c = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_HadronicInteraction_u = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_HadronicInteraction_muon = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_OtherSecondary_incl = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_OtherSecondary_b = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_OtherSecondary_c = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_OtherSecondary_u = nullptr; 
      TH1* m_SV1_fracTracks_Secondaries_OtherSecondary_muon = nullptr; 
      TH1* m_SV1_fracTracks_OtherOrigin_incl = nullptr; 
      TH1* m_SV1_fracTracks_OtherOrigin_b = nullptr; 
      TH1* m_SV1_fracTracks_OtherOrigin_c = nullptr; 
      TH1* m_SV1_fracTracks_OtherOrigin_u = nullptr; 
      TH1* m_SV1_fracTracks_OtherOrigin_muon = nullptr; 

      // JetFitter related vars
      TH1* m_JetFitter_N2Tpair_incl = nullptr;
      TH1* m_JetFitter_nVTX_incl = nullptr;
      TH1* m_JetFitter_nSingleTracks_incl = nullptr;
      TH1* m_JetFitter_nTracksAtVtx_incl = nullptr;
      TH1* m_JetFitter_mass_incl = nullptr;
      TH1* m_JetFitter_energyFraction_incl = nullptr;
      TH1* m_JetFitter_significance3d_incl = nullptr;
      TH1* m_JetFitter_purity_incl = nullptr;
      TH1* m_JetFitter_N2Tpair_b = nullptr;
      TH1* m_JetFitter_nVTX_b = nullptr;
      TH1* m_JetFitter_nSingleTracks_b = nullptr;
      TH1* m_JetFitter_nTracksAtVtx_b = nullptr;
      TH1* m_JetFitter_mass_b = nullptr;
      TH1* m_JetFitter_energyFraction_b = nullptr;
      TH1* m_JetFitter_significance3d_b = nullptr;
      TH1* m_JetFitter_purity_b = nullptr;
      TH1* m_JetFitter_N2Tpair_c = nullptr;
      TH1* m_JetFitter_nVTX_c = nullptr;
      TH1* m_JetFitter_nSingleTracks_c = nullptr;
      TH1* m_JetFitter_nTracksAtVtx_c = nullptr;
      TH1* m_JetFitter_mass_c = nullptr;
      TH1* m_JetFitter_energyFraction_c = nullptr;
      TH1* m_JetFitter_significance3d_c = nullptr;
      TH1* m_JetFitter_purity_c = nullptr;
      TH1* m_JetFitter_N2Tpair_l = nullptr;
      TH1* m_JetFitter_nVTX_l = nullptr;
      TH1* m_JetFitter_nSingleTracks_l = nullptr;
      TH1* m_JetFitter_nTracksAtVtx_l = nullptr;
      TH1* m_JetFitter_mass_l = nullptr;
      TH1* m_JetFitter_energyFraction_l = nullptr;
      TH1* m_JetFitter_significance3d_l = nullptr;
      TH1* m_JetFitter_purity_l = nullptr;
      TH1* m_JetFitter_N2Tpair_muon = nullptr;
      TH1* m_JetFitter_nVTX_muon = nullptr;
      TH1* m_JetFitter_nSingleTracks_muon = nullptr;
      TH1* m_JetFitter_nTracksAtVtx_muon = nullptr;
      TH1* m_JetFitter_mass_muon = nullptr;
      TH1* m_JetFitter_energyFraction_muon = nullptr;
      TH1* m_JetFitter_significance3d_muon = nullptr;
      TH1* m_JetFitter_purity_muon = nullptr;

      // IPs and IP significances
      TH1* m_track_d0_incl = nullptr;
      TH1* m_track_z0_incl = nullptr;
      TH1* m_track_sigd0_incl = nullptr;
      TH1* m_track_sigz0_incl = nullptr;

      TH1* m_track_d0_b = nullptr;
      TH1* m_track_z0_b = nullptr;
      TH1* m_track_sigd0_b = nullptr;
      TH1* m_track_sigz0_b = nullptr;

      TH1* m_track_d0_c = nullptr;
      TH1* m_track_z0_c = nullptr;
      TH1* m_track_sigd0_c = nullptr;
      TH1* m_track_sigz0_c = nullptr;

      TH1* m_track_d0_u = nullptr;
      TH1* m_track_z0_u = nullptr;
      TH1* m_track_sigd0_u = nullptr;
      TH1* m_track_sigz0_u = nullptr;

      TH1* m_track_d0_muon = nullptr;
      TH1* m_track_z0_muon = nullptr;
      TH1* m_track_sigd0_muon = nullptr;
      TH1* m_track_sigz0_muon = nullptr;

      // pT_frac
      TH1* m_track_pT_frac_incl = nullptr;
      TH1* m_track_pT_frac_b = nullptr;
      TH1* m_track_pT_frac_c = nullptr;
      TH1* m_track_pT_frac_u = nullptr;
      TH1* m_track_pT_frac_muon = nullptr;

      // DeltaR_jet_track
      TH1* m_DeltaR_jet_track_incl = nullptr;
      TH1* m_DeltaR_jet_track_b = nullptr;
      TH1* m_DeltaR_jet_track_c = nullptr;
      TH1* m_DeltaR_jet_track_u = nullptr;
      TH1* m_DeltaR_jet_track_muon = nullptr;

      // numTracks_perJet
      TH1* m_numTracks_perJet_incl = nullptr;
      TH1* m_numTracks_perJet_b = nullptr;
      TH1* m_numTracks_perJet_c = nullptr;
      TH1* m_numTracks_perJet_u = nullptr;
      TH1* m_numTracks_perJet_muon = nullptr;

      // number of tracks variables
      TH1* m_numTracks_B_incl = nullptr; 
      TH1* m_numTracks_C_incl = nullptr; 
      TH1* m_numTracks_Fragmentation_incl = nullptr; 
      TH1* m_numTracks_Secondaries_incl = nullptr; 
      TH1* m_numTracks_Pileup_incl = nullptr; 
      TH1* m_numTracks_Fake_incl = nullptr; 

      TH1* m_numTracks_B_b = nullptr; 
      TH1* m_numTracks_C_b = nullptr; 
      TH1* m_numTracks_Fragmentation_b = nullptr; 
      TH1* m_numTracks_Secondaries_b = nullptr; 
      TH1* m_numTracks_Pileup_b = nullptr; 
      TH1* m_numTracks_Fake_b = nullptr; 

      TH1* m_numTracks_B_c = nullptr; 
      TH1* m_numTracks_C_c = nullptr; 
      TH1* m_numTracks_Fragmentation_c = nullptr; 
      TH1* m_numTracks_Secondaries_c = nullptr; 
      TH1* m_numTracks_Pileup_c = nullptr; 
      TH1* m_numTracks_Fake_c = nullptr; 

      TH1* m_numTracks_B_u = nullptr; 
      TH1* m_numTracks_C_u = nullptr; 
      TH1* m_numTracks_Fragmentation_u = nullptr; 
      TH1* m_numTracks_Secondaries_u = nullptr; 
      TH1* m_numTracks_Pileup_u = nullptr; 
      TH1* m_numTracks_Fake_u = nullptr; 

      TH1* m_numTracks_B_muon = nullptr; 
      TH1* m_numTracks_C_muon = nullptr; 
      TH1* m_numTracks_Fragmentation_muon = nullptr; 
      TH1* m_numTracks_Secondaries_muon = nullptr; 
      TH1* m_numTracks_Pileup_muon = nullptr; 
      TH1* m_numTracks_Fake_muon = nullptr; 

      // these are only filled for detail level above 10
      TH1* m_numTracks_Secondaries_KshortDecay_incl = nullptr; 
      TH1* m_numTracks_Secondaries_KshortDecay_b = nullptr; 
      TH1* m_numTracks_Secondaries_KshortDecay_c = nullptr; 
      TH1* m_numTracks_Secondaries_KshortDecay_u = nullptr; 
      TH1* m_numTracks_Secondaries_KshortDecay_muon = nullptr; 
  
      TH1* m_numTracks_Secondaries_LambdaDecay_incl = nullptr; 
      TH1* m_numTracks_Secondaries_LambdaDecay_b = nullptr; 
      TH1* m_numTracks_Secondaries_LambdaDecay_c = nullptr; 
      TH1* m_numTracks_Secondaries_LambdaDecay_u = nullptr; 
      TH1* m_numTracks_Secondaries_LambdaDecay_muon = nullptr; 
  
      TH1* m_numTracks_Secondaries_GammaConversion_incl = nullptr; 
      TH1* m_numTracks_Secondaries_GammaConversion_b = nullptr; 
      TH1* m_numTracks_Secondaries_GammaConversion_c = nullptr; 
      TH1* m_numTracks_Secondaries_GammaConversion_u = nullptr; 
      TH1* m_numTracks_Secondaries_GammaConversion_muon = nullptr; 
  
      TH1* m_numTracks_Secondaries_OtherDecay_incl = nullptr; 
      TH1* m_numTracks_Secondaries_OtherDecay_b = nullptr; 
      TH1* m_numTracks_Secondaries_OtherDecay_c = nullptr; 
      TH1* m_numTracks_Secondaries_OtherDecay_u = nullptr; 
      TH1* m_numTracks_Secondaries_OtherDecay_muon = nullptr; 
  
      TH1* m_numTracks_Secondaries_HadronicInteraction_incl = nullptr; 
      TH1* m_numTracks_Secondaries_HadronicInteraction_b = nullptr; 
      TH1* m_numTracks_Secondaries_HadronicInteraction_c = nullptr; 
      TH1* m_numTracks_Secondaries_HadronicInteraction_u = nullptr; 
      TH1* m_numTracks_Secondaries_HadronicInteraction_muon = nullptr; 
  
      TH1* m_numTracks_Secondaries_OtherSecondary_incl = nullptr; 
      TH1* m_numTracks_Secondaries_OtherSecondary_b = nullptr; 
      TH1* m_numTracks_Secondaries_OtherSecondary_c = nullptr; 
      TH1* m_numTracks_Secondaries_OtherSecondary_u = nullptr; 
      TH1* m_numTracks_Secondaries_OtherSecondary_muon = nullptr; 
  
      TH1* m_numTracks_OtherOrigin_incl = nullptr; 
      TH1* m_numTracks_OtherOrigin_b = nullptr; 
      TH1* m_numTracks_OtherOrigin_c = nullptr; 
      TH1* m_numTracks_OtherOrigin_u = nullptr; 
      TH1* m_numTracks_OtherOrigin_muon = nullptr; 

      // tracker hits
      TH1* m_nInnHits_incl = nullptr;
      TH1* m_nNextToInnHits_incl = nullptr;
      TH1* m_nBLHits_incl = nullptr;
      TH1* m_nsharedBLHits_incl = nullptr;
      TH1* m_nsplitBLHits_incl = nullptr;
      TH1* m_nPixHits_incl = nullptr;
      TH1* m_nPixHoles_incl = nullptr;
      TH1* m_nsharedPixHits_incl = nullptr;
      TH1* m_nsplitPixHits_incl = nullptr;
      TH1* m_nSCTHits_incl = nullptr;
      TH1* m_nSCTHoles_incl = nullptr;
      TH1* m_nsharedSCTHits_incl = nullptr;

      TH1* m_nInnHits_b = nullptr;
      TH1* m_nNextToInnHits_b = nullptr;
      TH1* m_nBLHits_b = nullptr;
      TH1* m_nsharedBLHits_b = nullptr;
      TH1* m_nsplitBLHits_b = nullptr;
      TH1* m_nPixHits_b = nullptr;
      TH1* m_nPixHoles_b = nullptr;
      TH1* m_nsharedPixHits_b = nullptr;
      TH1* m_nsplitPixHits_b = nullptr;
      TH1* m_nSCTHits_b = nullptr;
      TH1* m_nSCTHoles_b = nullptr;
      TH1* m_nsharedSCTHits_b = nullptr;

      TH1* m_nInnHits_c = nullptr;
      TH1* m_nNextToInnHits_c = nullptr;
      TH1* m_nBLHits_c = nullptr;
      TH1* m_nsharedBLHits_c = nullptr;
      TH1* m_nsplitBLHits_c = nullptr;
      TH1* m_nPixHits_c = nullptr;
      TH1* m_nPixHoles_c = nullptr;
      TH1* m_nsharedPixHits_c = nullptr;
      TH1* m_nsplitPixHits_c = nullptr;
      TH1* m_nSCTHits_c = nullptr;
      TH1* m_nSCTHoles_c = nullptr;
      TH1* m_nsharedSCTHits_c = nullptr;

      TH1* m_nInnHits_u = nullptr;
      TH1* m_nNextToInnHits_u = nullptr;
      TH1* m_nBLHits_u = nullptr;
      TH1* m_nsharedBLHits_u = nullptr;
      TH1* m_nsplitBLHits_u = nullptr;
      TH1* m_nPixHits_u = nullptr;
      TH1* m_nPixHoles_u = nullptr;
      TH1* m_nsharedPixHits_u = nullptr;
      TH1* m_nsplitPixHits_u = nullptr;
      TH1* m_nSCTHits_u = nullptr;
      TH1* m_nSCTHoles_u = nullptr;
      TH1* m_nsharedSCTHits_u = nullptr;

      TH1* m_nInnHits_muon = nullptr;
      TH1* m_nNextToInnHits_muon = nullptr;
      TH1* m_nBLHits_muon = nullptr;
      TH1* m_nsharedBLHits_muon = nullptr;
      TH1* m_nsplitBLHits_muon = nullptr;
      TH1* m_nPixHits_muon = nullptr;
      TH1* m_nPixHoles_muon = nullptr;
      TH1* m_nsharedPixHits_muon = nullptr;
      TH1* m_nsplitPixHits_muon = nullptr;
      TH1* m_nSCTHits_muon = nullptr;
      TH1* m_nSCTHoles_muon = nullptr;
      TH1* m_nsharedSCTHits_muon = nullptr;

      // tagger
      TH1* m_IP3D_pb = nullptr;
      TH1* m_IP3D_pc = nullptr;
      TH1* m_IP3D_pu = nullptr;

      TH1* m_RNNIP_pb = nullptr;
      TH1* m_RNNIP_pc = nullptr;
      TH1* m_RNNIP_pu = nullptr;

      TH1* m_SV1_pb = nullptr;
      TH1* m_SV1_pc = nullptr;
      TH1* m_SV1_pu = nullptr;
      
      TH1* m_DL1_pb = nullptr;
      TH1* m_DL1_pc = nullptr;
      TH1* m_DL1_pu = nullptr;
      TH1* m_DL1d_pb = nullptr;
      TH1* m_DL1d_pc = nullptr;
      TH1* m_DL1d_pu = nullptr;
      TH1* m_DL1r_pb = nullptr;
      TH1* m_DL1r_pc = nullptr;
      TH1* m_DL1r_pu = nullptr;

      // TH1* m_SV0_NGTinSvx = nullptr;
      TH1* m_nGTinSV1_incl = nullptr;
      TH1* m_nGTinSV1_b = nullptr;
      TH1* m_nGTinSV1_c = nullptr;
      TH1* m_nGTinSV1_l = nullptr;
      TH1* m_nGTinSV1_muon = nullptr;

      TH1* m_IP2D_nTracks_incl = nullptr;
      TH1* m_IP2D_nTracks_b = nullptr;
      TH1* m_IP2D_nTracks_c = nullptr;
      TH1* m_IP2D_nTracks_l = nullptr;
      TH1* m_IP2D_nTracks_muon = nullptr;

      TH1* m_IP3D_nTracks_incl = nullptr;
      TH1* m_IP3D_nTracks_b = nullptr;
      TH1* m_IP3D_nTracks_c = nullptr;
      TH1* m_IP3D_nTracks_l = nullptr;
      TH1* m_IP3D_nTracks_muon = nullptr;

      TH1* m_IP3D_gradeOfTracks_incl = nullptr;
      TH1* m_IP2D_gradeOfTracks_incl = nullptr;
      TH1* m_IP3D_gradeOfTracks_b = nullptr;
      TH1* m_IP2D_gradeOfTracks_b = nullptr;
      TH1* m_IP3D_gradeOfTracks_c = nullptr;
      TH1* m_IP2D_gradeOfTracks_c = nullptr;
      TH1* m_IP3D_gradeOfTracks_l = nullptr;
      TH1* m_IP2D_gradeOfTracks_l = nullptr;
      TH1* m_IP3D_gradeOfTracks_muon = nullptr;
      TH1* m_IP2D_gradeOfTracks_muon = nullptr;

      TH1* m_tmpD0 = nullptr;
      TH1* m_tmpZ0 = nullptr;
      TH1* m_tmpD0sig = nullptr;
      TH1* m_tmpZ0sig = nullptr;
      TH1* m_IP3D_weightBofTracks = nullptr;
      TH1* m_IP3D_weightCofTracks = nullptr;
      TH1* m_IP3D_weightUofTracks = nullptr;
      TH1* m_IP2D_weightBofTracks = nullptr;
      TH1* m_IP2D_weightCofTracks = nullptr;
      TH1* m_IP2D_weightUofTracks = nullptr;

      // B hadron Lxy
      TH1* m_Truth_Lxy_b = nullptr;
      TH1* m_Truth_Lxy_c = nullptr;
      
      // B hadron deltaR wrt jet 
      TH1* m_deltaR_truthBHadron_jet_b = nullptr;
      TH1* m_deltaR_truthCHadron_jet_c = nullptr;
      
      std::vector<std::string> m_taggers;
      std::map<std::string, int> m_truthLabels;
      std::map<std::string, double> m_IP3D_workingPoints;
      std::map<std::string, double> m_IP2D_workingPoints;
      std::map<std::string, double> m_RNNIP_workingPoints;
      std::map<std::string, double> m_SV1_workingPoints;
      std::map<std::string, double> m_IP3DSV1_workingPoints;
      std::map<std::string, double> m_JetFitter_workingPoints;
      std::map<std::string, double> m_MV2c10_workingPoints;
      std::map<std::string, double> m_DL1_workingPoints;
      std::map<std::string, double> m_DL1d_workingPoints;
      std::map<std::string, double> m_DL1r_workingPoints;
      double m_RNNIP_fc;
      double m_DL1_fc;
      double m_DL1d_fc;
      double m_DL1r_fc;
      std::map<std::string, TH1*> m_weight_histos; 

      std::map<std::string, TH1*> m_nJetsThatPassedWPCutsHistos; 

      // detail level
      void setDetailLevel(const unsigned int& detailLevel);

      // a setter for the HistogramDefinitions and the jvt and TMP cuts
      void setHistogramDefinitions( std::map< std::string, std::vector< std::string > > HistogramDefinitions);
      void setIsDataJVTCutsAndTMPCut(bool isData, float JVTCutAntiKt4EMTopoJets, float JVTCutLargerEtaAntiKt4EMTopoJets, float JVTCutAntiKt4EMPFlowJets, float truthMatchProbabilityCut);

      // jvt variables 
      bool m_JVT_defined;
      float m_JVT_cut;
      bool m_JVTLargerEta_defined;
      float m_JVTLargerEta_cut;

    private:
      virtual void initializePlots();     
      virtual void finalizePlots(); 

      // detail level
      unsigned int m_detailLevel;

      // map with histogram definitions and the corresponding enum
      std::map< std::string, std::vector< std::string > > m_HistogramDefinitions;
      enum position{histo_name, histo_title, histo_path, histo_xbins, histo_xmin, histo_xmax, histo_type, histo_ymin, histo_ymax};
      float m_truthMatchProbabilityCut;
      bool m_isData;
      // some helper functions
      TH1* bookHistogram(std::string histo_name, std::string var_name, std::string part = "", std::string prefix = "");
      int getTrackHits(const xAOD::TrackParticle& part, xAOD::SummaryType info);
      void fillDiscriminantHistograms(const std::string& tagger_name, const double& discriminant_value, const std::map<std::string, double>& working_points, const int& truth_label, std::map<std::string, TH1*>::const_iterator hist_iter, std::map<std::string, int>::const_iterator label_iter, const bool& pass_nTracksCut, const double& jet_pT, const double& jet_Lxy, const bool& onZprime, const xAOD::EventInfo* event);
      void bookDiscriminantVsPTAndLxyHistograms(const std::string& tagger_name, const std::map<std::string, double>& workingPoints, const bool& isOldTagger, std::map<std::string, int>::const_iterator label_iter, const std::string& m_sParticleType);
      template <class T>
      void fillHistoWithTruthCases(T value, TH1* histo_incl, TH1* histo_b, TH1* histo_c, TH1* histo_l, TH1* histo_muon, const int& truth_label, const bool& has_muon, const xAOD::EventInfo* event);

    protected:
      // Log a message using the Athena controlled logging system
      MsgStream& msg(MSG::Level lvl) const {
        return m_msg.get() << lvl;
      }
      // Check whether the logging system is active at the provided verbosity level
      bool msgLvl(MSG::Level lvl) {
        return m_msg.get().level() <= lvl;
      }
      // Private message stream member
      mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;

  };
    
}
    
#endif
