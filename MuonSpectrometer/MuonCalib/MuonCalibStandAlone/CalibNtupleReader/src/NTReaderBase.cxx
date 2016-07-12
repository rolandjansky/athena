/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibNtupleReader/NTReaderBase.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

namespace MuonCalib {

void NTReaderBase::Loop() {
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

NTReaderBase::NTReaderBase(TTree *tree) {
   
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
  m_storeTruth=(tree->GetBranch("truth_nTruth") != NULL);
  Init(tree);
}

NTReaderBase::~NTReaderBase() {
  if (!fChain) return;
  delete fChain->GetCurrentFile();
}

Int_t NTReaderBase::GetEntry(Long64_t entry) {
// Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}
 
Long64_t NTReaderBase::LoadTree(Long64_t entry) {
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

void NTReaderBase::Init(TTree *tree) {
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

  if(m_storeTruth){
    fChain->SetBranchAddress("truth_nTruth",&truth_nTruth);
    fChain->SetBranchAddress("truth_kinEnergy",truth_kinEnergy);
    fChain->SetBranchAddress("truth_gPosX",truth_gPosX);
    fChain->SetBranchAddress("truth_gPosY",truth_gPosY);
    fChain->SetBranchAddress("truth_gPosZ",truth_gPosZ);
    fChain->SetBranchAddress("truth_pX",truth_pX);
    fChain->SetBranchAddress("truth_pY",truth_pY);
    fChain->SetBranchAddress("truth_pZ",truth_pZ);
    fChain->SetBranchAddress("truth_PDGCode",truth_PDGCode);
    
    fChain->SetBranchAddress("truth_barCode",truth_barCode);
    fChain->SetBranchAddress("mdtTruth_nMdtTruthHit",&mdtTruth_nMdtTruthHit);
    fChain->SetBranchAddress("mdtTruth_id",mdtTruth_id);
    fChain->SetBranchAddress("mdtTruth_barCode",mdtTruth_barCode);
    fChain->SetBranchAddress("mdtTruth_driftRadius",mdtTruth_driftRadius);
    fChain->SetBranchAddress("mdtTruth_posAlongTube",mdtTruth_posAlongTube);
    fChain->SetBranchAddress("rpcTruth_nRpcTruthHit",&rpcTruth_nRpcTruthHit);
    fChain->SetBranchAddress("rpcTruth_id",rpcTruth_id);
    fChain->SetBranchAddress("rpcTruth_barCode",rpcTruth_barCode);
    fChain->SetBranchAddress("rpcTruth_time",rpcTruth_time);
    fChain->SetBranchAddress("tgcTruth_nTgcTruthHit",&tgcTruth_nTgcTruthHit);
    fChain->SetBranchAddress("tgcTruth_id",tgcTruth_id);
    fChain->SetBranchAddress("tgcTruth_barCode",tgcTruth_barCode);
    fChain->SetBranchAddress("tgcTruth_time",tgcTruth_time);
    fChain->SetBranchAddress("cscTruth_nCscTruthHit",&cscTruth_nCscTruthHit);
    fChain->SetBranchAddress("cscTruth_id",cscTruth_id);
    fChain->SetBranchAddress("cscTruth_barCode",cscTruth_barCode);
    fChain->SetBranchAddress("cscTruth_time",cscTruth_time);
  } else {
    truth_nTruth=0;
    mdtTruth_nMdtTruthHit=0;
    rpcTruth_nRpcTruthHit=0;
    tgcTruth_nTgcTruthHit=0;
    cscTruth_nCscTruthHit=0;
  }

  fChain->SetBranchAddress("event_nEvent",&event_nEvent);
  fChain->SetBranchAddress("event_eventNumber",&event_eventNumber);
  fChain->SetBranchAddress("event_runNumber",&event_runNumber);
  fChain->SetBranchAddress("event_timeStamp",&event_timeStamp);
  if (fChain->GetBranch("event_lumiBlock"))
    fChain->SetBranchAddress("event_lumiBlock",&event_lumiBlock);
  else
    event_lumiBlock=-1;
  if (fChain->GetBranch("event_bcId"))
    fChain->SetBranchAddress("event_bcId",&event_bcId);
  else
    event_bcId=-1; 
  if(fChain->GetBranch("event_mbtsTimeDiff"))
    fChain->SetBranchAddress("event_mbtsTimeDiff",&event_mbtsTimeDiff);
  else
    event_mbtsTimeDiff=-1;
  if (fChain->GetBranch("event_larTimeDiff"))
    fChain->SetBranchAddress("event_larTimeDiff",&event_larTimeDiff);
  else
    event_larTimeDiff=-1;
  fChain->SetBranchAddress("event_eventTag",event_eventTag );
  event_n_on_TAV_bits=0;
  fChain->SetBranchAddress("event_n_on_TAV_bits", &event_n_on_TAV_bits );
  fChain->SetBranchAddress("event_on_TAV_bits", &event_on_TAV_bits );
  event_n_on_TAP_bits=0;
  fChain->SetBranchAddress("event_n_on_TAP_bits", &event_n_on_TAP_bits );
  fChain->SetBranchAddress("event_on_TAP_bits", &event_on_TAP_bits );
  
  pat_nPatterns = 0;
  if( fChain->GetBranch("pat_nPatterns") != NULL ) {
    fChain->SetBranchAddress("pat_nPatterns",&pat_nPatterns);
    fChain->SetBranchAddress("pat_chi2",pat_chi2);
    fChain->SetBranchAddress("pat_z0",pat_z0);
    fChain->SetBranchAddress("pat_r0",pat_r0);
    fChain->SetBranchAddress("pat_invP",pat_invP);
    fChain->SetBranchAddress("pat_phi",pat_phi);
    fChain->SetBranchAddress("pat_theta",pat_theta);
    fChain->SetBranchAddress("pat_nSegments",pat_nSegments);
    fChain->SetBranchAddress("pat_nmdt",pat_nmdt);
    fChain->SetBranchAddress("pat_nrpc",pat_nrpc);
    fChain->SetBranchAddress("pat_ncsc",pat_ncsc);
    fChain->SetBranchAddress("pat_ntgc",pat_ntgc);
  }

  fChain->SetBranchAddress("seg_nSegments",&seg_nSegments);
  if( fChain->GetBranch("seg_patIndex") != NULL ) {
    fChain->SetBranchAddress("seg_patIndex",seg_patIndex);
  }
  for(int i=0; i<500; i++) {
    seg_author[i]=0;
    seg_patIndex[i]=-1;
  }
  fChain->SetBranchAddress("seg_author",seg_author);
  fChain->SetBranchAddress("seg_quality",seg_quality); 
  fChain->SetBranchAddress("seg_chi2",seg_chi2);
  fChain->SetBranchAddress("seg_fittedT0",seg_fittedT0);
  fChain->SetBranchAddress("seg_gPosX",seg_gPosX);
  fChain->SetBranchAddress("seg_gPosY",seg_gPosY);
  fChain->SetBranchAddress("seg_gPosZ",seg_gPosZ);
  fChain->SetBranchAddress("seg_gDirX",seg_gDirX);
  fChain->SetBranchAddress("seg_gDirY",seg_gDirY);
  fChain->SetBranchAddress("seg_gDirZ",seg_gDirZ);
  fChain->SetBranchAddress("seg_posX",seg_posX);
  fChain->SetBranchAddress("seg_posY",seg_posY);
  fChain->SetBranchAddress("seg_posZ",seg_posZ);
  fChain->SetBranchAddress("seg_dirX",seg_dirX);
  fChain->SetBranchAddress("seg_dirY",seg_dirY);
  fChain->SetBranchAddress("seg_dirZ",seg_dirZ);
  fChain->SetBranchAddress("seg_nHits",seg_nHits);
  fChain->SetBranchAddress("seg_nMdtHits",seg_nMdtHits);
  fChain->SetBranchAddress("seg_nRpcHits",seg_nRpcHits);
  fChain->SetBranchAddress("seg_nTgcHits",seg_nTgcHits);
  fChain->SetBranchAddress("seg_nCscHits",seg_nCscHits);
  fChain->SetBranchAddress("seg_transPhi",seg_transPhi);
  fChain->SetBranchAddress("seg_transTheta",seg_transTheta);
  fChain->SetBranchAddress("seg_transPsi",seg_transPsi);
  fChain->SetBranchAddress("seg_transX",seg_transX);
  fChain->SetBranchAddress("seg_transY",seg_transY);
  fChain->SetBranchAddress("seg_transZ",seg_transZ);
  fChain->SetBranchAddress("mdt_nMdt",&mdt_nMdt);
  fChain->SetBranchAddress("mdt_segIndex",mdt_segIndex);
  fChain->SetBranchAddress("mdt_id",mdt_id);
  fChain->SetBranchAddress("mdt_tdc",mdt_tdc);
  fChain->SetBranchAddress("mdt_adc",mdt_adc);
  fChain->SetBranchAddress("mdt_t",mdt_t);
  fChain->SetBranchAddress("mdt_r",mdt_r);
  fChain->SetBranchAddress("mdt_dr",mdt_dr);
  fChain->SetBranchAddress("mdt_rTrk",mdt_rTrk);
  fChain->SetBranchAddress("mdt_drTrk",mdt_drTrk);
  fChain->SetBranchAddress("mdt_resi",mdt_resi);
  fChain->SetBranchAddress("mdt_distRO",mdt_distRO);
  fChain->SetBranchAddress("mdt_slewTime",mdt_slewTime);
  fChain->SetBranchAddress("mdt_lorTime",mdt_lorTime);
  fChain->SetBranchAddress("mdt_propTime",mdt_propTime);
  fChain->SetBranchAddress("mdt_tof",mdt_tof);
  fChain->SetBranchAddress("mdt_bFieldPerp",mdt_bFieldPerp);
  fChain->SetBranchAddress("mdt_bFieldPara",mdt_bFieldPara);
  fChain->SetBranchAddress("mdt_temperature",mdt_temperature);
  fChain->SetBranchAddress("mdt_projSag",mdt_projSag);  
  fChain->SetBranchAddress("mdt_posX",mdt_posX);
  fChain->SetBranchAddress("mdt_posY",mdt_posY);
  fChain->SetBranchAddress("mdt_posZ",mdt_posZ);
  fChain->SetBranchAddress("mdt_gPosX",mdt_gPosX);
  fChain->SetBranchAddress("mdt_gPosY",mdt_gPosY);
  fChain->SetBranchAddress("mdt_gPosZ",mdt_gPosZ);
//if branch does not exist set t0s to 0 (hmm, 0 is actually a possible T0 value)
  if(fChain->GetBranch("mdt_tube_t0") == NULL) {
    for(unsigned int i=0; i<3000; i++) mdt_tube_t0[i]=0.;
  } else {
    fChain->SetBranchAddress("mdt_tube_t0",mdt_tube_t0);
  }
  if(fChain->GetBranch("mdt_tube_adccal") == NULL) {
    for(unsigned int i=0; i<3000; i++) mdt_tube_adccal[i]=0.;
  } else {
    fChain->SetBranchAddress("mdt_tube_adccal",mdt_tube_adccal);
  }

//if branch does not exist set twin position to 999999.
  if(fChain->GetBranch("mdt_xtwin") == NULL) {
    for(unsigned int i=0; i<3000; i++) mdt_xtwin[i]=-9999999.;
  } else {
    fChain->SetBranchAddress("mdt_xtwin",mdt_xtwin);
  }

  if(fChain->GetBranch("mdt_segmentT0Applied") == NULL) {
    for(unsigned int i=0; i<3000; i++) mdt_segmentT0Applied[i]=false;
  } else {
    fChain->SetBranchAddress("mdt_segmentT0Applied", mdt_segmentT0Applied);
  }

   //check if branch is called with old or new nomenclature (rpc_ or rpcOs_) 
  rpcOs_nRpcHits = 0;
  rpc_nRpcHits = 0;
  if(fChain->GetBranch("rpcOs_nRpcHits") != NULL){ 
    fChain->SetBranchAddress("rpcOs_nRpcHits",&rpcOs_nRpcHits);
    fChain->SetBranchAddress("rpcOs_segIndex",&rpcOs_segIndex);
    fChain->SetBranchAddress("rpcOs_id",&rpcOs_id);
    fChain->SetBranchAddress("rpcOs_nStrips",&rpcOs_nStrips);
    fChain->SetBranchAddress("rpcOs_stripWidth",&rpcOs_stripWidth);
    fChain->SetBranchAddress("rpcOs_stripLength",&rpcOs_stripLength);
    fChain->SetBranchAddress("rpcOs_error",&rpcOs_error);
    fChain->SetBranchAddress("rpcOs_time",&rpcOs_time);
    fChain->SetBranchAddress("rpcOs_posX",&rpcOs_posX);
    fChain->SetBranchAddress("rpcOs_posY",&rpcOs_posY);
    fChain->SetBranchAddress("rpcOs_posZ",&rpcOs_posZ);
    fChain->SetBranchAddress("rpcOs_gPosX",&rpcOs_gPosX);
    fChain->SetBranchAddress("rpcOs_gPosY",&rpcOs_gPosY);
    fChain->SetBranchAddress("rpcOs_gPosZ",&rpcOs_gPosZ);
    fChain->SetBranchAddress("rpcOs_distanceToRO",&rpcOs_distanceToRO);
    fChain->SetBranchAddress("rpc_fchan",&rpc_fchan);
    fChain->SetBranchAddress("rpc_sector",&rpc_sector);
    fChain->SetBranchAddress("rpc_padId",&rpc_padId);
    fChain->SetBranchAddress("rpc_status",&rpc_status);
    fChain->SetBranchAddress("rpc_ercode",&rpc_ercode);
    fChain->SetBranchAddress("rpc_cmaId",&rpc_cmaId);
    fChain->SetBranchAddress("rpc_fel1Id",&rpc_fel1Id);
    fChain->SetBranchAddress("rpc_febcId",&rpc_febcId);
    fChain->SetBranchAddress("rpc_crc",&rpc_crc);
    fChain->SetBranchAddress("rpc_bcId",&rpc_bcId);
    fChain->SetBranchAddress("rpc_ticks",&rpc_ticks);
    fChain->SetBranchAddress("rpc_ijk",&rpc_ijk);
    fChain->SetBranchAddress("rpc_cmachan",&rpc_cmachan);
    fChain->SetBranchAddress("rpc_overlap",&rpc_overlap);
    fChain->SetBranchAddress("rpc_threshold",&rpc_threshold);
  } else {
    fChain->SetBranchAddress("rpc_nRpcHits",&rpc_nRpcHits);
    fChain->SetBranchAddress("rpc_segIndex",&rpc_segIndex);
    fChain->SetBranchAddress("rpc_id",&rpc_id);
    fChain->SetBranchAddress("rpc_nStrips",&rpc_nStrips);
    fChain->SetBranchAddress("rpc_stripWidth",&rpc_stripWidth);
    fChain->SetBranchAddress("rpc_stripLength",&rpc_stripLength);
    fChain->SetBranchAddress("rpc_error",&rpc_error);
    fChain->SetBranchAddress("rpc_time",&rpc_time);
    fChain->SetBranchAddress("rpc_posX",&rpc_posX);
    fChain->SetBranchAddress("rpc_posY",&rpc_posY);
    fChain->SetBranchAddress("rpc_posZ",&rpc_posZ);
    fChain->SetBranchAddress("rpc_gPosX",&rpc_gPosX);
    fChain->SetBranchAddress("rpc_gPosY",&rpc_gPosY);
    fChain->SetBranchAddress("rpc_gPosZ",&rpc_gPosZ);
    fChain->SetBranchAddress("rpc_distanceToRO",&rpc_distanceToRO);
  }
   
  fChain->SetBranchAddress("tgc_nTgc",&tgc_nTgc);
  fChain->SetBranchAddress("tgc_segIndex",&tgc_segIndex);
  fChain->SetBranchAddress("tgc_id",&tgc_id);
  fChain->SetBranchAddress("tgc_nStrips",&tgc_nStrips);
  fChain->SetBranchAddress("tgc_stripWidth",&tgc_stripWidth);
  fChain->SetBranchAddress("tgc_stripLength",&tgc_stripLength);
  fChain->SetBranchAddress("tgc_error",&tgc_error);
  fChain->SetBranchAddress("tgc_posX",&tgc_posX);
  fChain->SetBranchAddress("tgc_posY",&tgc_posY);
  fChain->SetBranchAddress("tgc_posZ",&tgc_posZ);
  fChain->SetBranchAddress("tgc_gPosX",&tgc_gPosX);
  fChain->SetBranchAddress("tgc_gPosY",&tgc_gPosY);
  fChain->SetBranchAddress("tgc_gPosZ",&tgc_gPosZ);
  fChain->SetBranchAddress("csc_nCsc",&csc_nCsc);
  fChain->SetBranchAddress("csc_segIndex",&csc_segIndex);
  fChain->SetBranchAddress("csc_id",&csc_id);
  fChain->SetBranchAddress("csc_nStrips",&csc_nStrips);
  fChain->SetBranchAddress("csc_stripWidth",&csc_stripWidth);
  fChain->SetBranchAddress("csc_charge",&csc_charge);
  fChain->SetBranchAddress("csc_error",&csc_error);
  fChain->SetBranchAddress("csc_posX",&csc_posX);
  fChain->SetBranchAddress("csc_posY",&csc_posY);
  fChain->SetBranchAddress("csc_posZ",&csc_posZ);
  fChain->SetBranchAddress("csc_gPosX",&csc_gPosX);
  fChain->SetBranchAddress("csc_gPosY",&csc_gPosY);
  fChain->SetBranchAddress("csc_gPosZ",&csc_gPosZ);
  fChain->SetBranchAddress("rawMdt_nRMdt",&rawMdt_nRMdt);
//   fChain->SetBranchAddress("rawMdt_occupancy",rawMdt_occupancy);
  fChain->SetBranchAddress("rawMdt_id",rawMdt_id);
  fChain->SetBranchAddress("rawMdt_tdc",rawMdt_tdc);
  fChain->SetBranchAddress("rawMdt_adc",rawMdt_adc);
//   fChain->SetBranchAddress("rawMdt_t",rawMdt_t);
//   fChain->SetBranchAddress("rawMdt_r",rawMdt_r);
//   fChain->SetBranchAddress("rawMdt_dr",rawMdt_dr);
  if (fChain->GetBranch("rawMdt_posX")) {
    fChain->SetBranchAddress("rawMdt_posX",rawMdt_posX);
    fChain->SetBranchAddress("rawMdt_posY",rawMdt_posY);
    fChain->SetBranchAddress("rawMdt_posZ",rawMdt_posZ);
  } else {
    for (unsigned int i=0; i<3000; i++) {
      rawMdt_posX[i]=9e9;
      rawMdt_posY[i]=9e9;
      rawMdt_posZ[i]=9e9;
    }
  }
  fChain->SetBranchAddress("rawMdt_gPosX",rawMdt_gPosX);
  fChain->SetBranchAddress("rawMdt_gPosY",rawMdt_gPosY);
  fChain->SetBranchAddress("rawMdt_gPosZ",rawMdt_gPosZ);
  fChain->SetBranchAddress("rawRpc_nRRpc",&rawRpc_nRRpc);
  fChain->SetBranchAddress("rawRpc_occupancy",rawRpc_occupancy);
  fChain->SetBranchAddress("rawRpc_id",rawRpc_id);
  fChain->SetBranchAddress("rawRpc_t",rawRpc_t);
  fChain->SetBranchAddress("rawRpc_width",rawRpc_width);
  fChain->SetBranchAddress("rawRpc_length",rawRpc_length);   
  fChain->SetBranchAddress("rawRpc_gPosX",rawRpc_gPosX);
  fChain->SetBranchAddress("rawRpc_gPosY",rawRpc_gPosY);
  fChain->SetBranchAddress("rawRpc_gPosZ",rawRpc_gPosZ);
  if( fChain->GetBranch("rawTgc_nRTgc_All_Bunch") == NULL ) { 
    rawTgc_nRTgc_All_Bunch = 0;
  } else {
    fChain->SetBranchAddress("rawTgc_nRTgc_All_Bunch",&rawTgc_nRTgc_All_Bunch);
    fChain->SetBranchAddress("rawTgc_bcTag",rawTgc_bcTag);
    fChain->SetBranchAddress("rawTgc_num_Prev",rawTgc_num_Prev);
    fChain->SetBranchAddress("rawTgc_num_Curr",rawTgc_num_Curr);
    fChain->SetBranchAddress("rawTgc_num_Next",rawTgc_num_Next);
    fChain->SetBranchAddress("rawTgc_nRTgc",&rawTgc_nRTgc);
    fChain->SetBranchAddress("rawTgc_occupancy",rawTgc_occupancy);
    fChain->SetBranchAddress("rawTgc_id",rawTgc_id);
    fChain->SetBranchAddress("rawTgc_station",rawTgc_station);
    fChain->SetBranchAddress("rawTgc_eta",rawTgc_eta);
    fChain->SetBranchAddress("rawTgc_phi",rawTgc_phi);
    fChain->SetBranchAddress("rawTgc_gasGap",rawTgc_gasGap);   
    fChain->SetBranchAddress("rawTgc_isStrip",rawTgc_isStrip);   
    fChain->SetBranchAddress("rawTgc_channel",rawTgc_channel);   
    fChain->SetBranchAddress("rawTgc_width",rawTgc_width);
    fChain->SetBranchAddress("rawTgc_shortWidth",rawTgc_shortWidth);   
    fChain->SetBranchAddress("rawTgc_longWidth",rawTgc_longWidth);   
    fChain->SetBranchAddress("rawTgc_length",rawTgc_length);
    fChain->SetBranchAddress("rawTgc_gPosX",rawTgc_gPosX);
    fChain->SetBranchAddress("rawTgc_gPosY",rawTgc_gPosY);
    fChain->SetBranchAddress("rawTgc_gPosZ",rawTgc_gPosZ);
    fChain->SetBranchAddress("rawTgc_nRTgc_Prev",&rawTgc_nRTgc_Prev);
    fChain->SetBranchAddress("rawTgc_occupancy_Prev",rawTgc_occupancy_Prev);
    fChain->SetBranchAddress("rawTgc_id_Prev",rawTgc_id_Prev);
    fChain->SetBranchAddress("rawTgc_station_Prev",rawTgc_station_Prev);
    fChain->SetBranchAddress("rawTgc_eta_Prev",rawTgc_eta_Prev);
    fChain->SetBranchAddress("rawTgc_phi_Prev",rawTgc_phi_Prev);
    fChain->SetBranchAddress("rawTgc_gasGap_Prev",rawTgc_gasGap_Prev);   
    fChain->SetBranchAddress("rawTgc_isStrip_Prev",rawTgc_isStrip_Prev);   
    fChain->SetBranchAddress("rawTgc_channel_Prev",rawTgc_channel_Prev);   
    fChain->SetBranchAddress("rawTgc_width_Prev",rawTgc_width_Prev);
    fChain->SetBranchAddress("rawTgc_shortWidth_Prev",rawTgc_shortWidth_Prev);   
    fChain->SetBranchAddress("rawTgc_longWidth_Prev",rawTgc_longWidth_Prev);   
    fChain->SetBranchAddress("rawTgc_length_Prev",rawTgc_length_Prev);
    fChain->SetBranchAddress("rawTgc_gPosX_Prev",rawTgc_gPosX_Prev);
    fChain->SetBranchAddress("rawTgc_gPosY_Prev",rawTgc_gPosY_Prev);
    fChain->SetBranchAddress("rawTgc_gPosZ_Prev",rawTgc_gPosZ_Prev);
    fChain->SetBranchAddress("rawTgc_nRTgc_Next",&rawTgc_nRTgc_Next);
    fChain->SetBranchAddress("rawTgc_occupancy_Next",rawTgc_occupancy_Next);
    fChain->SetBranchAddress("rawTgc_id_Next",rawTgc_id_Next);
    fChain->SetBranchAddress("rawTgc_station_Next",rawTgc_station_Next);
    fChain->SetBranchAddress("rawTgc_eta_Next",rawTgc_eta_Next);
    fChain->SetBranchAddress("rawTgc_phi_Next",rawTgc_phi_Next);
    fChain->SetBranchAddress("rawTgc_gasGap_Next",rawTgc_gasGap_Next);   
    fChain->SetBranchAddress("rawTgc_isStrip_Next",rawTgc_isStrip_Next);   
    fChain->SetBranchAddress("rawTgc_channel_Next",rawTgc_channel_Next);   
    fChain->SetBranchAddress("rawTgc_width_Next",rawTgc_width_Next);
    fChain->SetBranchAddress("rawTgc_shortWidth_Next",rawTgc_shortWidth_Next);   
    fChain->SetBranchAddress("rawTgc_longWidth_Next",rawTgc_longWidth_Next);   
    fChain->SetBranchAddress("rawTgc_length_Next",rawTgc_length_Next);
    fChain->SetBranchAddress("rawTgc_gPosX_Next",rawTgc_gPosX_Next);
    fChain->SetBranchAddress("rawTgc_gPosY_Next",rawTgc_gPosY_Next);
    fChain->SetBranchAddress("rawTgc_gPosZ_Next",rawTgc_gPosZ_Next);
  }
  if( fChain->GetBranch("rawTgcCoin_nRTgc_All") == NULL ) { 
    rawTgcCoin_nRTgc_All = 0;
  } else {
    fChain->SetBranchAddress("rawTgcCoin_nRTgc_All",&rawTgcCoin_nRTgc_All);
    fChain->SetBranchAddress("rawTgcCoin_type_bcTag",&rawTgcCoin_type_bcTag);
    fChain->SetBranchAddress("rawTgcCoin_bcTag_tracklet",rawTgcCoin_bcTag_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_bcTag_highpt",rawTgcCoin_bcTag_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_bcTag_sl",rawTgcCoin_bcTag_sl);
    fChain->SetBranchAddress("rawTgcCoin_type",rawTgcCoin_type);
    fChain->SetBranchAddress("rawTgcCoin_type_Prev",rawTgcCoin_type_Prev);
    fChain->SetBranchAddress("rawTgcCoin_type_Next",rawTgcCoin_type_Next);
    fChain->SetBranchAddress("rawTgcCoin_num_tracklet_Prev",rawTgcCoin_num_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_num_tracklet_Curr",rawTgcCoin_num_tracklet_Curr);   
    fChain->SetBranchAddress("rawTgcCoin_num_tracklet_Next",rawTgcCoin_num_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_num_highpt_Prev",rawTgcCoin_num_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_num_highpt_Curr",rawTgcCoin_num_highpt_Curr);   
    fChain->SetBranchAddress("rawTgcCoin_num_highpt_Next",rawTgcCoin_num_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_num_sl_Prev",rawTgcCoin_num_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_num_sl_Curr",rawTgcCoin_num_sl_Curr);   
    fChain->SetBranchAddress("rawTgcCoin_num_sl_Next",rawTgcCoin_num_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_nRTgc",&rawTgcCoin_nRTgc);
    fChain->SetBranchAddress("rawTgcCoin_nRTgc_Prev",&rawTgcCoin_nRTgc_Prev);
    fChain->SetBranchAddress("rawTgcCoin_nRTgc_Next",&rawTgcCoin_nRTgc_Next);
    fChain->SetBranchAddress("rawTgcCoin_nRTgc_tracklet",&rawTgcCoin_nRTgc_tracklet);
    fChain->SetBranchAddress("rawTgcCoin_nRTgc_highpt",&rawTgcCoin_nRTgc_highpt);
    fChain->SetBranchAddress("rawTgcCoin_nRTgc_sl",&rawTgcCoin_nRTgc_sl);
    fChain->SetBranchAddress("rawTgcCoin_nRTgcTracklet",&rawTgcCoin_nRTgcTracklet);
    fChain->SetBranchAddress("rawTgcCoin_nRTgcTracklet_Prev",&rawTgcCoin_nRTgcTracklet_Prev);
    fChain->SetBranchAddress("rawTgcCoin_nRTgcTracklet_Next",&rawTgcCoin_nRTgcTracklet_Next);
    fChain->SetBranchAddress("rawTgcCoin_nRTgcHighPt",&rawTgcCoin_nRTgcHighPt);
    fChain->SetBranchAddress("rawTgcCoin_nRTgcHighPt_Prev",&rawTgcCoin_nRTgcHighPt_Prev);
    fChain->SetBranchAddress("rawTgcCoin_nRTgcHighPt_Next",&rawTgcCoin_nRTgcHighPt_Next);
    fChain->SetBranchAddress("rawTgcCoin_nRTgcSl",&rawTgcCoin_nRTgcSl);
    fChain->SetBranchAddress("rawTgcCoin_nRTgcSl_Prev",&rawTgcCoin_nRTgcSl_Prev);
    fChain->SetBranchAddress("rawTgcCoin_nRTgcSl_Next",&rawTgcCoin_nRTgcSl_Next);
    fChain->SetBranchAddress("rawTgcCoin_id_tracklet",rawTgcCoin_id_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_eta_tracklet",rawTgcCoin_eta_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_phi_tracklet",rawTgcCoin_phi_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_sector_tracklet",rawTgcCoin_sector_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_isForward_tracklet",rawTgcCoin_isForward_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_isStrip_tracklet",rawTgcCoin_isStrip_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_trackletId_tracklet",rawTgcCoin_trackletId_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_delta_tracklet",rawTgcCoin_delta_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_sub_tracklet",rawTgcCoin_sub_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_widthIn_tracklet",rawTgcCoin_widthIn_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_widthOut_tracklet",rawTgcCoin_widthOut_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInX_tracklet",rawTgcCoin_gPosInX_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInY_tracklet",rawTgcCoin_gPosInY_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInZ_tracklet",rawTgcCoin_gPosInZ_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutX_tracklet",rawTgcCoin_gPosOutX_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutY_tracklet",rawTgcCoin_gPosOutY_tracklet);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutZ_tracklet",rawTgcCoin_gPosOutZ_tracklet);
    fChain->SetBranchAddress("rawTgcCoin_id_highpt",rawTgcCoin_id_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_eta_highpt",rawTgcCoin_eta_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_phi_highpt",rawTgcCoin_phi_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_sector_highpt",rawTgcCoin_sector_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_isForward_highpt",rawTgcCoin_isForward_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_isStrip_highpt",rawTgcCoin_isStrip_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_trackletId_highpt",rawTgcCoin_trackletId_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_delta_highpt",rawTgcCoin_delta_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_sub_highpt",rawTgcCoin_sub_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_widthIn_highpt",rawTgcCoin_widthIn_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_widthOut_highpt",rawTgcCoin_widthOut_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInX_highpt",rawTgcCoin_gPosInX_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInY_highpt",rawTgcCoin_gPosInY_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInZ_highpt",rawTgcCoin_gPosInZ_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutX_highpt",rawTgcCoin_gPosOutX_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutY_highpt",rawTgcCoin_gPosOutY_highpt);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutZ_highpt",rawTgcCoin_gPosOutZ_highpt);
    fChain->SetBranchAddress("rawTgcCoin_id_sl",rawTgcCoin_id_sl);   
    fChain->SetBranchAddress("rawTgcCoin_eta_sl",rawTgcCoin_eta_sl);   
    fChain->SetBranchAddress("rawTgcCoin_phi_sl",rawTgcCoin_phi_sl);   
    fChain->SetBranchAddress("rawTgcCoin_sector_sl",rawTgcCoin_sector_sl);   
    fChain->SetBranchAddress("rawTgcCoin_isForward_sl",rawTgcCoin_isForward_sl);   
    fChain->SetBranchAddress("rawTgcCoin_trackletId_sl",rawTgcCoin_trackletId_sl);   
    fChain->SetBranchAddress("rawTgcCoin_trackletIdStrip_sl",rawTgcCoin_trackletIdStrip_sl);   
    fChain->SetBranchAddress("rawTgcCoin_roi_sl",rawTgcCoin_roi_sl);   
    fChain->SetBranchAddress("rawTgcCoin_pt_sl",rawTgcCoin_pt_sl);   
    fChain->SetBranchAddress("rawTgcCoin_isPositiveDeltaR_sl",rawTgcCoin_isPositiveDeltaR_sl);   
    fChain->SetBranchAddress("rawTgcCoin_widthR_sl",rawTgcCoin_widthR_sl);   
    fChain->SetBranchAddress("rawTgcCoin_widthPhi_sl",rawTgcCoin_widthPhi_sl);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutX_sl",rawTgcCoin_gPosOutX_sl);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutY_sl",rawTgcCoin_gPosOutY_sl);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutZ_sl",rawTgcCoin_gPosOutZ_sl);
    fChain->SetBranchAddress("rawTgcCoin_id_tracklet_Prev",rawTgcCoin_id_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_eta_tracklet_Prev",rawTgcCoin_eta_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_phi_tracklet_Prev",rawTgcCoin_phi_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_sector_tracklet_Prev",rawTgcCoin_sector_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_isForward_tracklet_Prev",rawTgcCoin_isForward_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_isStrip_tracklet_Prev",rawTgcCoin_isStrip_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_trackletId_tracklet_Prev",rawTgcCoin_trackletId_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_delta_tracklet_Prev",rawTgcCoin_delta_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_sub_tracklet_Prev",rawTgcCoin_sub_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_widthIn_tracklet_Prev",rawTgcCoin_widthIn_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_widthOut_tracklet_Prev",rawTgcCoin_widthOut_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInX_tracklet_Prev",rawTgcCoin_gPosInX_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInY_tracklet_Prev",rawTgcCoin_gPosInY_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInZ_tracklet_Prev",rawTgcCoin_gPosInZ_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutX_tracklet_Prev",rawTgcCoin_gPosOutX_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutY_tracklet_Prev",rawTgcCoin_gPosOutY_tracklet_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutZ_tracklet_Prev",rawTgcCoin_gPosOutZ_tracklet_Prev);
    fChain->SetBranchAddress("rawTgcCoin_id_highpt_Prev",rawTgcCoin_id_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_eta_highpt_Prev",rawTgcCoin_eta_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_phi_highpt_Prev",rawTgcCoin_phi_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_sector_highpt_Prev",rawTgcCoin_sector_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_isForward_highpt_Prev",rawTgcCoin_isForward_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_isStrip_highpt_Prev",rawTgcCoin_isStrip_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_trackletId_highpt_Prev",rawTgcCoin_trackletId_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_delta_highpt_Prev",rawTgcCoin_delta_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_sub_highpt_Prev",rawTgcCoin_sub_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_widthIn_highpt_Prev",rawTgcCoin_widthIn_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_widthOut_highpt_Prev",rawTgcCoin_widthOut_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInX_highpt_Prev",rawTgcCoin_gPosInX_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInY_highpt_Prev",rawTgcCoin_gPosInY_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInZ_highpt_Prev",rawTgcCoin_gPosInZ_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutX_highpt_Prev",rawTgcCoin_gPosOutX_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutY_highpt_Prev",rawTgcCoin_gPosOutY_highpt_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutZ_highpt_Prev",rawTgcCoin_gPosOutZ_highpt_Prev);
    fChain->SetBranchAddress("rawTgcCoin_id_sl_Prev",rawTgcCoin_id_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_eta_sl_Prev",rawTgcCoin_eta_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_phi_sl_Prev",rawTgcCoin_phi_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_sector_sl_Prev",rawTgcCoin_sector_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_isForward_sl_Prev",rawTgcCoin_isForward_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_trackletId_sl_Prev",rawTgcCoin_trackletId_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_trackletIdStrip_sl_Prev",rawTgcCoin_trackletIdStrip_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_roi_sl_Prev",rawTgcCoin_roi_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_pt_sl_Prev",rawTgcCoin_pt_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_isPositiveDeltaR_sl_Prev",rawTgcCoin_isPositiveDeltaR_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_widthR_sl_Prev",rawTgcCoin_widthR_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_widthPhi_sl_Prev",rawTgcCoin_widthPhi_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutX_sl_Prev",rawTgcCoin_gPosOutX_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutY_sl_Prev",rawTgcCoin_gPosOutY_sl_Prev);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutZ_sl_Prev",rawTgcCoin_gPosOutZ_sl_Prev);
    fChain->SetBranchAddress("rawTgcCoin_id_tracklet_Next",rawTgcCoin_id_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_eta_tracklet_Next",rawTgcCoin_eta_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_phi_tracklet_Next",rawTgcCoin_phi_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_sector_tracklet_Next",rawTgcCoin_sector_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_isForward_tracklet_Next",rawTgcCoin_isForward_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_isStrip_tracklet_Next",rawTgcCoin_isStrip_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_trackletId_tracklet_Next",rawTgcCoin_trackletId_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_delta_tracklet_Next",rawTgcCoin_delta_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_sub_tracklet_Next",rawTgcCoin_sub_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_widthIn_tracklet_Next",rawTgcCoin_widthIn_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_widthOut_tracklet_Next",rawTgcCoin_widthOut_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInX_tracklet_Next",rawTgcCoin_gPosInX_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInY_tracklet_Next",rawTgcCoin_gPosInY_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInZ_tracklet_Next",rawTgcCoin_gPosInZ_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutX_tracklet_Next",rawTgcCoin_gPosOutX_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutY_tracklet_Next",rawTgcCoin_gPosOutY_tracklet_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutZ_tracklet_Next",rawTgcCoin_gPosOutZ_tracklet_Next);
    fChain->SetBranchAddress("rawTgcCoin_id_highpt_Next",rawTgcCoin_id_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_eta_highpt_Next",rawTgcCoin_eta_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_phi_highpt_Next",rawTgcCoin_phi_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_sector_highpt_Next",rawTgcCoin_sector_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_isForward_highpt_Next",rawTgcCoin_isForward_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_isStrip_highpt_Next",rawTgcCoin_isStrip_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_trackletId_highpt_Next",rawTgcCoin_trackletId_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_delta_highpt_Next",rawTgcCoin_delta_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_sub_highpt_Next",rawTgcCoin_sub_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_widthIn_highpt_Next",rawTgcCoin_widthIn_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_widthOut_highpt_Next",rawTgcCoin_widthOut_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInX_highpt_Next",rawTgcCoin_gPosInX_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInY_highpt_Next",rawTgcCoin_gPosInY_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosInZ_highpt_Next",rawTgcCoin_gPosInZ_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutX_highpt_Next",rawTgcCoin_gPosOutX_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutY_highpt_Next",rawTgcCoin_gPosOutY_highpt_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutZ_highpt_Next",rawTgcCoin_gPosOutZ_highpt_Next);
    fChain->SetBranchAddress("rawTgcCoin_id_sl_Next",rawTgcCoin_id_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_eta_sl_Next",rawTgcCoin_eta_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_phi_sl_Next",rawTgcCoin_phi_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_sector_sl_Next",rawTgcCoin_sector_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_isForward_sl_Next",rawTgcCoin_isForward_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_trackletId_sl_Next",rawTgcCoin_trackletId_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_trackletIdStrip_sl_Next",rawTgcCoin_trackletIdStrip_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_roi_sl_Next",rawTgcCoin_roi_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_pt_sl_Next",rawTgcCoin_pt_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_isPositiveDeltaR_sl_Next",rawTgcCoin_isPositiveDeltaR_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_widthR_sl_Next",rawTgcCoin_widthR_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_widthPhi_sl_Next",rawTgcCoin_widthPhi_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutX_sl_Next",rawTgcCoin_gPosOutX_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutY_sl_Next",rawTgcCoin_gPosOutY_sl_Next);   
    fChain->SetBranchAddress("rawTgcCoin_gPosOutZ_sl_Next",rawTgcCoin_gPosOutZ_sl_Next);
  }
  fChain->SetBranchAddress("rawCsc_nRCsc",&rawCsc_nRCsc);
  fChain->SetBranchAddress("rawCsc_occupancy",rawCsc_occupancy);
  fChain->SetBranchAddress("rawCsc_id",rawCsc_id);
  fChain->SetBranchAddress("rawCsc_width",rawCsc_width);
  fChain->SetBranchAddress("rawCsc_charge",rawCsc_charge);
  fChain->SetBranchAddress("rawCsc_t",rawCsc_t);
  fChain->SetBranchAddress("rawCsc_gPosX",rawCsc_gPosX);
  fChain->SetBranchAddress("rawCsc_gPosY",rawCsc_gPosY);
  fChain->SetBranchAddress("rawCsc_gPosZ",rawCsc_gPosZ);
}
  
void NTReaderBase::Show(Long64_t entry) {
  // Print contents of entry.
// If entry is not specified, print current entry
  if (!fChain) return;
  fChain->Show(entry);
}

Int_t NTReaderBase::Cut(Long64_t /*entry*/) {
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
  return 1;
}

} //namespace MuonCalib
