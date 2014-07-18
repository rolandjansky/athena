/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Dec  3 12:42:27 2010 by ROOT version 5.26/00e
// from TTree InDetTrackTree/InDetTrackTree
// found on file: castor:///castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00001.root
//////////////////////////////////////////////////////////

#ifndef InDetTrackTree_h
#define InDetTrackTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

using namespace std;


class InDetTrackTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          ei_RunNumber;
   UInt_t          ei_EventNumber;
   UInt_t          ei_timestamp;
   UInt_t          ei_timestamp_ns;
   UInt_t          ei_lbn;
   UInt_t          ei_bcid;
   UInt_t          ei_detmask0;
   UInt_t          ei_detmask1;
   Int_t           trk_n;
   vector<float>   *trk_pt;
   vector<float>   *trk_eta;
   vector<float>   *trk_d0_wrtPV;
   vector<float>   *trk_z0_wrtPV;
   vector<float>   *trk_phi_wrtPV;
   vector<float>   *trk_theta_wrtPV;
   vector<float>   *trk_qoverp_wrtPV;
   vector<float>   *trk_cov_d0_wrtPV;
   vector<float>   *trk_cov_z0_wrtPV;
   vector<float>   *trk_cov_phi_wrtPV;
   vector<float>   *trk_cov_theta_wrtPV;
   vector<float>   *trk_cov_qoverp_wrtPV;
   vector<float>   *trk_chi2;
   vector<int>     *trk_ndof;
   vector<int>     *trk_nBLHits;
   vector<int>     *trk_nPixHits;
   vector<int>     *trk_nSCTHits;
   vector<int>     *trk_nTRTHits;
   vector<int>     *trk_nPixHoles;
   vector<int>     *trk_nSCTHoles;
   vector<int>     *trk_nMDTHits;
   vector<int>     *trk_nCSCEtaHits;
   vector<int>     *trk_nCSCPhiHits;
   vector<int>     *trk_nRPCEtaHits;
   vector<int>     *trk_nRPCPhiHits;
   vector<int>     *trk_nTGCEtaHits;
   vector<int>     *trk_nTGCPhiHits;
   vector<int>     *trk_nHits;
   vector<int>     *trk_nHoles;
   vector<int>     *trk_hitPattern;
   vector<float>   *trk_TRTHighTHitsRatio;
   vector<float>   *trk_pixeldEdx;
   vector<int>     *trk_fitter;
   vector<int>     *trk_patternReco1;
   vector<int>     *trk_patternReco2;
   vector<int>     *trk_seedFinder;
   vector<int>     *trk_blayerPrediction_expectHit;
   Int_t           vxbc_n;
   vector<float>   *vxbc_x;
   vector<float>   *vxbc_y;
   vector<float>   *vxbc_z;
   vector<float>   *vxbc_cov_x;
   vector<float>   *vxbc_cov_y;
   vector<float>   *vxbc_cov_z;
   vector<float>   *vxbc_chi2;
   vector<int>     *vxbc_ndof;
   vector<float>   *vxbc_px;
   vector<float>   *vxbc_py;
   vector<float>   *vxbc_pz;
   vector<float>   *vxbc_E;
   vector<float>   *vxbc_m;
   vector<int>     *vxbc_nTracks;
   vector<float>   *vxbc_sumPt;
   vector<int>     *vxbc_type;
   vector<int>     *vxbc_trk_n;
   vector<vector<int> > *vxbc_trk_index;
   Int_t           vxsplit_n;
   vector<float>   *vxsplit_x;
   vector<float>   *vxsplit_y;
   vector<float>   *vxsplit_z;
   vector<float>   *vxsplit_cov_x;
   vector<float>   *vxsplit_cov_y;
   vector<float>   *vxsplit_cov_z;
   vector<float>   *vxsplit_chi2;
   vector<int>     *vxsplit_ndof;
   vector<float>   *vxsplit_px;
   vector<float>   *vxsplit_py;
   vector<float>   *vxsplit_pz;
   vector<float>   *vxsplit_E;
   vector<float>   *vxsplit_m;
   vector<int>     *vxsplit_nTracks;
   vector<float>   *vxsplit_sumPt;
   vector<int>     *vxsplit_type;
   vector<int>     *vxsplit_trk_n;
   vector<vector<int> > *vxsplit_trk_index;
   Int_t           vxnbc_n;
   vector<float>   *vxnbc_x;
   vector<float>   *vxnbc_y;
   vector<float>   *vxnbc_z;
   vector<float>   *vxnbc_cov_x;
   vector<float>   *vxnbc_cov_y;
   vector<float>   *vxnbc_cov_z;
   vector<float>   *vxnbc_chi2;
   vector<int>     *vxnbc_ndof;
   vector<float>   *vxnbc_px;
   vector<float>   *vxnbc_py;
   vector<float>   *vxnbc_pz;
   vector<float>   *vxnbc_E;
   vector<float>   *vxnbc_m;
   vector<int>     *vxnbc_nTracks;
   vector<float>   *vxnbc_sumPt;
   vector<int>     *vxnbc_type;
   vector<int>     *vxnbc_trk_n;
   vector<vector<int> > *vxnbc_trk_index;
   vector<unsigned int> *trig_L1_TAV;
   vector<short>   *trig_L2_passedPhysics;
   vector<short>   *trig_EF_passedPhysics;
   UInt_t          trig_DB_SMK;
   UInt_t          trig_DB_L1PSK;
   UInt_t          trig_DB_HLTPSK;

   // List of branches
   TBranch        *b_ei_RunNumber;   //!
   TBranch        *b_ei_EventNumber;   //!
   TBranch        *b_ei_timestamp;   //!
   TBranch        *b_ei_timestamp_ns;   //!
   TBranch        *b_ei_lbn;   //!
   TBranch        *b_ei_bcid;   //!
   TBranch        *b_ei_detmask0;   //!
   TBranch        *b_ei_detmask1;   //!
   TBranch        *b_trk_n;   //!
   TBranch        *b_trk_pt;   //!
   TBranch        *b_trk_eta;   //!
   TBranch        *b_trk_d0_wrtPV;   //!
   TBranch        *b_trk_z0_wrtPV;   //!
   TBranch        *b_trk_phi_wrtPV;   //!
   TBranch        *b_trk_theta_wrtPV;   //!
   TBranch        *b_trk_qoverp_wrtPV;   //!
   TBranch        *b_trk_cov_d0_wrtPV;   //!
   TBranch        *b_trk_cov_z0_wrtPV;   //!
   TBranch        *b_trk_cov_phi_wrtPV;   //!
   TBranch        *b_trk_cov_theta_wrtPV;   //!
   TBranch        *b_trk_cov_qoverp_wrtPV;   //!
   TBranch        *b_trk_chi2;   //!
   TBranch        *b_trk_ndof;   //!
   TBranch        *b_trk_nBLHits;   //!
   TBranch        *b_trk_nPixHits;   //!
   TBranch        *b_trk_nSCTHits;   //!
   TBranch        *b_trk_nTRTHits;   //!
   TBranch        *b_trk_nPixHoles;   //!
   TBranch        *b_trk_nSCTHoles;   //!
   TBranch        *b_trk_nMDTHits;   //!
   TBranch        *b_trk_nCSCEtaHits;   //!
   TBranch        *b_trk_nCSCPhiHits;   //!
   TBranch        *b_trk_nRPCEtaHits;   //!
   TBranch        *b_trk_nRPCPhiHits;   //!
   TBranch        *b_trk_nTGCEtaHits;   //!
   TBranch        *b_trk_nTGCPhiHits;   //!
   TBranch        *b_trk_nHits;   //!
   TBranch        *b_trk_nHoles;   //!
   TBranch        *b_trk_hitPattern;   //!
   TBranch        *b_trk_TRTHighTHitsRatio;   //!
   TBranch        *b_trk_pixeldEdx;   //!
   TBranch        *b_trk_fitter;   //!
   TBranch        *b_trk_patternReco1;   //!
   TBranch        *b_trk_patternReco2;   //!
   TBranch        *b_trk_seedFinder;   //!
   TBranch        *b_trk_blayerPrediction_expectHit;   //!
   TBranch        *b_vxbc_n;   //!
   TBranch        *b_vxbc_x;   //!
   TBranch        *b_vxbc_y;   //!
   TBranch        *b_vxbc_z;   //!
   TBranch        *b_vxbc_cov_x;   //!
   TBranch        *b_vxbc_cov_y;   //!
   TBranch        *b_vxbc_cov_z;   //!
   TBranch        *b_vxbc_chi2;   //!
   TBranch        *b_vxbc_ndof;   //!
   TBranch        *b_vxbc_px;   //!
   TBranch        *b_vxbc_py;   //!
   TBranch        *b_vxbc_pz;   //!
   TBranch        *b_vxbc_E;   //!
   TBranch        *b_vxbc_m;   //!
   TBranch        *b_vxbc_nTracks;   //!
   TBranch        *b_vxbc_sumPt;   //!
   TBranch        *b_vxbc_type;   //!
   TBranch        *b_vxbc_trk_n;   //!
   TBranch        *b_vxbc_trk_index;   //!
   TBranch        *b_vxsplit_n;   //!
   TBranch        *b_vxsplit_x;   //!
   TBranch        *b_vxsplit_y;   //!
   TBranch        *b_vxsplit_z;   //!
   TBranch        *b_vxsplit_cov_x;   //!
   TBranch        *b_vxsplit_cov_y;   //!
   TBranch        *b_vxsplit_cov_z;   //!
   TBranch        *b_vxsplit_chi2;   //!
   TBranch        *b_vxsplit_ndof;   //!
   TBranch        *b_vxsplit_px;   //!
   TBranch        *b_vxsplit_py;   //!
   TBranch        *b_vxsplit_pz;   //!
   TBranch        *b_vxsplit_E;   //!
   TBranch        *b_vxsplit_m;   //!
   TBranch        *b_vxsplit_nTracks;   //!
   TBranch        *b_vxsplit_sumPt;   //!
   TBranch        *b_vxsplit_type;   //!
   TBranch        *b_vxsplit_trk_n;   //!
   TBranch        *b_vxsplit_trk_index;   //!
   TBranch        *b_vxnbc_n;   //!
   TBranch        *b_vxnbc_x;   //!
   TBranch        *b_vxnbc_y;   //!
   TBranch        *b_vxnbc_z;   //!
   TBranch        *b_vxnbc_cov_x;   //!
   TBranch        *b_vxnbc_cov_y;   //!
   TBranch        *b_vxnbc_cov_z;   //!
   TBranch        *b_vxnbc_chi2;   //!
   TBranch        *b_vxnbc_ndof;   //!
   TBranch        *b_vxnbc_px;   //!
   TBranch        *b_vxnbc_py;   //!
   TBranch        *b_vxnbc_pz;   //!
   TBranch        *b_vxnbc_E;   //!
   TBranch        *b_vxnbc_m;   //!
   TBranch        *b_vxnbc_nTracks;   //!
   TBranch        *b_vxnbc_sumPt;   //!
   TBranch        *b_vxnbc_type;   //!
   TBranch        *b_vxnbc_trk_n;   //!
   TBranch        *b_vxnbc_trk_index;   //!
   TBranch        *b_trig_L1_TAV;   //!
   TBranch        *b_trig_L2_passedPhysics;   //!
   TBranch        *b_trig_EF_passedPhysics;   //!
   TBranch        *b_trig_DB_SMK;   //!
   TBranch        *b_trig_DB_L1PSK;   //!
   TBranch        *b_trig_DB_HLTPSK;   //!

   InDetTrackTree(TTree *tree=0);
   virtual ~InDetTrackTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef InDetTrackTree_cxx
InDetTrackTree::InDetTrackTree(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("castor:///castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00001.root");
      if (!f) {
         f = new TFile("castor:///castor/cern.ch/user/w/wildauer/atlascernuserdisk/vertexing/ntuple/pileup/user.AndreasWildauer.data10_7TeV.00167776.physics_MinBias.merge.NTUP_VTXMON.r1774_p327_p333.wTrigTest/user.AndreasWildauer.000099.D3PD._00001.root");
      }
      tree = (TTree*)gDirectory->Get("InDetTrackTree");

   }
   Init(tree);
}

InDetTrackTree::~InDetTrackTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t InDetTrackTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t InDetTrackTree::LoadTree(Long64_t entry)
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

void InDetTrackTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   trk_pt = 0;
   trk_eta = 0;
   trk_d0_wrtPV = 0;
   trk_z0_wrtPV = 0;
   trk_phi_wrtPV = 0;
   trk_theta_wrtPV = 0;
   trk_qoverp_wrtPV = 0;
   trk_cov_d0_wrtPV = 0;
   trk_cov_z0_wrtPV = 0;
   trk_cov_phi_wrtPV = 0;
   trk_cov_theta_wrtPV = 0;
   trk_cov_qoverp_wrtPV = 0;
   trk_chi2 = 0;
   trk_ndof = 0;
   trk_nBLHits = 0;
   trk_nPixHits = 0;
   trk_nSCTHits = 0;
   trk_nTRTHits = 0;
   trk_nPixHoles = 0;
   trk_nSCTHoles = 0;
   trk_nMDTHits = 0;
   trk_nCSCEtaHits = 0;
   trk_nCSCPhiHits = 0;
   trk_nRPCEtaHits = 0;
   trk_nRPCPhiHits = 0;
   trk_nTGCEtaHits = 0;
   trk_nTGCPhiHits = 0;
   trk_nHits = 0;
   trk_nHoles = 0;
   trk_hitPattern = 0;
   trk_TRTHighTHitsRatio = 0;
   trk_pixeldEdx = 0;
   trk_fitter = 0;
   trk_patternReco1 = 0;
   trk_patternReco2 = 0;
   trk_seedFinder = 0;
   trk_blayerPrediction_expectHit = 0;
   vxbc_x = 0;
   vxbc_y = 0;
   vxbc_z = 0;
   vxbc_cov_x = 0;
   vxbc_cov_y = 0;
   vxbc_cov_z = 0;
   vxbc_chi2 = 0;
   vxbc_ndof = 0;
   vxbc_px = 0;
   vxbc_py = 0;
   vxbc_pz = 0;
   vxbc_E = 0;
   vxbc_m = 0;
   vxbc_nTracks = 0;
   vxbc_sumPt = 0;
   vxbc_type = 0;
   vxbc_trk_n = 0;
   vxbc_trk_index = 0;
   vxsplit_x = 0;
   vxsplit_y = 0;
   vxsplit_z = 0;
   vxsplit_cov_x = 0;
   vxsplit_cov_y = 0;
   vxsplit_cov_z = 0;
   vxsplit_chi2 = 0;
   vxsplit_ndof = 0;
   vxsplit_px = 0;
   vxsplit_py = 0;
   vxsplit_pz = 0;
   vxsplit_E = 0;
   vxsplit_m = 0;
   vxsplit_nTracks = 0;
   vxsplit_sumPt = 0;
   vxsplit_type = 0;
   vxsplit_trk_n = 0;
   vxsplit_trk_index = 0;
   vxnbc_x = 0;
   vxnbc_y = 0;
   vxnbc_z = 0;
   vxnbc_cov_x = 0;
   vxnbc_cov_y = 0;
   vxnbc_cov_z = 0;
   vxnbc_chi2 = 0;
   vxnbc_ndof = 0;
   vxnbc_px = 0;
   vxnbc_py = 0;
   vxnbc_pz = 0;
   vxnbc_E = 0;
   vxnbc_m = 0;
   vxnbc_nTracks = 0;
   vxnbc_sumPt = 0;
   vxnbc_type = 0;
   vxnbc_trk_n = 0;
   vxnbc_trk_index = 0;
   trig_L1_TAV = 0;
   trig_L2_passedPhysics = 0;
   trig_EF_passedPhysics = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("ei_RunNumber", &ei_RunNumber, &b_ei_RunNumber);
   fChain->SetBranchAddress("ei_EventNumber", &ei_EventNumber, &b_ei_EventNumber);
   fChain->SetBranchAddress("ei_timestamp", &ei_timestamp, &b_ei_timestamp);
   fChain->SetBranchAddress("ei_timestamp_ns", &ei_timestamp_ns, &b_ei_timestamp_ns);
   fChain->SetBranchAddress("ei_lbn", &ei_lbn, &b_ei_lbn);
   fChain->SetBranchAddress("ei_bcid", &ei_bcid, &b_ei_bcid);
   fChain->SetBranchAddress("ei_detmask0", &ei_detmask0, &b_ei_detmask0);
   fChain->SetBranchAddress("ei_detmask1", &ei_detmask1, &b_ei_detmask1);
   fChain->SetBranchAddress("trk_n", &trk_n, &b_trk_n);
   fChain->SetBranchAddress("trk_pt", &trk_pt, &b_trk_pt);
   fChain->SetBranchAddress("trk_eta", &trk_eta, &b_trk_eta);
   fChain->SetBranchAddress("trk_d0_wrtPV", &trk_d0_wrtPV, &b_trk_d0_wrtPV);
   fChain->SetBranchAddress("trk_z0_wrtPV", &trk_z0_wrtPV, &b_trk_z0_wrtPV);
   fChain->SetBranchAddress("trk_phi_wrtPV", &trk_phi_wrtPV, &b_trk_phi_wrtPV);
   fChain->SetBranchAddress("trk_theta_wrtPV", &trk_theta_wrtPV, &b_trk_theta_wrtPV);
   fChain->SetBranchAddress("trk_qoverp_wrtPV", &trk_qoverp_wrtPV, &b_trk_qoverp_wrtPV);
   fChain->SetBranchAddress("trk_cov_d0_wrtPV", &trk_cov_d0_wrtPV, &b_trk_cov_d0_wrtPV);
   fChain->SetBranchAddress("trk_cov_z0_wrtPV", &trk_cov_z0_wrtPV, &b_trk_cov_z0_wrtPV);
   fChain->SetBranchAddress("trk_cov_phi_wrtPV", &trk_cov_phi_wrtPV, &b_trk_cov_phi_wrtPV);
   fChain->SetBranchAddress("trk_cov_theta_wrtPV", &trk_cov_theta_wrtPV, &b_trk_cov_theta_wrtPV);
   fChain->SetBranchAddress("trk_cov_qoverp_wrtPV", &trk_cov_qoverp_wrtPV, &b_trk_cov_qoverp_wrtPV);
   fChain->SetBranchAddress("trk_chi2", &trk_chi2, &b_trk_chi2);
   fChain->SetBranchAddress("trk_ndof", &trk_ndof, &b_trk_ndof);
   fChain->SetBranchAddress("trk_nBLHits", &trk_nBLHits, &b_trk_nBLHits);
   fChain->SetBranchAddress("trk_nPixHits", &trk_nPixHits, &b_trk_nPixHits);
   fChain->SetBranchAddress("trk_nSCTHits", &trk_nSCTHits, &b_trk_nSCTHits);
   fChain->SetBranchAddress("trk_nTRTHits", &trk_nTRTHits, &b_trk_nTRTHits);
   fChain->SetBranchAddress("trk_nPixHoles", &trk_nPixHoles, &b_trk_nPixHoles);
   fChain->SetBranchAddress("trk_nSCTHoles", &trk_nSCTHoles, &b_trk_nSCTHoles);
   fChain->SetBranchAddress("trk_nMDTHits", &trk_nMDTHits, &b_trk_nMDTHits);
   fChain->SetBranchAddress("trk_nCSCEtaHits", &trk_nCSCEtaHits, &b_trk_nCSCEtaHits);
   fChain->SetBranchAddress("trk_nCSCPhiHits", &trk_nCSCPhiHits, &b_trk_nCSCPhiHits);
   fChain->SetBranchAddress("trk_nRPCEtaHits", &trk_nRPCEtaHits, &b_trk_nRPCEtaHits);
   fChain->SetBranchAddress("trk_nRPCPhiHits", &trk_nRPCPhiHits, &b_trk_nRPCPhiHits);
   fChain->SetBranchAddress("trk_nTGCEtaHits", &trk_nTGCEtaHits, &b_trk_nTGCEtaHits);
   fChain->SetBranchAddress("trk_nTGCPhiHits", &trk_nTGCPhiHits, &b_trk_nTGCPhiHits);
   fChain->SetBranchAddress("trk_nHits", &trk_nHits, &b_trk_nHits);
   fChain->SetBranchAddress("trk_nHoles", &trk_nHoles, &b_trk_nHoles);
   fChain->SetBranchAddress("trk_hitPattern", &trk_hitPattern, &b_trk_hitPattern);
   fChain->SetBranchAddress("trk_TRTHighTHitsRatio", &trk_TRTHighTHitsRatio, &b_trk_TRTHighTHitsRatio);
   fChain->SetBranchAddress("trk_pixeldEdx", &trk_pixeldEdx, &b_trk_pixeldEdx);
   fChain->SetBranchAddress("trk_fitter", &trk_fitter, &b_trk_fitter);
   fChain->SetBranchAddress("trk_patternReco1", &trk_patternReco1, &b_trk_patternReco1);
   fChain->SetBranchAddress("trk_patternReco2", &trk_patternReco2, &b_trk_patternReco2);
   fChain->SetBranchAddress("trk_seedFinder", &trk_seedFinder, &b_trk_seedFinder);
   fChain->SetBranchAddress("trk_blayerPrediction_expectHit", &trk_blayerPrediction_expectHit, &b_trk_blayerPrediction_expectHit);
   fChain->SetBranchAddress("vxbc_n", &vxbc_n, &b_vxbc_n);
   fChain->SetBranchAddress("vxbc_x", &vxbc_x, &b_vxbc_x);
   fChain->SetBranchAddress("vxbc_y", &vxbc_y, &b_vxbc_y);
   fChain->SetBranchAddress("vxbc_z", &vxbc_z, &b_vxbc_z);
   fChain->SetBranchAddress("vxbc_cov_x", &vxbc_cov_x, &b_vxbc_cov_x);
   fChain->SetBranchAddress("vxbc_cov_y", &vxbc_cov_y, &b_vxbc_cov_y);
   fChain->SetBranchAddress("vxbc_cov_z", &vxbc_cov_z, &b_vxbc_cov_z);
   fChain->SetBranchAddress("vxbc_chi2", &vxbc_chi2, &b_vxbc_chi2);
   fChain->SetBranchAddress("vxbc_ndof", &vxbc_ndof, &b_vxbc_ndof);
   fChain->SetBranchAddress("vxbc_px", &vxbc_px, &b_vxbc_px);
   fChain->SetBranchAddress("vxbc_py", &vxbc_py, &b_vxbc_py);
   fChain->SetBranchAddress("vxbc_pz", &vxbc_pz, &b_vxbc_pz);
   fChain->SetBranchAddress("vxbc_E", &vxbc_E, &b_vxbc_E);
   fChain->SetBranchAddress("vxbc_m", &vxbc_m, &b_vxbc_m);
   fChain->SetBranchAddress("vxbc_nTracks", &vxbc_nTracks, &b_vxbc_nTracks);
   fChain->SetBranchAddress("vxbc_sumPt", &vxbc_sumPt, &b_vxbc_sumPt);
   fChain->SetBranchAddress("vxbc_type", &vxbc_type, &b_vxbc_type);
   fChain->SetBranchAddress("vxbc_trk_n", &vxbc_trk_n, &b_vxbc_trk_n);
   fChain->SetBranchAddress("vxbc_trk_index", &vxbc_trk_index, &b_vxbc_trk_index);
   fChain->SetBranchAddress("vxsplit_n", &vxsplit_n, &b_vxsplit_n);
   fChain->SetBranchAddress("vxsplit_x", &vxsplit_x, &b_vxsplit_x);
   fChain->SetBranchAddress("vxsplit_y", &vxsplit_y, &b_vxsplit_y);
   fChain->SetBranchAddress("vxsplit_z", &vxsplit_z, &b_vxsplit_z);
   fChain->SetBranchAddress("vxsplit_cov_x", &vxsplit_cov_x, &b_vxsplit_cov_x);
   fChain->SetBranchAddress("vxsplit_cov_y", &vxsplit_cov_y, &b_vxsplit_cov_y);
   fChain->SetBranchAddress("vxsplit_cov_z", &vxsplit_cov_z, &b_vxsplit_cov_z);
   fChain->SetBranchAddress("vxsplit_chi2", &vxsplit_chi2, &b_vxsplit_chi2);
   fChain->SetBranchAddress("vxsplit_ndof", &vxsplit_ndof, &b_vxsplit_ndof);
   fChain->SetBranchAddress("vxsplit_px", &vxsplit_px, &b_vxsplit_px);
   fChain->SetBranchAddress("vxsplit_py", &vxsplit_py, &b_vxsplit_py);
   fChain->SetBranchAddress("vxsplit_pz", &vxsplit_pz, &b_vxsplit_pz);
   fChain->SetBranchAddress("vxsplit_E", &vxsplit_E, &b_vxsplit_E);
   fChain->SetBranchAddress("vxsplit_m", &vxsplit_m, &b_vxsplit_m);
   fChain->SetBranchAddress("vxsplit_nTracks", &vxsplit_nTracks, &b_vxsplit_nTracks);
   fChain->SetBranchAddress("vxsplit_sumPt", &vxsplit_sumPt, &b_vxsplit_sumPt);
   fChain->SetBranchAddress("vxsplit_type", &vxsplit_type, &b_vxsplit_type);
   fChain->SetBranchAddress("vxsplit_trk_n", &vxsplit_trk_n, &b_vxsplit_trk_n);
   fChain->SetBranchAddress("vxsplit_trk_index", &vxsplit_trk_index, &b_vxsplit_trk_index);
   fChain->SetBranchAddress("vxnbc_n", &vxnbc_n, &b_vxnbc_n);
   fChain->SetBranchAddress("vxnbc_x", &vxnbc_x, &b_vxnbc_x);
   fChain->SetBranchAddress("vxnbc_y", &vxnbc_y, &b_vxnbc_y);
   fChain->SetBranchAddress("vxnbc_z", &vxnbc_z, &b_vxnbc_z);
   fChain->SetBranchAddress("vxnbc_cov_x", &vxnbc_cov_x, &b_vxnbc_cov_x);
   fChain->SetBranchAddress("vxnbc_cov_y", &vxnbc_cov_y, &b_vxnbc_cov_y);
   fChain->SetBranchAddress("vxnbc_cov_z", &vxnbc_cov_z, &b_vxnbc_cov_z);
   fChain->SetBranchAddress("vxnbc_chi2", &vxnbc_chi2, &b_vxnbc_chi2);
   fChain->SetBranchAddress("vxnbc_ndof", &vxnbc_ndof, &b_vxnbc_ndof);
   fChain->SetBranchAddress("vxnbc_px", &vxnbc_px, &b_vxnbc_px);
   fChain->SetBranchAddress("vxnbc_py", &vxnbc_py, &b_vxnbc_py);
   fChain->SetBranchAddress("vxnbc_pz", &vxnbc_pz, &b_vxnbc_pz);
   fChain->SetBranchAddress("vxnbc_E", &vxnbc_E, &b_vxnbc_E);
   fChain->SetBranchAddress("vxnbc_m", &vxnbc_m, &b_vxnbc_m);
   fChain->SetBranchAddress("vxnbc_nTracks", &vxnbc_nTracks, &b_vxnbc_nTracks);
   fChain->SetBranchAddress("vxnbc_sumPt", &vxnbc_sumPt, &b_vxnbc_sumPt);
   fChain->SetBranchAddress("vxnbc_type", &vxnbc_type, &b_vxnbc_type);
   fChain->SetBranchAddress("vxnbc_trk_n", &vxnbc_trk_n, &b_vxnbc_trk_n);
   fChain->SetBranchAddress("vxnbc_trk_index", &vxnbc_trk_index, &b_vxnbc_trk_index);
   fChain->SetBranchAddress("trig_L1_TAV", &trig_L1_TAV, &b_trig_L1_TAV);
   fChain->SetBranchAddress("trig_L2_passedPhysics", &trig_L2_passedPhysics, &b_trig_L2_passedPhysics);
   fChain->SetBranchAddress("trig_EF_passedPhysics", &trig_EF_passedPhysics, &b_trig_EF_passedPhysics);
   fChain->SetBranchAddress("trig_DB_SMK", &trig_DB_SMK, &b_trig_DB_SMK);
   fChain->SetBranchAddress("trig_DB_L1PSK", &trig_DB_L1PSK, &b_trig_DB_L1PSK);
   fChain->SetBranchAddress("trig_DB_HLTPSK", &trig_DB_HLTPSK, &b_trig_DB_HLTPSK);
   Notify();
}

Bool_t InDetTrackTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void InDetTrackTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t InDetTrackTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef InDetTrackTree_cxx
