//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jul 28 11:00:33 2008 by ROOT version 5.18/00d
// from TTree AOD/Muons in the AOD
// found on file: t1_1.mugirl.root
//////////////////////////////////////////////////////////

#ifndef DataAod_h
#define DataAod_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class DataAod {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           RunNumber;
   Int_t           EventNumber;
   Int_t           NumMdtHits;
   Int_t           NumRpcPhiHits;
   Int_t           NumRpcEtaHits;
   Int_t           NumCscPhiHits;
   Int_t           NumCscEtaHits;
   Int_t           NumMdtSegs;
   Int_t           NumRpcSegs;
   Int_t           NumMdtHitsInSeg;
   Int_t           NumRpcHitsInSeg;
   Int_t           NumTgcRpcPhiHits;
   Int_t           NumTgcRpcEtaHits;
   Int_t           NumTgcRpcSegs;
   Int_t           NumTgcRpcHitsInSeg;
   Int_t           NumTgcPhiHits;
   Int_t           NumTgcEtaHits;
   Int_t           NumTgcSegs;
   Int_t           NumTgcHitsInSeg;
   Int_t           NumCscSegs;
   Float_t         IDEta;
   Float_t         IDPhi;
   Float_t         IDPt;
   Float_t         RefitEta;
   Float_t         RefitPhi;
   Float_t         RefitPt;
   Float_t         ChiSq;
   Int_t           Dof;
   Float_t         AnnBarrel;
   Float_t         AnnEndcap;
   Float_t         InnAngle;
   Float_t         InnDelta;
   Float_t         MidAngle;
   Float_t         MidDelta;
   Int_t           PdgID;
   Float_t         PassNNsel;
   Float_t         Beta;
   Float_t         Lhr;
   Int_t           SaveAnn;
   Int_t           SaveStau;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_NumMdtHits;   //!
   TBranch        *b_NumRpcPhiHits;   //!
   TBranch        *b_NumRpcEtaHits;   //!
   TBranch        *b_NumCscPhiHits;   //!
   TBranch        *b_NumCscEtaHits;   //!
   TBranch        *b_NumMdtSegs;   //!
   TBranch        *b_NumRpcSegs;   //!
   TBranch        *b_NumMdtHitsInSeg;   //!
   TBranch        *b_NumRpcHitsInSeg;   //!
   TBranch        *b_NumTgcRpcPhiHits;   //!
   TBranch        *b_NumTgcRpcEtaHits;   //!
   TBranch        *b_NumTgcRpcSegs;   //!
   TBranch        *b_NumTgcRpcHitsInSeg;   //!
   TBranch        *b_NumTgcPhiHits;   //!
   TBranch        *b_NumTgcEtaHits;   //!
   TBranch        *b_NumTgcSegs;   //!
   TBranch        *b_NumTgcHitsInSeg;   //!
   TBranch        *b_NumCscSegs;   //!
   TBranch        *b_IDEta;   //!
   TBranch        *b_IDPhi;   //!
   TBranch        *b_IDPt;   //!
   TBranch        *b_RefitEta;   //!
   TBranch        *b_RefitPhi;   //!
   TBranch        *b_RefitPt;   //!
   TBranch        *b_ChiSq;   //!
   TBranch        *b_Dof;   //!
   TBranch        *b_AnnBarrel;   //!
   TBranch        *b_AnnEndcap;   //!
   TBranch        *b_InnAngle;   //!
   TBranch        *b_InnDelta;   //!
   TBranch        *b_MidAngle;   //!
   TBranch        *b_MidDelta;   //!
   TBranch        *b_PdgID;   //!
   TBranch        *b_PassNNsel;   //!
   TBranch        *b_Beta;   //!
   TBranch        *b_Lhr;   //!
   TBranch        *b_SaveAnn;   //!
   TBranch        *b_SaveStau;   //!

   DataAod(TTree *tree=0);
   virtual ~DataAod();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef DataAod_cxx
DataAod::DataAod(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("t1_1.mugirl.root");
      if (!f) {
         f = new TFile("t1_1.mugirl.root");
      }
      tree = (TTree*)gDirectory->Get("AOD");

   }
   Init(tree);
}

DataAod::~DataAod()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t DataAod::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t DataAod::LoadTree(Long64_t entry)
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

void DataAod::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("NumMdtHits", &NumMdtHits, &b_NumMdtHits);
   fChain->SetBranchAddress("NumRpcPhiHits", &NumRpcPhiHits, &b_NumRpcPhiHits);
   fChain->SetBranchAddress("NumRpcEtaHits", &NumRpcEtaHits, &b_NumRpcEtaHits);
   fChain->SetBranchAddress("NumCscPhiHits", &NumCscPhiHits, &b_NumCscPhiHits);
   fChain->SetBranchAddress("NumCscEtaHits", &NumCscEtaHits, &b_NumCscEtaHits);
   fChain->SetBranchAddress("NumMdtSegs", &NumMdtSegs, &b_NumMdtSegs);
   fChain->SetBranchAddress("NumRpcSegs", &NumRpcSegs, &b_NumRpcSegs);
   fChain->SetBranchAddress("NumMdtHitsInSeg", &NumMdtHitsInSeg, &b_NumMdtHitsInSeg);
   fChain->SetBranchAddress("NumRpcHitsInSeg", &NumRpcHitsInSeg, &b_NumRpcHitsInSeg);
   fChain->SetBranchAddress("NumTgcRpcPhiHits", &NumTgcRpcPhiHits, &b_NumTgcRpcPhiHits);
   fChain->SetBranchAddress("NumTgcRpcEtaHits", &NumTgcRpcEtaHits, &b_NumTgcRpcEtaHits);
   fChain->SetBranchAddress("NumTgcRpcSegs", &NumTgcRpcSegs, &b_NumTgcRpcSegs);
   fChain->SetBranchAddress("NumTgcRpcHitsInSeg", &NumTgcRpcHitsInSeg, &b_NumTgcRpcHitsInSeg);
   fChain->SetBranchAddress("NumTgcPhiHits", &NumTgcPhiHits, &b_NumTgcPhiHits);
   fChain->SetBranchAddress("NumTgcEtaHits", &NumTgcEtaHits, &b_NumTgcEtaHits);
   fChain->SetBranchAddress("NumTgcSegs", &NumTgcSegs, &b_NumTgcSegs);
   fChain->SetBranchAddress("NumTgcHitsInSeg", &NumTgcHitsInSeg, &b_NumTgcHitsInSeg);
   fChain->SetBranchAddress("NumCscSegs", &NumCscSegs, &b_NumCscSegs);
   fChain->SetBranchAddress("IDEta", &IDEta, &b_IDEta);
   fChain->SetBranchAddress("IDPhi", &IDPhi, &b_IDPhi);
   fChain->SetBranchAddress("IDPt", &IDPt, &b_IDPt);
   fChain->SetBranchAddress("RefitEta", &RefitEta, &b_RefitEta);
   fChain->SetBranchAddress("RefitPhi", &RefitPhi, &b_RefitPhi);
   fChain->SetBranchAddress("RefitPt", &RefitPt, &b_RefitPt);
   fChain->SetBranchAddress("ChiSq", &ChiSq, &b_ChiSq);
   fChain->SetBranchAddress("Dof", &Dof, &b_Dof);
   fChain->SetBranchAddress("AnnBarrel", &AnnBarrel, &b_AnnBarrel);
   fChain->SetBranchAddress("AnnEndcap", &AnnEndcap, &b_AnnEndcap);
   fChain->SetBranchAddress("InnAngle", &InnAngle, &b_InnAngle);
   fChain->SetBranchAddress("InnDelta", &InnDelta, &b_InnDelta);
   fChain->SetBranchAddress("MidAngle", &MidAngle, &b_MidAngle);
   fChain->SetBranchAddress("MidDelta", &MidDelta, &b_MidDelta);
   fChain->SetBranchAddress("PdgID", &PdgID, &b_PdgID);
   fChain->SetBranchAddress("PassNNsel", &PassNNsel, &b_PassNNsel);
   fChain->SetBranchAddress("Beta", &Beta, &b_Beta);
   fChain->SetBranchAddress("Lhr", &Lhr, &b_Lhr);
   fChain->SetBranchAddress("SaveAnn", &SaveAnn, &b_SaveAnn);
   fChain->SetBranchAddress("SaveStau", &SaveStau, &b_SaveStau);
   Notify();
}

Bool_t DataAod::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void DataAod::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t DataAod::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef DataAod_cxx
