//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Mar  5 13:04:33 2020 by ROOT version 6.18/04
// from TTree calohitD3PD/calohitD3PD
// found on file: /eos/user/k/kiryunin/HEC_SF/10.1-FTFP_BERT_ATL/DumpHits/ele_e0100_sc1/01.root
//////////////////////////////////////////////////////////

#ifndef ClCaloHits_h
#define ClCaloHits_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"

class ClCaloHits {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UInt_t          RunNumber;
   ULong64_t       EventNumber;
   UInt_t          timestamp;
   UInt_t          timestamp_ns;
   UInt_t          lbn;
   UInt_t          bcid;
   UInt_t          detmask0;
   UInt_t          detmask1;
   Float_t         actualIntPerXing;
   Float_t         averageIntPerXing;
   UInt_t          pixelFlags;
   UInt_t          sctFlags;
   UInt_t          trtFlags;
   UInt_t          larFlags;
   UInt_t          tileFlags;
   UInt_t          muonFlags;
   UInt_t          fwdFlags;
   UInt_t          coreFlags;
   UInt_t          backgroundFlags;
   UInt_t          lumiFlags;
   UInt_t          pixelError;
   UInt_t          sctError;
   UInt_t          trtError;
   UInt_t          larError;
   UInt_t          tileError;
   UInt_t          muonError;
   UInt_t          fwdError;
   UInt_t          coreError;
   Bool_t          streamDecision_Egamma;
   Bool_t          streamDecision_Muons;
   Bool_t          streamDecision_JetTauEtmiss;
   UInt_t          l1id;
   Bool_t          isSimulation;
   Bool_t          isCalibration;
   Bool_t          isTestBeam;
   Int_t           hitemb_n;
   vector<float>   *hitemb_eta;
   vector<float>   *hitemb_phi;
   vector<float>   *hitemb_E;
   vector<float>   *hitemb_Time;
   vector<unsigned int> *hitemb_ID;
   Int_t           hitemec_n;
   vector<float>   *hitemec_eta;
   vector<float>   *hitemec_phi;
   vector<float>   *hitemec_E;
   vector<float>   *hitemec_Time;
   vector<unsigned int> *hitemec_ID;
   Int_t           hithec_n;
   vector<float>   *hithec_eta;
   vector<float>   *hithec_phi;
   vector<float>   *hithec_E;
   vector<float>   *hithec_Time;
   vector<unsigned int> *hithec_ID;
   Int_t           hitfcal_n;
   vector<float>   *hitfcal_eta;
   vector<float>   *hitfcal_phi;
   vector<float>   *hitfcal_E;
   vector<float>   *hitfcal_Time;
   vector<unsigned int> *hitfcal_ID;
   Int_t           TileHit_n;
   vector<vector<float> > *TileHit_energy;
   vector<vector<float> > *TileHit_time;
   vector<vector<int> > *TileHit_detector;
   vector<vector<int> > *TileHit_side;
   vector<vector<int> > *TileHit_sample;
   vector<vector<int> > *TileHit_pmt;
   vector<vector<int> > *TileHit_eta;
   vector<vector<int> > *TileHit_phi;
   Int_t           MBTSHit_n;
   vector<vector<float> > *MBTSHit_energy;
   vector<vector<float> > *MBTSHit_time;
   vector<vector<int> > *MBTSHit_detector;
   vector<vector<int> > *MBTSHit_side;
   vector<vector<int> > *MBTSHit_sample;
   vector<vector<int> > *MBTSHit_pmt;
   vector<vector<int> > *MBTSHit_eta;
   vector<vector<int> > *MBTSHit_phi;
   Int_t           laract_n;
   vector<float>   *laract_eta;
   vector<float>   *laract_phi;
   vector<unsigned int> *laract_ID;
   vector<float>   *laract_EnergyTot;
   vector<float>   *laract_EnergyVis;
   vector<unsigned int> *laract_detector;
   vector<unsigned int> *laract_sample;
   vector<unsigned int> *laract_side;
   vector<unsigned int> *laract_reg_sec;
   vector<unsigned int> *laract_eta_tower;
   vector<unsigned int> *laract_phi_module;
   vector<float>   *laract_EnergyEm;
   vector<float>   *laract_EnergyNonEm;
   vector<float>   *laract_EnergyInv;
   vector<float>   *laract_EnergyEsc;
   Int_t           larinact_n;
   vector<float>   *larinact_eta;
   vector<float>   *larinact_phi;
   vector<unsigned int> *larinact_ID;
   vector<float>   *larinact_EnergyTot;
   vector<float>   *larinact_EnergyVis;
   vector<unsigned int> *larinact_detector;
   vector<unsigned int> *larinact_sample;
   vector<unsigned int> *larinact_side;
   vector<unsigned int> *larinact_reg_sec;
   vector<unsigned int> *larinact_eta_tower;
   vector<unsigned int> *larinact_phi_module;
   vector<float>   *larinact_EnergyEm;
   vector<float>   *larinact_EnergyNonEm;
   vector<float>   *larinact_EnergyInv;
   vector<float>   *larinact_EnergyEsc;
   Int_t           tileact_n;
   vector<float>   *tileact_eta;
   vector<float>   *tileact_phi;
   vector<unsigned int> *tileact_ID;
   vector<float>   *tileact_EnergyTot;
   vector<float>   *tileact_EnergyVis;
   vector<unsigned int> *tileact_detector;
   vector<unsigned int> *tileact_sample;
   vector<unsigned int> *tileact_side;
   vector<unsigned int> *tileact_reg_sec;
   vector<unsigned int> *tileact_eta_tower;
   vector<unsigned int> *tileact_phi_module;
   vector<float>   *tileact_EnergyEm;
   vector<float>   *tileact_EnergyNonEm;
   vector<float>   *tileact_EnergyInv;
   vector<float>   *tileact_EnergyEsc;
   Int_t           tileinact_n;
   vector<float>   *tileinact_eta;
   vector<float>   *tileinact_phi;
   vector<unsigned int> *tileinact_ID;
   vector<float>   *tileinact_EnergyTot;
   vector<float>   *tileinact_EnergyVis;
   vector<unsigned int> *tileinact_detector;
   vector<unsigned int> *tileinact_sample;
   vector<unsigned int> *tileinact_side;
   vector<unsigned int> *tileinact_reg_sec;
   vector<unsigned int> *tileinact_eta_tower;
   vector<unsigned int> *tileinact_phi_module;
   vector<float>   *tileinact_EnergyEm;
   vector<float>   *tileinact_EnergyNonEm;
   vector<float>   *tileinact_EnergyInv;
   vector<float>   *tileinact_EnergyEsc;
   Int_t           lardm_n;
   vector<float>   *lardm_eta;
   vector<float>   *lardm_phi;
   vector<unsigned int> *lardm_ID;
   vector<float>   *lardm_EnergyTot;
   vector<float>   *lardm_EnergyVis;
   vector<unsigned int> *lardm_detzside;
   vector<unsigned int> *lardm_type;
   vector<unsigned int> *lardm_sampling;
   vector<unsigned int> *lardm_region;
   vector<unsigned int> *lardm_ieta;
   vector<unsigned int> *lardm_iphi;
   vector<float>   *lardm_EnergyEm;
   vector<float>   *lardm_EnergyNonEm;
   vector<float>   *lardm_EnergyInv;
   vector<float>   *lardm_EnergyEsc;
   Int_t           tiledm_n;
   vector<float>   *tiledm_eta;
   vector<float>   *tiledm_phi;
   vector<unsigned int> *tiledm_ID;
   vector<float>   *tiledm_EnergyTot;
   vector<float>   *tiledm_EnergyVis;
   vector<unsigned int> *tiledm_detzside;
   vector<unsigned int> *tiledm_type;
   vector<unsigned int> *tiledm_sampling;
   vector<unsigned int> *tiledm_region;
   vector<unsigned int> *tiledm_ieta;
   vector<unsigned int> *tiledm_iphi;
   vector<float>   *tiledm_EnergyEm;
   vector<float>   *tiledm_EnergyNonEm;
   vector<float>   *tiledm_EnergyInv;
   vector<float>   *tiledm_EnergyEsc;
   Int_t           mcvtx_n;
   vector<float>   *mcvtx_x;
   vector<float>   *mcvtx_y;
   vector<float>   *mcvtx_z;
   vector<int>     *mcvtx_barcode;
   Int_t           mcpart_n;
   vector<float>   *mcpart_pt;
   vector<float>   *mcpart_m;
   vector<float>   *mcpart_eta;
   vector<float>   *mcpart_phi;
   vector<int>     *mcpart_type;
   vector<int>     *mcpart_status;
   vector<int>     *mcpart_barcode;
   vector<int>     *mcpart_mothertype;
   vector<int>     *mcpart_motherbarcode;
   vector<int>     *mcpart_mcprodvtx_index;
   vector<int>     *mcpart_mother_n;
   vector<vector<int> > *mcpart_mother_index;
   vector<int>     *mcpart_mcdecayvtx_index;
   vector<int>     *mcpart_child_n;
   vector<vector<int> > *mcpart_child_index;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_timestamp;   //!
   TBranch        *b_timestamp_ns;   //!
   TBranch        *b_lbn;   //!
   TBranch        *b_bcid;   //!
   TBranch        *b_detmask0;   //!
   TBranch        *b_detmask1;   //!
   TBranch        *b_actualIntPerXing;   //!
   TBranch        *b_averageIntPerXing;   //!
   TBranch        *b_pixelFlags;   //!
   TBranch        *b_sctFlags;   //!
   TBranch        *b_trtFlags;   //!
   TBranch        *b_larFlags;   //!
   TBranch        *b_tileFlags;   //!
   TBranch        *b_muonFlags;   //!
   TBranch        *b_fwdFlags;   //!
   TBranch        *b_coreFlags;   //!
   TBranch        *b_backgroundFlags;   //!
   TBranch        *b_lumiFlags;   //!
   TBranch        *b_pixelError;   //!
   TBranch        *b_sctError;   //!
   TBranch        *b_trtError;   //!
   TBranch        *b_larError;   //!
   TBranch        *b_tileError;   //!
   TBranch        *b_muonError;   //!
   TBranch        *b_fwdError;   //!
   TBranch        *b_coreError;   //!
   TBranch        *b_streamDecision_Egamma;   //!
   TBranch        *b_streamDecision_Muons;   //!
   TBranch        *b_streamDecision_JetTauEtmiss;   //!
   TBranch        *b_l1id;   //!
   TBranch        *b_isSimulation;   //!
   TBranch        *b_isCalibration;   //!
   TBranch        *b_isTestBeam;   //!
   TBranch        *b_hitemb_n;   //!
   TBranch        *b_hitemb_eta;   //!
   TBranch        *b_hitemb_phi;   //!
   TBranch        *b_hitemb_E;   //!
   TBranch        *b_hitemb_Time;   //!
   TBranch        *b_hitemb_ID;   //!
   TBranch        *b_hitemec_n;   //!
   TBranch        *b_hitemec_eta;   //!
   TBranch        *b_hitemec_phi;   //!
   TBranch        *b_hitemec_E;   //!
   TBranch        *b_hitemec_Time;   //!
   TBranch        *b_hitemec_ID;   //!
   TBranch        *b_hithec_n;   //!
   TBranch        *b_hithec_eta;   //!
   TBranch        *b_hithec_phi;   //!
   TBranch        *b_hithec_E;   //!
   TBranch        *b_hithec_Time;   //!
   TBranch        *b_hithec_ID;   //!
   TBranch        *b_hitfcal_n;   //!
   TBranch        *b_hitfcal_eta;   //!
   TBranch        *b_hitfcal_phi;   //!
   TBranch        *b_hitfcal_E;   //!
   TBranch        *b_hitfcal_Time;   //!
   TBranch        *b_hitfcal_ID;   //!
   TBranch        *b_TileHit_n;   //!
   TBranch        *b_TileHit_energy;   //!
   TBranch        *b_TileHit_time;   //!
   TBranch        *b_TileHit_detector;   //!
   TBranch        *b_TileHit_side;   //!
   TBranch        *b_TileHit_sample;   //!
   TBranch        *b_TileHit_pmt;   //!
   TBranch        *b_TileHit_eta;   //!
   TBranch        *b_TileHit_phi;   //!
   TBranch        *b_MBTSHit_n;   //!
   TBranch        *b_MBTSHit_energy;   //!
   TBranch        *b_MBTSHit_time;   //!
   TBranch        *b_MBTSHit_detector;   //!
   TBranch        *b_MBTSHit_side;   //!
   TBranch        *b_MBTSHit_sample;   //!
   TBranch        *b_MBTSHit_pmt;   //!
   TBranch        *b_MBTSHit_eta;   //!
   TBranch        *b_MBTSHit_phi;   //!
   TBranch        *b_laract_n;   //!
   TBranch        *b_laract_eta;   //!
   TBranch        *b_laract_phi;   //!
   TBranch        *b_laract_ID;   //!
   TBranch        *b_laract_EnergyTot;   //!
   TBranch        *b_laract_EnergyVis;   //!
   TBranch        *b_laract_detector;   //!
   TBranch        *b_laract_sample;   //!
   TBranch        *b_laract_side;   //!
   TBranch        *b_laract_reg_sec;   //!
   TBranch        *b_laract_eta_tower;   //!
   TBranch        *b_laract_phi_module;   //!
   TBranch        *b_laract_EnergyEm;   //!
   TBranch        *b_laract_EnergyNonEm;   //!
   TBranch        *b_laract_EnergyInv;   //!
   TBranch        *b_laract_EnergyEsc;   //!
   TBranch        *b_larinact_n;   //!
   TBranch        *b_larinact_eta;   //!
   TBranch        *b_larinact_phi;   //!
   TBranch        *b_larinact_ID;   //!
   TBranch        *b_larinact_EnergyTot;   //!
   TBranch        *b_larinact_EnergyVis;   //!
   TBranch        *b_larinact_detector;   //!
   TBranch        *b_larinact_sample;   //!
   TBranch        *b_larinact_side;   //!
   TBranch        *b_larinact_reg_sec;   //!
   TBranch        *b_larinact_eta_tower;   //!
   TBranch        *b_larinact_phi_module;   //!
   TBranch        *b_larinact_EnergyEm;   //!
   TBranch        *b_larinact_EnergyNonEm;   //!
   TBranch        *b_larinact_EnergyInv;   //!
   TBranch        *b_larinact_EnergyEsc;   //!
   TBranch        *b_tileact_n;   //!
   TBranch        *b_tileact_eta;   //!
   TBranch        *b_tileact_phi;   //!
   TBranch        *b_tileact_ID;   //!
   TBranch        *b_tileact_EnergyTot;   //!
   TBranch        *b_tileact_EnergyVis;   //!
   TBranch        *b_tileact_detector;   //!
   TBranch        *b_tileact_sample;   //!
   TBranch        *b_tileact_side;   //!
   TBranch        *b_tileact_reg_sec;   //!
   TBranch        *b_tileact_eta_tower;   //!
   TBranch        *b_tileact_phi_module;   //!
   TBranch        *b_tileact_EnergyEm;   //!
   TBranch        *b_tileact_EnergyNonEm;   //!
   TBranch        *b_tileact_EnergyInv;   //!
   TBranch        *b_tileact_EnergyEsc;   //!
   TBranch        *b_tileinact_n;   //!
   TBranch        *b_tileinact_eta;   //!
   TBranch        *b_tileinact_phi;   //!
   TBranch        *b_tileinact_ID;   //!
   TBranch        *b_tileinact_EnergyTot;   //!
   TBranch        *b_tileinact_EnergyVis;   //!
   TBranch        *b_tileinact_detector;   //!
   TBranch        *b_tileinact_sample;   //!
   TBranch        *b_tileinact_side;   //!
   TBranch        *b_tileinact_reg_sec;   //!
   TBranch        *b_tileinact_eta_tower;   //!
   TBranch        *b_tileinact_phi_module;   //!
   TBranch        *b_tileinact_EnergyEm;   //!
   TBranch        *b_tileinact_EnergyNonEm;   //!
   TBranch        *b_tileinact_EnergyInv;   //!
   TBranch        *b_tileinact_EnergyEsc;   //!
   TBranch        *b_lardm_n;   //!
   TBranch        *b_lardm_eta;   //!
   TBranch        *b_lardm_phi;   //!
   TBranch        *b_lardm_ID;   //!
   TBranch        *b_lardm_EnergyTot;   //!
   TBranch        *b_lardm_EnergyVis;   //!
   TBranch        *b_lardm_detzside;   //!
   TBranch        *b_lardm_type;   //!
   TBranch        *b_lardm_sampling;   //!
   TBranch        *b_lardm_region;   //!
   TBranch        *b_lardm_ieta;   //!
   TBranch        *b_lardm_iphi;   //!
   TBranch        *b_lardm_EnergyEm;   //!
   TBranch        *b_lardm_EnergyNonEm;   //!
   TBranch        *b_lardm_EnergyInv;   //!
   TBranch        *b_lardm_EnergyEsc;   //!
   TBranch        *b_tiledm_n;   //!
   TBranch        *b_tiledm_eta;   //!
   TBranch        *b_tiledm_phi;   //!
   TBranch        *b_tiledm_ID;   //!
   TBranch        *b_tiledm_EnergyTot;   //!
   TBranch        *b_tiledm_EnergyVis;   //!
   TBranch        *b_tiledm_detzside;   //!
   TBranch        *b_tiledm_type;   //!
   TBranch        *b_tiledm_sampling;   //!
   TBranch        *b_tiledm_region;   //!
   TBranch        *b_tiledm_ieta;   //!
   TBranch        *b_tiledm_iphi;   //!
   TBranch        *b_tiledm_EnergyEm;   //!
   TBranch        *b_tiledm_EnergyNonEm;   //!
   TBranch        *b_tiledm_EnergyInv;   //!
   TBranch        *b_tiledm_EnergyEsc;   //!
   TBranch        *b_mcvtx_n;   //!
   TBranch        *b_mcvtx_x;   //!
   TBranch        *b_mcvtx_y;   //!
   TBranch        *b_mcvtx_z;   //!
   TBranch        *b_mcvtx_barcode;   //!
   TBranch        *b_mcpart_n;   //!
   TBranch        *b_mcpart_pt;   //!
   TBranch        *b_mcpart_m;   //!
   TBranch        *b_mcpart_eta;   //!
   TBranch        *b_mcpart_phi;   //!
   TBranch        *b_mcpart_type;   //!
   TBranch        *b_mcpart_status;   //!
   TBranch        *b_mcpart_barcode;   //!
   TBranch        *b_mcpart_mothertype;   //!
   TBranch        *b_mcpart_motherbarcode;   //!
   TBranch        *b_mcpart_mcprodvtx_index;   //!
   TBranch        *b_mcpart_mother_n;   //!
   TBranch        *b_mcpart_mother_index;   //!
   TBranch        *b_mcpart_mcdecayvtx_index;   //!
   TBranch        *b_mcpart_child_n;   //!
   TBranch        *b_mcpart_child_index;   //!

   ClCaloHits(TTree *tree=0);
   virtual ~ClCaloHits();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ClCaloHits_cxx
ClCaloHits::ClCaloHits(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/eos/user/k/kiryunin/HEC_SF/10.1-FTFP_BERT_ATL/DumpHits/ele_e0100_sc1/01.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/eos/user/k/kiryunin/HEC_SF/10.1-FTFP_BERT_ATL/DumpHits/ele_e0100_sc1/01.root");
      }
      f->GetObject("calohitD3PD",tree);

   }
   Init(tree);
}

ClCaloHits::~ClCaloHits()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ClCaloHits::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ClCaloHits::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void ClCaloHits::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   hitemb_eta = 0;
   hitemb_phi = 0;
   hitemb_E = 0;
   hitemb_Time = 0;
   hitemb_ID = 0;
   hitemec_eta = 0;
   hitemec_phi = 0;
   hitemec_E = 0;
   hitemec_Time = 0;
   hitemec_ID = 0;
   hithec_eta = 0;
   hithec_phi = 0;
   hithec_E = 0;
   hithec_Time = 0;
   hithec_ID = 0;
   hitfcal_eta = 0;
   hitfcal_phi = 0;
   hitfcal_E = 0;
   hitfcal_Time = 0;
   hitfcal_ID = 0;
   TileHit_energy = 0;
   TileHit_time = 0;
   TileHit_detector = 0;
   TileHit_side = 0;
   TileHit_sample = 0;
   TileHit_pmt = 0;
   TileHit_eta = 0;
   TileHit_phi = 0;
   MBTSHit_energy = 0;
   MBTSHit_time = 0;
   MBTSHit_detector = 0;
   MBTSHit_side = 0;
   MBTSHit_sample = 0;
   MBTSHit_pmt = 0;
   MBTSHit_eta = 0;
   MBTSHit_phi = 0;
   laract_eta = 0;
   laract_phi = 0;
   laract_ID = 0;
   laract_EnergyTot = 0;
   laract_EnergyVis = 0;
   laract_detector = 0;
   laract_sample = 0;
   laract_side = 0;
   laract_reg_sec = 0;
   laract_eta_tower = 0;
   laract_phi_module = 0;
   laract_EnergyEm = 0;
   laract_EnergyNonEm = 0;
   laract_EnergyInv = 0;
   laract_EnergyEsc = 0;
   larinact_eta = 0;
   larinact_phi = 0;
   larinact_ID = 0;
   larinact_EnergyTot = 0;
   larinact_EnergyVis = 0;
   larinact_detector = 0;
   larinact_sample = 0;
   larinact_side = 0;
   larinact_reg_sec = 0;
   larinact_eta_tower = 0;
   larinact_phi_module = 0;
   larinact_EnergyEm = 0;
   larinact_EnergyNonEm = 0;
   larinact_EnergyInv = 0;
   larinact_EnergyEsc = 0;
   tileact_eta = 0;
   tileact_phi = 0;
   tileact_ID = 0;
   tileact_EnergyTot = 0;
   tileact_EnergyVis = 0;
   tileact_detector = 0;
   tileact_sample = 0;
   tileact_side = 0;
   tileact_reg_sec = 0;
   tileact_eta_tower = 0;
   tileact_phi_module = 0;
   tileact_EnergyEm = 0;
   tileact_EnergyNonEm = 0;
   tileact_EnergyInv = 0;
   tileact_EnergyEsc = 0;
   tileinact_eta = 0;
   tileinact_phi = 0;
   tileinact_ID = 0;
   tileinact_EnergyTot = 0;
   tileinact_EnergyVis = 0;
   tileinact_detector = 0;
   tileinact_sample = 0;
   tileinact_side = 0;
   tileinact_reg_sec = 0;
   tileinact_eta_tower = 0;
   tileinact_phi_module = 0;
   tileinact_EnergyEm = 0;
   tileinact_EnergyNonEm = 0;
   tileinact_EnergyInv = 0;
   tileinact_EnergyEsc = 0;
   lardm_eta = 0;
   lardm_phi = 0;
   lardm_ID = 0;
   lardm_EnergyTot = 0;
   lardm_EnergyVis = 0;
   lardm_detzside = 0;
   lardm_type = 0;
   lardm_sampling = 0;
   lardm_region = 0;
   lardm_ieta = 0;
   lardm_iphi = 0;
   lardm_EnergyEm = 0;
   lardm_EnergyNonEm = 0;
   lardm_EnergyInv = 0;
   lardm_EnergyEsc = 0;
   tiledm_eta = 0;
   tiledm_phi = 0;
   tiledm_ID = 0;
   tiledm_EnergyTot = 0;
   tiledm_EnergyVis = 0;
   tiledm_detzside = 0;
   tiledm_type = 0;
   tiledm_sampling = 0;
   tiledm_region = 0;
   tiledm_ieta = 0;
   tiledm_iphi = 0;
   tiledm_EnergyEm = 0;
   tiledm_EnergyNonEm = 0;
   tiledm_EnergyInv = 0;
   tiledm_EnergyEsc = 0;
   mcvtx_x = 0;
   mcvtx_y = 0;
   mcvtx_z = 0;
   mcvtx_barcode = 0;
   mcpart_pt = 0;
   mcpart_m = 0;
   mcpart_eta = 0;
   mcpart_phi = 0;
   mcpart_type = 0;
   mcpart_status = 0;
   mcpart_barcode = 0;
   mcpart_mothertype = 0;
   mcpart_motherbarcode = 0;
   mcpart_mcprodvtx_index = 0;
   mcpart_mother_n = 0;
   mcpart_mother_index = 0;
   mcpart_mcdecayvtx_index = 0;
   mcpart_child_n = 0;
   mcpart_child_index = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("timestamp", &timestamp, &b_timestamp);
   fChain->SetBranchAddress("timestamp_ns", &timestamp_ns, &b_timestamp_ns);
   fChain->SetBranchAddress("lbn", &lbn, &b_lbn);
   fChain->SetBranchAddress("bcid", &bcid, &b_bcid);
   fChain->SetBranchAddress("detmask0", &detmask0, &b_detmask0);
   fChain->SetBranchAddress("detmask1", &detmask1, &b_detmask1);
   fChain->SetBranchAddress("actualIntPerXing", &actualIntPerXing, &b_actualIntPerXing);
   fChain->SetBranchAddress("averageIntPerXing", &averageIntPerXing, &b_averageIntPerXing);
   fChain->SetBranchAddress("pixelFlags", &pixelFlags, &b_pixelFlags);
   fChain->SetBranchAddress("sctFlags", &sctFlags, &b_sctFlags);
   fChain->SetBranchAddress("trtFlags", &trtFlags, &b_trtFlags);
   fChain->SetBranchAddress("larFlags", &larFlags, &b_larFlags);
   fChain->SetBranchAddress("tileFlags", &tileFlags, &b_tileFlags);
   fChain->SetBranchAddress("muonFlags", &muonFlags, &b_muonFlags);
   fChain->SetBranchAddress("fwdFlags", &fwdFlags, &b_fwdFlags);
   fChain->SetBranchAddress("coreFlags", &coreFlags, &b_coreFlags);
   fChain->SetBranchAddress("backgroundFlags", &backgroundFlags, &b_backgroundFlags);
   fChain->SetBranchAddress("lumiFlags", &lumiFlags, &b_lumiFlags);
   fChain->SetBranchAddress("pixelError", &pixelError, &b_pixelError);
   fChain->SetBranchAddress("sctError", &sctError, &b_sctError);
   fChain->SetBranchAddress("trtError", &trtError, &b_trtError);
   fChain->SetBranchAddress("larError", &larError, &b_larError);
   fChain->SetBranchAddress("tileError", &tileError, &b_tileError);
   fChain->SetBranchAddress("muonError", &muonError, &b_muonError);
   fChain->SetBranchAddress("fwdError", &fwdError, &b_fwdError);
   fChain->SetBranchAddress("coreError", &coreError, &b_coreError);
   fChain->SetBranchAddress("streamDecision_Egamma", &streamDecision_Egamma, &b_streamDecision_Egamma);
   fChain->SetBranchAddress("streamDecision_Muons", &streamDecision_Muons, &b_streamDecision_Muons);
   fChain->SetBranchAddress("streamDecision_JetTauEtmiss", &streamDecision_JetTauEtmiss, &b_streamDecision_JetTauEtmiss);
   fChain->SetBranchAddress("l1id", &l1id, &b_l1id);
   fChain->SetBranchAddress("isSimulation", &isSimulation, &b_isSimulation);
   fChain->SetBranchAddress("isCalibration", &isCalibration, &b_isCalibration);
   fChain->SetBranchAddress("isTestBeam", &isTestBeam, &b_isTestBeam);
   fChain->SetBranchAddress("hitemb_n", &hitemb_n, &b_hitemb_n);
   fChain->SetBranchAddress("hitemb_eta", &hitemb_eta, &b_hitemb_eta);
   fChain->SetBranchAddress("hitemb_phi", &hitemb_phi, &b_hitemb_phi);
   fChain->SetBranchAddress("hitemb_E", &hitemb_E, &b_hitemb_E);
   fChain->SetBranchAddress("hitemb_Time", &hitemb_Time, &b_hitemb_Time);
   fChain->SetBranchAddress("hitemb_ID", &hitemb_ID, &b_hitemb_ID);
   fChain->SetBranchAddress("hitemec_n", &hitemec_n, &b_hitemec_n);
   fChain->SetBranchAddress("hitemec_eta", &hitemec_eta, &b_hitemec_eta);
   fChain->SetBranchAddress("hitemec_phi", &hitemec_phi, &b_hitemec_phi);
   fChain->SetBranchAddress("hitemec_E", &hitemec_E, &b_hitemec_E);
   fChain->SetBranchAddress("hitemec_Time", &hitemec_Time, &b_hitemec_Time);
   fChain->SetBranchAddress("hitemec_ID", &hitemec_ID, &b_hitemec_ID);
   fChain->SetBranchAddress("hithec_n", &hithec_n, &b_hithec_n);
   fChain->SetBranchAddress("hithec_eta", &hithec_eta, &b_hithec_eta);
   fChain->SetBranchAddress("hithec_phi", &hithec_phi, &b_hithec_phi);
   fChain->SetBranchAddress("hithec_E", &hithec_E, &b_hithec_E);
   fChain->SetBranchAddress("hithec_Time", &hithec_Time, &b_hithec_Time);
   fChain->SetBranchAddress("hithec_ID", &hithec_ID, &b_hithec_ID);
   fChain->SetBranchAddress("hitfcal_n", &hitfcal_n, &b_hitfcal_n);
   fChain->SetBranchAddress("hitfcal_eta", &hitfcal_eta, &b_hitfcal_eta);
   fChain->SetBranchAddress("hitfcal_phi", &hitfcal_phi, &b_hitfcal_phi);
   fChain->SetBranchAddress("hitfcal_E", &hitfcal_E, &b_hitfcal_E);
   fChain->SetBranchAddress("hitfcal_Time", &hitfcal_Time, &b_hitfcal_Time);
   fChain->SetBranchAddress("hitfcal_ID", &hitfcal_ID, &b_hitfcal_ID);
   fChain->SetBranchAddress("TileHit_n", &TileHit_n, &b_TileHit_n);
   fChain->SetBranchAddress("TileHit_energy", &TileHit_energy, &b_TileHit_energy);
   fChain->SetBranchAddress("TileHit_time", &TileHit_time, &b_TileHit_time);
   fChain->SetBranchAddress("TileHit_detector", &TileHit_detector, &b_TileHit_detector);
   fChain->SetBranchAddress("TileHit_side", &TileHit_side, &b_TileHit_side);
   fChain->SetBranchAddress("TileHit_sample", &TileHit_sample, &b_TileHit_sample);
   fChain->SetBranchAddress("TileHit_pmt", &TileHit_pmt, &b_TileHit_pmt);
   fChain->SetBranchAddress("TileHit_eta", &TileHit_eta, &b_TileHit_eta);
   fChain->SetBranchAddress("TileHit_phi", &TileHit_phi, &b_TileHit_phi);
   fChain->SetBranchAddress("MBTSHit_n", &MBTSHit_n, &b_MBTSHit_n);
   fChain->SetBranchAddress("MBTSHit_energy", &MBTSHit_energy, &b_MBTSHit_energy);
   fChain->SetBranchAddress("MBTSHit_time", &MBTSHit_time, &b_MBTSHit_time);
   fChain->SetBranchAddress("MBTSHit_detector", &MBTSHit_detector, &b_MBTSHit_detector);
   fChain->SetBranchAddress("MBTSHit_side", &MBTSHit_side, &b_MBTSHit_side);
   fChain->SetBranchAddress("MBTSHit_sample", &MBTSHit_sample, &b_MBTSHit_sample);
   fChain->SetBranchAddress("MBTSHit_pmt", &MBTSHit_pmt, &b_MBTSHit_pmt);
   fChain->SetBranchAddress("MBTSHit_eta", &MBTSHit_eta, &b_MBTSHit_eta);
   fChain->SetBranchAddress("MBTSHit_phi", &MBTSHit_phi, &b_MBTSHit_phi);
   fChain->SetBranchAddress("laract_n", &laract_n, &b_laract_n);
   fChain->SetBranchAddress("laract_eta", &laract_eta, &b_laract_eta);
   fChain->SetBranchAddress("laract_phi", &laract_phi, &b_laract_phi);
   fChain->SetBranchAddress("laract_ID", &laract_ID, &b_laract_ID);
   fChain->SetBranchAddress("laract_EnergyTot", &laract_EnergyTot, &b_laract_EnergyTot);
   fChain->SetBranchAddress("laract_EnergyVis", &laract_EnergyVis, &b_laract_EnergyVis);
   fChain->SetBranchAddress("laract_detector", &laract_detector, &b_laract_detector);
   fChain->SetBranchAddress("laract_sample", &laract_sample, &b_laract_sample);
   fChain->SetBranchAddress("laract_side", &laract_side, &b_laract_side);
   fChain->SetBranchAddress("laract_reg_sec", &laract_reg_sec, &b_laract_reg_sec);
   fChain->SetBranchAddress("laract_eta_tower", &laract_eta_tower, &b_laract_eta_tower);
   fChain->SetBranchAddress("laract_phi_module", &laract_phi_module, &b_laract_phi_module);
   fChain->SetBranchAddress("laract_EnergyEm", &laract_EnergyEm, &b_laract_EnergyEm);
   fChain->SetBranchAddress("laract_EnergyNonEm", &laract_EnergyNonEm, &b_laract_EnergyNonEm);
   fChain->SetBranchAddress("laract_EnergyInv", &laract_EnergyInv, &b_laract_EnergyInv);
   fChain->SetBranchAddress("laract_EnergyEsc", &laract_EnergyEsc, &b_laract_EnergyEsc);
   fChain->SetBranchAddress("larinact_n", &larinact_n, &b_larinact_n);
   fChain->SetBranchAddress("larinact_eta", &larinact_eta, &b_larinact_eta);
   fChain->SetBranchAddress("larinact_phi", &larinact_phi, &b_larinact_phi);
   fChain->SetBranchAddress("larinact_ID", &larinact_ID, &b_larinact_ID);
   fChain->SetBranchAddress("larinact_EnergyTot", &larinact_EnergyTot, &b_larinact_EnergyTot);
   fChain->SetBranchAddress("larinact_EnergyVis", &larinact_EnergyVis, &b_larinact_EnergyVis);
   fChain->SetBranchAddress("larinact_detector", &larinact_detector, &b_larinact_detector);
   fChain->SetBranchAddress("larinact_sample", &larinact_sample, &b_larinact_sample);
   fChain->SetBranchAddress("larinact_side", &larinact_side, &b_larinact_side);
   fChain->SetBranchAddress("larinact_reg_sec", &larinact_reg_sec, &b_larinact_reg_sec);
   fChain->SetBranchAddress("larinact_eta_tower", &larinact_eta_tower, &b_larinact_eta_tower);
   fChain->SetBranchAddress("larinact_phi_module", &larinact_phi_module, &b_larinact_phi_module);
   fChain->SetBranchAddress("larinact_EnergyEm", &larinact_EnergyEm, &b_larinact_EnergyEm);
   fChain->SetBranchAddress("larinact_EnergyNonEm", &larinact_EnergyNonEm, &b_larinact_EnergyNonEm);
   fChain->SetBranchAddress("larinact_EnergyInv", &larinact_EnergyInv, &b_larinact_EnergyInv);
   fChain->SetBranchAddress("larinact_EnergyEsc", &larinact_EnergyEsc, &b_larinact_EnergyEsc);
   fChain->SetBranchAddress("tileact_n", &tileact_n, &b_tileact_n);
   fChain->SetBranchAddress("tileact_eta", &tileact_eta, &b_tileact_eta);
   fChain->SetBranchAddress("tileact_phi", &tileact_phi, &b_tileact_phi);
   fChain->SetBranchAddress("tileact_ID", &tileact_ID, &b_tileact_ID);
   fChain->SetBranchAddress("tileact_EnergyTot", &tileact_EnergyTot, &b_tileact_EnergyTot);
   fChain->SetBranchAddress("tileact_EnergyVis", &tileact_EnergyVis, &b_tileact_EnergyVis);
   fChain->SetBranchAddress("tileact_detector", &tileact_detector, &b_tileact_detector);
   fChain->SetBranchAddress("tileact_sample", &tileact_sample, &b_tileact_sample);
   fChain->SetBranchAddress("tileact_side", &tileact_side, &b_tileact_side);
   fChain->SetBranchAddress("tileact_reg_sec", &tileact_reg_sec, &b_tileact_reg_sec);
   fChain->SetBranchAddress("tileact_eta_tower", &tileact_eta_tower, &b_tileact_eta_tower);
   fChain->SetBranchAddress("tileact_phi_module", &tileact_phi_module, &b_tileact_phi_module);
   fChain->SetBranchAddress("tileact_EnergyEm", &tileact_EnergyEm, &b_tileact_EnergyEm);
   fChain->SetBranchAddress("tileact_EnergyNonEm", &tileact_EnergyNonEm, &b_tileact_EnergyNonEm);
   fChain->SetBranchAddress("tileact_EnergyInv", &tileact_EnergyInv, &b_tileact_EnergyInv);
   fChain->SetBranchAddress("tileact_EnergyEsc", &tileact_EnergyEsc, &b_tileact_EnergyEsc);
   fChain->SetBranchAddress("tileinact_n", &tileinact_n, &b_tileinact_n);
   fChain->SetBranchAddress("tileinact_eta", &tileinact_eta, &b_tileinact_eta);
   fChain->SetBranchAddress("tileinact_phi", &tileinact_phi, &b_tileinact_phi);
   fChain->SetBranchAddress("tileinact_ID", &tileinact_ID, &b_tileinact_ID);
   fChain->SetBranchAddress("tileinact_EnergyTot", &tileinact_EnergyTot, &b_tileinact_EnergyTot);
   fChain->SetBranchAddress("tileinact_EnergyVis", &tileinact_EnergyVis, &b_tileinact_EnergyVis);
   fChain->SetBranchAddress("tileinact_detector", &tileinact_detector, &b_tileinact_detector);
   fChain->SetBranchAddress("tileinact_sample", &tileinact_sample, &b_tileinact_sample);
   fChain->SetBranchAddress("tileinact_side", &tileinact_side, &b_tileinact_side);
   fChain->SetBranchAddress("tileinact_reg_sec", &tileinact_reg_sec, &b_tileinact_reg_sec);
   fChain->SetBranchAddress("tileinact_eta_tower", &tileinact_eta_tower, &b_tileinact_eta_tower);
   fChain->SetBranchAddress("tileinact_phi_module", &tileinact_phi_module, &b_tileinact_phi_module);
   fChain->SetBranchAddress("tileinact_EnergyEm", &tileinact_EnergyEm, &b_tileinact_EnergyEm);
   fChain->SetBranchAddress("tileinact_EnergyNonEm", &tileinact_EnergyNonEm, &b_tileinact_EnergyNonEm);
   fChain->SetBranchAddress("tileinact_EnergyInv", &tileinact_EnergyInv, &b_tileinact_EnergyInv);
   fChain->SetBranchAddress("tileinact_EnergyEsc", &tileinact_EnergyEsc, &b_tileinact_EnergyEsc);
   fChain->SetBranchAddress("lardm_n", &lardm_n, &b_lardm_n);
   fChain->SetBranchAddress("lardm_eta", &lardm_eta, &b_lardm_eta);
   fChain->SetBranchAddress("lardm_phi", &lardm_phi, &b_lardm_phi);
   fChain->SetBranchAddress("lardm_ID", &lardm_ID, &b_lardm_ID);
   fChain->SetBranchAddress("lardm_EnergyTot", &lardm_EnergyTot, &b_lardm_EnergyTot);
   fChain->SetBranchAddress("lardm_EnergyVis", &lardm_EnergyVis, &b_lardm_EnergyVis);
   fChain->SetBranchAddress("lardm_detzside", &lardm_detzside, &b_lardm_detzside);
   fChain->SetBranchAddress("lardm_type", &lardm_type, &b_lardm_type);
   fChain->SetBranchAddress("lardm_sampling", &lardm_sampling, &b_lardm_sampling);
   fChain->SetBranchAddress("lardm_region", &lardm_region, &b_lardm_region);
   fChain->SetBranchAddress("lardm_ieta", &lardm_ieta, &b_lardm_ieta);
   fChain->SetBranchAddress("lardm_iphi", &lardm_iphi, &b_lardm_iphi);
   fChain->SetBranchAddress("lardm_EnergyEm", &lardm_EnergyEm, &b_lardm_EnergyEm);
   fChain->SetBranchAddress("lardm_EnergyNonEm", &lardm_EnergyNonEm, &b_lardm_EnergyNonEm);
   fChain->SetBranchAddress("lardm_EnergyInv", &lardm_EnergyInv, &b_lardm_EnergyInv);
   fChain->SetBranchAddress("lardm_EnergyEsc", &lardm_EnergyEsc, &b_lardm_EnergyEsc);
   fChain->SetBranchAddress("tiledm_n", &tiledm_n, &b_tiledm_n);
   fChain->SetBranchAddress("tiledm_eta", &tiledm_eta, &b_tiledm_eta);
   fChain->SetBranchAddress("tiledm_phi", &tiledm_phi, &b_tiledm_phi);
   fChain->SetBranchAddress("tiledm_ID", &tiledm_ID, &b_tiledm_ID);
   fChain->SetBranchAddress("tiledm_EnergyTot", &tiledm_EnergyTot, &b_tiledm_EnergyTot);
   fChain->SetBranchAddress("tiledm_EnergyVis", &tiledm_EnergyVis, &b_tiledm_EnergyVis);
   fChain->SetBranchAddress("tiledm_detzside", &tiledm_detzside, &b_tiledm_detzside);
   fChain->SetBranchAddress("tiledm_type", &tiledm_type, &b_tiledm_type);
   fChain->SetBranchAddress("tiledm_sampling", &tiledm_sampling, &b_tiledm_sampling);
   fChain->SetBranchAddress("tiledm_region", &tiledm_region, &b_tiledm_region);
   fChain->SetBranchAddress("tiledm_ieta", &tiledm_ieta, &b_tiledm_ieta);
   fChain->SetBranchAddress("tiledm_iphi", &tiledm_iphi, &b_tiledm_iphi);
   fChain->SetBranchAddress("tiledm_EnergyEm", &tiledm_EnergyEm, &b_tiledm_EnergyEm);
   fChain->SetBranchAddress("tiledm_EnergyNonEm", &tiledm_EnergyNonEm, &b_tiledm_EnergyNonEm);
   fChain->SetBranchAddress("tiledm_EnergyInv", &tiledm_EnergyInv, &b_tiledm_EnergyInv);
   fChain->SetBranchAddress("tiledm_EnergyEsc", &tiledm_EnergyEsc, &b_tiledm_EnergyEsc);
   fChain->SetBranchAddress("mcvtx_n", &mcvtx_n, &b_mcvtx_n);
   fChain->SetBranchAddress("mcvtx_x", &mcvtx_x, &b_mcvtx_x);
   fChain->SetBranchAddress("mcvtx_y", &mcvtx_y, &b_mcvtx_y);
   fChain->SetBranchAddress("mcvtx_z", &mcvtx_z, &b_mcvtx_z);
   fChain->SetBranchAddress("mcvtx_barcode", &mcvtx_barcode, &b_mcvtx_barcode);
   fChain->SetBranchAddress("mcpart_n", &mcpart_n, &b_mcpart_n);
   fChain->SetBranchAddress("mcpart_pt", &mcpart_pt, &b_mcpart_pt);
   fChain->SetBranchAddress("mcpart_m", &mcpart_m, &b_mcpart_m);
   fChain->SetBranchAddress("mcpart_eta", &mcpart_eta, &b_mcpart_eta);
   fChain->SetBranchAddress("mcpart_phi", &mcpart_phi, &b_mcpart_phi);
   fChain->SetBranchAddress("mcpart_type", &mcpart_type, &b_mcpart_type);
   fChain->SetBranchAddress("mcpart_status", &mcpart_status, &b_mcpart_status);
   fChain->SetBranchAddress("mcpart_barcode", &mcpart_barcode, &b_mcpart_barcode);
   fChain->SetBranchAddress("mcpart_mothertype", &mcpart_mothertype, &b_mcpart_mothertype);
   fChain->SetBranchAddress("mcpart_motherbarcode", &mcpart_motherbarcode, &b_mcpart_motherbarcode);
   fChain->SetBranchAddress("mcpart_mcprodvtx_index", &mcpart_mcprodvtx_index, &b_mcpart_mcprodvtx_index);
   fChain->SetBranchAddress("mcpart_mother_n", &mcpart_mother_n, &b_mcpart_mother_n);
   fChain->SetBranchAddress("mcpart_mother_index", &mcpart_mother_index, &b_mcpart_mother_index);
   fChain->SetBranchAddress("mcpart_mcdecayvtx_index", &mcpart_mcdecayvtx_index, &b_mcpart_mcdecayvtx_index);
   fChain->SetBranchAddress("mcpart_child_n", &mcpart_child_n, &b_mcpart_child_n);
   fChain->SetBranchAddress("mcpart_child_index", &mcpart_child_index, &b_mcpart_child_index);
   Notify();
}

Bool_t ClCaloHits::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ClCaloHits::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ClCaloHits::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ClCaloHits_cxx
