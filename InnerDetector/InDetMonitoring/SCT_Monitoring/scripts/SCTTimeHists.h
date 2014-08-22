/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jul  9 18:58:34 2008 by ROOT version 5.18/00d
// from TTree SCTTimeHists/SCTTimeHists
// found on file: SCT_CBNT.root
//////////////////////////////////////////////////////////

#ifndef SCTTimeHists_h
#define SCTTimeHists_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>
class SCTTimeHists {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           RunNumber;
   Int_t           EventNumber;
   Char_t          Token[93];
   Int_t           Run;
   Int_t           Event;
   Int_t           Time;
   Int_t           LumiBlock;
   Int_t           BCID;
   Int_t           LVL1ID;
   Double_t        Weight;
   Int_t           IEvent;
   Int_t           StatusElement;
   Int_t           LVL1TriggerType;
   vector<long>    *LVL1TriggerInfo;
   vector<long>    *LVL2TriggerInfo;
   vector<long>    *EventFilterInfo;
   vector<string>  *StreamTagName;
   vector<string>  *StreamTagType;
   Int_t           event_number;
   vector<int>     *sct_barrelec;
   vector<int>     *sct_layer;
   vector<int>     *sct_eta;
   vector<int>     *sct_phi;
   vector<int>     *sct_side;
   vector<int>     *sct_numHitsInWafer;
   vector<int>     *sct_waferHash;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_Token;   //!
   TBranch        *b_Run;   //!
   TBranch        *b_Event;   //!
   TBranch        *b_Time;   //!
   TBranch        *b_LumiBlock;   //!
   TBranch        *b_BCID;   //!
   TBranch        *b_LVL1ID;   //!
   TBranch        *b_Weight;   //!
   TBranch        *b_IEvent;   //!
   TBranch        *b_StatusElement;   //!
   TBranch        *b_LVL1TriggerType;   //!
   TBranch        *b_LVL1TriggerInfo;   //!
   TBranch        *b_LVL2TriggerInfo;   //!
   TBranch        *b_EventFilterInfo;   //!
   TBranch        *b_StreamTagName;   //!
   TBranch        *b_StreamTagType;   //!
   TBranch        *b_event_number;   //!
   TBranch        *b_sct_barrelec;   //!
   TBranch        *b_sct_layer;   //!
   TBranch        *b_sct_eta;   //!
   TBranch        *b_sct_phi;   //!
   TBranch        *b_sct_side;   //!
   TBranch        *b_sct_numHitsInWafer;   //!
   TBranch        *b_sct_waferHash;   //!

   SCTTimeHists(TTree *tree=0);
   virtual ~SCTTimeHists();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef SCTTimeHists_cxx
SCTTimeHists::SCTTimeHists(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("SCT_CBNT.root");
      if (!f) {
         f = new TFile("SCT_CBNT.root");
      }
      tree = (TTree*)gDirectory->Get("CollectionTree");

   }
   Init(tree);
}

SCTTimeHists::~SCTTimeHists()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SCTTimeHists::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SCTTimeHists::LoadTree(Long64_t entry)
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

void SCTTimeHists::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   LVL1TriggerInfo = 0;
   LVL2TriggerInfo = 0;
   EventFilterInfo = 0;
   StreamTagName = 0;
   StreamTagType = 0;
   sct_barrelec = 0;
   sct_layer = 0;
   sct_eta = 0;
   sct_phi = 0;
   sct_side = 0;
   sct_numHitsInWafer = 0;
   sct_waferHash = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("Token", Token, &b_Token);
   fChain->SetBranchAddress("Run", &Run, &b_Run);
   fChain->SetBranchAddress("Event", &Event, &b_Event);
   fChain->SetBranchAddress("Time", &Time, &b_Time);
   fChain->SetBranchAddress("LumiBlock", &LumiBlock, &b_LumiBlock);
   fChain->SetBranchAddress("BCID", &BCID, &b_BCID);
   fChain->SetBranchAddress("LVL1ID", &LVL1ID, &b_LVL1ID);
   fChain->SetBranchAddress("Weight", &Weight, &b_Weight);
   fChain->SetBranchAddress("IEvent", &IEvent, &b_IEvent);
   fChain->SetBranchAddress("StatusElement", &StatusElement, &b_StatusElement);
   fChain->SetBranchAddress("LVL1TriggerType", &LVL1TriggerType, &b_LVL1TriggerType);
   fChain->SetBranchAddress("LVL1TriggerInfo", &LVL1TriggerInfo, &b_LVL1TriggerInfo);
   fChain->SetBranchAddress("LVL2TriggerInfo", &LVL2TriggerInfo, &b_LVL2TriggerInfo);
   fChain->SetBranchAddress("EventFilterInfo", &EventFilterInfo, &b_EventFilterInfo);
   fChain->SetBranchAddress("StreamTagName", &StreamTagName, &b_StreamTagName);
   fChain->SetBranchAddress("StreamTagType", &StreamTagType, &b_StreamTagType);
   fChain->SetBranchAddress("event_number", &event_number, &b_event_number);
   fChain->SetBranchAddress("sct_barrelec", &sct_barrelec, &b_sct_barrelec);
   fChain->SetBranchAddress("sct_layer", &sct_layer, &b_sct_layer);
   fChain->SetBranchAddress("sct_eta", &sct_eta, &b_sct_eta);
   fChain->SetBranchAddress("sct_phi", &sct_phi, &b_sct_phi);
   fChain->SetBranchAddress("sct_side", &sct_side, &b_sct_side);
   fChain->SetBranchAddress("sct_numHitsInWafer", &sct_numHitsInWafer, &b_sct_numHitsInWafer);
   fChain->SetBranchAddress("sct_waferHash", &sct_waferHash, &b_sct_waferHash);
   Notify();
}

Bool_t SCTTimeHists::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void SCTTimeHists::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t SCTTimeHists::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef SCTTimeHists_cxx
