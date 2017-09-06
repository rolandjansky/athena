/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri May  2 18:25:08 2014 by ROOT version 5.34/05
// from TTree CaloHitAna/CaloHitAna
// found on file: ISF_HitAnalysispion_eta1_020514.root
//////////////////////////////////////////////////////////

#ifndef CaloHitAna_h
#define CaloHitAna_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <FCS_Cell.h>

#include <iostream>
// Fixed size dimensions of array or collections stored in the TTree if any.

class CaloHitAna {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
   TString         fFilename;

   //output structure
   TFile *m_Output;
   TString m_OutputName;
   TTree *m_OutputTree;
   std::vector<Int_t> m_Settings;
   Float_t m_TimingCut;
   Int_t m_Debug;
   Int_t m_PrintOutFrequency;
   Int_t m_max_nentries;

   static const int MAX_LAYER;

   // Declaration of leaf types
   std::vector<float>   *HitX;
   std::vector<float>   *HitY;
   std::vector<float>   *HitZ;
   std::vector<float>   *HitE;
   std::vector<float>   *HitT;
   std::vector<Long64_t>  *HitIdentifier;
   std::vector<Long64_t>  *HitCellIdentifier;
   std::vector<bool>    *HitIsLArBarrel;
   std::vector<bool>    *HitIsLArEndCap;
   std::vector<bool>    *HitIsHEC;
   std::vector<bool>    *HitIsFCAL;
   std::vector<bool>    *HitIsTile;
   std::vector<int>     *HitSampling;
   std::vector<float>   *HitSamplingFraction;
   std::vector<float>   *TruthE;
   std::vector<float>   *TruthPx;
   std::vector<float>   *TruthPy;
   std::vector<float>   *TruthPz;
   std::vector<int>     *TruthPDG;
   std::vector<int>     *TruthBarcode;
   std::vector<int>     *TruthVtxBarcode;
   std::vector<Long64_t>  *CellIdentifier;
   std::vector<float>   *CellE;
   std::vector<int>     *CellSampling;
   std::vector<float>   *G4HitE;
   std::vector<float>   *G4HitT;
   std::vector<Long64_t>  *G4HitIdentifier;
   std::vector<Long64_t>  *G4HitCellIdentifier;
   std::vector<float>   *G4HitSamplingFraction;
   std::vector<int>     *G4HitSampling;
   /*
   std::vector<std::vector<double> > *TTC_back_eta;
   std::vector<std::vector<double> > *TTC_back_phi;
   std::vector<std::vector<double> > *TTC_back_r;
   std::vector<std::vector<double> > *TTC_back_z;
   std::vector<std::vector<double> > *TTC_entrance_eta;
   std::vector<std::vector<double> > *TTC_entrance_phi;
   std::vector<std::vector<double> > *TTC_entrance_r;
   std::vector<std::vector<double> > *TTC_entrance_z;
   std::vector<double>  *TTC_IDCaloBoundary_eta; 
   std::vector<double>  *TTC_IDCaloBoundary_phi;
   std::vector<double>  *TTC_IDCaloBoundary_r;
   std::vector<double>  *TTC_IDCaloBoundary_z;
   std::vector<double>  *TTC_Angle3D;
   std::vector<double>  *TTC_AngleEta;
   */
   std::vector<std::vector<double> >* newTTC_entrance_eta;
   std::vector<std::vector<double> >* newTTC_entrance_phi;
   std::vector<std::vector<double> >* newTTC_entrance_r;
   std::vector<std::vector<double> >* newTTC_entrance_z;
   std::vector<std::vector<double> >* newTTC_back_eta;
   std::vector<std::vector<double> >* newTTC_back_phi;
   std::vector<std::vector<double> >* newTTC_back_r;
   std::vector<std::vector<double> >* newTTC_back_z;
   std::vector<double>* newTTC_IDCaloBoundary_eta;
   std::vector<double>* newTTC_IDCaloBoundary_phi;
   std::vector<double>* newTTC_IDCaloBoundary_r;
   std::vector<double>* newTTC_IDCaloBoundary_z;
   std::vector<double>* newTTC_Angle3D;
   std::vector<double>* newTTC_AngleEta;
   
   //If this won't work, we will have to change it... (memory??)
   std::vector<FCS_matchedcellvector> m_all_cells; //hm, make it a vector of (vector of FCS_matchedcell) and have it for all 1000 events at once in memory??

   // List of branches
   TBranch        *b_HitX;   //!
   TBranch        *b_HitY;   //!
   TBranch        *b_HitZ;   //!
   TBranch        *b_HitE;   //!
   TBranch        *b_HitT;   //!
   TBranch        *b_HitIdentifier;   //!
   TBranch        *b_HitCellIdentifier;   //!
   TBranch        *b_HitIsLArBarrel;   //!
   TBranch        *b_HitIsLArEndCap;   //!
   TBranch        *b_HitIsHEC;   //!
   TBranch        *b_HitIsFCAL;   //!
   TBranch        *b_HitIsTile;   //!
   TBranch        *b_HitSampling;   //!
   TBranch        *b_HitSamplingFraction;   //!
   TBranch        *b_TruthE;   //!
   TBranch        *b_TruthPx;   //!
   TBranch        *b_TruthPy;   //!
   TBranch        *b_TruthPz;   //!
   TBranch        *b_TruthPDG;   //!
   TBranch        *b_TruthBarcode; //!
   TBranch        *b_TruthVtxBarcode; //!
   TBranch        *b_CellIdentifier;   //!
   TBranch        *b_CellE;   //!
   TBranch        *b_CellSampling;   //!
   TBranch        *b_G4HitE;   //!
   TBranch        *b_G4HitT;   //!
   TBranch        *b_G4HitIdentifier;   //!
   TBranch        *b_G4HitCellIdentifier;   //!
   TBranch        *b_G4HitSamplingFraction;   //!
   TBranch        *b_G4HitSampling;   //!
   /*
   TBranch        *b_TTC_back_eta;   //!
   TBranch        *b_TTC_back_phi;   //!
   TBranch        *b_TTC_back_r;   //!
   TBranch        *b_TTC_back_z;   //!
   TBranch        *b_TTC_entrance_eta;   //!
   TBranch        *b_TTC_entrance_phi;   //!
   TBranch        *b_TTC_entrance_r;   //!
   TBranch        *b_TTC_entrance_z;   //!
   TBranch        *b_TTC_IDCaloBoundary_eta;   //!
   TBranch        *b_TTC_IDCaloBoundary_phi;   //!
   TBranch        *b_TTC_IDCaloBoundary_r;   //!
   TBranch        *b_TTC_IDCaloBoundary_z;   //!
   TBranch        *b_TTC_Angle3D;   //!
   TBranch        *b_TTC_AngleEta;   //!
   */
   
   TBranch        *b_newTTC_back_eta;   //!
   TBranch        *b_newTTC_back_phi;   //!
   TBranch        *b_newTTC_back_r;   //!
   TBranch        *b_newTTC_back_z;   //!
   TBranch        *b_newTTC_entrance_eta;   //!
   TBranch        *b_newTTC_entrance_phi;   //!
   TBranch        *b_newTTC_entrance_r;   //!
   TBranch        *b_newTTC_entrance_z;   //!
   TBranch        *b_newTTC_IDCaloBoundary_eta;   //!
   TBranch        *b_newTTC_IDCaloBoundary_phi;   //!
   TBranch        *b_newTTC_IDCaloBoundary_r;   //!
   TBranch        *b_newTTC_IDCaloBoundary_z;   //!
   TBranch        *b_newTTC_Angle3D;   //!
   TBranch        *b_newTTC_AngleEta;   //!
   
   
   CaloHitAna(TString filename="ISF_HitAnalysispion_eta1.root", TString outputname="output1.root", std::vector<Int_t> settings=std::vector<Int_t>(), Float_t timingcut=999999., Int_t debug=0, TTree *tree=0);
   virtual ~CaloHitAna();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual void     Finish(std::vector<Int_t> settings, TString outputname="output_cells.root");
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   
   void SetDebug(Int_t debug=0) {m_Debug=debug;};
   void SetTimingCut(Int_t timingcut=999999) {m_TimingCut=timingcut;};
   void SetDoAllCells(Int_t doit=0) {if(m_Settings.size()<1) m_Settings.resize(1,0);m_Settings[0]=doit;};
   void SetDoLayers(Int_t doit=0) {if(m_Settings.size()<2) m_Settings.resize(2,0);m_Settings[1]=doit;};
   void SetDoLayerSums(Int_t doit=0) {if(m_Settings.size()<3) m_Settings.resize(3,0);m_Settings[2]=doit;};
};

#endif

#ifdef CaloHitAna_cxx
CaloHitAna::CaloHitAna(TString filename, TString outputname, std::vector<Int_t> settings, Float_t timingcut, Int_t debug, TTree *tree) : fChain(0) 
{
  fFilename= filename;
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
     TFile *f = TFile::Open(filename,"READ");
     TString dirname=filename;
     dirname+=":/ISF_HitAnalysis";
     TDirectory * dir = (TDirectory*)f->Get(dirname);
     dir->GetObject("CaloHitAna",tree);

   }
   //tree->Print();
   Init(tree);
   m_Settings = settings;
   m_Debug = debug;
   m_TimingCut = timingcut;
   m_OutputName = outputname;
   m_PrintOutFrequency = 100;
   m_max_nentries=-1;
   //std::cout <<"Input: "<<fFilename<<" output: "<<m_OutputName<<" debug: "<<m_Debug<<" TC: "<<m_TimingCut<<std::endl;
}

CaloHitAna::~CaloHitAna()
{
  //if (m_OutputTree) delete m_OutputTree;
  //if (m_Output) delete m_Output;
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t CaloHitAna::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t CaloHitAna::LoadTree(Long64_t entry)
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

void CaloHitAna::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   HitX = 0;
   HitY = 0;
   HitZ = 0;
   HitE = 0;
   HitT = 0;
   HitIdentifier = 0;
   HitCellIdentifier = 0;
   HitIsLArBarrel = 0;
   HitIsLArEndCap = 0;
   HitIsHEC = 0;
   HitIsFCAL = 0;
   HitIsTile = 0;
   HitSampling = 0;
   HitSamplingFraction = 0;
   TruthE = 0;
   TruthPx = 0;
   TruthPy = 0;
   TruthPz = 0;
   TruthPDG = 0;
   TruthBarcode = 0;
   TruthVtxBarcode = 0;
   CellIdentifier = 0;
   CellE = 0;
   CellSampling = 0;
   G4HitE = 0;
   G4HitT = 0;
   G4HitIdentifier = 0;
   G4HitCellIdentifier = 0;
   G4HitSamplingFraction = 0;
   G4HitSampling = 0;
   /*
   TTC_back_eta = 0;
   TTC_back_phi = 0;
   TTC_back_r = 0;
   TTC_back_z = 0;
   TTC_entrance_eta = 0;
   TTC_entrance_phi = 0;
   TTC_entrance_r = 0;
   TTC_entrance_z = 0;
   TTC_IDCaloBoundary_eta = 0;
   TTC_IDCaloBoundary_phi = 0;
   TTC_IDCaloBoundary_r = 0;
   TTC_IDCaloBoundary_z = 0;
   TTC_Angle3D = 0;
   TTC_AngleEta = 0;
   */
   newTTC_back_eta = 0;
   newTTC_back_phi = 0;
   newTTC_back_r = 0;
   newTTC_back_z = 0;
   newTTC_entrance_eta = 0;
   newTTC_entrance_phi = 0;
   newTTC_entrance_r = 0;
   newTTC_entrance_z = 0;
   newTTC_IDCaloBoundary_eta = 0;
   newTTC_IDCaloBoundary_phi = 0;
   newTTC_IDCaloBoundary_r = 0;
   newTTC_IDCaloBoundary_z = 0;
   newTTC_Angle3D = 0;
   newTTC_AngleEta = 0;
   
   
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("HitX", &HitX, &b_HitX);
   fChain->SetBranchAddress("HitY", &HitY, &b_HitY);
   fChain->SetBranchAddress("HitZ", &HitZ, &b_HitZ);
   fChain->SetBranchAddress("HitE", &HitE, &b_HitE);
   fChain->SetBranchAddress("HitT", &HitT, &b_HitT);
   fChain->SetBranchAddress("HitIdentifier", &HitIdentifier, &b_HitIdentifier);
   fChain->SetBranchAddress("HitCellIdentifier", &HitCellIdentifier, &b_HitCellIdentifier);
   fChain->SetBranchAddress("HitIsLArBarrel", &HitIsLArBarrel, &b_HitIsLArBarrel);
   fChain->SetBranchAddress("HitIsLArEndCap", &HitIsLArEndCap, &b_HitIsLArEndCap);
   fChain->SetBranchAddress("HitIsHEC", &HitIsHEC, &b_HitIsHEC);
   fChain->SetBranchAddress("HitIsFCAL", &HitIsFCAL, &b_HitIsFCAL);
   fChain->SetBranchAddress("HitIsTile", &HitIsTile, &b_HitIsTile);
   fChain->SetBranchAddress("HitSampling", &HitSampling, &b_HitSampling);
   fChain->SetBranchAddress("HitSamplingFraction", &HitSamplingFraction, &b_HitSamplingFraction);
   fChain->SetBranchAddress("TruthE", &TruthE, &b_TruthE);
   fChain->SetBranchAddress("TruthPx", &TruthPx, &b_TruthPx);
   fChain->SetBranchAddress("TruthPy", &TruthPy, &b_TruthPy);
   fChain->SetBranchAddress("TruthPz", &TruthPz, &b_TruthPz);
   fChain->SetBranchAddress("TruthPDG", &TruthPDG, &b_TruthPDG);
   fChain->SetBranchAddress("TruthBarcode", &TruthBarcode, &b_TruthBarcode);
   fChain->SetBranchAddress("TruthVtxBarcode", &TruthVtxBarcode, &b_TruthVtxBarcode);
   fChain->SetBranchAddress("CellIdentifier", &CellIdentifier, &b_CellIdentifier);
   fChain->SetBranchAddress("CellE", &CellE, &b_CellE);
   fChain->SetBranchAddress("CellSampling", &CellSampling, &b_CellSampling);
   fChain->SetBranchAddress("G4HitE", &G4HitE, &b_G4HitE);
   fChain->SetBranchAddress("G4HitT", &G4HitT, &b_G4HitT);
   fChain->SetBranchAddress("G4HitIdentifier", &G4HitIdentifier, &b_G4HitIdentifier);
   fChain->SetBranchAddress("G4HitCellIdentifier", &G4HitCellIdentifier, &b_G4HitCellIdentifier);
   fChain->SetBranchAddress("G4HitSamplingFraction", &G4HitSamplingFraction, &b_G4HitSamplingFraction);
   fChain->SetBranchAddress("G4HitSampling", &G4HitSampling, &b_G4HitSampling);
   /*
   fChain->SetBranchAddress("TTC_back_eta", &TTC_back_eta, &b_TTC_back_eta);
   fChain->SetBranchAddress("TTC_back_phi", &TTC_back_phi, &b_TTC_back_phi);
   fChain->SetBranchAddress("TTC_back_r", &TTC_back_r, &b_TTC_back_r);
   fChain->SetBranchAddress("TTC_back_z", &TTC_back_z, &b_TTC_back_z);
   fChain->SetBranchAddress("TTC_entrance_eta", &TTC_entrance_eta, &b_TTC_entrance_eta);
   fChain->SetBranchAddress("TTC_entrance_phi", &TTC_entrance_phi, &b_TTC_entrance_phi);
   fChain->SetBranchAddress("TTC_entrance_r", &TTC_entrance_r, &b_TTC_entrance_r);
   fChain->SetBranchAddress("TTC_entrance_z", &TTC_entrance_z, &b_TTC_entrance_z);
   fChain->SetBranchAddress("TTC_IDCaloBoundary_eta", &TTC_IDCaloBoundary_eta, &b_TTC_IDCaloBoundary_eta);
   fChain->SetBranchAddress("TTC_IDCaloBoundary_phi", &TTC_IDCaloBoundary_phi, &b_TTC_IDCaloBoundary_phi);
   fChain->SetBranchAddress("TTC_IDCaloBoundary_r", &TTC_IDCaloBoundary_r, &b_TTC_IDCaloBoundary_r);
   fChain->SetBranchAddress("TTC_IDCaloBoundary_z", &TTC_IDCaloBoundary_z, &b_TTC_IDCaloBoundary_z);
   fChain->SetBranchAddress("TTC_Angle3D",     &TTC_Angle3D, &b_TTC_Angle3D);
   fChain->SetBranchAddress("TTC_AngleEta",    &TTC_AngleEta, &b_TTC_AngleEta);
   */
   fChain->SetBranchAddress("newTTC_back_eta", &newTTC_back_eta, &b_newTTC_back_eta);
   fChain->SetBranchAddress("newTTC_back_phi", &newTTC_back_phi, &b_newTTC_back_phi);
   fChain->SetBranchAddress("newTTC_back_r",   &newTTC_back_r, &b_newTTC_back_r);
   fChain->SetBranchAddress("newTTC_back_z",   &newTTC_back_z, &b_newTTC_back_z);
   fChain->SetBranchAddress("newTTC_entrance_eta", &newTTC_entrance_eta, &b_newTTC_entrance_eta);
   fChain->SetBranchAddress("newTTC_entrance_phi", &newTTC_entrance_phi, &b_newTTC_entrance_phi);
   fChain->SetBranchAddress("newTTC_entrance_r",   &newTTC_entrance_r, &b_newTTC_entrance_r);
   fChain->SetBranchAddress("newTTC_entrance_z",   &newTTC_entrance_z, &b_newTTC_entrance_z);
   fChain->SetBranchAddress("newTTC_IDCaloBoundary_eta", &newTTC_IDCaloBoundary_eta, &b_newTTC_IDCaloBoundary_eta);
   fChain->SetBranchAddress("newTTC_IDCaloBoundary_phi", &newTTC_IDCaloBoundary_phi, &b_newTTC_IDCaloBoundary_phi);
   fChain->SetBranchAddress("newTTC_IDCaloBoundary_r",   &newTTC_IDCaloBoundary_r, &b_newTTC_IDCaloBoundary_r);
   fChain->SetBranchAddress("newTTC_IDCaloBoundary_z",   &newTTC_IDCaloBoundary_z, &b_newTTC_IDCaloBoundary_z);
   fChain->SetBranchAddress("newTTC_Angle3D",  &newTTC_Angle3D, &b_newTTC_Angle3D);
   fChain->SetBranchAddress("newTTC_AngleEta", &newTTC_AngleEta, &b_newTTC_AngleEta);
   
   Notify();
}

Bool_t CaloHitAna::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void CaloHitAna::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t CaloHitAna::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
  std::cout <<entry<<std::endl;
   return 1;
}
#endif // #ifdef CaloHitAna_cxx
