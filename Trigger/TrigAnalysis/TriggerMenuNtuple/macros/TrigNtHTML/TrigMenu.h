/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Dec  3 18:24:37 2009 by ROOT version 5.22/00d
// from TTree TrigMenu/Trigger menu information
// found on file: trigntupleTTBAR.root
//////////////////////////////////////////////////////////

#ifndef TrigMenu_h
#define TrigMenu_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class TrigMenu {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
 //TrigDecSummary  *TrigDecSummary;
   vector<unsigned int> mL1_TBP;
   vector<unsigned int> mL1_TAP;
   vector<unsigned int> mL1_TAV;
   vector<unsigned int> mL2_Raw;
   vector<unsigned int> mL2_PT;
   vector<unsigned int> mEF_Raw;
   vector<unsigned int> mEF_PT;
   vector<unsigned int> *L1MuonRoI_RoIWord;
   vector<float>   *L1MuonRoI_pt;
   vector<float>   *L1MuonRoI_eta;
   vector<float>   *L1MuonRoI_phi;
   vector<unsigned int> *L1EmTauRoI_RoIWord;
   vector<float>   *L1EmTauRoI_pt;
   vector<float>   *L1EmTauRoI_eta;
   vector<float>   *L1EmTauRoI_phi;
   vector<float>   *L1EmTauRoI_Core;
   vector<float>   *L1EmTauRoI_EmClus;
   vector<float>   *L1EmTauRoI_TauClus;
   vector<float>   *L1EmTauRoI_EmIsol;
   vector<float>   *L1EmTauRoI_HadIsol;
   vector<float>   *L1EmTauRoI_EmCore;
   vector<unsigned int> *L1JetRoI_RoIWord;
   vector<float>   *L1JetRoI_pt;
   vector<float>   *L1JetRoI_eta;
   vector<float>   *L1JetRoI_phi;
   vector<float>   *L1JetRoI_4x4;
   vector<float>   *L1JetRoI_6x6;
   vector<float>   *L1JetRoI_8x8;
   vector<int>     *Chain_Id;
   vector<vector<int> > *Chain_RoIType;
   vector<vector<int> > *Chain_RoIIndex;
   vector<int>     *RoI_L2_mu_type;
   vector<int>     *RoI_L2_mu_MuonFeature;
   vector<int>     *RoI_L2_mu_MuonFeatureStatus;
   vector<int>     *RoI_L2_mu_CombinedMuonFeature;
   vector<int>     *RoI_L2_mu_CombinedMuonFeatureStatus;
   vector<int>     *RoI_L2_mu_Muon_ROI;
   vector<int>     *RoI_L2_mu_Muon_ROIStatus;
   vector<vector<int> > *RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon;
   vector<vector<int> > *RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus;
   vector<vector<int> > *RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon;
   vector<vector<int> > *RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus;
   vector<int>     *RoI_EF_mu_type;
   vector<int>     *RoI_EF_mu_Muon_ROI;
   vector<int>     *RoI_EF_mu_Muon_ROIStatus;
   vector<vector<int> > *RoI_EF_mu_TrigMuonEFInfoContainer;
   vector<vector<int> > *RoI_EF_mu_TrigMuonEFInfoContainerStatus;
   vector<int>     *RoI_L2_TileMu_type;
   vector<int>     *RoI_L2_TileMu_TileMuFeature;
   vector<int>     *RoI_L2_TileMu_TileMuFeatureStatus;
   vector<int>     *RoI_L2_TileMu_TileTrackMuFeature;
   vector<int>     *RoI_L2_TileMu_TileTrackMuFeatureStatus;
   vector<int>     *RoI_L2_TileMu_Muon_ROI;
   vector<int>     *RoI_L2_TileMu_Muon_ROIStatus;
   vector<vector<int> > *RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Muon;
   vector<vector<int> > *RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus;
   vector<vector<int> > *RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Muon;
   vector<vector<int> > *RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus;
   vector<int>     *RoI_EF_TileMu_type;
   vector<int>     *RoI_EF_TileMu_Muon_ROI;
   vector<int>     *RoI_EF_TileMu_Muon_ROIStatus;
   vector<vector<int> > *RoI_EF_TileMu_TrigMuonEFContainer;
   vector<vector<int> > *RoI_EF_TileMu_TrigMuonEFContainerStatus;
   vector<vector<int> > *RoI_EF_TileMu_TrigMuonEFInfoContainer;
   vector<vector<int> > *RoI_EF_TileMu_TrigMuonEFInfoContainerStatus;
   vector<int>     *RoI_L2_e_type;
   vector<int>     *RoI_L2_e_TrigEMCluster;
   vector<int>     *RoI_L2_e_TrigEMClusterStatus;
   vector<int>     *RoI_L2_e_EmTau_ROI;
   vector<int>     *RoI_L2_e_EmTau_ROIStatus;
   vector<vector<int> > *RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma;
   vector<vector<int> > *RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus;
   vector<vector<int> > *RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma;
   vector<vector<int> > *RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus;
   vector<vector<int> > *RoI_L2_e_TrigElectronContainer;
   vector<vector<int> > *RoI_L2_e_TrigElectronContainerStatus;
   vector<int>     *RoI_EF_e_type;
   vector<int>     *RoI_EF_e_EmTau_ROI;
   vector<int>     *RoI_EF_e_EmTau_ROIStatus;
   vector<int>     *RoI_L2_tau_type;
   vector<int>     *RoI_L2_tau_TrigRoiDescriptor;
   vector<int>     *RoI_L2_tau_TrigRoiDescriptorStatus;
   vector<int>     *RoI_L2_tau_TrigTauCluster;
   vector<int>     *RoI_L2_tau_TrigTauClusterStatus;
   vector<int>     *RoI_L2_tau_TrigTauClusterDetails;
   vector<int>     *RoI_L2_tau_TrigTauClusterDetailsStatus;
   vector<int>     *RoI_L2_tau_TrigTauTracksInfo;
   vector<int>     *RoI_L2_tau_TrigTauTracksInfoStatus;
   vector<int>     *RoI_L2_tau_TrigTau;
   vector<int>     *RoI_L2_tau_TrigTauStatus;
   vector<int>     *RoI_L2_tau_EmTau_ROI;
   vector<int>     *RoI_L2_tau_EmTau_ROIStatus;
   vector<vector<int> > *RoI_L2_tau_TrigInDetTrackCollection;
   vector<vector<int> > *RoI_L2_tau_TrigInDetTrackCollectionStatus;
   vector<int>     *RoI_EF_tau_type;
   vector<int>     *RoI_EF_tau_EmTau_ROI;
   vector<int>     *RoI_EF_tau_EmTau_ROIStatus;
   vector<int>     *RoI_L2_j_type;
   vector<int>     *RoI_L2_j_TrigT2Jet;
   vector<int>     *RoI_L2_j_TrigT2JetStatus;
   vector<int>     *RoI_L2_j_Jet_ROI;
   vector<int>     *RoI_L2_j_Jet_ROIStatus;
   vector<int>     *RoI_EF_j_type;
   vector<vector<int> > *RoI_EF_j_JetCollection;
   vector<vector<int> > *RoI_EF_j_JetCollectionStatus;
   Int_t           NL2Jets;
   vector<float>   *TrigT2Jet_et;
   vector<float>   *TrigT2Jet_ehad0;
   vector<float>   *TrigT2Jet_eem0;
   vector<float>   *TrigT2Jet_eta;
   vector<float>   *TrigT2Jet_phi;
   Int_t           NEFJets;
   vector<float>   *Jet_EF_et;
   vector<float>   *Jet_EF_eta;
   vector<float>   *Jet_EF_phi;

   // List of branches
   TBranch        *b_TrigDecSummary_mL1_TBP;   //!
   TBranch        *b_TrigDecSummary_mL1_TAP;   //!
   TBranch        *b_TrigDecSummary_mL1_TAV;   //!
   TBranch        *b_TrigDecSummary_mL2_Raw;   //!
   TBranch        *b_TrigDecSummary_mL2_PT;   //!
   TBranch        *b_TrigDecSummary_mEF_Raw;   //!
   TBranch        *b_TrigDecSummary_mEF_PT;   //!
   TBranch        *b_L1MuonRoI_RoIWord;   //!
   TBranch        *b_L1MuonRoI_pt;   //!
   TBranch        *b_L1MuonRoI_eta;   //!
   TBranch        *b_L1MuonRoI_phi;   //!
   TBranch        *b_L1EmTauRoI_RoIWord;   //!
   TBranch        *b_L1EmTauRoI_pt;   //!
   TBranch        *b_L1EmTauRoI_eta;   //!
   TBranch        *b_L1EmTauRoI_phi;   //!
   TBranch        *b_L1EmTauRoI_Core;   //!
   TBranch        *b_L1EmTauRoI_EmClus;   //!
   TBranch        *b_L1EmTauRoI_TauClus;   //!
   TBranch        *b_L1EmTauRoI_EmIsol;   //!
   TBranch        *b_L1EmTauRoI_HadIsol;   //!
   TBranch        *b_L1EmTauRoI_EmCore;   //!
   TBranch        *b_L1JetRoI_RoIWord;   //!
   TBranch        *b_L1JetRoI_pt;   //!
   TBranch        *b_L1JetRoI_eta;   //!
   TBranch        *b_L1JetRoI_phi;   //!
   TBranch        *b_L1JetRoI_4x4;   //!
   TBranch        *b_L1JetRoI_6x6;   //!
   TBranch        *b_L1JetRoI_8x8;   //!
   TBranch        *b_Chain_Id;   //!
   TBranch        *b_Chain_RoIType;   //!
   TBranch        *b_Chain_RoIIndex;   //!
   TBranch        *b_RoI_L2_mu_type;   //!
   TBranch        *b_RoI_L2_mu_MuonFeature;   //!
   TBranch        *b_RoI_L2_mu_MuonFeatureStatus;   //!
   TBranch        *b_RoI_L2_mu_CombinedMuonFeature;   //!
   TBranch        *b_RoI_L2_mu_CombinedMuonFeatureStatus;   //!
   TBranch        *b_RoI_L2_mu_Muon_ROI;   //!
   TBranch        *b_RoI_L2_mu_Muon_ROIStatus;   //!
   TBranch        *b_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon;   //!
   TBranch        *b_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus;   //!
   TBranch        *b_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon;   //!
   TBranch        *b_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus;   //!
   TBranch        *b_RoI_EF_mu_type;   //!
   TBranch        *b_RoI_EF_mu_Muon_ROI;   //!
   TBranch        *b_RoI_EF_mu_Muon_ROIStatus;   //!
   TBranch        *b_RoI_EF_mu_TrigMuonEFInfoContainer;   //!
   TBranch        *b_RoI_EF_mu_TrigMuonEFInfoContainerStatus;   //!
   TBranch        *b_RoI_L2_TileMu_type;   //!
   TBranch        *b_RoI_L2_TileMu_TileMuFeature;   //!
   TBranch        *b_RoI_L2_TileMu_TileMuFeatureStatus;   //!
   TBranch        *b_RoI_L2_TileMu_TileTrackMuFeature;   //!
   TBranch        *b_RoI_L2_TileMu_TileTrackMuFeatureStatus;   //!
   TBranch        *b_RoI_L2_TileMu_Muon_ROI;   //!
   TBranch        *b_RoI_L2_TileMu_Muon_ROIStatus;   //!
   TBranch        *b_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Muon;   //!
   TBranch        *b_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus;   //!
   TBranch        *b_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Muon;   //!
   TBranch        *b_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus;   //!
   TBranch        *b_RoI_EF_TileMu_type;   //!
   TBranch        *b_RoI_EF_TileMu_Muon_ROI;   //!
   TBranch        *b_RoI_EF_TileMu_Muon_ROIStatus;   //!
   TBranch        *b_RoI_EF_TileMu_TrigMuonEFContainer;   //!
   TBranch        *b_RoI_EF_TileMu_TrigMuonEFContainerStatus;   //!
   TBranch        *b_RoI_EF_TileMu_TrigMuonEFInfoContainer;   //!
   TBranch        *b_RoI_EF_TileMu_TrigMuonEFInfoContainerStatus;   //!
   TBranch        *b_RoI_L2_e_type;   //!
   TBranch        *b_RoI_L2_e_TrigEMCluster;   //!
   TBranch        *b_RoI_L2_e_TrigEMClusterStatus;   //!
   TBranch        *b_RoI_L2_e_EmTau_ROI;   //!
   TBranch        *b_RoI_L2_e_EmTau_ROIStatus;   //!
   TBranch        *b_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma;   //!
   TBranch        *b_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus;   //!
   TBranch        *b_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma;   //!
   TBranch        *b_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus;   //!
   TBranch        *b_RoI_L2_e_TrigElectronContainer;   //!
   TBranch        *b_RoI_L2_e_TrigElectronContainerStatus;   //!
   TBranch        *b_RoI_EF_e_type;   //!
   TBranch        *b_RoI_EF_e_EmTau_ROI;   //!
   TBranch        *b_RoI_EF_e_EmTau_ROIStatus;   //!
   TBranch        *b_RoI_L2_tau_type;   //!
   TBranch        *b_RoI_L2_tau_TrigRoiDescriptor;   //!
   TBranch        *b_RoI_L2_tau_TrigRoiDescriptorStatus;   //!
   TBranch        *b_RoI_L2_tau_TrigTauCluster;   //!
   TBranch        *b_RoI_L2_tau_TrigTauClusterStatus;   //!
   TBranch        *b_RoI_L2_tau_TrigTauClusterDetails;   //!
   TBranch        *b_RoI_L2_tau_TrigTauClusterDetailsStatus;   //!
   TBranch        *b_RoI_L2_tau_TrigTauTracksInfo;   //!
   TBranch        *b_RoI_L2_tau_TrigTauTracksInfoStatus;   //!
   TBranch        *b_RoI_L2_tau_TrigTau;   //!
   TBranch        *b_RoI_L2_tau_TrigTauStatus;   //!
   TBranch        *b_RoI_L2_tau_EmTau_ROI;   //!
   TBranch        *b_RoI_L2_tau_EmTau_ROIStatus;   //!
   TBranch        *b_RoI_L2_tau_TrigInDetTrackCollection;   //!
   TBranch        *b_RoI_L2_tau_TrigInDetTrackCollectionStatus;   //!
   TBranch        *b_RoI_EF_tau_type;   //!
   TBranch        *b_RoI_EF_tau_EmTau_ROI;   //!
   TBranch        *b_RoI_EF_tau_EmTau_ROIStatus;   //!
   TBranch        *b_RoI_L2_j_type;   //!
   TBranch        *b_RoI_L2_j_TrigT2Jet;   //!
   TBranch        *b_RoI_L2_j_TrigT2JetStatus;   //!
   TBranch        *b_RoI_L2_j_Jet_ROI;   //!
   TBranch        *b_RoI_L2_j_Jet_ROIStatus;   //!
   TBranch        *b_RoI_EF_j_type;   //!
   TBranch        *b_RoI_EF_j_JetCollection;   //!
   TBranch        *b_RoI_EF_j_JetCollectionStatus;   //!
   TBranch        *b_NL2Jets;   //!
   TBranch        *b_TrigT2Jet_et;   //!
   TBranch        *b_TrigT2Jet_ehad0;   //!
   TBranch        *b_TrigT2Jet_eem0;   //!
   TBranch        *b_TrigT2Jet_eta;   //!
   TBranch        *b_TrigT2Jet_phi;   //!
   TBranch        *b_NEFJets;   //!
   TBranch        *b_Jet_EF_et;   //!
   TBranch        *b_Jet_EF_eta;   //!
   TBranch        *b_Jet_EF_phi;   //!

   TrigMenu(TTree *tree=0);
   virtual ~TrigMenu();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TrigMenu_cxx
TrigMenu::TrigMenu(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("trigntupleTTBAR.root");
      if (!f) {
         f = new TFile("trigntupleTTBAR.root");
         f->cd("trigntupleTTBAR.root:/Menu");
      }
      tree = (TTree*)gDirectory->Get("TrigMenu");

   }
   Init(tree);
}

TrigMenu::~TrigMenu()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TrigMenu::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TrigMenu::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TrigMenu::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   L1MuonRoI_RoIWord = 0;
   L1MuonRoI_pt = 0;
   L1MuonRoI_eta = 0;
   L1MuonRoI_phi = 0;
   L1EmTauRoI_RoIWord = 0;
   L1EmTauRoI_pt = 0;
   L1EmTauRoI_eta = 0;
   L1EmTauRoI_phi = 0;
   L1EmTauRoI_Core = 0;
   L1EmTauRoI_EmClus = 0;
   L1EmTauRoI_TauClus = 0;
   L1EmTauRoI_EmIsol = 0;
   L1EmTauRoI_HadIsol = 0;
   L1EmTauRoI_EmCore = 0;
   L1JetRoI_RoIWord = 0;
   L1JetRoI_pt = 0;
   L1JetRoI_eta = 0;
   L1JetRoI_phi = 0;
   L1JetRoI_4x4 = 0;
   L1JetRoI_6x6 = 0;
   L1JetRoI_8x8 = 0;
   Chain_Id = 0;
   Chain_RoIType = 0;
   Chain_RoIIndex = 0;
   RoI_L2_mu_type = 0;
   RoI_L2_mu_MuonFeature = 0;
   RoI_L2_mu_MuonFeatureStatus = 0;
   RoI_L2_mu_CombinedMuonFeature = 0;
   RoI_L2_mu_CombinedMuonFeatureStatus = 0;
   RoI_L2_mu_Muon_ROI = 0;
   RoI_L2_mu_Muon_ROIStatus = 0;
   RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon = 0;
   RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus = 0;
   RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon = 0;
   RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus = 0;
   RoI_EF_mu_type = 0;
   RoI_EF_mu_Muon_ROI = 0;
   RoI_EF_mu_Muon_ROIStatus = 0;
   RoI_EF_mu_TrigMuonEFInfoContainer = 0;
   RoI_EF_mu_TrigMuonEFInfoContainerStatus = 0;
   RoI_L2_TileMu_type = 0;
   RoI_L2_TileMu_TileMuFeature = 0;
   RoI_L2_TileMu_TileMuFeatureStatus = 0;
   RoI_L2_TileMu_TileTrackMuFeature = 0;
   RoI_L2_TileMu_TileTrackMuFeatureStatus = 0;
   RoI_L2_TileMu_Muon_ROI = 0;
   RoI_L2_TileMu_Muon_ROIStatus = 0;
   RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Muon = 0;
   RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus = 0;
   RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Muon = 0;
   RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus = 0;
   RoI_EF_TileMu_type = 0;
   RoI_EF_TileMu_Muon_ROI = 0;
   RoI_EF_TileMu_Muon_ROIStatus = 0;
   RoI_EF_TileMu_TrigMuonEFContainer = 0;
   RoI_EF_TileMu_TrigMuonEFContainerStatus = 0;
   RoI_EF_TileMu_TrigMuonEFInfoContainer = 0;
   RoI_EF_TileMu_TrigMuonEFInfoContainerStatus = 0;
   RoI_L2_e_type = 0;
   RoI_L2_e_TrigEMCluster = 0;
   RoI_L2_e_TrigEMClusterStatus = 0;
   RoI_L2_e_EmTau_ROI = 0;
   RoI_L2_e_EmTau_ROIStatus = 0;
   RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma = 0;
   RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus = 0;
   RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma = 0;
   RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus = 0;
   RoI_L2_e_TrigElectronContainer = 0;
   RoI_L2_e_TrigElectronContainerStatus = 0;
   RoI_EF_e_type = 0;
   RoI_EF_e_EmTau_ROI = 0;
   RoI_EF_e_EmTau_ROIStatus = 0;
   RoI_L2_tau_type = 0;
   RoI_L2_tau_TrigRoiDescriptor = 0;
   RoI_L2_tau_TrigRoiDescriptorStatus = 0;
   RoI_L2_tau_TrigTauCluster = 0;
   RoI_L2_tau_TrigTauClusterStatus = 0;
   RoI_L2_tau_TrigTauClusterDetails = 0;
   RoI_L2_tau_TrigTauClusterDetailsStatus = 0;
   RoI_L2_tau_TrigTauTracksInfo = 0;
   RoI_L2_tau_TrigTauTracksInfoStatus = 0;
   RoI_L2_tau_TrigTau = 0;
   RoI_L2_tau_TrigTauStatus = 0;
   RoI_L2_tau_EmTau_ROI = 0;
   RoI_L2_tau_EmTau_ROIStatus = 0;
   RoI_L2_tau_TrigInDetTrackCollection = 0;
   RoI_L2_tau_TrigInDetTrackCollectionStatus = 0;
   RoI_EF_tau_type = 0;
   RoI_EF_tau_EmTau_ROI = 0;
   RoI_EF_tau_EmTau_ROIStatus = 0;
   RoI_L2_j_type = 0;
   RoI_L2_j_TrigT2Jet = 0;
   RoI_L2_j_TrigT2JetStatus = 0;
   RoI_L2_j_Jet_ROI = 0;
   RoI_L2_j_Jet_ROIStatus = 0;
   RoI_EF_j_type = 0;
   RoI_EF_j_JetCollection = 0;
   RoI_EF_j_JetCollectionStatus = 0;
   TrigT2Jet_et = 0;
   TrigT2Jet_ehad0 = 0;
   TrigT2Jet_eem0 = 0;
   TrigT2Jet_eta = 0;
   TrigT2Jet_phi = 0;
   Jet_EF_et = 0;
   Jet_EF_eta = 0;
   Jet_EF_phi = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("mL1_TBP", &mL1_TBP, &b_TrigDecSummary_mL1_TBP);
   fChain->SetBranchAddress("mL1_TAP", &mL1_TAP, &b_TrigDecSummary_mL1_TAP);
   fChain->SetBranchAddress("mL1_TAV", &mL1_TAV, &b_TrigDecSummary_mL1_TAV);
   fChain->SetBranchAddress("mL2_Raw", &mL2_Raw, &b_TrigDecSummary_mL2_Raw);
   fChain->SetBranchAddress("mL2_PT", &mL2_PT, &b_TrigDecSummary_mL2_PT);
   fChain->SetBranchAddress("mEF_Raw", &mEF_Raw, &b_TrigDecSummary_mEF_Raw);
   fChain->SetBranchAddress("mEF_PT", &mEF_PT, &b_TrigDecSummary_mEF_PT);
   fChain->SetBranchAddress("L1MuonRoI_RoIWord", &L1MuonRoI_RoIWord, &b_L1MuonRoI_RoIWord);
   fChain->SetBranchAddress("L1MuonRoI_pt", &L1MuonRoI_pt, &b_L1MuonRoI_pt);
   fChain->SetBranchAddress("L1MuonRoI_eta", &L1MuonRoI_eta, &b_L1MuonRoI_eta);
   fChain->SetBranchAddress("L1MuonRoI_phi", &L1MuonRoI_phi, &b_L1MuonRoI_phi);
   fChain->SetBranchAddress("L1EmTauRoI_RoIWord", &L1EmTauRoI_RoIWord, &b_L1EmTauRoI_RoIWord);
   fChain->SetBranchAddress("L1EmTauRoI_pt", &L1EmTauRoI_pt, &b_L1EmTauRoI_pt);
   fChain->SetBranchAddress("L1EmTauRoI_eta", &L1EmTauRoI_eta, &b_L1EmTauRoI_eta);
   fChain->SetBranchAddress("L1EmTauRoI_phi", &L1EmTauRoI_phi, &b_L1EmTauRoI_phi);
   fChain->SetBranchAddress("L1EmTauRoI_Core", &L1EmTauRoI_Core, &b_L1EmTauRoI_Core);
   fChain->SetBranchAddress("L1EmTauRoI_EmClus", &L1EmTauRoI_EmClus, &b_L1EmTauRoI_EmClus);
   fChain->SetBranchAddress("L1EmTauRoI_TauClus", &L1EmTauRoI_TauClus, &b_L1EmTauRoI_TauClus);
   fChain->SetBranchAddress("L1EmTauRoI_EmIsol", &L1EmTauRoI_EmIsol, &b_L1EmTauRoI_EmIsol);
   fChain->SetBranchAddress("L1EmTauRoI_HadIsol", &L1EmTauRoI_HadIsol, &b_L1EmTauRoI_HadIsol);
   fChain->SetBranchAddress("L1EmTauRoI_EmCore", &L1EmTauRoI_EmCore, &b_L1EmTauRoI_EmCore);
   fChain->SetBranchAddress("L1JetRoI_RoIWord", &L1JetRoI_RoIWord, &b_L1JetRoI_RoIWord);
   fChain->SetBranchAddress("L1JetRoI_pt", &L1JetRoI_pt, &b_L1JetRoI_pt);
   fChain->SetBranchAddress("L1JetRoI_eta", &L1JetRoI_eta, &b_L1JetRoI_eta);
   fChain->SetBranchAddress("L1JetRoI_phi", &L1JetRoI_phi, &b_L1JetRoI_phi);
   fChain->SetBranchAddress("L1JetRoI_4x4", &L1JetRoI_4x4, &b_L1JetRoI_4x4);
   fChain->SetBranchAddress("L1JetRoI_6x6", &L1JetRoI_6x6, &b_L1JetRoI_6x6);
   fChain->SetBranchAddress("L1JetRoI_8x8", &L1JetRoI_8x8, &b_L1JetRoI_8x8);
   fChain->SetBranchAddress("Chain_Id", &Chain_Id, &b_Chain_Id);
   fChain->SetBranchAddress("Chain_RoIType", &Chain_RoIType, &b_Chain_RoIType);
   fChain->SetBranchAddress("Chain_RoIIndex", &Chain_RoIIndex, &b_Chain_RoIIndex);
   fChain->SetBranchAddress("RoI_L2_mu_type", &RoI_L2_mu_type, &b_RoI_L2_mu_type);
   fChain->SetBranchAddress("RoI_L2_mu_MuonFeature", &RoI_L2_mu_MuonFeature, &b_RoI_L2_mu_MuonFeature);
   fChain->SetBranchAddress("RoI_L2_mu_MuonFeatureStatus", &RoI_L2_mu_MuonFeatureStatus, &b_RoI_L2_mu_MuonFeatureStatus);
   fChain->SetBranchAddress("RoI_L2_mu_CombinedMuonFeature", &RoI_L2_mu_CombinedMuonFeature, &b_RoI_L2_mu_CombinedMuonFeature);
   fChain->SetBranchAddress("RoI_L2_mu_CombinedMuonFeatureStatus", &RoI_L2_mu_CombinedMuonFeatureStatus, &b_RoI_L2_mu_CombinedMuonFeatureStatus);
   fChain->SetBranchAddress("RoI_L2_mu_Muon_ROI", &RoI_L2_mu_Muon_ROI, &b_RoI_L2_mu_Muon_ROI);
   fChain->SetBranchAddress("RoI_L2_mu_Muon_ROIStatus", &RoI_L2_mu_Muon_ROIStatus, &b_RoI_L2_mu_Muon_ROIStatus);
   fChain->SetBranchAddress("RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon", &RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon, &b_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon);
   fChain->SetBranchAddress("RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus", &RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus, &b_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus);
   fChain->SetBranchAddress("RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon", &RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon, &b_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon);
   fChain->SetBranchAddress("RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus", &RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus, &b_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus);
   fChain->SetBranchAddress("RoI_EF_mu_type", &RoI_EF_mu_type, &b_RoI_EF_mu_type);
   fChain->SetBranchAddress("RoI_EF_mu_Muon_ROI", &RoI_EF_mu_Muon_ROI, &b_RoI_EF_mu_Muon_ROI);
   fChain->SetBranchAddress("RoI_EF_mu_Muon_ROIStatus", &RoI_EF_mu_Muon_ROIStatus, &b_RoI_EF_mu_Muon_ROIStatus);
   fChain->SetBranchAddress("RoI_EF_mu_TrigMuonEFInfoContainer", &RoI_EF_mu_TrigMuonEFInfoContainer, &b_RoI_EF_mu_TrigMuonEFInfoContainer);
   fChain->SetBranchAddress("RoI_EF_mu_TrigMuonEFInfoContainerStatus", &RoI_EF_mu_TrigMuonEFInfoContainerStatus, &b_RoI_EF_mu_TrigMuonEFInfoContainerStatus);
   fChain->SetBranchAddress("RoI_L2_TileMu_type", &RoI_L2_TileMu_type, &b_RoI_L2_TileMu_type);
   fChain->SetBranchAddress("RoI_L2_TileMu_TileMuFeature", &RoI_L2_TileMu_TileMuFeature, &b_RoI_L2_TileMu_TileMuFeature);
   fChain->SetBranchAddress("RoI_L2_TileMu_TileMuFeatureStatus", &RoI_L2_TileMu_TileMuFeatureStatus, &b_RoI_L2_TileMu_TileMuFeatureStatus);
   fChain->SetBranchAddress("RoI_L2_TileMu_TileTrackMuFeature", &RoI_L2_TileMu_TileTrackMuFeature, &b_RoI_L2_TileMu_TileTrackMuFeature);
   fChain->SetBranchAddress("RoI_L2_TileMu_TileTrackMuFeatureStatus", &RoI_L2_TileMu_TileTrackMuFeatureStatus, &b_RoI_L2_TileMu_TileTrackMuFeatureStatus);
   fChain->SetBranchAddress("RoI_L2_TileMu_Muon_ROI", &RoI_L2_TileMu_Muon_ROI, &b_RoI_L2_TileMu_Muon_ROI);
   fChain->SetBranchAddress("RoI_L2_TileMu_Muon_ROIStatus", &RoI_L2_TileMu_Muon_ROIStatus, &b_RoI_L2_TileMu_Muon_ROIStatus);
   fChain->SetBranchAddress("RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Muon", &RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Muon, &b_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Muon);
   fChain->SetBranchAddress("RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus", &RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus, &b_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus);
   fChain->SetBranchAddress("RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Muon", &RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Muon, &b_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Muon);
   fChain->SetBranchAddress("RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus", &RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus, &b_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus);
   fChain->SetBranchAddress("RoI_EF_TileMu_type", &RoI_EF_TileMu_type, &b_RoI_EF_TileMu_type);
   fChain->SetBranchAddress("RoI_EF_TileMu_Muon_ROI", &RoI_EF_TileMu_Muon_ROI, &b_RoI_EF_TileMu_Muon_ROI);
   fChain->SetBranchAddress("RoI_EF_TileMu_Muon_ROIStatus", &RoI_EF_TileMu_Muon_ROIStatus, &b_RoI_EF_TileMu_Muon_ROIStatus);
   fChain->SetBranchAddress("RoI_EF_TileMu_TrigMuonEFContainer", &RoI_EF_TileMu_TrigMuonEFContainer, &b_RoI_EF_TileMu_TrigMuonEFContainer);
   fChain->SetBranchAddress("RoI_EF_TileMu_TrigMuonEFContainerStatus", &RoI_EF_TileMu_TrigMuonEFContainerStatus, &b_RoI_EF_TileMu_TrigMuonEFContainerStatus);
   fChain->SetBranchAddress("RoI_EF_TileMu_TrigMuonEFInfoContainer", &RoI_EF_TileMu_TrigMuonEFInfoContainer, &b_RoI_EF_TileMu_TrigMuonEFInfoContainer);
   fChain->SetBranchAddress("RoI_EF_TileMu_TrigMuonEFInfoContainerStatus", &RoI_EF_TileMu_TrigMuonEFInfoContainerStatus, &b_RoI_EF_TileMu_TrigMuonEFInfoContainerStatus);
   fChain->SetBranchAddress("RoI_L2_e_type", &RoI_L2_e_type, &b_RoI_L2_e_type);
   fChain->SetBranchAddress("RoI_L2_e_TrigEMCluster", &RoI_L2_e_TrigEMCluster, &b_RoI_L2_e_TrigEMCluster);
   fChain->SetBranchAddress("RoI_L2_e_TrigEMClusterStatus", &RoI_L2_e_TrigEMClusterStatus, &b_RoI_L2_e_TrigEMClusterStatus);
   fChain->SetBranchAddress("RoI_L2_e_EmTau_ROI", &RoI_L2_e_EmTau_ROI, &b_RoI_L2_e_EmTau_ROI);
   fChain->SetBranchAddress("RoI_L2_e_EmTau_ROIStatus", &RoI_L2_e_EmTau_ROIStatus, &b_RoI_L2_e_EmTau_ROIStatus);
   fChain->SetBranchAddress("RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma", &RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma, &b_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma);
   fChain->SetBranchAddress("RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus", &RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus, &b_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus);
   fChain->SetBranchAddress("RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma", &RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma, &b_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma);
   fChain->SetBranchAddress("RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus", &RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus, &b_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus);
   fChain->SetBranchAddress("RoI_L2_e_TrigElectronContainer", &RoI_L2_e_TrigElectronContainer, &b_RoI_L2_e_TrigElectronContainer);
   fChain->SetBranchAddress("RoI_L2_e_TrigElectronContainerStatus", &RoI_L2_e_TrigElectronContainerStatus, &b_RoI_L2_e_TrigElectronContainerStatus);
   fChain->SetBranchAddress("RoI_EF_e_type", &RoI_EF_e_type, &b_RoI_EF_e_type);
   fChain->SetBranchAddress("RoI_EF_e_EmTau_ROI", &RoI_EF_e_EmTau_ROI, &b_RoI_EF_e_EmTau_ROI);
   fChain->SetBranchAddress("RoI_EF_e_EmTau_ROIStatus", &RoI_EF_e_EmTau_ROIStatus, &b_RoI_EF_e_EmTau_ROIStatus);
   fChain->SetBranchAddress("RoI_L2_tau_type", &RoI_L2_tau_type, &b_RoI_L2_tau_type);
   fChain->SetBranchAddress("RoI_L2_tau_TrigRoiDescriptor", &RoI_L2_tau_TrigRoiDescriptor, &b_RoI_L2_tau_TrigRoiDescriptor);
   fChain->SetBranchAddress("RoI_L2_tau_TrigRoiDescriptorStatus", &RoI_L2_tau_TrigRoiDescriptorStatus, &b_RoI_L2_tau_TrigRoiDescriptorStatus);
   fChain->SetBranchAddress("RoI_L2_tau_TrigTauCluster", &RoI_L2_tau_TrigTauCluster, &b_RoI_L2_tau_TrigTauCluster);
   fChain->SetBranchAddress("RoI_L2_tau_TrigTauClusterStatus", &RoI_L2_tau_TrigTauClusterStatus, &b_RoI_L2_tau_TrigTauClusterStatus);
   fChain->SetBranchAddress("RoI_L2_tau_TrigTauClusterDetails", &RoI_L2_tau_TrigTauClusterDetails, &b_RoI_L2_tau_TrigTauClusterDetails);
   fChain->SetBranchAddress("RoI_L2_tau_TrigTauClusterDetailsStatus", &RoI_L2_tau_TrigTauClusterDetailsStatus, &b_RoI_L2_tau_TrigTauClusterDetailsStatus);
   fChain->SetBranchAddress("RoI_L2_tau_TrigTauTracksInfo", &RoI_L2_tau_TrigTauTracksInfo, &b_RoI_L2_tau_TrigTauTracksInfo);
   fChain->SetBranchAddress("RoI_L2_tau_TrigTauTracksInfoStatus", &RoI_L2_tau_TrigTauTracksInfoStatus, &b_RoI_L2_tau_TrigTauTracksInfoStatus);
   fChain->SetBranchAddress("RoI_L2_tau_TrigTau", &RoI_L2_tau_TrigTau, &b_RoI_L2_tau_TrigTau);
   fChain->SetBranchAddress("RoI_L2_tau_TrigTauStatus", &RoI_L2_tau_TrigTauStatus, &b_RoI_L2_tau_TrigTauStatus);
   fChain->SetBranchAddress("RoI_L2_tau_EmTau_ROI", &RoI_L2_tau_EmTau_ROI, &b_RoI_L2_tau_EmTau_ROI);
   fChain->SetBranchAddress("RoI_L2_tau_EmTau_ROIStatus", &RoI_L2_tau_EmTau_ROIStatus, &b_RoI_L2_tau_EmTau_ROIStatus);
   fChain->SetBranchAddress("RoI_L2_tau_TrigInDetTrackCollection", &RoI_L2_tau_TrigInDetTrackCollection, &b_RoI_L2_tau_TrigInDetTrackCollection);
   fChain->SetBranchAddress("RoI_L2_tau_TrigInDetTrackCollectionStatus", &RoI_L2_tau_TrigInDetTrackCollectionStatus, &b_RoI_L2_tau_TrigInDetTrackCollectionStatus);
   fChain->SetBranchAddress("RoI_EF_tau_type", &RoI_EF_tau_type, &b_RoI_EF_tau_type);
   fChain->SetBranchAddress("RoI_EF_tau_EmTau_ROI", &RoI_EF_tau_EmTau_ROI, &b_RoI_EF_tau_EmTau_ROI);
   fChain->SetBranchAddress("RoI_EF_tau_EmTau_ROIStatus", &RoI_EF_tau_EmTau_ROIStatus, &b_RoI_EF_tau_EmTau_ROIStatus);
   fChain->SetBranchAddress("RoI_L2_j_type", &RoI_L2_j_type, &b_RoI_L2_j_type);
   fChain->SetBranchAddress("RoI_L2_j_TrigT2Jet", &RoI_L2_j_TrigT2Jet, &b_RoI_L2_j_TrigT2Jet);
   fChain->SetBranchAddress("RoI_L2_j_TrigT2JetStatus", &RoI_L2_j_TrigT2JetStatus, &b_RoI_L2_j_TrigT2JetStatus);
   fChain->SetBranchAddress("RoI_L2_j_Jet_ROI", &RoI_L2_j_Jet_ROI, &b_RoI_L2_j_Jet_ROI);
   fChain->SetBranchAddress("RoI_L2_j_Jet_ROIStatus", &RoI_L2_j_Jet_ROIStatus, &b_RoI_L2_j_Jet_ROIStatus);
   fChain->SetBranchAddress("RoI_EF_j_type", &RoI_EF_j_type, &b_RoI_EF_j_type);
   fChain->SetBranchAddress("RoI_EF_j_JetCollection", &RoI_EF_j_JetCollection, &b_RoI_EF_j_JetCollection);
   fChain->SetBranchAddress("RoI_EF_j_JetCollectionStatus", &RoI_EF_j_JetCollectionStatus, &b_RoI_EF_j_JetCollectionStatus);
   fChain->SetBranchAddress("NL2Jets", &NL2Jets, &b_NL2Jets);
   fChain->SetBranchAddress("TrigT2Jet_et", &TrigT2Jet_et, &b_TrigT2Jet_et);
   fChain->SetBranchAddress("TrigT2Jet_ehad0", &TrigT2Jet_ehad0, &b_TrigT2Jet_ehad0);
   fChain->SetBranchAddress("TrigT2Jet_eem0", &TrigT2Jet_eem0, &b_TrigT2Jet_eem0);
   fChain->SetBranchAddress("TrigT2Jet_eta", &TrigT2Jet_eta, &b_TrigT2Jet_eta);
   fChain->SetBranchAddress("TrigT2Jet_phi", &TrigT2Jet_phi, &b_TrigT2Jet_phi);
   fChain->SetBranchAddress("NEFJets", &NEFJets, &b_NEFJets);
   fChain->SetBranchAddress("Jet_EF_et", &Jet_EF_et, &b_Jet_EF_et);
   fChain->SetBranchAddress("Jet_EF_eta", &Jet_EF_eta, &b_Jet_EF_eta);
   fChain->SetBranchAddress("Jet_EF_phi", &Jet_EF_phi, &b_Jet_EF_phi);
   Notify();
}

Bool_t TrigMenu::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TrigMenu::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TrigMenu::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TrigMenu_cxx
