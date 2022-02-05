/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Oct 24 14:42:24 2012 by ROOT version 5.32/02
// from TTree caloD3PD/caloD3PD
// found on file: caloD3PD_from_esd_timing_test.root
//////////////////////////////////////////////////////////

#ifndef CaloD3PDClass_h
#define CaloD3PDClass_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>


// Fixed size dimensions of array or collections stored in the TTree if any.

class CaloD3PDClass {
public :
   template <class T>
   using vector = std::vector<T>;

   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          RunNumber;
   UInt_t          EventNumber;
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
   Bool_t          isSimulation;
   Bool_t          isCalibration;
   Bool_t          isTestBeam;
   Int_t           cc_sel_n;
   vector<float>   *cc_sel_E;
   vector<float>   *cc_sel_pt;
   vector<float>   *cc_sel_eta;
   vector<float>   *cc_sel_phi;
   vector<int>     *cc_sel_QCells;
   vector<int>     *cc_sel_GainCells;
   vector<int>     *cc_sel_DetCells;
   vector<float>   *cc_sel_TimeCells;
   vector<float>   *cc_sel_xCells;
   vector<float>   *cc_sel_yCells;
   vector<float>   *cc_sel_zCells;
   vector<int>     *cc_sel_BadCells;
   vector<unsigned int> *cc_sel_IdCells;
   vector<float>   *cc_sel_Sigma;
   Int_t           ccc_Nh_Calo;
   Float_t         ccc_Eh_Calo;
   Int_t           ccc_Nh_EM;
   Float_t         ccc_Eh_EM;
   Int_t           ccc_Nh_HAD;
   Float_t         ccc_Eh_HAD;
   Int_t           ccc_Nh_PresB;
   vector<float>   *ccc_Eh_PresB;
   Int_t           ccc_Nh_EMB;
   vector<float>   *ccc_Eh_EMB;
   Int_t           ccc_Nh_EMEC;
   vector<float>   *ccc_Eh_EMEC;
   Int_t           ccc_Nh_Tile;
   vector<float>   *ccc_Eh_Tile;
   Int_t           ccc_Nh_TileGap;
   vector<float>   *ccc_Eh_TileGap;
   Int_t           ccc_Nh_HEC;
   vector<float>   *ccc_Eh_HEC;
   Int_t           ccc_Nh_FCal;
   vector<float>   *ccc_Eh_FCal;
   Int_t           ccc_Nh_PresE;
   vector<float>   *ccc_Eh_PresE;
   Int_t           ccc_Nh_Scint;
   vector<float>   *ccc_Eh_Scint;
   Int_t           cl_n;
   vector<float>   *cl_pt;
   vector<float>   *cl_eta;
   vector<float>   *cl_phi;
   vector<float>   *cl_E_em;
   vector<float>   *cl_E_had;
   vector<float>   *cl_firstEdens;
   vector<float>   *cl_cellmaxfrac;
   vector<float>   *cl_longitudinal;
   vector<float>   *cl_secondlambda;
   vector<float>   *cl_lateral;
   vector<float>   *cl_secondR;
   vector<float>   *cl_centerlambda;
   vector<float>   *cl_deltaTheta;
   vector<float>   *cl_deltaPhi;
   vector<float>   *cl_centermag;
   vector<float>   *cl_time;
   vector<float>   *cl_E_PreSamplerB;
   vector<float>   *cl_E_EMB1;
   vector<float>   *cl_E_EMB2;
   vector<float>   *cl_E_EMB3;
   vector<float>   *cl_E_PreSamplerE;
   vector<float>   *cl_E_EME1;
   vector<float>   *cl_E_EME2;
   vector<float>   *cl_E_EME3;
   vector<float>   *cl_E_HEC0;
   vector<float>   *cl_E_HEC1;
   vector<float>   *cl_E_HEC2;
   vector<float>   *cl_E_HEC3;
   vector<float>   *cl_E_TileBar0;
   vector<float>   *cl_E_TileBar1;
   vector<float>   *cl_E_TileBar2;
   vector<float>   *cl_E_TileGap1;
   vector<float>   *cl_E_TileGap2;
   vector<float>   *cl_E_TileGap3;
   vector<float>   *cl_E_TileExt0;
   vector<float>   *cl_E_TileExt1;
   vector<float>   *cl_E_TileExt2;
   vector<float>   *cl_E_FCAL0;
   vector<float>   *cl_E_FCAL1;
   vector<float>   *cl_E_FCAL2;
   vector<float>   *cl_eta_PreSamplerB;
   vector<float>   *cl_phi_PreSamplerB;
   vector<float>   *cl_eta_EMB1;
   vector<float>   *cl_phi_EMB1;
   vector<float>   *cl_eta_EMB2;
   vector<float>   *cl_phi_EMB2;
   vector<float>   *cl_eta_EMB3;
   vector<float>   *cl_phi_EMB3;
   vector<float>   *cl_eta_PreSamplerE;
   vector<float>   *cl_phi_PreSamplerE;
   vector<float>   *cl_eta_EME1;
   vector<float>   *cl_phi_EME1;
   vector<float>   *cl_eta_EME2;
   vector<float>   *cl_phi_EME2;
   vector<float>   *cl_eta_EME3;
   vector<float>   *cl_phi_EME3;
   vector<float>   *cl_eta_HEC0;
   vector<float>   *cl_phi_HEC0;
   vector<float>   *cl_eta_HEC1;
   vector<float>   *cl_phi_HEC1;
   vector<float>   *cl_eta_HEC2;
   vector<float>   *cl_phi_HEC2;
   vector<float>   *cl_eta_HEC3;
   vector<float>   *cl_phi_HEC3;
   vector<float>   *cl_eta_TileBar0;
   vector<float>   *cl_phi_TileBar0;
   vector<float>   *cl_eta_TileBar1;
   vector<float>   *cl_phi_TileBar1;
   vector<float>   *cl_eta_TileBar2;
   vector<float>   *cl_phi_TileBar2;
   vector<float>   *cl_eta_TileGap1;
   vector<float>   *cl_phi_TileGap1;
   vector<float>   *cl_eta_TileGap2;
   vector<float>   *cl_phi_TileGap2;
   vector<float>   *cl_eta_TileGap3;
   vector<float>   *cl_phi_TileGap3;
   vector<float>   *cl_eta_TileExt0;
   vector<float>   *cl_phi_TileExt0;
   vector<float>   *cl_eta_TileExt1;
   vector<float>   *cl_phi_TileExt1;
   vector<float>   *cl_eta_TileExt2;
   vector<float>   *cl_phi_TileExt2;
   vector<float>   *cl_eta_FCAL0;
   vector<float>   *cl_phi_FCAL0;
   vector<float>   *cl_eta_FCAL1;
   vector<float>   *cl_phi_FCAL1;
   vector<float>   *cl_eta_FCAL2;
   vector<float>   *cl_phi_FCAL2;
   vector<int>     *cl_cell_n;
   vector<vector<float> > *cl_cell_E;
   vector<vector<float> > *cl_cell_pt;
   vector<vector<float> > *cl_cell_eta;
   vector<vector<float> > *cl_cell_phi;
   vector<vector<int> > *cl_cell_QCells;
   vector<vector<int> > *cl_cell_GainCells;
   vector<vector<int> > *cl_cell_DetCells;
   vector<vector<float> > *cl_cell_TimeCells;
   vector<vector<float> > *cl_cell_xCells;
   vector<vector<float> > *cl_cell_yCells;
   vector<vector<float> > *cl_cell_zCells;
   vector<vector<int> > *cl_cell_BadCells;
   vector<vector<unsigned int> > *cl_cell_IdCells;
   Int_t           lardigit_n;
   vector<int>     *lardigit_layer;
   vector<int>     *lardigit_ieta;
   vector<int>     *lardigit_calo;
   vector<int>     *lardigit_iphi;
   vector<int>     *lardigit_barrel_ec;
   vector<int>     *lardigit_pos_neg;
   vector<int>     *lardigit_FT;
   vector<int>     *lardigit_slot;
   vector<int>     *lardigit_channel;
   vector<int>     *lardigit_gain;
   vector<vector<int> > *lardigit_Samples;
   vector<unsigned int> *lardigit_offlineId;
   vector<vector<unsigned int> > *lardigit_sca_add;
   vector<int>     *lardigit_iter_valid;
   vector<int>     *lardigit_iter_converged;
   vector<short>   *lardigit_iter_nIter;
   vector<float>   *lardigit_iter_ampl;
   vector<float>   *lardigit_iter_tau;
   vector<short>   *lardigit_iter_peakSample;
   vector<float>   *lardigit_iter_delay;
   vector<float>   *lardigit_iter_qual;
   vector<short>   *lardigit_iter_index;

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
   TBranch        *b_isSimulation;   //!
   TBranch        *b_isCalibration;   //!
   TBranch        *b_isTestBeam;   //!
   TBranch        *b_cc_sel_n;   //!
   TBranch        *b_cc_sel_E;   //!
   TBranch        *b_cc_sel_pt;   //!
   TBranch        *b_cc_sel_eta;   //!
   TBranch        *b_cc_sel_phi;   //!
   TBranch        *b_cc_sel_QCells;   //!
   TBranch        *b_cc_sel_GainCells;   //!
   TBranch        *b_cc_sel_DetCells;   //!
   TBranch        *b_cc_sel_TimeCells;   //!
   TBranch        *b_cc_sel_xCells;   //!
   TBranch        *b_cc_sel_yCells;   //!
   TBranch        *b_cc_sel_zCells;   //!
   TBranch        *b_cc_sel_BadCells;   //!
   TBranch        *b_cc_sel_IdCells;   //!
   TBranch        *b_cc_sel_Sigma;   //!
   TBranch        *b_ccc_Nh_Calo;   //!
   TBranch        *b_ccc_Eh_Calo;   //!
   TBranch        *b_ccc_Nh_EM;   //!
   TBranch        *b_ccc_Eh_EM;   //!
   TBranch        *b_ccc_Nh_HAD;   //!
   TBranch        *b_ccc_Eh_HAD;   //!
   TBranch        *b_ccc_Nh_PresB;   //!
   TBranch        *b_ccc_Eh_PresB;   //!
   TBranch        *b_ccc_Nh_EMB;   //!
   TBranch        *b_ccc_Eh_EMB;   //!
   TBranch        *b_ccc_Nh_EMEC;   //!
   TBranch        *b_ccc_Eh_EMEC;   //!
   TBranch        *b_ccc_Nh_Tile;   //!
   TBranch        *b_ccc_Eh_Tile;   //!
   TBranch        *b_ccc_Nh_TileGap;   //!
   TBranch        *b_ccc_Eh_TileGap;   //!
   TBranch        *b_ccc_Nh_HEC;   //!
   TBranch        *b_ccc_Eh_HEC;   //!
   TBranch        *b_ccc_Nh_FCal;   //!
   TBranch        *b_ccc_Eh_FCal;   //!
   TBranch        *b_ccc_Nh_PresE;   //!
   TBranch        *b_ccc_Eh_PresE;   //!
   TBranch        *b_ccc_Nh_Scint;   //!
   TBranch        *b_ccc_Eh_Scint;   //!
   TBranch        *b_cl_n;   //!
   TBranch        *b_cl_pt;   //!
   TBranch        *b_cl_eta;   //!
   TBranch        *b_cl_phi;   //!
   TBranch        *b_cl_E_em;   //!
   TBranch        *b_cl_E_had;   //!
   TBranch        *b_cl_firstEdens;   //!
   TBranch        *b_cl_cellmaxfrac;   //!
   TBranch        *b_cl_longitudinal;   //!
   TBranch        *b_cl_secondlambda;   //!
   TBranch        *b_cl_lateral;   //!
   TBranch        *b_cl_secondR;   //!
   TBranch        *b_cl_centerlambda;   //!
   TBranch        *b_cl_deltaTheta;   //!
   TBranch        *b_cl_deltaPhi;   //!
   TBranch        *b_cl_centermag;   //!
   TBranch        *b_cl_time;   //!
   TBranch        *b_cl_E_PreSamplerB;   //!
   TBranch        *b_cl_E_EMB1;   //!
   TBranch        *b_cl_E_EMB2;   //!
   TBranch        *b_cl_E_EMB3;   //!
   TBranch        *b_cl_E_PreSamplerE;   //!
   TBranch        *b_cl_E_EME1;   //!
   TBranch        *b_cl_E_EME2;   //!
   TBranch        *b_cl_E_EME3;   //!
   TBranch        *b_cl_E_HEC0;   //!
   TBranch        *b_cl_E_HEC1;   //!
   TBranch        *b_cl_E_HEC2;   //!
   TBranch        *b_cl_E_HEC3;   //!
   TBranch        *b_cl_E_TileBar0;   //!
   TBranch        *b_cl_E_TileBar1;   //!
   TBranch        *b_cl_E_TileBar2;   //!
   TBranch        *b_cl_E_TileGap1;   //!
   TBranch        *b_cl_E_TileGap2;   //!
   TBranch        *b_cl_E_TileGap3;   //!
   TBranch        *b_cl_E_TileExt0;   //!
   TBranch        *b_cl_E_TileExt1;   //!
   TBranch        *b_cl_E_TileExt2;   //!
   TBranch        *b_cl_E_FCAL0;   //!
   TBranch        *b_cl_E_FCAL1;   //!
   TBranch        *b_cl_E_FCAL2;   //!
   TBranch        *b_cl_eta_PreSamplerB;   //!
   TBranch        *b_cl_phi_PreSamplerB;   //!
   TBranch        *b_cl_eta_EMB1;   //!
   TBranch        *b_cl_phi_EMB1;   //!
   TBranch        *b_cl_eta_EMB2;   //!
   TBranch        *b_cl_phi_EMB2;   //!
   TBranch        *b_cl_eta_EMB3;   //!
   TBranch        *b_cl_phi_EMB3;   //!
   TBranch        *b_cl_eta_PreSamplerE;   //!
   TBranch        *b_cl_phi_PreSamplerE;   //!
   TBranch        *b_cl_eta_EME1;   //!
   TBranch        *b_cl_phi_EME1;   //!
   TBranch        *b_cl_eta_EME2;   //!
   TBranch        *b_cl_phi_EME2;   //!
   TBranch        *b_cl_eta_EME3;   //!
   TBranch        *b_cl_phi_EME3;   //!
   TBranch        *b_cl_eta_HEC0;   //!
   TBranch        *b_cl_phi_HEC0;   //!
   TBranch        *b_cl_eta_HEC1;   //!
   TBranch        *b_cl_phi_HEC1;   //!
   TBranch        *b_cl_eta_HEC2;   //!
   TBranch        *b_cl_phi_HEC2;   //!
   TBranch        *b_cl_eta_HEC3;   //!
   TBranch        *b_cl_phi_HEC3;   //!
   TBranch        *b_cl_eta_TileBar0;   //!
   TBranch        *b_cl_phi_TileBar0;   //!
   TBranch        *b_cl_eta_TileBar1;   //!
   TBranch        *b_cl_phi_TileBar1;   //!
   TBranch        *b_cl_eta_TileBar2;   //!
   TBranch        *b_cl_phi_TileBar2;   //!
   TBranch        *b_cl_eta_TileGap1;   //!
   TBranch        *b_cl_phi_TileGap1;   //!
   TBranch        *b_cl_eta_TileGap2;   //!
   TBranch        *b_cl_phi_TileGap2;   //!
   TBranch        *b_cl_eta_TileGap3;   //!
   TBranch        *b_cl_phi_TileGap3;   //!
   TBranch        *b_cl_eta_TileExt0;   //!
   TBranch        *b_cl_phi_TileExt0;   //!
   TBranch        *b_cl_eta_TileExt1;   //!
   TBranch        *b_cl_phi_TileExt1;   //!
   TBranch        *b_cl_eta_TileExt2;   //!
   TBranch        *b_cl_phi_TileExt2;   //!
   TBranch        *b_cl_eta_FCAL0;   //!
   TBranch        *b_cl_phi_FCAL0;   //!
   TBranch        *b_cl_eta_FCAL1;   //!
   TBranch        *b_cl_phi_FCAL1;   //!
   TBranch        *b_cl_eta_FCAL2;   //!
   TBranch        *b_cl_phi_FCAL2;   //!
   TBranch        *b_cl_cell_n;   //!
   TBranch        *b_cl_cell_E;   //!
   TBranch        *b_cl_cell_pt;   //!
   TBranch        *b_cl_cell_eta;   //!
   TBranch        *b_cl_cell_phi;   //!
   TBranch        *b_cl_cell_QCells;   //!
   TBranch        *b_cl_cell_GainCells;   //!
   TBranch        *b_cl_cell_DetCells;   //!
   TBranch        *b_cl_cell_TimeCells;   //!
   TBranch        *b_cl_cell_xCells;   //!
   TBranch        *b_cl_cell_yCells;   //!
   TBranch        *b_cl_cell_zCells;   //!
   TBranch        *b_cl_cell_BadCells;   //!
   TBranch        *b_cl_cell_IdCells;   //!
   TBranch        *b_lardigit_n;   //!
   TBranch        *b_lardigit_layer;   //!
   TBranch        *b_lardigit_ieta;   //!
   TBranch        *b_lardigit_calo;   //!
   TBranch        *b_lardigit_iphi;   //!
   TBranch        *b_lardigit_barrel_ec;   //!
   TBranch        *b_lardigit_pos_neg;   //!
   TBranch        *b_lardigit_FT;   //!
   TBranch        *b_lardigit_slot;   //!
   TBranch        *b_lardigit_channel;   //!
   TBranch        *b_lardigit_gain;   //!
   TBranch        *b_lardigit_Samples;   //!
   TBranch        *b_lardigit_offlineId;   //!
   TBranch        *b_lardigit_sca_add;   //!
   TBranch        *b_lardigit_iter_valid;   //!
   TBranch        *b_lardigit_iter_converged;   //!
   TBranch        *b_lardigit_iter_nIter;   //!
   TBranch        *b_lardigit_iter_ampl;   //!
   TBranch        *b_lardigit_iter_tau;   //!
   TBranch        *b_lardigit_iter_peakSample;   //!
   TBranch        *b_lardigit_iter_delay;   //!
   TBranch        *b_lardigit_iter_qual;   //!
   TBranch        *b_lardigit_iter_index;   //!

   CaloD3PDClass(TTree *tree=0);
   virtual ~CaloD3PDClass();
   virtual Int_t    Cut(/*Long64_t entry*/);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef CaloD3PDClass_cxx
CaloD3PDClass::CaloD3PDClass(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("caloD3PD_from_esd_timing_test.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("caloD3PD_from_esd_timing_test.root");
      }
      f->GetObject("caloD3PD",tree);

   }
   CaloD3PDClass::Init(tree);
}

CaloD3PDClass::~CaloD3PDClass()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t CaloD3PDClass::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t CaloD3PDClass::LoadTree(Long64_t entry)
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

void CaloD3PDClass::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   cc_sel_E = 0;
   cc_sel_pt = 0;
   cc_sel_eta = 0;
   cc_sel_phi = 0;
   cc_sel_QCells = 0;
   cc_sel_GainCells = 0;
   cc_sel_DetCells = 0;
   cc_sel_TimeCells = 0;
   cc_sel_xCells = 0;
   cc_sel_yCells = 0;
   cc_sel_zCells = 0;
   cc_sel_BadCells = 0;
   cc_sel_IdCells = 0;
   cc_sel_Sigma = 0;
   ccc_Eh_PresB = 0;
   ccc_Eh_EMB = 0;
   ccc_Eh_EMEC = 0;
   ccc_Eh_Tile = 0;
   ccc_Eh_TileGap = 0;
   ccc_Eh_HEC = 0;
   ccc_Eh_FCal = 0;
   ccc_Eh_PresE = 0;
   ccc_Eh_Scint = 0;
   cl_pt = 0;
   cl_eta = 0;
   cl_phi = 0;
   cl_E_em = 0;
   cl_E_had = 0;
   cl_firstEdens = 0;
   cl_cellmaxfrac = 0;
   cl_longitudinal = 0;
   cl_secondlambda = 0;
   cl_lateral = 0;
   cl_secondR = 0;
   cl_centerlambda = 0;
   cl_deltaTheta = 0;
   cl_deltaPhi = 0;
   cl_centermag = 0;
   cl_time = 0;
   cl_E_PreSamplerB = 0;
   cl_E_EMB1 = 0;
   cl_E_EMB2 = 0;
   cl_E_EMB3 = 0;
   cl_E_PreSamplerE = 0;
   cl_E_EME1 = 0;
   cl_E_EME2 = 0;
   cl_E_EME3 = 0;
   cl_E_HEC0 = 0;
   cl_E_HEC1 = 0;
   cl_E_HEC2 = 0;
   cl_E_HEC3 = 0;
   cl_E_TileBar0 = 0;
   cl_E_TileBar1 = 0;
   cl_E_TileBar2 = 0;
   cl_E_TileGap1 = 0;
   cl_E_TileGap2 = 0;
   cl_E_TileGap3 = 0;
   cl_E_TileExt0 = 0;
   cl_E_TileExt1 = 0;
   cl_E_TileExt2 = 0;
   cl_E_FCAL0 = 0;
   cl_E_FCAL1 = 0;
   cl_E_FCAL2 = 0;
   cl_eta_PreSamplerB = 0;
   cl_phi_PreSamplerB = 0;
   cl_eta_EMB1 = 0;
   cl_phi_EMB1 = 0;
   cl_eta_EMB2 = 0;
   cl_phi_EMB2 = 0;
   cl_eta_EMB3 = 0;
   cl_phi_EMB3 = 0;
   cl_eta_PreSamplerE = 0;
   cl_phi_PreSamplerE = 0;
   cl_eta_EME1 = 0;
   cl_phi_EME1 = 0;
   cl_eta_EME2 = 0;
   cl_phi_EME2 = 0;
   cl_eta_EME3 = 0;
   cl_phi_EME3 = 0;
   cl_eta_HEC0 = 0;
   cl_phi_HEC0 = 0;
   cl_eta_HEC1 = 0;
   cl_phi_HEC1 = 0;
   cl_eta_HEC2 = 0;
   cl_phi_HEC2 = 0;
   cl_eta_HEC3 = 0;
   cl_phi_HEC3 = 0;
   cl_eta_TileBar0 = 0;
   cl_phi_TileBar0 = 0;
   cl_eta_TileBar1 = 0;
   cl_phi_TileBar1 = 0;
   cl_eta_TileBar2 = 0;
   cl_phi_TileBar2 = 0;
   cl_eta_TileGap1 = 0;
   cl_phi_TileGap1 = 0;
   cl_eta_TileGap2 = 0;
   cl_phi_TileGap2 = 0;
   cl_eta_TileGap3 = 0;
   cl_phi_TileGap3 = 0;
   cl_eta_TileExt0 = 0;
   cl_phi_TileExt0 = 0;
   cl_eta_TileExt1 = 0;
   cl_phi_TileExt1 = 0;
   cl_eta_TileExt2 = 0;
   cl_phi_TileExt2 = 0;
   cl_eta_FCAL0 = 0;
   cl_phi_FCAL0 = 0;
   cl_eta_FCAL1 = 0;
   cl_phi_FCAL1 = 0;
   cl_eta_FCAL2 = 0;
   cl_phi_FCAL2 = 0;
   cl_cell_n = 0;
   cl_cell_E = 0;
   cl_cell_pt = 0;
   cl_cell_eta = 0;
   cl_cell_phi = 0;
   cl_cell_QCells = 0;
   cl_cell_GainCells = 0;
   cl_cell_DetCells = 0;
   cl_cell_TimeCells = 0;
   cl_cell_xCells = 0;
   cl_cell_yCells = 0;
   cl_cell_zCells = 0;
   cl_cell_BadCells = 0;
   cl_cell_IdCells = 0;
   lardigit_layer = 0;
   lardigit_ieta = 0;
   lardigit_calo = 0;
   lardigit_iphi = 0;
   lardigit_barrel_ec = 0;
   lardigit_pos_neg = 0;
   lardigit_FT = 0;
   lardigit_slot = 0;
   lardigit_channel = 0;
   lardigit_gain = 0;
   lardigit_Samples = 0;
   lardigit_offlineId = 0;
   lardigit_sca_add = 0;
   lardigit_iter_valid = 0;
   lardigit_iter_converged = 0;
   lardigit_iter_nIter = 0;
   lardigit_iter_ampl = 0;
   lardigit_iter_tau = 0;
   lardigit_iter_peakSample = 0;
   lardigit_iter_delay = 0;
   lardigit_iter_qual = 0;
   lardigit_iter_index = 0;
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
   fChain->SetBranchAddress("isSimulation", &isSimulation, &b_isSimulation);
   fChain->SetBranchAddress("isCalibration", &isCalibration, &b_isCalibration);
   fChain->SetBranchAddress("isTestBeam", &isTestBeam, &b_isTestBeam);
   fChain->SetBranchAddress("cc_sel_n", &cc_sel_n, &b_cc_sel_n);
   fChain->SetBranchAddress("cc_sel_E", &cc_sel_E, &b_cc_sel_E);
   fChain->SetBranchAddress("cc_sel_pt", &cc_sel_pt, &b_cc_sel_pt);
   fChain->SetBranchAddress("cc_sel_eta", &cc_sel_eta, &b_cc_sel_eta);
   fChain->SetBranchAddress("cc_sel_phi", &cc_sel_phi, &b_cc_sel_phi);
   fChain->SetBranchAddress("cc_sel_QCells", &cc_sel_QCells, &b_cc_sel_QCells);
   fChain->SetBranchAddress("cc_sel_GainCells", &cc_sel_GainCells, &b_cc_sel_GainCells);
   fChain->SetBranchAddress("cc_sel_DetCells", &cc_sel_DetCells, &b_cc_sel_DetCells);
   fChain->SetBranchAddress("cc_sel_TimeCells", &cc_sel_TimeCells, &b_cc_sel_TimeCells);
   fChain->SetBranchAddress("cc_sel_xCells", &cc_sel_xCells, &b_cc_sel_xCells);
   fChain->SetBranchAddress("cc_sel_yCells", &cc_sel_yCells, &b_cc_sel_yCells);
   fChain->SetBranchAddress("cc_sel_zCells", &cc_sel_zCells, &b_cc_sel_zCells);
   fChain->SetBranchAddress("cc_sel_BadCells", &cc_sel_BadCells, &b_cc_sel_BadCells);
   fChain->SetBranchAddress("cc_sel_IdCells", &cc_sel_IdCells, &b_cc_sel_IdCells);
   fChain->SetBranchAddress("cc_sel_Sigma", &cc_sel_Sigma, &b_cc_sel_Sigma);
   fChain->SetBranchAddress("ccc_Nh_Calo", &ccc_Nh_Calo, &b_ccc_Nh_Calo);
   fChain->SetBranchAddress("ccc_Eh_Calo", &ccc_Eh_Calo, &b_ccc_Eh_Calo);
   fChain->SetBranchAddress("ccc_Nh_EM", &ccc_Nh_EM, &b_ccc_Nh_EM);
   fChain->SetBranchAddress("ccc_Eh_EM", &ccc_Eh_EM, &b_ccc_Eh_EM);
   fChain->SetBranchAddress("ccc_Nh_HAD", &ccc_Nh_HAD, &b_ccc_Nh_HAD);
   fChain->SetBranchAddress("ccc_Eh_HAD", &ccc_Eh_HAD, &b_ccc_Eh_HAD);
   fChain->SetBranchAddress("ccc_Nh_PresB", &ccc_Nh_PresB, &b_ccc_Nh_PresB);
   fChain->SetBranchAddress("ccc_Eh_PresB", &ccc_Eh_PresB, &b_ccc_Eh_PresB);
   fChain->SetBranchAddress("ccc_Nh_EMB", &ccc_Nh_EMB, &b_ccc_Nh_EMB);
   fChain->SetBranchAddress("ccc_Eh_EMB", &ccc_Eh_EMB, &b_ccc_Eh_EMB);
   fChain->SetBranchAddress("ccc_Nh_EMEC", &ccc_Nh_EMEC, &b_ccc_Nh_EMEC);
   fChain->SetBranchAddress("ccc_Eh_EMEC", &ccc_Eh_EMEC, &b_ccc_Eh_EMEC);
   fChain->SetBranchAddress("ccc_Nh_Tile", &ccc_Nh_Tile, &b_ccc_Nh_Tile);
   fChain->SetBranchAddress("ccc_Eh_Tile", &ccc_Eh_Tile, &b_ccc_Eh_Tile);
   fChain->SetBranchAddress("ccc_Nh_TileGap", &ccc_Nh_TileGap, &b_ccc_Nh_TileGap);
   fChain->SetBranchAddress("ccc_Eh_TileGap", &ccc_Eh_TileGap, &b_ccc_Eh_TileGap);
   fChain->SetBranchAddress("ccc_Nh_HEC", &ccc_Nh_HEC, &b_ccc_Nh_HEC);
   fChain->SetBranchAddress("ccc_Eh_HEC", &ccc_Eh_HEC, &b_ccc_Eh_HEC);
   fChain->SetBranchAddress("ccc_Nh_FCal", &ccc_Nh_FCal, &b_ccc_Nh_FCal);
   fChain->SetBranchAddress("ccc_Eh_FCal", &ccc_Eh_FCal, &b_ccc_Eh_FCal);
   fChain->SetBranchAddress("ccc_Nh_PresE", &ccc_Nh_PresE, &b_ccc_Nh_PresE);
   fChain->SetBranchAddress("ccc_Eh_PresE", &ccc_Eh_PresE, &b_ccc_Eh_PresE);
   fChain->SetBranchAddress("ccc_Nh_Scint", &ccc_Nh_Scint, &b_ccc_Nh_Scint);
   fChain->SetBranchAddress("ccc_Eh_Scint", &ccc_Eh_Scint, &b_ccc_Eh_Scint);
   fChain->SetBranchAddress("cl_n", &cl_n, &b_cl_n);
   fChain->SetBranchAddress("cl_pt", &cl_pt, &b_cl_pt);
   fChain->SetBranchAddress("cl_eta", &cl_eta, &b_cl_eta);
   fChain->SetBranchAddress("cl_phi", &cl_phi, &b_cl_phi);
   fChain->SetBranchAddress("cl_E_em", &cl_E_em, &b_cl_E_em);
   fChain->SetBranchAddress("cl_E_had", &cl_E_had, &b_cl_E_had);
   fChain->SetBranchAddress("cl_firstEdens", &cl_firstEdens, &b_cl_firstEdens);
   fChain->SetBranchAddress("cl_cellmaxfrac", &cl_cellmaxfrac, &b_cl_cellmaxfrac);
   fChain->SetBranchAddress("cl_longitudinal", &cl_longitudinal, &b_cl_longitudinal);
   fChain->SetBranchAddress("cl_secondlambda", &cl_secondlambda, &b_cl_secondlambda);
   fChain->SetBranchAddress("cl_lateral", &cl_lateral, &b_cl_lateral);
   fChain->SetBranchAddress("cl_secondR", &cl_secondR, &b_cl_secondR);
   fChain->SetBranchAddress("cl_centerlambda", &cl_centerlambda, &b_cl_centerlambda);
   fChain->SetBranchAddress("cl_deltaTheta", &cl_deltaTheta, &b_cl_deltaTheta);
   fChain->SetBranchAddress("cl_deltaPhi", &cl_deltaPhi, &b_cl_deltaPhi);
   fChain->SetBranchAddress("cl_centermag", &cl_centermag, &b_cl_centermag);
   fChain->SetBranchAddress("cl_time", &cl_time, &b_cl_time);
   fChain->SetBranchAddress("cl_E_PreSamplerB", &cl_E_PreSamplerB, &b_cl_E_PreSamplerB);
   fChain->SetBranchAddress("cl_E_EMB1", &cl_E_EMB1, &b_cl_E_EMB1);
   fChain->SetBranchAddress("cl_E_EMB2", &cl_E_EMB2, &b_cl_E_EMB2);
   fChain->SetBranchAddress("cl_E_EMB3", &cl_E_EMB3, &b_cl_E_EMB3);
   fChain->SetBranchAddress("cl_E_PreSamplerE", &cl_E_PreSamplerE, &b_cl_E_PreSamplerE);
   fChain->SetBranchAddress("cl_E_EME1", &cl_E_EME1, &b_cl_E_EME1);
   fChain->SetBranchAddress("cl_E_EME2", &cl_E_EME2, &b_cl_E_EME2);
   fChain->SetBranchAddress("cl_E_EME3", &cl_E_EME3, &b_cl_E_EME3);
   fChain->SetBranchAddress("cl_E_HEC0", &cl_E_HEC0, &b_cl_E_HEC0);
   fChain->SetBranchAddress("cl_E_HEC1", &cl_E_HEC1, &b_cl_E_HEC1);
   fChain->SetBranchAddress("cl_E_HEC2", &cl_E_HEC2, &b_cl_E_HEC2);
   fChain->SetBranchAddress("cl_E_HEC3", &cl_E_HEC3, &b_cl_E_HEC3);
   fChain->SetBranchAddress("cl_E_TileBar0", &cl_E_TileBar0, &b_cl_E_TileBar0);
   fChain->SetBranchAddress("cl_E_TileBar1", &cl_E_TileBar1, &b_cl_E_TileBar1);
   fChain->SetBranchAddress("cl_E_TileBar2", &cl_E_TileBar2, &b_cl_E_TileBar2);
   fChain->SetBranchAddress("cl_E_TileGap1", &cl_E_TileGap1, &b_cl_E_TileGap1);
   fChain->SetBranchAddress("cl_E_TileGap2", &cl_E_TileGap2, &b_cl_E_TileGap2);
   fChain->SetBranchAddress("cl_E_TileGap3", &cl_E_TileGap3, &b_cl_E_TileGap3);
   fChain->SetBranchAddress("cl_E_TileExt0", &cl_E_TileExt0, &b_cl_E_TileExt0);
   fChain->SetBranchAddress("cl_E_TileExt1", &cl_E_TileExt1, &b_cl_E_TileExt1);
   fChain->SetBranchAddress("cl_E_TileExt2", &cl_E_TileExt2, &b_cl_E_TileExt2);
   fChain->SetBranchAddress("cl_E_FCAL0", &cl_E_FCAL0, &b_cl_E_FCAL0);
   fChain->SetBranchAddress("cl_E_FCAL1", &cl_E_FCAL1, &b_cl_E_FCAL1);
   fChain->SetBranchAddress("cl_E_FCAL2", &cl_E_FCAL2, &b_cl_E_FCAL2);
   fChain->SetBranchAddress("cl_eta_PreSamplerB", &cl_eta_PreSamplerB, &b_cl_eta_PreSamplerB);
   fChain->SetBranchAddress("cl_phi_PreSamplerB", &cl_phi_PreSamplerB, &b_cl_phi_PreSamplerB);
   fChain->SetBranchAddress("cl_eta_EMB1", &cl_eta_EMB1, &b_cl_eta_EMB1);
   fChain->SetBranchAddress("cl_phi_EMB1", &cl_phi_EMB1, &b_cl_phi_EMB1);
   fChain->SetBranchAddress("cl_eta_EMB2", &cl_eta_EMB2, &b_cl_eta_EMB2);
   fChain->SetBranchAddress("cl_phi_EMB2", &cl_phi_EMB2, &b_cl_phi_EMB2);
   fChain->SetBranchAddress("cl_eta_EMB3", &cl_eta_EMB3, &b_cl_eta_EMB3);
   fChain->SetBranchAddress("cl_phi_EMB3", &cl_phi_EMB3, &b_cl_phi_EMB3);
   fChain->SetBranchAddress("cl_eta_PreSamplerE", &cl_eta_PreSamplerE, &b_cl_eta_PreSamplerE);
   fChain->SetBranchAddress("cl_phi_PreSamplerE", &cl_phi_PreSamplerE, &b_cl_phi_PreSamplerE);
   fChain->SetBranchAddress("cl_eta_EME1", &cl_eta_EME1, &b_cl_eta_EME1);
   fChain->SetBranchAddress("cl_phi_EME1", &cl_phi_EME1, &b_cl_phi_EME1);
   fChain->SetBranchAddress("cl_eta_EME2", &cl_eta_EME2, &b_cl_eta_EME2);
   fChain->SetBranchAddress("cl_phi_EME2", &cl_phi_EME2, &b_cl_phi_EME2);
   fChain->SetBranchAddress("cl_eta_EME3", &cl_eta_EME3, &b_cl_eta_EME3);
   fChain->SetBranchAddress("cl_phi_EME3", &cl_phi_EME3, &b_cl_phi_EME3);
   fChain->SetBranchAddress("cl_eta_HEC0", &cl_eta_HEC0, &b_cl_eta_HEC0);
   fChain->SetBranchAddress("cl_phi_HEC0", &cl_phi_HEC0, &b_cl_phi_HEC0);
   fChain->SetBranchAddress("cl_eta_HEC1", &cl_eta_HEC1, &b_cl_eta_HEC1);
   fChain->SetBranchAddress("cl_phi_HEC1", &cl_phi_HEC1, &b_cl_phi_HEC1);
   fChain->SetBranchAddress("cl_eta_HEC2", &cl_eta_HEC2, &b_cl_eta_HEC2);
   fChain->SetBranchAddress("cl_phi_HEC2", &cl_phi_HEC2, &b_cl_phi_HEC2);
   fChain->SetBranchAddress("cl_eta_HEC3", &cl_eta_HEC3, &b_cl_eta_HEC3);
   fChain->SetBranchAddress("cl_phi_HEC3", &cl_phi_HEC3, &b_cl_phi_HEC3);
   fChain->SetBranchAddress("cl_eta_TileBar0", &cl_eta_TileBar0, &b_cl_eta_TileBar0);
   fChain->SetBranchAddress("cl_phi_TileBar0", &cl_phi_TileBar0, &b_cl_phi_TileBar0);
   fChain->SetBranchAddress("cl_eta_TileBar1", &cl_eta_TileBar1, &b_cl_eta_TileBar1);
   fChain->SetBranchAddress("cl_phi_TileBar1", &cl_phi_TileBar1, &b_cl_phi_TileBar1);
   fChain->SetBranchAddress("cl_eta_TileBar2", &cl_eta_TileBar2, &b_cl_eta_TileBar2);
   fChain->SetBranchAddress("cl_phi_TileBar2", &cl_phi_TileBar2, &b_cl_phi_TileBar2);
   fChain->SetBranchAddress("cl_eta_TileGap1", &cl_eta_TileGap1, &b_cl_eta_TileGap1);
   fChain->SetBranchAddress("cl_phi_TileGap1", &cl_phi_TileGap1, &b_cl_phi_TileGap1);
   fChain->SetBranchAddress("cl_eta_TileGap2", &cl_eta_TileGap2, &b_cl_eta_TileGap2);
   fChain->SetBranchAddress("cl_phi_TileGap2", &cl_phi_TileGap2, &b_cl_phi_TileGap2);
   fChain->SetBranchAddress("cl_eta_TileGap3", &cl_eta_TileGap3, &b_cl_eta_TileGap3);
   fChain->SetBranchAddress("cl_phi_TileGap3", &cl_phi_TileGap3, &b_cl_phi_TileGap3);
   fChain->SetBranchAddress("cl_eta_TileExt0", &cl_eta_TileExt0, &b_cl_eta_TileExt0);
   fChain->SetBranchAddress("cl_phi_TileExt0", &cl_phi_TileExt0, &b_cl_phi_TileExt0);
   fChain->SetBranchAddress("cl_eta_TileExt1", &cl_eta_TileExt1, &b_cl_eta_TileExt1);
   fChain->SetBranchAddress("cl_phi_TileExt1", &cl_phi_TileExt1, &b_cl_phi_TileExt1);
   fChain->SetBranchAddress("cl_eta_TileExt2", &cl_eta_TileExt2, &b_cl_eta_TileExt2);
   fChain->SetBranchAddress("cl_phi_TileExt2", &cl_phi_TileExt2, &b_cl_phi_TileExt2);
   fChain->SetBranchAddress("cl_eta_FCAL0", &cl_eta_FCAL0, &b_cl_eta_FCAL0);
   fChain->SetBranchAddress("cl_phi_FCAL0", &cl_phi_FCAL0, &b_cl_phi_FCAL0);
   fChain->SetBranchAddress("cl_eta_FCAL1", &cl_eta_FCAL1, &b_cl_eta_FCAL1);
   fChain->SetBranchAddress("cl_phi_FCAL1", &cl_phi_FCAL1, &b_cl_phi_FCAL1);
   fChain->SetBranchAddress("cl_eta_FCAL2", &cl_eta_FCAL2, &b_cl_eta_FCAL2);
   fChain->SetBranchAddress("cl_phi_FCAL2", &cl_phi_FCAL2, &b_cl_phi_FCAL2);
   fChain->SetBranchAddress("cl_cell_n", &cl_cell_n, &b_cl_cell_n);
   fChain->SetBranchAddress("cl_cell_E", &cl_cell_E, &b_cl_cell_E);
   fChain->SetBranchAddress("cl_cell_pt", &cl_cell_pt, &b_cl_cell_pt);
   fChain->SetBranchAddress("cl_cell_eta", &cl_cell_eta, &b_cl_cell_eta);
   fChain->SetBranchAddress("cl_cell_phi", &cl_cell_phi, &b_cl_cell_phi);
   fChain->SetBranchAddress("cl_cell_QCells", &cl_cell_QCells, &b_cl_cell_QCells);
   fChain->SetBranchAddress("cl_cell_GainCells", &cl_cell_GainCells, &b_cl_cell_GainCells);
   fChain->SetBranchAddress("cl_cell_DetCells", &cl_cell_DetCells, &b_cl_cell_DetCells);
   fChain->SetBranchAddress("cl_cell_TimeCells", &cl_cell_TimeCells, &b_cl_cell_TimeCells);
   fChain->SetBranchAddress("cl_cell_xCells", &cl_cell_xCells, &b_cl_cell_xCells);
   fChain->SetBranchAddress("cl_cell_yCells", &cl_cell_yCells, &b_cl_cell_yCells);
   fChain->SetBranchAddress("cl_cell_zCells", &cl_cell_zCells, &b_cl_cell_zCells);
   fChain->SetBranchAddress("cl_cell_BadCells", &cl_cell_BadCells, &b_cl_cell_BadCells);
   fChain->SetBranchAddress("cl_cell_IdCells", &cl_cell_IdCells, &b_cl_cell_IdCells);
   fChain->SetBranchAddress("lardigit_n", &lardigit_n, &b_lardigit_n);
   fChain->SetBranchAddress("lardigit_layer", &lardigit_layer, &b_lardigit_layer);
   fChain->SetBranchAddress("lardigit_ieta", &lardigit_ieta, &b_lardigit_ieta);
   fChain->SetBranchAddress("lardigit_calo", &lardigit_calo, &b_lardigit_calo);
   fChain->SetBranchAddress("lardigit_iphi", &lardigit_iphi, &b_lardigit_iphi);
   fChain->SetBranchAddress("lardigit_barrel_ec", &lardigit_barrel_ec, &b_lardigit_barrel_ec);
   fChain->SetBranchAddress("lardigit_pos_neg", &lardigit_pos_neg, &b_lardigit_pos_neg);
   fChain->SetBranchAddress("lardigit_FT", &lardigit_FT, &b_lardigit_FT);
   fChain->SetBranchAddress("lardigit_slot", &lardigit_slot, &b_lardigit_slot);
   fChain->SetBranchAddress("lardigit_channel", &lardigit_channel, &b_lardigit_channel);
   fChain->SetBranchAddress("lardigit_gain", &lardigit_gain, &b_lardigit_gain);
   fChain->SetBranchAddress("lardigit_Samples", &lardigit_Samples, &b_lardigit_Samples);
   fChain->SetBranchAddress("lardigit_offlineId", &lardigit_offlineId, &b_lardigit_offlineId);
   fChain->SetBranchAddress("lardigit_sca_add", &lardigit_sca_add, &b_lardigit_sca_add);
   fChain->SetBranchAddress("lardigit_iter_valid", &lardigit_iter_valid, &b_lardigit_iter_valid);
   fChain->SetBranchAddress("lardigit_iter_converged", &lardigit_iter_converged, &b_lardigit_iter_converged);
   fChain->SetBranchAddress("lardigit_iter_nIter", &lardigit_iter_nIter, &b_lardigit_iter_nIter);
   fChain->SetBranchAddress("lardigit_iter_ampl", &lardigit_iter_ampl, &b_lardigit_iter_ampl);
   fChain->SetBranchAddress("lardigit_iter_tau", &lardigit_iter_tau, &b_lardigit_iter_tau);
   fChain->SetBranchAddress("lardigit_iter_peakSample", &lardigit_iter_peakSample, &b_lardigit_iter_peakSample);
   fChain->SetBranchAddress("lardigit_iter_delay", &lardigit_iter_delay, &b_lardigit_iter_delay);
   fChain->SetBranchAddress("lardigit_iter_qual", &lardigit_iter_qual, &b_lardigit_iter_qual);
   fChain->SetBranchAddress("lardigit_iter_index", &lardigit_iter_index, &b_lardigit_iter_index);
   Notify();
}

Bool_t CaloD3PDClass::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void CaloD3PDClass::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t CaloD3PDClass::Cut(/*Long64_t entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef CaloD3PDClass_cxx
