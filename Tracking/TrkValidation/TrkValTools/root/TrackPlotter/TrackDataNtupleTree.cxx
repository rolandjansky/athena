/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackDataNtupleTree.h"
#include <iostream>

TrackDataNtupleTree::TrackDataNtupleTree(TTree *tree, bool includingDAFdata) :
    TrkValidationNtupleOutputTrack(tree),
    m_includingDAFdata(includingDAFdata)
{
    //std::cout<<"calling Constr from TrackDataNtupleTree with tree "<< tree<< std::endl;
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
//     for (int i=0; i<300; i++){
//         (*CompROTnContainedROTs)[i]=0;
//     }
}

Int_t TrackDataNtupleTree::Cut(Long64_t entry)
{
    //std::cout<<"calling Cut from TrackDataNtupleTree"<< std::endl;
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}


TrackDataNtupleTree::~TrackDataNtupleTree()
{
   //std::cout<<"calling Destr from TrackDataNtupleTree"<< std::endl;
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TrackDataNtupleTree::GetEntry(Long64_t entry)
{
    //std::cout<<"calling GetEntry from TrackDataNtupleTree"<< std::endl;
    fChain->SetBranchStatus("*",0);  // disable all branches
    fChain->SetBranchStatus("RunNumber",1);
    fChain->SetBranchStatus("EventNumber",1);
    fChain->SetBranchStatus("TrackID",1);
    fChain->SetBranchStatus("IterationIndex",1);
    fChain->SetBranchStatus("nHits",1);
    fChain->SetBranchStatus("DetectorType",1);
    fChain->SetBranchStatus("outlierFlag",1);
    fChain->SetBranchStatus("HitX",1);
    fChain->SetBranchStatus("HitY",1);
    fChain->SetBranchStatus("HitZ",1);
    fChain->SetBranchStatus("HitLocal1",1);
    fChain->SetBranchStatus("HitLocal2",1);
    fChain->SetBranchStatus("HitSurfaceX",1);
    fChain->SetBranchStatus("HitSurfaceY",1);
    fChain->SetBranchStatus("HitSurfaceZ",1);
    fChain->SetBranchStatus("TrackX",1);
    fChain->SetBranchStatus("TrackY",1);
    fChain->SetBranchStatus("TrackZ",1);
    fChain->SetBranchStatus("TrackMomentumX",1);
    fChain->SetBranchStatus("TrackMomentumY",1);
    fChain->SetBranchStatus("TrackMomentumZ",1);
    fChain->SetBranchStatus("TrackLocal1",1);
    fChain->SetBranchStatus("TrackLocal2",1);
    if (m_includingDAFdata){
        fChain->SetBranchStatus("CompROTnContainedROTs",1);
        fChain->SetBranchStatus("CompROTcontainedROTsAssgnProbs",1);
    //    fChain->SetBranchStatus("CompROTcontainedROTsHitX",1);
    //    fChain->SetBranchStatus("CompROTcontainedROTsHitY",1);
    //    fChain->SetBranchStatus("CompROTcontainedROTsHitZ",1);
        fChain->SetBranchStatus("CompROTcontainedROTsHitLocal1",1);
        fChain->SetBranchStatus("CompROTcontainedROTsHitLocal2",1);
        fChain->SetBranchStatus("CompROTcontainedROTsHitSurfaceX",1);
        fChain->SetBranchStatus("CompROTcontainedROTsHitSurfaceY",1);
        fChain->SetBranchStatus("CompROTcontainedROTsHitSurfaceZ",1);
    }
// Read contents of entry.
   if (!fChain) return 0;
   Int_t result = fChain->GetEntry(entry);
   
/*    for (int i=0; i<300; i++){
        CompROTnContainedROTs[i]=0;
    }*/
    return result;
}

Long64_t TrackDataNtupleTree::LoadTree(Long64_t entry)
{
    //std::cout<<"calling LoadTree from TrackDataNtupleTree"<< std::endl;
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->IsA() != TChain::Class()) return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TrackDataNtupleTree::Init(TTree *tree)
{
//    // The Init() function is called when the selector needs to initialize
//    // a new tree or chain. Typically here the branch addresses of the tree
//    // will be set. It is normaly not necessary to make changes to the
//    // generated code, but the routine can be extended by the user if needed.
//    // Init() will be called many times when running with PROOF.
// 
//    // Set branch addresses
    //std::cout<<"calling Init from TrackDataNtupleTree"<< std::endl;
    if (tree == 0) return;
    fChain = tree;
    fCurrent = -1;
    fChain->SetMakeClass(1);
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
// 
   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_run_number);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_event_number);
   fChain->SetBranchAddress("TrackID", &TrackID, &b_ID_of_track_in_event);
   fChain->SetBranchAddress("IterationIndex", &IterationIndex, &b_Iteration_of_a_track);
   fChain->SetBranchAddress("nHits", &nHits, &b_Number_of_measurements_on_track);
/*   fChain->SetBranchAddress("TrackStatesUnbiased", &TrackStatesUnbiased, &b_are_track_states_unbiased);
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
   fChain->SetBranchAddress("residualLocY", &residualLocY, &b_residualLocY);*/
   fChain->SetBranchAddress("DetectorType", &DetectorType, &b_DetectorType);
   fChain->SetBranchAddress("outlierFlag", &outlierFlag, &b_outlierFlag);
/*   fChain->SetBranchAddress("nPixelHits", &nPixelHits, &b_number_Pixel_measurements);
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
   fChain->SetBranchAddress("TRTRes", &TRTRes, &b_TRTRes);*/
   fChain->SetBranchAddress("HitX", &HitX, &b_HitX);
   fChain->SetBranchAddress("HitY", &HitY, &b_HitY);
   fChain->SetBranchAddress("HitZ", &HitZ, &b_HitZ);
/*   fChain->SetBranchAddress("HitRadius", &HitRadius, &b_HitRadius);
   fChain->SetBranchAddress("HitPhi", &HitPhi, &b_HitPhi);*/
   fChain->SetBranchAddress("HitLocal1", &HitLocal1, &b_HitLocal1);
   fChain->SetBranchAddress("HitLocal2", &HitLocal2, &b_HitLocal2);
   fChain->SetBranchAddress("HitSurfaceX", &HitSurfaceX, &b_HitSurfaceX);
   fChain->SetBranchAddress("HitSurfaceY", &HitSurfaceY, &b_HitSurfaceY);
   fChain->SetBranchAddress("HitSurfaceZ", &HitSurfaceZ, &b_HitSurfaceZ);
/*   fChain->SetBranchAddress("HitSurfaceRadius", &HitSurfaceRadius, &b_HitSurfaceRadius);
   fChain->SetBranchAddress("HitSurfacePhi", &HitSurfacePhi, &b_HitSurfacePhi);
   fChain->SetBranchAddress("HitSurfaceTheta", &HitSurfaceTheta, &b_HitSurfaceTheta);*/
   fChain->SetBranchAddress("TrackX", &TrackX, &b_TrackX);
   fChain->SetBranchAddress("TrackY", &TrackY, &b_TrackY);
   fChain->SetBranchAddress("TrackZ", &TrackZ, &b_TrackZ);
   fChain->SetBranchAddress("TrackMomentumX", &TrackMomentumX, &b_TrackMomentumX);
   fChain->SetBranchAddress("TrackMomentumY", &TrackMomentumY, &b_TrackMomentumY);
   fChain->SetBranchAddress("TrackMomentumZ", &TrackMomentumZ, &b_TrackMomentumZ);
   fChain->SetBranchAddress("TrackLocal1", &TrackLocal1, &b_TrackLocal1);
   fChain->SetBranchAddress("TrackLocal2", &TrackLocal2, &b_TrackLocal2);
   if (m_includingDAFdata){
        std::cout<<"setting branches to DAF data"<< std::endl;
        fChain->SetBranchAddress("CompROTnContainedROTs", &CompROTnContainedROTs, &b_CompROTnContainedROTs);
        fChain->SetBranchAddress("CompROTindexOfMaxAssgnProb", &CompROTindexOfMaxAssgnProb, &b_CompROTindexOfMaxAssgnProb);
        fChain->SetBranchAddress("CompROTmaxAssgnProb", &CompROTmaxAssgnProb, &b_CompROTmaxAssgnProb);
    //    fChain->SetBranchAddress("CompROTindexOfMaxAssgnProb",CompROTindexOfMaxAssgnProb);
    //    fChain->SetBranchAddress("CompROTmaxAssgnProb",CompROTmaxAssgnProb);
/*        fChain->SetBranchAddress("CompROTcontainedROTsAssgnProbs",CompROTcontainedROTsAssgnProbs);
    //    fChain->SetBranchAddress("CompROTcontainedROTsResLocX",CompROTcontainedROTsResLocX);
    //    fChain->SetBranchAddress("CompROTcontainedROTsResLocY",CompROTcontainedROTsResLocY);
    //    fChain->SetBranchAddress("CompROTcontainedROTsPullLocX",CompROTcontainedROTsPullLocX);
    //    fChain->SetBranchAddress("CompROTcontainedROTsPullLocY",CompROTcontainedROTsPullLocY);
    //    fChain->SetBranchAddress("CompROTcontainedROTsHitX",CompROTcontainedROTsHitX);
    //    fChain->SetBranchAddress("CompROTcontainedROTsHitY",CompROTcontainedROTsHitY);
    //    fChain->SetBranchAddress("CompROTcontainedROTsHitZ",CompROTcontainedROTsHitZ);
        fChain->SetBranchAddress("CompROTcontainedROTsHitLocal1",CompROTcontainedROTsHitLocal1);
        fChain->SetBranchAddress("CompROTcontainedROTsHitLocal2",CompROTcontainedROTsHitLocal2);
        fChain->SetBranchAddress("CompROTcontainedROTsHitSurfaceX",CompROTcontainedROTsHitSurfaceX);
        fChain->SetBranchAddress("CompROTcontainedROTsHitSurfaceY",CompROTcontainedROTsHitSurfaceY);
        fChain->SetBranchAddress("CompROTcontainedROTsHitSurfaceZ",CompROTcontainedROTsHitSurfaceZ);*/
    }
    Notify();
}

 Bool_t TrackDataNtupleTree::Notify()
 {
//    // The Notify() function is called when a new file is opened. This
//    // can be either for a new TTree in a TChain or when when a new TTree
//    // is started when using PROOF. Typically here the branch pointers
//    // will be retrieved. It is normaly not necessary to make changes
//    // to the generated code, but the routine can be extended by the
//    // user if needed.
// 
//std::cout<<"calling Notify from TrackDataNtupleTree"<< std::endl;
return kTRUE;
//    // Get branch pointers
//     b_RunNumber = fChain->GetBranch("RunNumber");
//     b_EventNumber = fChain->GetBranch("EventNumber");
//     b_TrackID = fChain->GetBranch("TrackID");
//     b_IterationIndex = fChain->GetBranch("IterationIndex");
//     b_nHits = fChain->GetBranch("nHits");
//    b_TrackStatesUnbiased = fChain->GetBranch("TrackStatesUnbiased");
//    b_RecD0 = fChain->GetBranch("RecD0");
//    b_RecZ0 = fChain->GetBranch("RecZ0");
//    b_RecPhi0 = fChain->GetBranch("RecPhi0");
//    b_RecTheta = fChain->GetBranch("RecTheta");
//    b_RecEta = fChain->GetBranch("RecEta");
//    b_RecQoverP = fChain->GetBranch("RecQoverP");
//    b_RecErrD0 = fChain->GetBranch("RecErrD0");
//    b_RecErrZ0 = fChain->GetBranch("RecErrZ0");
//    b_RecErrPhi0 = fChain->GetBranch("RecErrPhi0");
//    b_RecErrTheta = fChain->GetBranch("RecErrTheta");
//    b_RecErrQoverP = fChain->GetBranch("RecErrQoverP");
//    b_Chi2overNdof = fChain->GetBranch("Chi2overNdof");
//    b_Ndof = fChain->GetBranch("Ndof");
//    b_pullLocX = fChain->GetBranch("pullLocX");
//    b_pullLocY = fChain->GetBranch("pullLocY");
//    b_residualLocX = fChain->GetBranch("residualLocX");
//    b_residualLocY = fChain->GetBranch("residualLocY");
    b_DetectorType = fChain->GetBranch("DetectorType");
    b_outlierFlag = fChain->GetBranch("outlierFlag");
//    b_nPixelHits = fChain->GetBranch("nPixelHits");
//    b_nSCTHits = fChain->GetBranch("nSCTHits");
//    b_nTRTHits = fChain->GetBranch("nTRTHits");
//    b_nMDTHits = fChain->GetBranch("nMDTHits");
//    b_nCSCHits = fChain->GetBranch("nCSCHits");
//    b_nRPCHits = fChain->GetBranch("nRPCHits");
//    b_nTGCHits = fChain->GetBranch("nTGCHits");
//    b_PixPullPhi = fChain->GetBranch("PixPullPhi");
//    b_PixPullEta = fChain->GetBranch("PixPullEta");
//    b_PixResPhi = fChain->GetBranch("PixResPhi");
//    b_PixResEta = fChain->GetBranch("PixResEta");
//    b_SCTPull = fChain->GetBranch("SCTPull");
//    b_SCTRes = fChain->GetBranch("SCTRes");
//    b_TRTPull = fChain->GetBranch("TRTPull");
//    b_TRTRes = fChain->GetBranch("TRTRes");
    b_HitX = fChain->GetBranch("HitX");
    b_HitY = fChain->GetBranch("HitY");
    b_HitZ = fChain->GetBranch("HitZ");
//    b_HitRadius = fChain->GetBranch("HitRadius");
//    b_HitPhi = fChain->GetBranch("HitPhi");
    b_HitLocal1 = fChain->GetBranch("HitLocal1");
    b_HitLocal2 = fChain->GetBranch("HitLocal2");
    b_HitSurfaceX = fChain->GetBranch("HitSurfaceX");
    b_HitSurfaceY = fChain->GetBranch("HitSurfaceY");
    b_HitSurfaceZ = fChain->GetBranch("HitSurfaceZ");
//    b_HitSurfaceRadius = fChain->GetBranch("HitSurfaceRadius");
//    b_HitSurfacePhi = fChain->GetBranch("HitSurfacePhi");
//    b_HitSurfaceTheta = fChain->GetBranch("HitSurfaceTheta");
    b_TrackX = fChain->GetBranch("TrackX");
    b_TrackY = fChain->GetBranch("TrackY");
    b_TrackZ = fChain->GetBranch("TrackZ");
    b_TrackMomentumX = fChain->GetBranch("TrackMomentumX");
    b_TrackMomentumY = fChain->GetBranch("TrackMomentumY");
    b_TrackMomentumZ = fChain->GetBranch("TrackMomentumZ");
    b_TrackLocal1 = fChain->GetBranch("TrackLocal1");
    b_TrackLocal2 = fChain->GetBranch("TrackLocal2");
// 
    if (m_includingDAFdata){
        std::cout<<"getting branches to DAF data"<< std::endl;
        b_CompROTnContainedROTs = fChain->GetBranch("CompROTnContainedROTs");
        //   b_CompROTindexOfMaxAssgnProb = fChain->GetBranch("CompROTindexOfMaxAssgnProb");
        //   b_CompROTmaxAssgnProb = fChain->GetBranch("CompROTmaxAssgnProb");
//         b_CompROTcontainedROTsAssgnProbs = fChain->GetBranch("CompROTcontainedROTsAssgnProbs");
//         //   b_CompROTcontainedROTsResLocX = fChain->GetBranch("CompROTcontainedROTsResLocX");
//         //   b_CompROTcontainedROTsResLocY = fChain->GetBranch("CompROTcontainedROTsResLocY");
//         //   b_CompROTcontainedROTsPullLocX = fChain->GetBranch("CompROTcontainedROTsPullLocX");
//         //   b_CompROTcontainedROTsPullLocY = fChain->GetBranch("CompROTcontainedROTsPullLocY");
//         //   b_CompROTcontainedROTsHitX = fChain->GetBranch("CompROTcontainedROTsHitX");
//         //   b_CompROTcontainedROTsHitY = fChain->GetBranch("CompROTcontainedROTsHitY");
//         //   b_CompROTcontainedROTsHitZ = fChain->GetBranch("CompROTcontainedROTsHitZ");
//         b_CompROTcontainedROTsHitLocal1 = fChain->GetBranch("CompROTcontainedROTsHitLocal1");
//         b_CompROTcontainedROTsHitLocal2 = fChain->GetBranch("CompROTcontainedROTsHitLocal2");
//         b_CompROTcontainedROTsHitSurfaceX = fChain->GetBranch("CompROTcontainedROTsHitSurfaceX");
//         b_CompROTcontainedROTsHitSurfaceY = fChain->GetBranch("CompROTcontainedROTsHitSurfaceY");
//         b_CompROTcontainedROTsHitSurfaceZ = fChain->GetBranch("CompROTcontainedROTsHitSurfaceZ");
    }
    return kTRUE;
 }

void TrackDataNtupleTree::Show(Long64_t entry)
{
//std::cout<<"calling Show from TrackDataNtupleTree"<< std::endl;
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

void TrackDataNtupleTree::Loop()
{
//std::cout<<"calling Loop from TrackDataNtupleTree"<< std::endl;
//   In a ROOT session, you can do:
//      Root > .L TrkValidationNtupleOutputTrack.C
//      Root > TrkValidationNtupleOutputTrack t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Int_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}

