/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri May 18 14:09:50 2007 by ROOT version 5.14/00
// from TTree OutputTrack/Track Validation
// found on file: TrkValidation.root
//////////////////////////////////////////////////////////

#ifndef TrkValidationNtupleOutputTrack_h
#define TrkValidationNtupleOutputTrack_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>
using namespace std;

#ifdef __CINT__
#pragma link C++ class vector< int >;
#pragma link C++ class vector< float >;
#endif
template class vector< vector< int > >;
template class vector< vector< float > >;

class TrkValidationNtupleOutputTrack {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Int_t           RunNumber;
   Int_t           EventNumber;
   Int_t           TrackID;
   Int_t           IterationIndex;
   Int_t           nHits;
   Int_t           TrackStatesUnbiased;
   Float_t         RecD0;
   Float_t         RecZ0;
   Float_t         RecPhi0;
   Float_t         RecTheta;
   Float_t         RecEta;
   Float_t         RecQoverP;
   Float_t         RecErrD0;
   Float_t         RecErrZ0;
   Float_t         RecErrPhi0;
   Float_t         RecErrTheta;
   Float_t         RecErrQoverP;
   Float_t         Chi2overNdof;
   Int_t           Ndof;
   Float_t         trk_Mc_d0;
   Float_t         trk_Mc_z0;
   Float_t         trk_Mc_phi0;
   Float_t         trk_Mc_theta;
   Float_t         trk_Mc_qOverP;
   Float_t         trk_Mc_qOverPt;
   Float_t         trk_Mc_eta;
   Float_t         trk_Mc_diff_d0;
   Float_t         trk_Mc_diff_z0;
   Float_t         trk_Mc_diff_phi0;
   Float_t         trk_Mc_diff_theta;
   Float_t         trk_Mc_diff_qOverP;
   Float_t         trk_Mc_pull_d0;
   Float_t         trk_Mc_pull_z0;
   Float_t         trk_Mc_pull_phi0;
   Float_t         trk_Mc_pull_theta;
   Float_t         trk_Mc_pull_qOverP;
   Int_t           trk_Mc_particleID;
   Int_t           trk_Mc_barcode;
   Float_t         trk_Mc_energy;
   vector<float>   *pullLocX;
   vector<float>   *pullLocY;
   vector<float>   *residualLocX;
   vector<float>   *residualLocY;
   vector<int>     *DetectorType;
   vector<int>     *outlierFlag;
   Int_t           nPixelHits;
   Int_t           nSCTHits;
   Int_t           nTRTHits;
   Int_t           nMDTHits;
   Int_t           nCSCHits;
   Int_t           nRPCHits;
   Int_t           nTGCHits;
   vector<float>   *PixPullPhi;
   vector<float>   *PixPullEta;
   vector<float>   *PixResPhi;
   vector<float>   *PixResEta;
   vector<float>   *SCTPull;
   vector<float>   *SCTRes;
   vector<float>   *TRTPull;
   vector<float>   *TRTRes;
   vector<float>   *HitX;
   vector<float>   *HitY;
   vector<float>   *HitZ;
   vector<float>   *HitRadius;
   vector<float>   *HitPhi;
   vector<float>   *HitLocal1;
   vector<float>   *HitLocal2;
   vector<float>   *HitSurfaceX;
   vector<float>   *HitSurfaceY;
   vector<float>   *HitSurfaceZ;
   vector<float>   *HitSurfaceRadius;
   vector<float>   *HitSurfacePhi;
   vector<float>   *HitSurfaceTheta;
   vector<float>   *TrackX;
   vector<float>   *TrackY;
   vector<float>   *TrackZ;
   vector<float>   *TrackMomentumX;
   vector<float>   *TrackMomentumY;
   vector<float>   *TrackMomentumZ;
   vector<float>   *TrackLocal1;
   vector<float>   *TrackLocal2;
   vector<int>     *CompROTnContainedROTs;
   vector<int>     *CompROTindexOfMaxAssgnProb;
   vector<float>   *CompROTmaxAssgnProb;

   // List of branches
   TBranch        *b_run_number;   //!
   TBranch        *b_event_number;   //!
   TBranch        *b_ID_of_track_in_event;   //!
   TBranch        *b_Iteration_of_a_track;   //!
   TBranch        *b_Number_of_measurements_on_track;   //!
   TBranch        *b_are_track_states_unbiased;   //!
   TBranch        *b_Reconstructed_d0;   //!
   TBranch        *b_Reconstructed_z0;   //!
   TBranch        *b_Reconstructed_phi0;   //!
   TBranch        *b_Reconstructed_theta;   //!
   TBranch        *b_Reconstructed_eta;   //!
   TBranch        *b_Reconstructed_Q_over_p;   //!
   TBranch        *b_err_d0;   //!
   TBranch        *b_err_z0;   //!
   TBranch        *b_err_phi0;   //!
   TBranch        *b_err_theta;   //!
   TBranch        *b_err_Q_over_p;   //!
   TBranch        *b_Reco_Chi2_over_Ndof;   //!
   TBranch        *b_Reco_Ndof;   //!
   TBranch        *b_truth_d0;   //!
   TBranch        *b_truth_z0;   //!
   TBranch        *b_truth_phi0;   //!
   TBranch        *b_truth_theta;   //!
   TBranch        *b_truth_Q_over_p;   //!
   TBranch        *b_truth_Q_over_pT;   //!
   TBranch        *b_truth_eta;   //!
   TBranch        *b_truth_reco_diff_d0;   //!
   TBranch        *b_truth_reco_diff_z0;   //!
   TBranch        *b_truth_reco_diff_phi0;   //!
   TBranch        *b_truth_reco_diff_theta;   //!
   TBranch        *b_truth_reco_diff_Q_over_p;   //!
   TBranch        *b_truth_reco_pull_d0;   //!
   TBranch        *b_truth_PDG_ID;   //!
   TBranch        *b_truth_particle_barcode;   //!
   TBranch        *b_truth_energy;   //!
   TBranch        *b_pullLocX;   //!
   TBranch        *b_pullLocY;   //!
   TBranch        *b_residualLocX;   //!
   TBranch        *b_residualLocY;   //!
   TBranch        *b_DetectorType;   //!
   TBranch        *b_outlierFlag;   //!
   TBranch        *b_number_Pixel_measurements;   //!
   TBranch        *b_number_SCT_measurements;   //!
   TBranch        *b_number_TRT_measurements;   //!
   TBranch        *b_number_MDT_measurements;   //!
   TBranch        *b_number_CSC_measurements;   //!
   TBranch        *b_number_RPC_measurements;   //!
   TBranch        *b_number_TGC_measurements;   //!
   TBranch        *b_PixPullPhi;   //!
   TBranch        *b_PixPullEta;   //!
   TBranch        *b_PixResPhi;   //!
   TBranch        *b_PixResEta;   //!
   TBranch        *b_SCTPull;   //!
   TBranch        *b_SCTRes;   //!
   TBranch        *b_TRTPull;   //!
   TBranch        *b_TRTRes;   //!
   TBranch        *b_HitX;   //!
   TBranch        *b_HitY;   //!
   TBranch        *b_HitZ;   //!
   TBranch        *b_HitRadius;   //!
   TBranch        *b_HitPhi;   //!
   TBranch        *b_HitLocal1;   //!
   TBranch        *b_HitLocal2;   //!
   TBranch        *b_HitSurfaceX;   //!
   TBranch        *b_HitSurfaceY;   //!
   TBranch        *b_HitSurfaceZ;   //!
   TBranch        *b_HitSurfaceRadius;   //!
   TBranch        *b_HitSurfacePhi;   //!
   TBranch        *b_HitSurfaceTheta;   //!
   TBranch        *b_TrackX;   //!
   TBranch        *b_TrackY;   //!
   TBranch        *b_TrackZ;   //!
   TBranch        *b_TrackMomentumX;   //!
   TBranch        *b_TrackMomentumY;   //!
   TBranch        *b_TrackMomentumZ;   //!
   TBranch        *b_TrackLocal1;   //!
   TBranch        *b_TrackLocal2;   //!
   TBranch        *b_CompROTnContainedROTs;   //!
   TBranch        *b_CompROTindexOfMaxAssgnProb;   //!
   TBranch        *b_CompROTmaxAssgnProb;   //!

   TrkValidationNtupleOutputTrack(TTree *tree=0);
   virtual ~TrkValidationNtupleOutputTrack();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TrkValidationNtupleOutputTrack_cxx
TrkValidationNtupleOutputTrack::TrkValidationNtupleOutputTrack(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("TrkValidation.root");
      if (!f) {
         f = new TFile("TrkValidation.root");
         f->cd("TrkValidation.root:/FitterValidation");
      }
      tree = (TTree*)gDirectory->Get("OutputTrack");

   }
   Init(tree);
}

TrkValidationNtupleOutputTrack::~TrkValidationNtupleOutputTrack()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TrkValidationNtupleOutputTrack::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TrkValidationNtupleOutputTrack::LoadTree(Long64_t entry)
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

void TrkValidationNtupleOutputTrack::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normaly not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   pullLocX = 0;
   pullLocY = 0;
   residualLocX = 0;
   residualLocY = 0;
   DetectorType = 0;
   outlierFlag = 0;
   PixPullPhi = 0;
   PixPullEta = 0;
   PixResPhi = 0;
   PixResEta = 0;
   SCTPull = 0;
   SCTRes = 0;
   TRTPull = 0;
   TRTRes = 0;
   HitX = 0;
   HitY = 0;
   HitZ = 0;
   HitRadius = 0;
   HitPhi = 0;
   HitLocal1 = 0;
   HitLocal2 = 0;
   HitSurfaceX = 0;
   HitSurfaceY = 0;
   HitSurfaceZ = 0;
   HitSurfaceRadius = 0;
   HitSurfacePhi = 0;
   HitSurfaceTheta = 0;
   TrackX = 0;
   TrackY = 0;
   TrackZ = 0;
   TrackMomentumX = 0;
   TrackMomentumY = 0;
   TrackMomentumZ = 0;
   TrackLocal1 = 0;
   TrackLocal2 = 0;
   CompROTnContainedROTs = 0;
   CompROTindexOfMaxAssgnProb = 0;
   CompROTmaxAssgnProb = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

/*   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_run_number);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_event_number);
   fChain->SetBranchAddress("TrackID", &TrackID, &b_ID_of_track_in_event);
   fChain->SetBranchAddress("IterationIndex", &IterationIndex, &b_Iteration_of_a_track);
   fChain->SetBranchAddress("nHits", &nHits, &b_Number_of_measurements_on_track);
   fChain->SetBranchAddress("TrackStatesUnbiased", &TrackStatesUnbiased, &b_are_track_states_unbiased);
   fChain->SetBranchAddress("RecD0", &RecD0, &b_Reconstructed_d0);
   fChain->SetBranchAddress("RecZ0", &RecZ0, &b_Reconstructed_z0);
   fChain->SetBranchAddress("RecPhi0", &RecPhi0, &b_Reconstructed_phi0);
   fChain->SetBranchAddress("RecTheta", &RecTheta, &b_Reconstructed_theta);
   fChain->SetBranchAddress("RecEta", &RecEta, &b_Reconstructed_eta);
   fChain->SetBranchAddress("RecQoverP", &RecQoverP, &b_Reconstructed_Q_over_p);
   fChain->SetBranchAddress("RecErrD0", &RecErrD0, &b_err_d0);
   fChain->SetBranchAddress("RecErrZ0", &RecErrZ0, &b_err_z0);
   fChain->SetBranchAddress("RecErrPhi0", &RecErrPhi0, &b_err_phi0);
   fChain->SetBranchAddress("RecErrTheta", &RecErrTheta, &b_err_theta);
   fChain->SetBranchAddress("RecErrQoverP", &RecErrQoverP, &b_err_Q_over_p);
   fChain->SetBranchAddress("Chi2overNdof", &Chi2overNdof, &b_Reco_Chi2_over_Ndof);
   fChain->SetBranchAddress("Ndof", &Ndof, &b_Reco_Ndof);
   fChain->SetBranchAddress("trk_Mc_d0", &trk_Mc_d0, &b_truth_d0);
   fChain->SetBranchAddress("trk_Mc_z0", &trk_Mc_z0, &b_truth_z0);
   fChain->SetBranchAddress("trk_Mc_phi0", &trk_Mc_phi0, &b_truth_phi0);
   fChain->SetBranchAddress("trk_Mc_theta", &trk_Mc_theta, &b_truth_theta);
   fChain->SetBranchAddress("trk_Mc_qOverP", &trk_Mc_qOverP, &b_truth_Q_over_p);
   fChain->SetBranchAddress("trk_Mc_qOverPt", &trk_Mc_qOverPt, &b_truth_Q_over_pT);
   fChain->SetBranchAddress("trk_Mc_eta", &trk_Mc_eta, &b_truth_eta);
   fChain->SetBranchAddress("trk_Mc_diff_d0", &trk_Mc_diff_d0, &b_truth_reco_diff_d0);
   fChain->SetBranchAddress("trk_Mc_diff_z0", &trk_Mc_diff_z0, &b_truth_reco_diff_z0);
   fChain->SetBranchAddress("trk_Mc_diff_phi0", &trk_Mc_diff_phi0, &b_truth_reco_diff_phi0);
   fChain->SetBranchAddress("trk_Mc_diff_theta", &trk_Mc_diff_theta, &b_truth_reco_diff_theta);
   fChain->SetBranchAddress("trk_Mc_diff_qOverP", &trk_Mc_diff_qOverP, &b_truth_reco_diff_Q_over_p);
   fChain->SetBranchAddress("trk_Mc_pull_d0", &trk_Mc_pull_d0, &b_truth_reco_pull_d0);
   fChain->SetBranchAddress("trk_Mc_pull_z0", &trk_Mc_pull_z0, &b_truth_reco_pull_d0);
   fChain->SetBranchAddress("trk_Mc_pull_phi0", &trk_Mc_pull_phi0, &b_truth_reco_pull_d0);
   fChain->SetBranchAddress("trk_Mc_pull_theta", &trk_Mc_pull_theta, &b_truth_reco_pull_d0);
   fChain->SetBranchAddress("trk_Mc_pull_qOverP", &trk_Mc_pull_qOverP, &b_truth_reco_pull_d0);
   fChain->SetBranchAddress("trk_Mc_particleID", &trk_Mc_particleID, &b_truth_PDG_ID);
   fChain->SetBranchAddress("trk_Mc_barcode", &trk_Mc_barcode, &b_truth_particle_barcode);
   fChain->SetBranchAddress("trk_Mc_energy", &trk_Mc_energy, &b_truth_energy);
   fChain->SetBranchAddress("pullLocX", &pullLocX, &b_pullLocX);
   fChain->SetBranchAddress("pullLocY", &pullLocY, &b_pullLocY);
   fChain->SetBranchAddress("residualLocX", &residualLocX, &b_residualLocX);
   fChain->SetBranchAddress("residualLocY", &residualLocY, &b_residualLocY);
   fChain->SetBranchAddress("DetectorType", &DetectorType, &b_DetectorType);
   fChain->SetBranchAddress("outlierFlag", &outlierFlag, &b_outlierFlag);
   fChain->SetBranchAddress("nPixelHits", &nPixelHits, &b_number_Pixel_measurements);
   fChain->SetBranchAddress("nSCTHits", &nSCTHits, &b_number_SCT_measurements);
   fChain->SetBranchAddress("nTRTHits", &nTRTHits, &b_number_TRT_measurements);
   fChain->SetBranchAddress("nMDTHits", &nMDTHits, &b_number_MDT_measurements);
   fChain->SetBranchAddress("nCSCHits", &nCSCHits, &b_number_CSC_measurements);
   fChain->SetBranchAddress("nRPCHits", &nRPCHits, &b_number_RPC_measurements);
   fChain->SetBranchAddress("nTGCHits", &nTGCHits, &b_number_TGC_measurements);
   fChain->SetBranchAddress("PixPullPhi", &PixPullPhi, &b_PixPullPhi);
   fChain->SetBranchAddress("PixPullEta", &PixPullEta, &b_PixPullEta);
   fChain->SetBranchAddress("PixResPhi", &PixResPhi, &b_PixResPhi);
   fChain->SetBranchAddress("PixResEta", &PixResEta, &b_PixResEta);
   fChain->SetBranchAddress("SCTPull", &SCTPull, &b_SCTPull);
   fChain->SetBranchAddress("SCTRes", &SCTRes, &b_SCTRes);
   fChain->SetBranchAddress("TRTPull", &TRTPull, &b_TRTPull);
   fChain->SetBranchAddress("TRTRes", &TRTRes, &b_TRTRes);
   fChain->SetBranchAddress("HitX", &HitX, &b_HitX);
   fChain->SetBranchAddress("HitY", &HitY, &b_HitY);
   fChain->SetBranchAddress("HitZ", &HitZ, &b_HitZ);
   fChain->SetBranchAddress("HitRadius", &HitRadius, &b_HitRadius);
   fChain->SetBranchAddress("HitPhi", &HitPhi, &b_HitPhi);
   fChain->SetBranchAddress("HitLocal1", &HitLocal1, &b_HitLocal1);
   fChain->SetBranchAddress("HitLocal2", &HitLocal2, &b_HitLocal2);
   fChain->SetBranchAddress("HitSurfaceX", &HitSurfaceX, &b_HitSurfaceX);
   fChain->SetBranchAddress("HitSurfaceY", &HitSurfaceY, &b_HitSurfaceY);
   fChain->SetBranchAddress("HitSurfaceZ", &HitSurfaceZ, &b_HitSurfaceZ);
   fChain->SetBranchAddress("HitSurfaceRadius", &HitSurfaceRadius, &b_HitSurfaceRadius);
   fChain->SetBranchAddress("HitSurfacePhi", &HitSurfacePhi, &b_HitSurfacePhi);
   fChain->SetBranchAddress("HitSurfaceTheta", &HitSurfaceTheta, &b_HitSurfaceTheta);
   fChain->SetBranchAddress("TrackX", &TrackX, &b_TrackX);
   fChain->SetBranchAddress("TrackY", &TrackY, &b_TrackY);
   fChain->SetBranchAddress("TrackZ", &TrackZ, &b_TrackZ);
   fChain->SetBranchAddress("TrackMomentumX", &TrackMomentumX, &b_TrackMomentumX);
   fChain->SetBranchAddress("TrackMomentumY", &TrackMomentumY, &b_TrackMomentumY);
   fChain->SetBranchAddress("TrackMomentumZ", &TrackMomentumZ, &b_TrackMomentumZ);
   fChain->SetBranchAddress("TrackLocal1", &TrackLocal1, &b_TrackLocal1);
   fChain->SetBranchAddress("TrackLocal2", &TrackLocal2, &b_TrackLocal2);
   fChain->SetBranchAddress("CompROTnContainedROTs", &CompROTnContainedROTs, &b_CompROTnContainedROTs);
   fChain->SetBranchAddress("CompROTindexOfMaxAssgnProb", &CompROTindexOfMaxAssgnProb, &b_CompROTindexOfMaxAssgnProb);
   fChain->SetBranchAddress("CompROTmaxAssgnProb", &CompROTmaxAssgnProb, &b_CompROTmaxAssgnProb);
   Notify();*/
}

Bool_t TrkValidationNtupleOutputTrack::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TrkValidationNtupleOutputTrack::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TrkValidationNtupleOutputTrack::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TrkValidationNtupleOutputTrack_cxx
