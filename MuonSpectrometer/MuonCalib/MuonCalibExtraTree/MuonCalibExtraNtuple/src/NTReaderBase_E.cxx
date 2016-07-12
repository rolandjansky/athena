/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraNtuple/NTReaderBase_E.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

namespace MuonCalib {

  void NTReaderBase_E::Loop() {
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

  NTReaderBase_E::NTReaderBase_E(TTree *tree) : NTReaderBase( tree ) {
    // if parameter tree is not specified (or zero), connect the file
    // used to generate this class and read the Tree.
    if (tree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Memory Directory");
      if (!f) {
	f = new TFile("Memory Directory");
	f->cd("SegmentNtuple.root:/PatternNtupleMaker");
      }
      tree = (TTree*)gDirectory->Get("NTReaderBase");

#else // SINGLE_TREE
      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("NTReaderBase","");
      chain->Add("SegmentNtuple.root/PatternNtupleMaker/Segments");
      tree = chain;
#endif // SINGLE_TREE

    }
    Init(tree);
  }

  NTReaderBase_E::~NTReaderBase_E() {
  }

  Int_t NTReaderBase_E::GetEntry(Long64_t entry) {
    // Read contents of entry.
    if (!fChain) return 0;
    return fChain->GetEntry(entry);
  }

  Long64_t NTReaderBase_E::LoadTree(Long64_t entry) {
    // Set the environment to read one entry
    if (!fChain) return -5;
    Long64_t centry = fChain->LoadTree(entry);
    if (centry < 0) return centry;
    if (fChain->IsA() != TChain::Class()) return centry;
    TChain *chain = (TChain*)fChain;
    if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
    }
    return centry;
  }

  void NTReaderBase_E::Init(TTree *tree) {
    // The Init() function is called when the selector needs to initialize
    // a new tree or chain. Typically here the branch addresses of the tree
    // will be set. It is normaly not necessary to make changes to the
    // generated code, but the routine can be extended by the user if needed.
    // Init() will be called many times when running with PROOF.

    // Set branch addresses
    if (tree == 0) return;
    fChain = tree;
    fCurrent = -1;
    fChain->SetMakeClass(1);

// trigger info
    ctp_nTriggerInfo =0;
    fChain->SetBranchAddress("ctp_nTriggerInfo",&ctp_nTriggerInfo);
    fChain->SetBranchAddress("ctp_numberBC", &ctp_numberBC);
    fChain->SetBranchAddress("ctp_L1A_BC", &ctp_L1A_BC);
    fChain->SetBranchAddress("ctp_timeNs", &ctp_timeNs);
    fChain->SetBranchAddress("ctp_randomTrig", &ctp_randomTrig);
    fChain->SetBranchAddress("ctp_firedItemsBeforePrescale", &ctp_firedItemsBeforePrescale);
    fChain->SetBranchAddress("ctp_prescaledClock", &ctp_prescaledClock);
    fChain->SetBranchAddress("ctp_bcid", &ctp_bcid);
    fChain->SetBranchAddress("ctp_bcIndexPIT", &ctp_bcIndexPIT);
    fChain->SetBranchAddress("ctp_pit", &ctp_pit);
    fChain->SetBranchAddress("ctp_bcIndexTriggerItems", &ctp_bcIndexTriggerItems);
    fChain->SetBranchAddress("ctp_tbp", &ctp_tbp);
    fChain->SetBranchAddress("ctp_tap", &ctp_tap);
    fChain->SetBranchAddress("ctp_tav", &ctp_tav);
    fChain->SetBranchAddress("ctp_type",&ctp_type);
    fChain->SetBranchAddress("ctp_delay",&ctp_delay);

//mu ctpi info
    muctpi_nHits=0;
    fChain->SetBranchAddress("muctpi_nHits", &muctpi_nHits);
    fChain->SetBranchAddress("muctpi_roiWord", &muctpi_roiWord);
    fChain->SetBranchAddress("muctpi_bcID", &muctpi_bcID);
    fChain->SetBranchAddress("muctpi_sysID", &muctpi_sysID);
    fChain->SetBranchAddress("muctpi_subsysID", &muctpi_subsysID);
    fChain->SetBranchAddress("muctpi_sectorID", &muctpi_sectorID);
    fChain->SetBranchAddress("muctpi_thresholdNumber", &muctpi_thresholdNumber);
    fChain->SetBranchAddress("muctpi_thresholdValue", &muctpi_thresholdValue);
    fChain->SetBranchAddress("muctpi_roINumber", &muctpi_roINumber);
    fChain->SetBranchAddress("muctpi_overlap", &muctpi_overlap);
    fChain->SetBranchAddress("muctpi_firstCandidate", &muctpi_firstCandidate);
    fChain->SetBranchAddress("muctpi_sectorOverflow", &muctpi_sectorOverflow);
    fChain->SetBranchAddress("muctpi_padOverflow", &muctpi_padOverflow);
    fChain->SetBranchAddress("muctpi_phi", &muctpi_phi);
    fChain->SetBranchAddress("muctpi_eta", &muctpi_eta);

//mu rpcsl info
    if(fChain->GetBranch("rpc_SL_nSectors") != NULL) {
      rpc_SL_nSectors = 0;
      fChain->SetBranchAddress("rpc_SL_nSectors", &rpc_SL_nSectors);
      fChain->SetBranchAddress("rpc_SL_sectorId", &rpc_SL_sectorId);
      fChain->SetBranchAddress("rpc_SL_lvl1id", &rpc_SL_lvl1id);
      fChain->SetBranchAddress("rpc_SL_bcid", &rpc_SL_bcid);
      fChain->SetBranchAddress("rpc_SL_errorCode", &rpc_SL_errorCode);
      fChain->SetBranchAddress("rpc_SL_crc", &rpc_SL_crc);
      fChain->SetBranchAddress("rpc_SL_hasMoreThan2TriggerCand", &rpc_SL_hasMoreThan2TriggerCand);
      fChain->SetBranchAddress("rpc_SLnTriggerHits", &rpc_SLnTriggerHits);
      fChain->SetBranchAddress("rpc_SLhit_sector", &rpc_SLhit_sector);
      fChain->SetBranchAddress("rpc_SLhit_rowinBcid", &rpc_SLhit_rowinBcid);
      fChain->SetBranchAddress("rpc_SLhit_padId", &rpc_SLhit_padId);
      fChain->SetBranchAddress("rpc_SLhit_ptId", &rpc_SLhit_ptId);
      fChain->SetBranchAddress("rpc_SLhit_roi", &rpc_SLhit_roi);
      fChain->SetBranchAddress("rpc_SLhit_outerPlane", &rpc_SLhit_outerPlane);
      fChain->SetBranchAddress("rpc_SLhit_overlapPhi", &rpc_SLhit_overlapPhi);
      fChain->SetBranchAddress("rpc_SLhit_overlapEta", &rpc_SLhit_overlapEta);
      fChain->SetBranchAddress("rpc_SLhit_triggerBcid", &rpc_SLhit_triggerBcid);
      fChain->SetBranchAddress("rpc_SLhit_isInput", &rpc_SLhit_isInput);
    }else{
      //Reading old nomenclature
      rpcSl_nLogics=0;
      fChain->SetBranchAddress("rpcSl_nLogics", &rpcSl_nLogics);    
      fChain->SetBranchAddress("rpcSl_sectorId", &rpcSl_sectorId);
      fChain->SetBranchAddress("rpcSl_fel1Id", &rpcSl_fel1Id);
      fChain->SetBranchAddress("rpcSl_bcid", &rpcSl_bcid);
      fChain->SetBranchAddress("rpcSl_errorCode", &rpcSl_errorCode);
      fChain->SetBranchAddress("rpcSl_crc", &rpcSl_crc);
      fChain->SetBranchAddress("rpcSl_hasMoreThan2TriggerCand", &rpcSl_hasMoreThan2TriggerCand);
      fChain->SetBranchAddress("rpcSl_nLogicsTimes", &rpcSl_nLogicsTimes);
      fChain->SetBranchAddress("rpcSl_slIndex", &rpcSl_slIndex);
      fChain->SetBranchAddress("rpcSl_triggerRates", &rpcSl_triggerRates);
      fChain->SetBranchAddress("rpcSl_counters", &rpcSl_counters);
      fChain->SetBranchAddress("rpcSl_nSLHits", &rpcSl_nSLHits);
      fChain->SetBranchAddress("rpcSl_slIndex2", &rpcSl_slIndex2);
      fChain->SetBranchAddress("rpcSl_rowinBcid", &rpcSl_rowinBcid);
      fChain->SetBranchAddress("rpcSl_padid", &rpcSl_padid);
      fChain->SetBranchAddress("rpcSl_ptid", &rpcSl_ptid);
      fChain->SetBranchAddress("rpcSl_roi", &rpcSl_roi);
      fChain->SetBranchAddress("rpcSl_outerPlane", &rpcSl_outerPlane);
      fChain->SetBranchAddress("rpcSl_overlapPhi", &rpcSl_overlapPhi);
      fChain->SetBranchAddress("rpcSl_overlapEta", &rpcSl_overlapEta);
      fChain->SetBranchAddress("rpcSl_triggerBcid", &rpcSl_triggerBcid);
      fChain->SetBranchAddress("rpcSl_isInput", &rpcSl_isInput);
    }
    
//rpc coin info
    rawRpcCoin_nRpcCoin =0;
    if(fChain->GetBranch("rawRpcCoin_nRpcCoin") != NULL) {
      fChain->SetBranchAddress("rawRpcCoin_nRpcCoin", &rawRpcCoin_nRpcCoin);
      fChain->SetBranchAddress("rawRpcCoin_occupancy", rawRpcCoin_occupancy);
      fChain->SetBranchAddress("rawRpcCoin_id", rawRpcCoin_id); 
      fChain->SetBranchAddress("rawRpcCoin_t", rawRpcCoin_t); 
      fChain->SetBranchAddress("rawRpcCoin_width", rawRpcCoin_width); 
      fChain->SetBranchAddress("rawRpcCoin_length", rawRpcCoin_length);
      fChain->SetBranchAddress("rawRpcCoin_gPosX", rawRpcCoin_gPosX);
      fChain->SetBranchAddress("rawRpcCoin_gPosY", rawRpcCoin_gPosY);
      fChain->SetBranchAddress("rawRpcCoin_gPosZ", rawRpcCoin_gPosZ);
      fChain->SetBranchAddress("rawRpcCoin_ijk", rawRpcCoin_ijk);
      fChain->SetBranchAddress("rawRpcCoin_threshold", rawRpcCoin_threshold);
      fChain->SetBranchAddress("rawRpcCoin_overlap", rawRpcCoin_overlap); 
      fChain->SetBranchAddress("rawRpcCoin_parentCmId", rawRpcCoin_parentCmId); 
      fChain->SetBranchAddress("rawRpcCoin_parentPadId", rawRpcCoin_parentPadId); 
      fChain->SetBranchAddress("rawRpcCoin_parentSectorId", rawRpcCoin_parentSectorId); 
      fChain->SetBranchAddress("rawRpcCoin_lowPtCm", rawRpcCoin_lowPtCm);
    }  

// calo hit info
    calo_nHits=0;
    if(fChain->GetBranch("calo_nHits") != NULL) {
      fChain->SetBranchAddress("calo_nHits",&calo_nHits);
      fChain->SetBranchAddress("calo_id",calo_id);
      fChain->SetBranchAddress("calo_posX",calo_posX);
      fChain->SetBranchAddress("calo_posY",calo_posY);
      fChain->SetBranchAddress("calo_posZ",calo_posZ);
      fChain->SetBranchAddress("calo_charge",calo_charge);
      fChain->SetBranchAddress("calo_time",calo_time);
    }
// mbts  hit info
    mbts_nHits=0;
    if(fChain->GetBranch("mbts_nHits") != NULL) {
      fChain->SetBranchAddress("mbts_nHits",&mbts_nHits);
      fChain->SetBranchAddress("mbts_id",mbts_id);
      fChain->SetBranchAddress("mbts_posX",mbts_posX);
      fChain->SetBranchAddress("mbts_posY",mbts_posY);
      fChain->SetBranchAddress("mbts_posZ",mbts_posZ);
      fChain->SetBranchAddress("mbts_charge",mbts_charge);
      fChain->SetBranchAddress("mbts_time",mbts_time);
    }

// phi hit info 
    phi_nPhiHits =0;
    if(fChain->GetBranch("phi_nPhiHits") != NULL) {
      fChain->SetBranchAddress("phi_nPhiHits",&phi_nPhiHits);
      fChain->SetBranchAddress("phi_phiPatIndex", phi_phiPatIndex);
      fChain->SetBranchAddress("phi_id",phi_id);
      fChain->SetBranchAddress("phi_posX",phi_posX);
      fChain->SetBranchAddress("phi_posY",phi_posY);
      fChain->SetBranchAddress("phi_posZ",phi_posZ);
      fChain->SetBranchAddress("phi_error",phi_error);
    }

    fChain->SetBranchAddress("trk_nTracks", &trk_nTracks);
    fChain->SetBranchAddress("trk_x0", trk_x0);
    fChain->SetBranchAddress("trk_y0", trk_y0);
    fChain->SetBranchAddress("trk_z0", trk_z0);
    fChain->SetBranchAddress("trk_theta", trk_theta);
    fChain->SetBranchAddress("trk_phi", trk_phi);
    fChain->SetBranchAddress("trk_qOverP", trk_qOverP);
    fChain->SetBranchAddress("trk_author", trk_author);
    fChain->SetBranchAddress("trk_cov00", trk_cov00);
    fChain->SetBranchAddress("trk_cov01", trk_cov01);
    fChain->SetBranchAddress("trk_cov02", trk_cov02);
    fChain->SetBranchAddress("trk_cov03", trk_cov03);
    fChain->SetBranchAddress("trk_cov04", trk_cov04);
    fChain->SetBranchAddress("trk_cov11", trk_cov11);
    fChain->SetBranchAddress("trk_cov12", trk_cov12);
    fChain->SetBranchAddress("trk_cov13", trk_cov13);
    fChain->SetBranchAddress("trk_cov14", trk_cov14);
    fChain->SetBranchAddress("trk_cov22", trk_cov22);
    fChain->SetBranchAddress("trk_cov23", trk_cov23);
    fChain->SetBranchAddress("trk_cov24", trk_cov24);
    fChain->SetBranchAddress("trk_cov33", trk_cov33);
    fChain->SetBranchAddress("trk_cov34", trk_cov34);
    fChain->SetBranchAddress("trk_cov44", trk_cov44);
//    fChain->SetBranchAddress("trk_errInvP", trk_errInvP);
    fChain->SetBranchAddress("trk_chi2", trk_chi2);
    fChain->SetBranchAddress("trk_ndof", trk_ndof);

    fChain->SetBranchAddress("trkHit_nHits", &trkHit_nHits);
    fChain->SetBranchAddress("trkHit_id", trkHit_id);
    fChain->SetBranchAddress("trkHit_trackIndex", trkHit_trackIndex);
    fChain->SetBranchAddress("trkHit_posX", trkHit_posX);
    fChain->SetBranchAddress("trkHit_posY", trkHit_posY);
    fChain->SetBranchAddress("trkHit_posZ", trkHit_posZ);
    if(fChain->GetBranch("trkHit_driftTime")!=0) {
      fChain->SetBranchAddress("trkHit_driftTime", trkHit_driftTime);
    } else {
      for( int i=0; i<3000; i++ ) trkHit_driftTime[i] = 0.;
    }
    fChain->SetBranchAddress("trkHit_driftRadius", trkHit_driftRadius);
    fChain->SetBranchAddress("trkHit_error", trkHit_error);
    fChain->SetBranchAddress("trkHit_resi", trkHit_resi);
    fChain->SetBranchAddress("trkHit_pull", trkHit_pull);
    fChain->SetBranchAddress("trkHit_type", trkHit_type);

    fChain->SetBranchAddress("trkHole_nHoles", &trkHole_nHoles);
    fChain->SetBranchAddress("trkHole_id", trkHole_id);
    fChain->SetBranchAddress("trkHole_trackIndex", trkHole_trackIndex);
    fChain->SetBranchAddress("trkHole_posX", trkHole_posX);
    fChain->SetBranchAddress("trkHole_posY", trkHole_posY);
    fChain->SetBranchAddress("trkHole_posZ", trkHole_posZ);
    
    if(fChain->GetBranch("trkSeg_nTrkSegs")!=0) {
      fChain->SetBranchAddress("trkSeg_nTrkSegs", &trkSeg_nTrkSegs);
      fChain->SetBranchAddress("trkSeg_trkIndex", trkSeg_trkIndex);
      fChain->SetBranchAddress("trkSeg_segIndex", trkSeg_segIndex);
      hasTrkSegs=true;
    } else {
      trkSeg_nTrkSegs=0;
      hasTrkSegs=false;
    }
    
  }  //end NTReaderBase_E::Init

  void NTReaderBase_E::Show(Long64_t entry) {
    // Print contents of entry.
    // If entry is not specified, print current entry
    if (!fChain) return;
    fChain->Show(entry);
  }

  Int_t NTReaderBase_E::Cut(Long64_t /*entry*/) {
    // This function may be called from Loop.
    // returns  1 if entry is accepted.
    // returns -1 otherwise.
    return 1;
  }

} //namespace MuonCalib
