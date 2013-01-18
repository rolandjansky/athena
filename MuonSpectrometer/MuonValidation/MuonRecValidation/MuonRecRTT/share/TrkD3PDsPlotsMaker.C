#include <vector>
#include "TH1D.h"
#include "TMath.h"

using namespace std;

TrkD3PDsPlotsMaker(){

  TFile input("MuonTrackD3PD.root");
  
  TTree* tree=(TTree*) input.Get("MuonTrackTree");
  if(tree==0){
    cout<<"ERROR: can't find TTree: MuonTrackTree inside MuonTrackD3PD.root"<<endl;
    return;
  }
  
  // Declare ntuple variables we're interested in
  int mu_moore_n;
  vector<float>* mu_moore_pt = new vector<float>;
  vector<float>* mu_moore_eta = new vector<float>;
  vector<float>* mu_moore_phi_wrtBL = new vector<float>;
  vector<float>* mu_moore_chi2 = new vector<float>;
  vector<int>*	mu_moore_ndof = new vector<int>;
  vector<int>*	mu_moore_nBLHits = new vector<int>;
  vector<int>*	mu_moore_nPixHits = new vector<int>;
  vector<int>*	mu_moore_nSCTHits = new vector<int>;
  vector<int>*	mu_moore_nTRTHits = new vector<int>;
  vector<int>*	mu_moore_nTRTHighTHits = new vector<int>;
  vector<int>*	mu_moore_nPixHoles = new vector<int>;
  vector<int>*	mu_moore_nSCTHoles = new vector<int>;
  vector<int>*	mu_moore_nTRTHoles = new vector<int>;
  vector<int>*	mu_moore_nMDTHits = new vector<int>;
  vector<int>*	mu_moore_nCSCEtaHits = new vector<int>;
  vector<int>*	mu_moore_nCSCPhiHits = new vector<int>;
  vector<int>*	mu_moore_nRPCEtaHits = new vector<int>;
  vector<int>*	mu_moore_nRPCPhiHits = new vector<int>;
  vector<int>*	mu_moore_nTGCEtaHits = new vector<int>;
  vector<int>*	mu_moore_nTGCPhiHits = new vector<int>;
  vector<int>*	mu_moore_nHits = new vector<int>;
  vector<int>*	mu_moore_nHoles = new vector<int>;  
  
  int mu_n;
  vector<float>* mu_pt = new vector<float>;
  vector<float>* mu_eta = new vector<float>;
  vector<float>* mu_phi_wrtBL = new vector<float>;
  vector<float>* mu_chi2 = new vector<float>;
  vector<int>*	mu_ndof = new vector<int>;
  vector<int>*	mu_nBLHits = new vector<int>;
  vector<int>*	mu_nPixHits = new vector<int>;
  vector<int>*	mu_nSCTHits = new vector<int>;
  vector<int>*	mu_nTRTHits = new vector<int>;
  vector<int>*	mu_nTRTHighTHits = new vector<int>;
  vector<int>*	mu_nPixHoles = new vector<int>;
  vector<int>*	mu_nSCTHoles = new vector<int>;
  vector<int>*	mu_nTRTHoles = new vector<int>;
  vector<int>*	mu_nMDTHits = new vector<int>;
  vector<int>*	mu_nCSCEtaHits = new vector<int>;
  vector<int>*	mu_nCSCPhiHits = new vector<int>;
  vector<int>*	mu_nRPCEtaHits = new vector<int>;
  vector<int>*	mu_nRPCPhiHits = new vector<int>;
  vector<int>*	mu_nTGCEtaHits = new vector<int>;
  vector<int>*	mu_nTGCPhiHits = new vector<int>;
  vector<int>*	mu_nHits = new vector<int>;
  vector<int>*	mu_nHoles = new vector<int>; 
   
  int mu_mboy_n;
  vector<float>* mu_mboy_pt = new vector<float>;
  vector<float>* mu_mboy_eta = new vector<float>;
  vector<float>* mu_mboy_phi_wrtBL = new vector<float>;
  vector<float>* mu_mboy_chi2 = new vector<float>;
  vector<int>*	mu_mboy_ndof = new vector<int>;
  vector<int>*	mu_mboy_nBLHits = new vector<int>;
  vector<int>*	mu_mboy_nPixHits = new vector<int>;
  vector<int>*	mu_mboy_nSCTHits = new vector<int>;
  vector<int>*	mu_mboy_nTRTHits = new vector<int>;
  vector<int>*	mu_mboy_nTRTHighTHits = new vector<int>;
  vector<int>*	mu_mboy_nPixHoles = new vector<int>;
  vector<int>*	mu_mboy_nSCTHoles = new vector<int>;
  vector<int>*	mu_mboy_nTRTHoles = new vector<int>;
  vector<int>*	mu_mboy_nMDTHits = new vector<int>;
  vector<int>*	mu_mboy_nCSCEtaHits = new vector<int>;
  vector<int>*	mu_mboy_nCSCPhiHits = new vector<int>;
  vector<int>*	mu_mboy_nRPCEtaHits = new vector<int>;
  vector<int>*	mu_mboy_nRPCPhiHits = new vector<int>;
  vector<int>*	mu_mboy_nTGCEtaHits = new vector<int>;
  vector<int>*	mu_mboy_nTGCPhiHits = new vector<int>;
  vector<int>*	mu_mboy_nHits = new vector<int>;
  vector<int>*	mu_mboy_nHoles = new vector<int>;
   
  int mc_n;
  vector<float>* mc_gen_pt = new vector<float>;
  vector<float>* mc_gen_eta = new vector<float>;
  vector<float>* mc_gen_phi = new vector<float>;
  vector<int>* mc_gen_type = new vector<int>;

  int ms_entry_truth_n;
  vector<int>* ms_entry_truth_pdgId = new vector<int>;
  vector<float>* ms_entry_truth_eta = new vector<float>;
  vector<float>* ms_entry_truth_phi = new vector<float>;
  vector<float>* ms_entry_truth_pt = new vector<float>;
  
  tree->SetBranchAddress("mu_moore_n",&mu_moore_n);
  tree->SetBranchAddress("mu_moore_pt",&mu_moore_pt);
  tree->SetBranchAddress("mu_moore_eta",&mu_moore_eta);
  tree->SetBranchAddress("mu_moore_phi_wrtBL",&mu_moore_phi_wrtBL);
  tree->SetBranchAddress("mu_moore_chi2",&mu_moore_chi2);
  tree->SetBranchAddress("mu_moore_ndof",&mu_moore_ndof);
  tree->SetBranchAddress("mu_moore_nBLHits",&mu_moore_nBLHits);
  tree->SetBranchAddress("mu_moore_nPixHits",&mu_moore_nPixHits);
  tree->SetBranchAddress("mu_moore_nSCTHits",&mu_moore_nSCTHits);
  tree->SetBranchAddress("mu_moore_nTRTHits",&mu_moore_nTRTHits);
  tree->SetBranchAddress("mu_moore_nTRTHighTHits",&mu_moore_nTRTHighTHits);
  tree->SetBranchAddress("mu_moore_nPixHoles",&mu_moore_nPixHoles);
  tree->SetBranchAddress("mu_moore_nSCTHoles",&mu_moore_nSCTHoles);
  tree->SetBranchAddress("mu_moore_nTRTHoles",&mu_moore_nTRTHoles);
  tree->SetBranchAddress("mu_moore_nMDTHits",&mu_moore_nMDTHits);
  tree->SetBranchAddress("mu_moore_nCSCEtaHits",&mu_moore_nCSCEtaHits);
  tree->SetBranchAddress("mu_moore_nCSCPhiHits",&mu_moore_nCSCPhiHits);
  tree->SetBranchAddress("mu_moore_nRPCEtaHits",&mu_moore_nRPCEtaHits);
  tree->SetBranchAddress("mu_moore_nRPCPhiHits",&mu_moore_nRPCPhiHits);
  tree->SetBranchAddress("mu_moore_nTGCEtaHits",&mu_moore_nTGCEtaHits);
  tree->SetBranchAddress("mu_moore_nTGCPhiHits",&mu_moore_nTGCPhiHits);
  tree->SetBranchAddress("mu_moore_nHits",&mu_moore_nHits);
  tree->SetBranchAddress("mu_moore_nHoles",&mu_moore_nHoles);
  
  tree->SetBranchAddress("mu_n",&mu_n);
  tree->SetBranchAddress("mu_pt",&mu_pt);
  tree->SetBranchAddress("mu_eta",&mu_eta);
  tree->SetBranchAddress("mu_phi_wrtBL",&mu_phi_wrtBL);
  tree->SetBranchAddress("mu_chi2",&mu_chi2);
  tree->SetBranchAddress("mu_ndof",&mu_ndof);
  tree->SetBranchAddress("mu_nBLHits",&mu_nBLHits);
  tree->SetBranchAddress("mu_nPixHits",&mu_nPixHits);
  tree->SetBranchAddress("mu_nSCTHits",&mu_nSCTHits);
  tree->SetBranchAddress("mu_nTRTHits",&mu_nTRTHits);
  tree->SetBranchAddress("mu_nTRTHighTHits",&mu_nTRTHighTHits);
  tree->SetBranchAddress("mu_nPixHoles",&mu_nPixHoles);
  tree->SetBranchAddress("mu_nSCTHoles",&mu_nSCTHoles);
  tree->SetBranchAddress("mu_nTRTHoles",&mu_nTRTHoles);
  tree->SetBranchAddress("mu_nMDTHits",&mu_nMDTHits);
  tree->SetBranchAddress("mu_nCSCEtaHits",&mu_nCSCEtaHits);
  tree->SetBranchAddress("mu_nCSCPhiHits",&mu_nCSCPhiHits);
  tree->SetBranchAddress("mu_nRPCEtaHits",&mu_nRPCEtaHits);
  tree->SetBranchAddress("mu_nRPCPhiHits",&mu_nRPCPhiHits);
  tree->SetBranchAddress("mu_nTGCEtaHits",&mu_nTGCEtaHits);
  tree->SetBranchAddress("mu_nTGCPhiHits",&mu_nTGCPhiHits);
  tree->SetBranchAddress("mu_nHits",&mu_nHits);
  tree->SetBranchAddress("mu_nHoles",&mu_nHoles);
   
  tree->SetBranchAddress("mu_mboy_n",&mu_mboy_n);
  tree->SetBranchAddress("mu_mboy_pt",&mu_mboy_pt);
  tree->SetBranchAddress("mu_mboy_eta",&mu_mboy_eta);
  tree->SetBranchAddress("mu_mboy_phi_wrtBL",&mu_mboy_phi_wrtBL);
  tree->SetBranchAddress("mu_mboy_chi2",&mu_mboy_chi2);
  tree->SetBranchAddress("mu_mboy_ndof",&mu_mboy_ndof);
  tree->SetBranchAddress("mu_mboy_nBLHits",&mu_mboy_nBLHits);
  tree->SetBranchAddress("mu_mboy_nPixHits",&mu_mboy_nPixHits);
  tree->SetBranchAddress("mu_mboy_nSCTHits",&mu_mboy_nSCTHits);
  tree->SetBranchAddress("mu_mboy_nTRTHits",&mu_mboy_nTRTHits);
  tree->SetBranchAddress("mu_mboy_nTRTHighTHits",&mu_mboy_nTRTHighTHits);
  tree->SetBranchAddress("mu_mboy_nPixHoles",&mu_mboy_nPixHoles);
  tree->SetBranchAddress("mu_mboy_nSCTHoles",&mu_mboy_nSCTHoles);
  tree->SetBranchAddress("mu_mboy_nTRTHoles",&mu_mboy_nTRTHoles);
  tree->SetBranchAddress("mu_mboy_nMDTHits",&mu_mboy_nMDTHits);
  tree->SetBranchAddress("mu_mboy_nCSCEtaHits",&mu_mboy_nCSCEtaHits);
  tree->SetBranchAddress("mu_mboy_nCSCPhiHits",&mu_mboy_nCSCPhiHits);
  tree->SetBranchAddress("mu_mboy_nRPCEtaHits",&mu_mboy_nRPCEtaHits);
  tree->SetBranchAddress("mu_mboy_nRPCPhiHits",&mu_mboy_nRPCPhiHits);
  tree->SetBranchAddress("mu_mboy_nTGCEtaHits",&mu_mboy_nTGCEtaHits);
  tree->SetBranchAddress("mu_mboy_nTGCPhiHits",&mu_mboy_nTGCPhiHits);
  tree->SetBranchAddress("mu_mboy_nHits",&mu_mboy_nHits);
  tree->SetBranchAddress("mu_mboy_nHoles",&mu_mboy_nHoles);
   
  tree->SetBranchAddress("mc_n",&mc_n);
  tree->SetBranchAddress("mc_gen_pt",&mc_gen_pt);
  tree->SetBranchAddress("mc_gen_eta",&mc_gen_eta);
  tree->SetBranchAddress("mc_gen_phi",&mc_gen_phi);
  tree->SetBranchAddress("mc_gen_type",&mc_gen_type);

  tree->SetBranchAddress("ms_entry_truth_n",&ms_entry_truth_n);
  tree->SetBranchAddress("ms_entry_truth_pt",&ms_entry_truth_pt);
  tree->SetBranchAddress("ms_entry_truth_eta",&ms_entry_truth_eta);
  tree->SetBranchAddress("ms_entry_truth_phi",&ms_entry_truth_phi);
  tree->SetBranchAddress("ms_entry_truth_pdgId",&ms_entry_truth_pdgId);
  
  TH1D* h_mu_moore_n = new TH1D("mu_moore_n","mu_moore_n;# muons",10,-0.5,9.5);
  TH1D* h_mu_moore_pt = new TH1D("h_mu_moore_pt","h_mu_moore_pt;p_{T}",50,0,100*1000);
  TH1D* h_mu_moore_pt_high = new TH1D("h_mu_moore_pt_high","h_mu_moore_pt_high;p_{T}",50,0,1000*1000);
  TH1D* h_mu_moore_eta = new TH1D("h_mu_moore_eta","h_mu_moore_eta;\eta",60,-3,3);
  TH1D* h_mu_moore_phi = new TH1D("h_mu_moore_phi","h_mu_moore_phi;\phi",32,-3.1416,3.1416);
  TH1D* h_mu_moore_chi2_over_ndof = new TH1D("h_mu_moore_chi2_over_ndof","h_mu_moore_chi2_over_ndof;\chi^{2}/ndf",100,0,10);
  TH1D* h_mu_moore_nBLHits = new TH1D("h_mu_moore_nBLHits","h_mu_moore_nBLHits;# BL Hits",10,0,10);
  TH1D* h_mu_moore_nPixHits = new TH1D("h_mu_moore_nPixHits","h_mu_moore_nPixHits;# Pixel Hits",10,0,10);
  TH1D* h_mu_moore_nSCTHits = new TH1D("h_mu_moore_nSCTHits","h_mu_moore_nSCTHits;# SCT Hits",10,0,10);
  TH1D* h_mu_moore_nTRTHits = new TH1D("h_mu_moore_nTRTHits","h_mu_moore_nTRTHits;# TRT Hits",10,0,10);
  TH1D* h_mu_moore_nTRTHighTHits = new TH1D("h_mu_moore_nTRTHighTHits","h_mu_moore_nTRTHighTHits;# TRT High Th Hits",10,0,10);
  TH1D* h_mu_moore_nPixHoles = new TH1D("h_mu_moore_nPixHoles","h_mu_moore_nPixHoles;# Pixel Holes",50,0,50);
  TH1D* h_mu_moore_nSCTHoles = new TH1D("h_mu_moore_nSCTHoles","h_mu_moore_nSCTHoles;# SCT Holes",50,0,50);
  TH1D* h_mu_moore_nTRTHoles = new TH1D("h_mu_moore_nTRTHoles","h_mu_moore_nTRTHoles;# TRT Holes",50,0,50);
  TH1D* h_mu_moore_nMDTHits = new TH1D("h_mu_moore_nMDTHits","h_mu_moore_nMDTHits;# MDT Hits",60,0,60);
  TH1D* h_mu_moore_nCSCEtaHits = new TH1D("h_mu_moore_nCSCEtaHits","h_mu_moore_nCSCEtaHits;# CSC Eta Hits",10,0,10);
  TH1D* h_mu_moore_nCSCPhiHits = new TH1D("h_mu_moore_nCSCPhiHits","h_mu_moore_nCSCPhiHits;# CSC Phi Hits",10,0,10);
  TH1D* h_mu_moore_nRPCEtaHits = new TH1D("h_mu_moore_nRPCEtaHits","h_mu_moore_nRPCEtaHits;# RPC Eta Hits",20,0,20);
  TH1D* h_mu_moore_nRPCPhiHits = new TH1D("h_mu_moore_nRPCPhiHits","h_mu_moore_nRPCPhiHits;# RPC Phi Hits",20,0,20);
  TH1D* h_mu_moore_nTGCEtaHits = new TH1D("h_mu_moore_nTGCEtaHits","h_mu_moore_nTGCEtaHits;# TGC Eta Hits",20,0,20);
  TH1D* h_mu_moore_nTGCPhiHits = new TH1D("h_mu_moore_nTGCPhiHits","h_mu_moore_nTGCPhiHits;# TGC Phi Hits",20,0,20);
  TH1D* h_mu_moore_nHits = new TH1D("h_mu_moore_nHits","h_mu_moore_nHits;# Hits",100,0,100);
  TH1D* h_mu_moore_nHoles = new TH1D("h_mu_moore_nHoles","h_mu_moore_nHoles;# Holes",10,0,10);  
  
  TH1D* h_mu_n = new TH1D("mu_n","mu_n;# muons",10,-0.5,9.5);
  TH1D* h_mu_pt = new TH1D("h_mu_pt","h_mu_pt;p_{T}",50,0,100*1000);
  TH1D* h_mu_pt_high = new TH1D("h_mu_pt_high","h_mu_pt_high;p_{T}",50,0,1000*1000);
  TH1D* h_mu_eta = new TH1D("h_mu_eta","h_mu_eta;\eta",60,-3,3);
  TH1D* h_mu_phi = new TH1D("h_mu_phi","h_mu_phi;\phi",32,-3.1416,3.1416);
  TH1D* h_mu_chi2_over_ndof = new TH1D("h_mu_chi2_over_ndof;\chi^{2}/ndf","h_mu_chi2_over_ndof",100,0,10);
  TH1D* h_mu_nBLHits = new TH1D("h_mu_nBLHits","h_mu_nBLHits;# BL Hits",10,0,10);
  TH1D* h_mu_nPixHits = new TH1D("h_mu_nPixHits","h_mu_nPixHits;# Pixel Hits",10,0,10);
  TH1D* h_mu_nSCTHits = new TH1D("h_mu_nSCTHits","h_mu_nSCTHits;# SCT Hits",10,0,10);
  TH1D* h_mu_nTRTHits = new TH1D("h_mu_nTRTHits","h_mu_nTRTHits;# TRT Hits",10,0,10);
  TH1D* h_mu_nTRTHighTHits = new TH1D("h_mu_nTRTHighTHits","h_mu_nTRTHighTHits;# TRT High Th Hits",10,0,10);
  TH1D* h_mu_nPixHoles = new TH1D("h_mu_nPixHoles","h_mu_nPixHoles;# Pixel Holes",50,0,50);
  TH1D* h_mu_nSCTHoles = new TH1D("h_mu_nSCTHoles","h_mu_nSCTHoles;# SCT Holes",50,0,50);
  TH1D* h_mu_nTRTHoles = new TH1D("h_mu_nTRTHoles","h_mu_nTRTHoles;# TRT Holes",50,0,50);
  TH1D* h_mu_nMDTHits = new TH1D("h_mu_nMDTHits","h_mu_nMDTHits;# MDT Hits",60,0,60);
  TH1D* h_mu_nCSCEtaHits = new TH1D("h_mu_nCSCEtaHits","h_mu_nCSCEtaHits;# CSC Eta Hits",10,0,10);
  TH1D* h_mu_nCSCPhiHits = new TH1D("h_mu_nCSCPhiHits","h_mu_nCSCPhiHits;# CSC Phi Hits",10,0,10);
  TH1D* h_mu_nRPCEtaHits = new TH1D("h_mu_nRPCEtaHits","h_mu_nRPCEtaHits;# RPC Eta Hits",20,0,20);
  TH1D* h_mu_nRPCPhiHits = new TH1D("h_mu_nRPCPhiHits","h_mu_nRPCPhiHits;# RPC Phi Hits",20,0,20);
  TH1D* h_mu_nTGCEtaHits = new TH1D("h_mu_nTGCEtaHits","h_mu_nTGCEtaHits;# TGC Eta Hits",20,0,20);
  TH1D* h_mu_nTGCPhiHits = new TH1D("h_mu_nTGCPhiHits","h_mu_nTGCPhiHits;# TGC Phi Hits",20,0,20);
  TH1D* h_mu_nHits = new TH1D("h_mu_nHits","h_mu_nHits;# Hits",100,0,100);
  TH1D* h_mu_nHoles = new TH1D("h_mu_nHoles","h_mu_nHoles;# Holes",10,0,10);  
   
  TH1D* h_mu_mboy_n = new TH1D("mu_mboy_n","mu_mboy_n;# muons",10,-0.5,9.5);
  TH1D* h_mu_mboy_pt = new TH1D("h_mu_mboy_pt","h_mu_mboy_pt;p_{T}",50,0,100*1000);
  TH1D* h_mu_mboy_pt_high = new TH1D("h_mu_mboy_pt_high","h_mu_mboy_pt_high;p_{T}",50,0,1000*1000);
  TH1D* h_mu_mboy_eta = new TH1D("h_mu_mboy_eta","h_mu_mboy_eta;\eta",60,-3,3);
  TH1D* h_mu_mboy_phi = new TH1D("h_mu_mboy_phi","h_mu_mboy_phi;\phi",32,-3.1416,3.1416);
  TH1D* h_mu_mboy_chi2_over_ndof = new TH1D("h_mu_mboy_chi2_over_ndof;\chi^{2}/ndf","h_mu_mboy_chi2_over_ndof",100,0,10);
  TH1D* h_mu_mboy_nBLHits = new TH1D("h_mu_mboy_nBLHits","h_mu_mboy_nBLHits;# BL Hits",10,0,10);
  TH1D* h_mu_mboy_nPixHits = new TH1D("h_mu_mboy_nPixHits","h_mu_mboy_nPixHits;# Pixel Hits",10,0,10);
  TH1D* h_mu_mboy_nSCTHits = new TH1D("h_mu_mboy_nSCTHits","h_mu_mboy_nSCTHits;# SCT Hits",10,0,10);
  TH1D* h_mu_mboy_nTRTHits = new TH1D("h_mu_mboy_nTRTHits","h_mu_mboy_nTRTHits;# TRT Hits",10,0,10);
  TH1D* h_mu_mboy_nTRTHighTHits = new TH1D("h_mu_mboy_nTRTHighTHits","h_mu_mboy_nTRTHighTHits;# TRT High Th Hits",10,0,10);
  TH1D* h_mu_mboy_nPixHoles = new TH1D("h_mu_mboy_nPixHoles","h_mu_mboy_nPixHoles;# Pixel Holes",50,0,50);
  TH1D* h_mu_mboy_nSCTHoles = new TH1D("h_mu_mboy_nSCTHoles","h_mu_mboy_nSCTHoles;# SCT Holes",50,0,50);
  TH1D* h_mu_mboy_nTRTHoles = new TH1D("h_mu_mboy_nTRTHoles","h_mu_mboy_nTRTHoles;# TRT Holes",50,0,50);
  TH1D* h_mu_mboy_nMDTHits = new TH1D("h_mu_mboy_nMDTHits","h_mu_mboy_nMDTHits;# MDT Hits",60,0,60);
  TH1D* h_mu_mboy_nCSCEtaHits = new TH1D("h_mu_mboy_nCSCEtaHits","h_mu_mboy_nCSCEtaHits;# CSC Eta Hits",10,0,10);
  TH1D* h_mu_mboy_nCSCPhiHits = new TH1D("h_mu_mboy_nCSCPhiHits","h_mu_mboy_nCSCPhiHits;# CSC Phi Hits",10,0,10);
  TH1D* h_mu_mboy_nRPCEtaHits = new TH1D("h_mu_mboy_nRPCEtaHits","h_mu_mboy_nRPCEtaHits;# RPC Eta Hits",20,0,20);
  TH1D* h_mu_mboy_nRPCPhiHits = new TH1D("h_mu_mboy_nRPCPhiHits","h_mu_mboy_nRPCPhiHits;# RPC Phi Hits",20,0,20);
  TH1D* h_mu_mboy_nTGCEtaHits = new TH1D("h_mu_mboy_nTGCEtaHits","h_mu_mboy_nTGCEtaHits;# TGC Eta Hits",20,0,20);
  TH1D* h_mu_mboy_nTGCPhiHits = new TH1D("h_mu_mboy_nTGCPhiHits","h_mu_mboy_nTGCPhiHits;# TGC Phi Hits",20,0,20);
  TH1D* h_mu_mboy_nHits = new TH1D("h_mu_mboy_nHits","h_mu_mboy_nHits;# Hits",100,0,100);
  TH1D* h_mu_mboy_nHoles = new TH1D("h_mu_mboy_nHoles","h_mu_mboy_nHoles;# Holes",10,0,10);  
   
  TH1D* h_mc_n = new TH1D("mc_n","mc_n;# muons",10,-0.5,9.5);
  TH1D* h_mc_gen_pt = new TH1D("h_mc_gen_pt","h_mc_gen_pt;p_{T}",50,0,100*1000);
  TH1D* h_mc_gen_pt_high = new TH1D("h_mc_gen_pt_high","h_mc_gen_pt_high;p_{T}",50,0,1000*1000);
  TH1D* h_mc_gen_eta = new TH1D("h_mc_gen_eta","h_mc_gen_eta pT > 5 GeV;\eta",60,-3,3);
  TH1D* h_mc_gen_phi = new TH1D("h_mc_gen_phi","h_mc_gen_phi pT > 5 GeV;\phi",32,-3.1416,3.1416);
  TH1D* h_eff_pt_moore = new TH1D("h_eff_pt_moore","h_eff_pt_moore |\eta| < 2.7;p_{T}",50,0,100*1000);
  TH1D* h_eff_pt_high_moore = new TH1D("h_eff_pt_high_moore","h_eff_pt_high_moore |\eta| < 2.7;p_{T}",50,0,1000*1000);
  TH1D* h_eff_pt_mu = new TH1D("h_eff_pt_mu","h_eff_pt_mu |\eta| < 2.7;p_{T}",50,0,100*1000);
  TH1D* h_eff_pt_high_mu = new TH1D("h_eff_pt_high_mu","h_eff_pt_high_mu |\eta| < 2.7;p_{T}",50,0,1000*1000);
  TH1D* h_eff_pt_mboy = new TH1D("h_eff_pt_mboy","h_eff_pt_mboy |\eta| < 2.7;p_{T}",50,0,100*1000);
  TH1D* h_eff_pt_high_mboy = new TH1D("h_eff_pt_high_mboy","h_eff_pt_high_mboy |\eta| < 2.7;p_{T}",50,0,1000*1000);
  TH1D* h_eff_eta_moore = new TH1D("h_eff_eta_moore","h_eff_eta_moore pT > 5 GeV;\eta",60,-3,3);
  TH1D* h_eff_eta_mu = new TH1D("h_eff_eta_mu","h_eff_eta_mu pT > 5 GeV;\eta",60,-3,3);
  TH1D* h_eff_eta_mboy = new TH1D("h_eff_eta_mboy","h_eff_eta_mboy pT > 5 GeV;\eta",60,-3,3);
  TH1D* h_res_pt_mu_moore = new TH1D("h_res_pt_mu_moore","h_res_pt_mu_moore;(p_{T} - p_{T,gen}) / p_{T,gen}",100,-1,1);
  TH1D* h_res_pt_mu = new TH1D("h_res_pt_mu","h_res_pt_mu;(p_{T} - p_{T,gen}) / p_{T,gen}",100,-1,1);
  TH1D* h_res_pt_mu_mboy = new TH1D("h_res_pt_mu_mboy","h_res_pt_mu_mboy;(p_{T} - p_{T,gen}) / p_{T,gen}",100,-1,1);
  TH1D* h_res_pt_ms_moore = new TH1D("h_res_pt_ms_moore","h_res_pt_ms_moore;(p_{T} - p_{T,gen}) / p_{T,gen}",100,-1,1);
  TH1D* h_res_pt_ms_mu = new TH1D("h_res_pt_ms_mu","h_res_pt_ms_mu;(p_{T} - p_{T,gen}) / p_{T,gen}",100,-1,1);
  TH1D* h_res_pt_ms_mboy = new TH1D("h_res_pt_ms_mboy","h_res_pt_ms_mboy;(p_{T} - p_{T,gen}) / p_{T,gen}",100,-1,1);
  TH1D* h_res_eta_mu_moore = new TH1D("h_res_eta_mu_moore","h_res_eta_mu_moore;(\eta - \eta_{gen}) / \eta_{gen}",100,-0.1,0.1);
  TH1D* h_res_eta_mu = new TH1D("h_res_eta_mu","h_res_eta_mu;(\eta - \eta_{gen}) / \eta_{gen}",100,-0.1,0.1);
  TH1D* h_res_eta_mu_mboy = new TH1D("h_res_eta_mu_mboy","h_res_eta_mu_mboy;(\eta - \eta_{gen}) / \eta_{gen}",100,-0.1,0.1);
  TH1D* h_res_phi_mu_moore = new TH1D("h_res_phi_mu_moore","h_res_phi_mu_moore;(\phi - \phi_{gen}) / \phi_{gen}",100,-0.1,0.1);
  TH1D* h_res_phi_mu = new TH1D("h_res_phi_mu","h_res_phi_mu;(\phi - \phi_{gen}) / \phi_{gen}",100,-0.1,0.1);
  TH1D* h_res_phi_mu_mboy = new TH1D("h_res_phi_mu_mboy","h_res_phi_mu_mboy;(\phi - \phi_{gen}) / \phi_{gen}",100,-0.1,0.1);

  h_mc_gen_pt->Sumw2();
  h_mc_gen_pt_high->Sumw2();
  h_mc_gen_eta->Sumw2();
  h_eff_pt_moore->Sumw2();
  h_eff_pt_high_moore->Sumw2();
  h_eff_pt_mu->Sumw2();
  h_eff_pt_high_mu->Sumw2();
  h_eff_pt_mboy->Sumw2();
  h_eff_pt_high_mboy->Sumw2();
  h_eff_eta_moore->Sumw2();
  h_eff_eta_mu->Sumw2();
  h_eff_eta_mboy->Sumw2();
    
  vector<TH1D*> mu_moore;
  vector<TH1D*> mu;
  vector<TH1D*> mu_mboy;
  vector<TH1D*> mc;
  vector<TString> names;
  vector<TString> mc_names;
  
  mu_moore.push_back(h_mu_moore_n);
  mu_moore.push_back(h_mu_moore_pt);
  mu_moore.push_back(h_mu_moore_pt_high);
  mu_moore.push_back(h_mu_moore_eta);
  mu_moore.push_back(h_mu_moore_phi);
  mu_moore.push_back(h_mu_moore_chi2_over_ndof);
  mu_moore.push_back(h_mu_moore_nBLHits);
  mu_moore.push_back(h_mu_moore_nPixHits);
  mu_moore.push_back(h_mu_moore_nSCTHits);
  mu_moore.push_back(h_mu_moore_nTRTHits);
  mu_moore.push_back(h_mu_moore_nTRTHighTHits);
  mu_moore.push_back(h_mu_moore_nPixHoles);
  mu_moore.push_back(h_mu_moore_nSCTHoles);
  mu_moore.push_back(h_mu_moore_nTRTHoles);
  mu_moore.push_back(h_mu_moore_nMDTHits);
  mu_moore.push_back(h_mu_moore_nCSCEtaHits);
  mu_moore.push_back(h_mu_moore_nCSCPhiHits);
  mu_moore.push_back(h_mu_moore_nRPCEtaHits);
  mu_moore.push_back(h_mu_moore_nRPCPhiHits);
  mu_moore.push_back(h_mu_moore_nTGCEtaHits);
  mu_moore.push_back(h_mu_moore_nTGCPhiHits);
  mu_moore.push_back(h_mu_moore_nHits);
  mu_moore.push_back(h_mu_moore_nHoles);
  
  mu.push_back(h_mu_n);
  mu.push_back(h_mu_pt);
  mu.push_back(h_mu_pt_high);
  mu.push_back(h_mu_eta);
  mu.push_back(h_mu_phi);
  mu.push_back(h_mu_chi2_over_ndof);
  mu.push_back(h_mu_nBLHits);
  mu.push_back(h_mu_nPixHits);
  mu.push_back(h_mu_nSCTHits);
  mu.push_back(h_mu_nTRTHits);
  mu.push_back(h_mu_nTRTHighTHits);
  mu.push_back(h_mu_nPixHoles);
  mu.push_back(h_mu_nSCTHoles);
  mu.push_back(h_mu_nTRTHoles);
  mu.push_back(h_mu_nMDTHits);
  mu.push_back(h_mu_nCSCEtaHits);
  mu.push_back(h_mu_nCSCPhiHits);
  mu.push_back(h_mu_nRPCEtaHits);
  mu.push_back(h_mu_nRPCPhiHits);
  mu.push_back(h_mu_nTGCEtaHits);
  mu.push_back(h_mu_nTGCPhiHits);
  mu.push_back(h_mu_nHits);
  mu.push_back(h_mu_nHoles);
  
  mu_mboy.push_back(h_mu_mboy_n);
  mu_mboy.push_back(h_mu_mboy_pt);
  mu_mboy.push_back(h_mu_mboy_pt_high);
  mu_mboy.push_back(h_mu_mboy_eta);
  mu_mboy.push_back(h_mu_mboy_phi);
  mu_mboy.push_back(h_mu_mboy_chi2_over_ndof);
  mu_mboy.push_back(h_mu_mboy_nBLHits);
  mu_mboy.push_back(h_mu_mboy_nPixHits);
  mu_mboy.push_back(h_mu_mboy_nSCTHits);
  mu_mboy.push_back(h_mu_mboy_nTRTHits);
  mu_mboy.push_back(h_mu_mboy_nTRTHighTHits);
  mu_mboy.push_back(h_mu_mboy_nPixHoles);
  mu_mboy.push_back(h_mu_mboy_nSCTHoles);
  mu_mboy.push_back(h_mu_mboy_nTRTHoles);
  mu_mboy.push_back(h_mu_mboy_nMDTHits);
  mu_mboy.push_back(h_mu_mboy_nCSCEtaHits);
  mu_mboy.push_back(h_mu_mboy_nCSCPhiHits);
  mu_mboy.push_back(h_mu_mboy_nRPCEtaHits);
  mu_mboy.push_back(h_mu_mboy_nRPCPhiHits);
  mu_mboy.push_back(h_mu_mboy_nTGCEtaHits);
  mu_mboy.push_back(h_mu_mboy_nTGCPhiHits);
  mu_mboy.push_back(h_mu_mboy_nHits);
  mu_mboy.push_back(h_mu_mboy_nHoles);
  
  mc.push_back(h_mc_n);
  mc.push_back(h_mc_gen_pt);
  mc.push_back(h_mc_gen_pt_high);
  mc.push_back(h_mc_gen_eta);
  mc.push_back(h_mc_gen_phi);
  mc.push_back(h_eff_pt_moore);
  mc.push_back(h_eff_pt_mu);
  mc.push_back(h_eff_pt_mboy);
  mc.push_back(h_eff_pt_high_moore);
  mc.push_back(h_eff_pt_high_mu);
  mc.push_back(h_eff_pt_high_mboy);
  mc.push_back(h_eff_eta_moore);
  mc.push_back(h_eff_eta_mu);
  mc.push_back(h_eff_eta_mboy);
  mc.push_back(h_res_pt_mu_moore);
  mc.push_back(h_res_pt_mu);
  mc.push_back(h_res_pt_mu_mboy);
  mc.push_back(h_res_eta_mu_moore);
  mc.push_back(h_res_eta_mu);
  mc.push_back(h_res_eta_mu_mboy);
  mc.push_back(h_res_phi_mu_moore);
  mc.push_back(h_res_phi_mu);
  mc.push_back(h_res_phi_mu_mboy);
  mc.push_back(h_res_pt_ms_moore);
  mc.push_back(h_res_pt_ms_mu);
  mc.push_back(h_res_pt_ms_mboy);
  
  mc_names.push_back("mc_n_muons");
  mc_names.push_back("mc_pt");
  mc_names.push_back("mc_high_pt");
  mc_names.push_back("mc_eta");
  mc_names.push_back("mc_phi");
  mc_names.push_back("efficiency_pt");
  mc_names.push_back("efficiency_pt_high");
  mc_names.push_back("efficiency_eta");
  mc_names.push_back("resolution_pt");
  mc_names.push_back("resolution_eta");
  mc_names.push_back("resolution_phi");
  mc_names.push_back("resolution_pt_ms");
  
  names.push_back("n_muons");
  names.push_back("pt");
  names.push_back("high_pt");
  names.push_back("eta");
  names.push_back("phi");
  names.push_back("chi2_over_ndof");
  names.push_back("nBLHits");
  names.push_back("nPixHits");
  names.push_back("nSCTHits");
  names.push_back("nTRTHits");
  names.push_back("nTRTHighTHits");
  names.push_back("nPixHoles");
  names.push_back("nSCTHoles");
  names.push_back("nTRTHoles");
  names.push_back("nMDTHits");
  names.push_back("nCSCEtaHits");
  names.push_back("nCSCPhiHits");
  names.push_back("nRPCEtaHits");
  names.push_back("nRPCPhiHits");
  names.push_back("nTGCEtaHits");
  names.push_back("nTGCPhiHits");
  names.push_back("nHits");
  names.push_back("nHoles");
  
  
  for(int i=0; i<tree->GetEntries(); i++){
    tree->GetEntry(i);
    
    h_mu_moore_n->Fill(mu_moore_n);
    for(unsigned int j=0 ; j<mu_moore_n; j++ ){
      h_mu_moore_pt->Fill(mu_moore_pt->at(j));
      h_mu_moore_pt_high->Fill(mu_moore_pt->at(j));
      h_mu_moore_eta->Fill(mu_moore_eta->at(j));
      h_mu_moore_phi->Fill(mu_moore_phi_wrtBL->at(j));
      h_mu_moore_chi2_over_ndof->Fill(mu_moore_chi2->at(j)/mu_moore_ndof->at(j));
      h_mu_moore_nBLHits->Fill(mu_moore_nBLHits->at(j));
      h_mu_moore_nPixHits->Fill(mu_moore_nPixHits->at(j));
      h_mu_moore_nSCTHits->Fill(mu_moore_nSCTHits->at(j));
      h_mu_moore_nTRTHits->Fill(mu_moore_nTRTHits->at(j));
      h_mu_moore_nTRTHighTHits->Fill(mu_moore_nTRTHighTHits->at(j));
      h_mu_moore_nPixHoles->Fill(mu_moore_nPixHoles->at(j));
      h_mu_moore_nSCTHoles->Fill(mu_moore_nSCTHoles->at(j));
      h_mu_moore_nTRTHoles->Fill(mu_moore_nTRTHoles->at(j));
      h_mu_moore_nMDTHits->Fill(mu_moore_nMDTHits->at(j));
      h_mu_moore_nCSCEtaHits->Fill(mu_moore_nCSCEtaHits->at(j));
      h_mu_moore_nCSCPhiHits->Fill(mu_moore_nCSCPhiHits->at(j));
      h_mu_moore_nRPCEtaHits->Fill(mu_moore_nRPCEtaHits->at(j));
      h_mu_moore_nRPCPhiHits->Fill(mu_moore_nRPCPhiHits->at(j));
      h_mu_moore_nTGCEtaHits->Fill(mu_moore_nTGCEtaHits->at(j));
      h_mu_moore_nTGCPhiHits->Fill(mu_moore_nTGCPhiHits->at(j));
      h_mu_moore_nHits->Fill(mu_moore_nHits->at(j));
      h_mu_moore_nHoles->Fill(mu_moore_nHoles->at(j));
    }

    h_mu_n->Fill(mu_n);
    for(unsigned int j=0 ; j<mu_n; j++ ){
      h_mu_pt->Fill(mu_pt->at(j));
      h_mu_pt_high->Fill(mu_pt->at(j));
      h_mu_eta->Fill(mu_eta->at(j));
      h_mu_phi->Fill(mu_phi_wrtBL->at(j));
      h_mu_chi2_over_ndof->Fill(mu_chi2->at(j)/mu_ndof->at(j));
      h_mu_nBLHits->Fill(mu_nBLHits->at(j));
      h_mu_nPixHits->Fill(mu_nPixHits->at(j));
      h_mu_nSCTHits->Fill(mu_nSCTHits->at(j));
      h_mu_nTRTHits->Fill(mu_nTRTHits->at(j));
      h_mu_nTRTHighTHits->Fill(mu_nTRTHighTHits->at(j));
      h_mu_nPixHoles->Fill(mu_nPixHoles->at(j));
      h_mu_nSCTHoles->Fill(mu_nSCTHoles->at(j));
      h_mu_nTRTHoles->Fill(mu_nTRTHoles->at(j));
      h_mu_nMDTHits->Fill(mu_nMDTHits->at(j));
      h_mu_nCSCEtaHits->Fill(mu_nCSCEtaHits->at(j));
      h_mu_nCSCPhiHits->Fill(mu_nCSCPhiHits->at(j));
      h_mu_nRPCEtaHits->Fill(mu_nRPCEtaHits->at(j));
      h_mu_nRPCPhiHits->Fill(mu_nRPCPhiHits->at(j));
      h_mu_nTGCEtaHits->Fill(mu_nTGCEtaHits->at(j));
      h_mu_nTGCPhiHits->Fill(mu_nTGCPhiHits->at(j));
      h_mu_nHits->Fill(mu_nHits->at(j));
      h_mu_nHoles->Fill(mu_nHoles->at(j));
    }

    h_mu_mboy_n->Fill(mu_mboy_n);
    for(unsigned int j=0 ; j<mu_mboy_n; j++ ){
      h_mu_mboy_pt->Fill(mu_mboy_pt->at(j));
      h_mu_mboy_pt_high->Fill(mu_mboy_pt->at(j));
      h_mu_mboy_eta->Fill(mu_mboy_eta->at(j));
      h_mu_mboy_phi->Fill(mu_mboy_phi_wrtBL->at(j));
      h_mu_mboy_chi2_over_ndof->Fill(mu_mboy_chi2->at(j)/mu_mboy_ndof->at(j));
      h_mu_mboy_nBLHits->Fill(mu_mboy_nBLHits->at(j));
      h_mu_mboy_nPixHits->Fill(mu_mboy_nPixHits->at(j));
      h_mu_mboy_nSCTHits->Fill(mu_mboy_nSCTHits->at(j));
      h_mu_mboy_nTRTHits->Fill(mu_mboy_nTRTHits->at(j));
      h_mu_mboy_nTRTHighTHits->Fill(mu_mboy_nTRTHighTHits->at(j));
      h_mu_mboy_nPixHoles->Fill(mu_mboy_nPixHoles->at(j));
      h_mu_mboy_nSCTHoles->Fill(mu_mboy_nSCTHoles->at(j));
      h_mu_mboy_nTRTHoles->Fill(mu_mboy_nTRTHoles->at(j));
      h_mu_mboy_nMDTHits->Fill(mu_mboy_nMDTHits->at(j));
      h_mu_mboy_nCSCEtaHits->Fill(mu_mboy_nCSCEtaHits->at(j));
      h_mu_mboy_nCSCPhiHits->Fill(mu_mboy_nCSCPhiHits->at(j));
      h_mu_mboy_nRPCEtaHits->Fill(mu_mboy_nRPCEtaHits->at(j));
      h_mu_mboy_nRPCPhiHits->Fill(mu_mboy_nRPCPhiHits->at(j));
      h_mu_mboy_nTGCEtaHits->Fill(mu_mboy_nTGCEtaHits->at(j));
      h_mu_mboy_nTGCPhiHits->Fill(mu_mboy_nTGCPhiHits->at(j));
      h_mu_mboy_nHits->Fill(mu_mboy_nHits->at(j));
      h_mu_mboy_nHoles->Fill(mu_mboy_nHoles->at(j));
    }

    // Loop over all true muons from the IP, require pT > 3 GeV and |eta| < 2.7
    unsigned int mc_muons = 0;
    for (unsigned int j=0 ; j<mc_n; j++ ){
      if ((mc_gen_type->at(j) == 13 || mc_gen_type->at(j) == -13) && mc_gen_pt->at(j) > 3000. && fabs(mc_gen_eta->at(j)) < 2.7) {
        mc_muons++;
        float deltaR = 0.1;
        int muonmu = -1, muonmu_moore = -1, muonmu_mboy = -1;

        for(unsigned int k=0 ; k<mu_moore_n; k++ ){
          if (TMath::Sqrt((mu_moore_phi_wrtBL->at(k) - mc_gen_phi->at(j)) * (mu_moore_phi_wrtBL->at(k) - mc_gen_phi->at(j)) +
                   (mu_moore_eta->at(k) - mc_gen_eta->at(j)) * (mu_moore_eta->at(k) - mc_gen_eta->at(j))) < deltaR) {
          muonmu_moore = k;
          deltaR = TMath::Sqrt((mu_moore_phi_wrtBL->at(k) - mc_gen_phi->at(j)) * (mu_moore_phi_wrtBL->at(k) - mc_gen_phi->at(j)) +
                        (mu_moore_eta->at(k) - mc_gen_eta->at(j)) * (mu_moore_eta->at(k) - mc_gen_eta->at(j)));
          }
        }
        deltaR = 0.1;
        for(unsigned int k=0 ; k<mu_n; k++ ){
          if (TMath::Sqrt((mu_phi_wrtBL->at(k) - mc_gen_phi->at(j)) * (mu_phi_wrtBL->at(k) - mc_gen_phi->at(j)) +
                   (mu_eta->at(k) - mc_gen_eta->at(j)) * (mu_eta->at(k) - mc_gen_eta->at(j))) < deltaR) {
          muonmu = k;
          deltaR = TMath::Sqrt((mu_phi_wrtBL->at(k) - mc_gen_phi->at(j)) * (mu_phi_wrtBL->at(k) - mc_gen_phi->at(j)) +
                        (mu_eta->at(k) - mc_gen_eta->at(j)) * (mu_eta->at(k) - mc_gen_eta->at(j)));
          }
        }
        deltaR = 0.1;
        for(unsigned int k=0 ; k<mu_mboy_n; k++ ){
          if (TMath::Sqrt((mu_mboy_phi_wrtBL->at(k) - mc_gen_phi->at(j)) * (mu_mboy_phi_wrtBL->at(k) - mc_gen_phi->at(j)) +
                   (mu_mboy_eta->at(k) - mc_gen_eta->at(j)) * (mu_mboy_eta->at(k) - mc_gen_eta->at(j))) < deltaR) {
          muonmu_mboy = k;
          deltaR = TMath::Sqrt((mu_mboy_phi_wrtBL->at(k) - mc_gen_phi->at(j)) * (mu_mboy_phi_wrtBL->at(k) - mc_gen_phi->at(j)) +
                        (mu_mboy_eta->at(k) - mc_gen_eta->at(j)) * (mu_mboy_eta->at(k) - mc_gen_eta->at(j)));
          }
        }
        
        h_mc_gen_pt->Fill(mc_gen_pt->at(j));
        h_mc_gen_pt_high->Fill(mc_gen_pt->at(j));
        if (mc_gen_pt->at(j) > 5000.) {
          h_mc_gen_eta->Fill(mc_gen_eta->at(j));
          h_mc_gen_phi->Fill(mc_gen_phi->at(j));
	}

        if (muonmu_moore >= 0) { // found Moore muon
          h_res_pt_mu_moore->Fill((mu_moore_pt->at(muonmu_moore) - mc_gen_pt->at(j))/mc_gen_pt->at(j));
          h_res_eta_mu_moore->Fill((mu_moore_eta->at(muonmu_moore) - mc_gen_eta->at(j))/mc_gen_eta->at(j));
          h_res_phi_mu_moore->Fill((mu_moore_phi_wrtBL->at(muonmu_moore) - mc_gen_phi->at(j))/mc_gen_phi->at(j));
          h_eff_pt_moore->Fill(mc_gen_pt->at(j));
          h_eff_pt_high_moore->Fill(mc_gen_pt->at(j));
          if (mc_gen_pt->at(j) > 5000.) h_eff_eta_moore->Fill(mc_gen_eta->at(j));
	}
        if (muonmu >= 0) { // found muon
          h_res_pt_mu->Fill((mu_pt->at(muonmu) - mc_gen_pt->at(j))/mc_gen_pt->at(j));
          h_res_eta_mu->Fill((mu_eta->at(muonmu) - mc_gen_eta->at(j))/mc_gen_eta->at(j));
          h_res_phi_mu->Fill((mu_phi_wrtBL->at(muonmu) - mc_gen_phi->at(j))/mc_gen_phi->at(j));
          h_eff_pt_mu->Fill(mc_gen_pt->at(j));
          h_eff_pt_high_mu->Fill(mc_gen_pt->at(j));
          if (mc_gen_pt->at(j) > 5000.) h_eff_eta_mu->Fill(mc_gen_eta->at(j));
	}
        if (muonmu_mboy >= 0) { // found Muonboy muon
          h_res_pt_mu_mboy->Fill((mu_mboy_pt->at(muonmu_mboy) - mc_gen_pt->at(j))/mc_gen_pt->at(j));
          h_res_eta_mu_mboy->Fill((mu_mboy_eta->at(muonmu_mboy) - mc_gen_eta->at(j))/mc_gen_eta->at(j));
          h_res_phi_mu_mboy->Fill((mu_mboy_phi_wrtBL->at(muonmu_mboy) - mc_gen_phi->at(j))/mc_gen_phi->at(j));
          h_eff_pt_mboy->Fill(mc_gen_pt->at(j));
          h_eff_pt_high_mboy->Fill(mc_gen_pt->at(j));
          if (mc_gen_pt->at(j) > 5000.) h_eff_eta_mboy->Fill(mc_gen_eta->at(j));
	}
      }
    }
    h_mc_n->Fill(mc_muons);

    // Loop over all true muons at the entrance to the Muon Spectrometer, require pT > 3 GeV and |eta| < 2.7
    unsigned int ms_entry_muons = 0;
    for (unsigned int j=0 ; j<ms_entry_truth_n; j++ ){
      if ((ms_entry_truth_pdgId->at(j) == 13 || ms_entry_truth_pdgId->at(j) == -13) && ms_entry_truth_pt->at(j) > 3000. && fabs(ms_entry_truth_eta->at(j)) < 2.7) {
        ms_entry_muons++;
        float deltaR = 0.1;
        int muonmu = -1, muonmu_moore = -1, muonmu_mboy = -1;

        for(unsigned int k=0 ; k<mu_moore_n; k++ ){
          if (TMath::Sqrt((mu_moore_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) * (mu_moore_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) +
                   (mu_moore_eta->at(k) - ms_entry_truth_eta->at(j)) * (mu_moore_eta->at(k) - ms_entry_truth_eta->at(j))) < deltaR) {
          muonmu_moore = k;
          deltaR = TMath::Sqrt((mu_moore_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) * (mu_moore_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) +
                        (mu_moore_eta->at(k) - ms_entry_truth_eta->at(j)) * (mu_moore_eta->at(k) - ms_entry_truth_eta->at(j)));
          }
        }
        deltaR = 0.1;
        for(unsigned int k=0 ; k<mu_n; k++ ){
          if (TMath::Sqrt((mu_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) * (mu_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) +
                   (mu_eta->at(k) - ms_entry_truth_eta->at(j)) * (mu_eta->at(k) - ms_entry_truth_eta->at(j))) < deltaR) {
          muonmu = k;
          deltaR = TMath::Sqrt((mu_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) * (mu_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) +
                        (mu_eta->at(k) - ms_entry_truth_eta->at(j)) * (mu_eta->at(k) - ms_entry_truth_eta->at(j)));
          }
        }
        deltaR = 0.1;
        for(unsigned int k=0 ; k<mu_mboy_n; k++ ){
          if (TMath::Sqrt((mu_mboy_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) * (mu_mboy_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) +
                   (mu_mboy_eta->at(k) - ms_entry_truth_eta->at(j)) * (mu_mboy_eta->at(k) - ms_entry_truth_eta->at(j))) < deltaR) {
          muonmu_mboy = k;
          deltaR = TMath::Sqrt((mu_mboy_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) * (mu_mboy_phi_wrtBL->at(k) - ms_entry_truth_phi->at(j)) +
                        (mu_mboy_eta->at(k) - ms_entry_truth_eta->at(j)) * (mu_mboy_eta->at(k) - ms_entry_truth_eta->at(j)));
          }
        }

        if (muonmu_moore >= 0) { // found Moore muon
          h_res_pt_ms_moore->Fill((mu_moore_pt->at(muonmu_moore) - ms_entry_truth_pt->at(j))/ms_entry_truth_pt->at(j));
	}
        if (muonmu >= 0) { // found muon
          h_res_pt_ms_mu->Fill((mu_pt->at(muonmu) - ms_entry_truth_pt->at(j))/ms_entry_truth_pt->at(j));
	}
        if (muonmu_mboy >= 0) { // found Muonboy muon
          h_res_pt_ms_mboy->Fill((mu_mboy_pt->at(muonmu_mboy) - ms_entry_truth_pt->at(j))/ms_entry_truth_pt->at(j));
	}
      }
    }
  }
  
  
  TCanvas c("Canvas","Canvas",800,600);

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gROOT->ForceStyle();
  gStyle->SetOptStat(0);
  
  for(unsigned int iFill=0; iFill<mu_moore.size(); iFill++){
    char numb[100]; 
    
    TLegend leg(0.8,0.7,0.95,0.95);
    
    leg.AddEntry(mu_moore[iFill],"mu_moore","l");
    sprintf(numb,"Overflow %i",mu_moore[iFill]->GetBinContent(mu_moore[iFill]->GetNbinsX()+1));
    leg.AddEntry(mu_moore[iFill],numb,"l");
    
    leg.AddEntry(mu[iFill],"mu","l");
    sprintf(numb,"Overflow %i",mu[iFill]->GetBinContent(mu[iFill]->GetNbinsX()+1));
    leg.AddEntry(mu[iFill],numb,"l");
    
    leg.AddEntry(mu_mboy[iFill],"mu_mboy","l");
    sprintf(numb,"Overflow %i",mu_mboy[iFill]->GetBinContent(mu_mboy[iFill]->GetNbinsX()+1));
    leg.AddEntry(mu_mboy[iFill],numb,"l");
      
    double max=mu_moore[iFill]->GetMaximum();
    if(mu[iFill]->GetMaximum()>max) max= mu[iFill]->GetMaximum();
    if(mu_mboy[iFill]->GetMaximum()>max) max= mu_mboy[iFill]->GetMaximum();
    
    max=max+max/10.;
    
    mu_moore[iFill]->SetLineColor(kRed);
    mu_moore[iFill]->SetLineStyle(1);
    mu_moore[iFill]->SetLineWidth(4);
    mu[iFill]->SetLineColor(kGreen+2);
    mu[iFill]->SetLineStyle(7);
    mu[iFill]->SetLineWidth(3);
    mu_mboy[iFill]->SetLineColor(kBlue);
    mu_mboy[iFill]->SetLineStyle(9);
    mu_mboy[iFill]->SetLineWidth(2);
    
    mu_moore[iFill]->GetYaxis()->SetRangeUser(0.,max);
    mu_moore[iFill]->SetLabelOffset(1.5,"X");
    c.Divide(1,2);
    c.cd(1);
    c.cd(1)->SetPad(0,0.3,1,1);
    c.cd(2);
    c.cd(2)->SetPad(0,-0.05,1,0.36);
    c.cd(2)->SetBottomMargin(0.3);//45);

    c.cd(1);
    mu_moore[iFill]->Draw("hist");
    mu[iFill]->Draw("hist same ");
    mu_mboy[iFill]->Draw("hist same");
    
    
    leg.Draw("same");
    
    c.cd(2);
    TH1D* mu_mooreratio=(TH1D*) mu_moore[iFill]->Clone();
    //mu_mooreratio->SetTitle("");
    mu_mooreratio->SetTitle("Rate relative to Moore");
    
    TH1D* muratio=(TH1D*) mu[iFill]->Clone();
    muratio->SetTitle("");
    TH1D* mu_mboyratio=(TH1D*) mu_mboy[iFill]->Clone();
    mu_mboyratio->SetTitle("");
    mu_mooreratio->Divide(mu_moore[iFill]);
    muratio->Divide(mu_moore[iFill]);
    mu_mboyratio->Divide(mu_moore[iFill]);
    
    double max_ratio=2;
    
    for(unsigned int iBin=1; iBin<=mu_mooreratio->GetNbinsX(); iBin++){
      if(mu_mooreratio->GetBinContent(iBin)>max_ratio && mu_mooreratio->GetBinContent(iBin)<10) max_ratio=mu_mooreratio->GetBinContent(iBin);
      if(muratio->GetBinContent(iBin)>max_ratio && muratio->GetBinContent(iBin)<10) max_ratio=muratio->GetBinContent(iBin);
      if(mu_mboyratio->GetBinContent(iBin)>max_ratio && mu_mboyratio->GetBinContent(iBin)<10) max_ratio=mu_mboyratio->GetBinContent(iBin);
    }
    
    mu_mooreratio->GetYaxis()->SetRangeUser(0.,max_ratio);
    mu_mooreratio->SetLabelOffset(0.02,"X");
    //mu_mooreratio->SetLabelOffset(0.65,"Y");
    mu_mooreratio->SetLabelSize(0.06,"X");
    mu_mooreratio->SetLabelSize(0.06,"Y");
    mu_mooreratio->SetTitleSize(0.06,"X");
    
    // muratio->SetLabelOffset(0.95,"X");
    // muratio->SetLabelOffset(0.65,"Y");
    // muratio->SetLabelSize(0.11,"X");
    // muratio->SetLabelSize(0.11,"Y");
    
    // mu_mboyratio->SetLabelOffset(0.95,"X");
    // mu_mboyratio->SetLabelOffset(0.65,"Y");
    // mu_mboyratio->SetLabelSize(0.11,"X");
    // mu_mboyratio->SetLabelSize(0.11,"Y");
        
    mu_mooreratio->Draw("hist");
    muratio->Draw("hist same");
    mu_mboyratio->Draw("hist same");
    
    c.SaveAs(names[iFill]+".png");
    c.Clear();
  }

  //for(unsigned int iFill=0; iFill<12; iFill++){
  for(unsigned int iFill=0; iFill<mc_names.size(); iFill++){
    char numb0[100]; 
    char numb1[100]; 
    char numb2[100]; 
   
    TLegend leg1(0.8,0.85,0.95,0.97);
    TLegend leg2(0.7,0.7,0.95,0.95);

    if (iFill > 4) {

      if (mc_names[iFill] == "efficiency_pt") { // handle efficiency vs. pT plots
        h_eff_pt_moore->Divide(h_eff_pt_moore, h_mc_gen_pt, 1.,  1., "B");
        h_eff_pt_mu->Divide(h_eff_pt_mu, h_mc_gen_pt, 1.,  1., "B");
        h_eff_pt_mboy->Divide(h_eff_pt_mboy, h_mc_gen_pt, 1.,  1., "B");
      }
      if (mc_names[iFill] == "efficiency_pt_high") { // handle efficiency vs. pT plots
        h_eff_pt_high_moore->Divide(h_eff_pt_high_moore, h_mc_gen_pt_high, 1.,  1., "B");
        h_eff_pt_high_mu->Divide(h_eff_pt_high_mu, h_mc_gen_pt_high, 1.,  1., "B");
        h_eff_pt_high_mboy->Divide(h_eff_pt_high_mboy, h_mc_gen_pt_high, 1.,  1., "B");
      }
      if (mc_names[iFill] == "efficiency_eta") { // handle efficiency vs. eta plots
        h_eff_eta_moore->Divide(h_eff_eta_moore, h_mc_gen_eta, 1.,  1., "B");
        h_eff_eta_mu->Divide(h_eff_eta_mu, h_mc_gen_eta, 1.,  1., "B");
        h_eff_eta_mboy->Divide(h_eff_eta_mboy, h_mc_gen_eta, 1.,  1., "B");
      }
      if (mc_names[iFill] == "resolution_pt") { // handle pT resolution plots
        h_res_pt_mu_moore->Fit("gaus","I");
        h_res_pt_mu->Fit("gaus","I");
        h_res_pt_mu_mboy->Fit("gaus","I");
        if (h_res_pt_mu_moore->GetEntries() > 100.) {
          sprintf(numb0,"Gaus width %f",h_res_pt_mu_moore->GetFunction("gaus")->GetParameter(2));
	} else { sprintf(numb0,"Gaus width NA");}
        if (h_res_pt_mu->GetEntries() > 100.) {
          sprintf(numb1,"Gaus width %f",h_res_pt_mu->GetFunction("gaus")->GetParameter(2));
	} else { sprintf(numb1,"Gaus width NA");}
        if (h_res_pt_mu_mboy->GetEntries() > 100.) {
          sprintf(numb2,"Gaus width %f",h_res_pt_mu_mboy->GetFunction("gaus")->GetParameter(2));
	} else { sprintf(numb2,"Gaus width NA");}
      }
      if (mc_names[iFill] == "resolution_pt_ms") { // handle pT resolution plots
        h_res_pt_ms_moore->Fit("gaus","I");
        h_res_pt_ms_mu->Fit("gaus","I");
        h_res_pt_ms_mboy->Fit("gaus","I");
        if (h_res_pt_ms_moore->GetEntries() > 100.) {
          sprintf(numb0,"Gaus width %f",h_res_pt_ms_moore->GetFunction("gaus")->GetParameter(2));
	} else { sprintf(numb0,"Gaus width NA");}
        if (h_res_pt_ms_mu->GetEntries() > 100.) {
          sprintf(numb1,"Gaus width %f",h_res_pt_ms_mu->GetFunction("gaus")->GetParameter(2));
	} else { sprintf(numb1,"Gaus width NA");}
        if (h_res_pt_ms_mboy->GetEntries() > 100.) {
          sprintf(numb2,"Gaus width %f",h_res_pt_ms_mboy->GetFunction("gaus")->GetParameter(2));
	} else { sprintf(numb2,"Gaus width NA");}
      }

      unsigned int add = (iFill - 5) * 2;
      leg1.AddEntry(mc[iFill + add],"mu_moore","l");
      leg2.AddEntry(mc[iFill + add],"mu_moore","l");
      if (mc_names[iFill] != "resolution_pt" && mc_names[iFill] != "resolution_pt_ms") sprintf(numb0,"Overflow %i",mc[iFill + add]->GetBinContent(mc[iFill + add]->GetNbinsX()+1));

      if (mc_names[iFill] != "efficiency_pt" && mc_names[iFill] != "efficiency_pt_high" && mc_names[iFill] != "efficiency_eta") {
        leg2.AddEntry(mc[iFill + add],numb0,"l");
      }  
 
      leg1.AddEntry(mc[iFill + add + 1],"mu","l");
      leg2.AddEntry(mc[iFill + add + 1],"mu","l");
      if (mc_names[iFill] != "resolution_pt" && mc_names[iFill] != "resolution_pt_ms") sprintf(numb1,"Overflow %i",mc[iFill + add + 1]->GetBinContent(mc[iFill + add + 1]->GetNbinsX()+1));
      if (mc_names[iFill] != "efficiency_pt" && mc_names[iFill] != "efficiency_pt_high" && mc_names[iFill] != "efficiency_eta") {
        leg2.AddEntry(mc[iFill + add + 1],numb1,"l");
      }
   
      leg1.AddEntry(mc[iFill + add + 2],"mu_mboy","l");
      leg2.AddEntry(mc[iFill + add + 2],"mu_mboy","l");
      if (mc_names[iFill] != "resolution_pt" && mc_names[iFill] != "resolution_pt_ms") sprintf(numb2,"Overflow %i",mc[iFill + add + 2]->GetBinContent(mc[iFill + add + 2]->GetNbinsX()+1));
      if (mc_names[iFill] != "efficiency_pt" && mc_names[iFill] != "efficiency_pt_high" && mc_names[iFill] != "efficiency_eta") {
        leg2.AddEntry(mc[iFill + add + 2],numb2,"l");
      }
   
      double max=mc[iFill + add]->GetMaximum();
      if(mc[iFill + add + 1]->GetMaximum()>max) max= mc[iFill + add + 1]->GetMaximum();
      if(mc[iFill + add + 2]->GetMaximum()>max) max= mc[iFill + add + 2]->GetMaximum();
   
      max=max+max/10.;
   
      mc[iFill + add]->SetLineColor(kRed);
      mc[iFill + add]->SetLineStyle(1);
      mc[iFill + add]->SetLineWidth(4);
      mc[iFill + add + 1]->SetLineColor(kGreen+2);
      mc[iFill + add + 1]->SetLineStyle(7);
      mc[iFill + add + 1]->SetLineWidth(3);
      mc[iFill + add + 2]->SetLineColor(kBlue);
      mc[iFill + add + 2]->SetLineStyle(9);
      mc[iFill + add + 2]->SetLineWidth(2);
    
      mc[iFill + add]->GetYaxis()->SetRangeUser(0.,max);
      if (mc_names[iFill] == "efficiency_pt" || mc_names[iFill] == "efficiency_pt_high" || mc_names[iFill] == "efficiency_eta") { // handle efficiency vs. pT, eta plots
        mc[iFill + add]->Draw("e0");
        mc[iFill + add + 1]->Draw("e0 same ");
        mc[iFill + add + 2]->Draw("e0 same");
        leg1.Draw("same");
      }
      else { // resolution plots
        mc[iFill + add]->Draw("hist");
        mc[iFill + add + 1]->Draw("hist same ");
        mc[iFill + add + 2]->Draw("hist same");
        leg2.Draw("same");
      }
        
      c.SaveAs(mc_names[iFill]+".png");
      c.Clear();
    }
    else { // handle first mc histograms here
      leg.AddEntry(mc[iFill],"mc_gen","l");
      sprintf(numb,"Overflow %i",mc[iFill]->GetBinContent(mc[iFill]->GetNbinsX()+1));
      leg.AddEntry(mc[iFill],numb,"l");
    
      double max=mc[iFill]->GetMaximum();
      max=max+max/10.;
    
      mc[iFill]->SetLineColor(kRed);
      mc[iFill]->SetLineStyle(1);
      mc[iFill]->SetLineWidth(4);
    
      mc[iFill]->GetYaxis()->SetRangeUser(0.,max);
      mc[iFill]->Draw("hist");
      leg.Draw("same");

      c.SaveAs(mc_names[iFill]+".png");
      c.Clear();
    }
  }
  return;
}
