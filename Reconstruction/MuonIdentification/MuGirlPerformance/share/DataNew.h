//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jun 19 17:58:48 2008 by ROOT version 5.18/00a
// from TTree MUGIRL/Muon Candidates
// found on file: /home/svalleco/t1_2.mugirl.root
//////////////////////////////////////////////////////////

#ifndef DataNew_h
#define DataNew_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class DataNew {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           RunNumber;
   Int_t           EventNumber;
   Int_t           CandidateNumber;
   Float_t         IDTrkPt;
   Float_t         IDTrkEta;
   Float_t         IDTrkPhi;
   Float_t         IDTrkTheta;
   Float_t         IDTrka0;
   Float_t         IDTrkz0;
   Float_t         IDTrkQOverP;
   Float_t         IDTrkCharge;
   Float_t         IDTrkChi2;
   Float_t         IDTrkNDofFr;
   Float_t         IDTrkChi2pr;
   Float_t         IDTrkCov11;
   Float_t         IDTrkCov22;
   Float_t         IDTrkCov33;
   Float_t         IDTrkCov44;
   Float_t         IDTrkCov55;
   Float_t         QOverP;
   Int_t           passNN;
   Int_t           passStau;
   Float_t         NNBarrel;
   Float_t         NNEndCap;
   Float_t         InnerAngle;
   Float_t         InnerDelta;
   Float_t         MiddleAngle;
   Float_t         MiddleDelta;
   Float_t         LHR;
   Int_t           Isolation;
   Int_t           NumChambers;
   Int_t           ChamberID[200];   //[NumChambers]
   Int_t           ChamberStation[200];   //[NumChambers]
   Int_t           ChamberRegion[200];   //[NumChambers]
   Int_t           ChamberDistance[200];   //[NumChambers]
   Int_t           ChamberTechnology[200];   //[NumChambers]
   Int_t           ChamberFineIsect[200];   //[NumChambers]
   Int_t           ChamberMdtIsect[200];   //[NumChambers]
   Int_t           ChamberRpcIsect[200];   //[NumChambers]
   Int_t           ChamberTgcIsect[200];   //[NumChambers]
   Int_t           ChamberCscIsect[200];   //[NumChambers]
   Int_t           ChamberCombIsect[200];   //[NumChambers]
   Int_t           ChamberMdtIsect1[200];   //[NumChambers]
   Int_t           ChamberRpcIsect1[200];   //[NumChambers]
   Int_t           ChamberTgcIsect1[200];   //[NumChambers]
   Int_t           ChamberCscIsect1[200];   //[NumChambers]
   Int_t           ChamberCombIsect1[200];   //[NumChambers]
   Float_t         ChamberRZ[200];   //[NumChambers]
   Float_t         ChamberPhi[200];   //[NumChambers]
   Int_t           NumIsections;
   Int_t           IsectType[300];   //[NumIsections]
   Int_t           IsectOrigTech[300];   //[NumIsections]
   Int_t           IsectOrigDist[300];   //[NumIsections]
   Int_t           IsectTargTech[300];   //[NumIsections]
   Int_t           IsectTargDist[300];   //[NumIsections]
   Int_t           IsectRegion[300];   //[NumIsections]
   Int_t           IsectChamber[300];   //[NumIsections]
   Float_t         IsectPosEta[300];   //[NumIsections]
   Float_t         IsectPosPhi[300];   //[NumIsections]
   Float_t         IsectPosR[300];   //[NumIsections]
   Float_t         IsectPosZ[300];   //[NumIsections]
   Float_t         IsectDirEta[300];   //[NumIsections]
   Float_t         IsectDirPhi[300];   //[NumIsections]
   Float_t         IsectDirZ[300];   //[NumIsections]
   Int_t           IsectSegment[300];   //[NumIsections]
   Int_t           NumSegments;
   Int_t           SegType[100];   //[NumSegments]
   Int_t           SegDistance[100];   //[NumSegments]
   Int_t           SegRegion[100];   //[NumSegments]
   Int_t           SegStation[100];   //[NumSegments]
   Int_t           SegHits[100];   //[NumSegments]
   Float_t         SegPosEta[100];   //[NumSegments]
   Float_t         SegPosPhi[100];   //[NumSegments]
   Float_t         SegPosR[100];   //[NumSegments]
   Float_t         SegPosZ[100];   //[NumSegments]
   Float_t         SegDirEta[100];   //[NumSegments]
   Float_t         SegDirPhi[100];   //[NumSegments]
   Float_t         SegDirZ[100];   //[NumSegments]
   Float_t         SegChi2[100];   //[NumSegments]
   Int_t           SegNdf[100];   //[NumSegments]
   Float_t         SegProb[100];   //[NumSegments]
   Int_t           SegIsect[100];   //[NumSegments]
   Int_t           NumHits;
   Int_t           HitChamber[300];   //[NumHits]
   Int_t           HitChamberID[300];   //[NumHits]
   Float_t         HitDeltaEta[300];   //[NumHits]
   Float_t         HitDeltaPhi[300];   //[NumHits]
   Float_t         HitEta[300];   //[NumHits]
   Float_t         HitPhi[300];   //[NumHits]
   Float_t         HitZ[300];   //[NumHits]
   Float_t         HitR[300];   //[NumHits]
   Int_t           HitIsEta[300];   //[NumHits]
   Int_t           HitDistance[300];   //[NumHits]
   Int_t           HitTechnology[300];   //[NumHits]
   Int_t           HitRegion[300];   //[NumHits]
   Int_t           HitStation[300];   //[NumHits]
   Int_t           HitFineIsect[300];   //[NumHits]
   Int_t           HitMdtIsect[300];   //[NumHits]
   Int_t           HitRpcIsect[300];   //[NumHits]
   Int_t           HitTgcIsect[300];   //[NumHits]
   Int_t           HitCscIsect[300];   //[NumHits]
   Int_t           HitCombIsect[300];   //[NumHits]
   Int_t           HitMdtIsect1[300];   //[NumHits]
   Int_t           HitRpcIsect1[300];   //[NumHits]
   Int_t           HitTgcIsect1[300];   //[NumHits]
   Int_t           HitCscIsect1[300];   //[NumHits]
   Int_t           HitCombIsect1[300];   //[NumHits]
   Int_t           HitInSegment[300];   //[NumHits]
   Float_t         HitDriftRadius[300];   //[NumHits]
   Float_t         HitError[300];   //[NumHits]
   Int_t           NumHitSegMaps;
   Int_t           HitSegMapHit[300];   //[NumHitSegMaps]
   Int_t           HitSegMapSeg[300];   //[NumHitSegMaps]
   Float_t         StauBeta;
   Float_t         StauMdtBeta;
   Float_t         StauTrigBeta;
   Float_t         StauMass;
   Float_t         StauTZero;
   Int_t           StauMdtIterations;
   Float_t         StauMdtIterBeta[300];   //[StauMdtIterations]
   Float_t         StauMdtIterChi2[300];   //[StauMdtIterations]
   Int_t           StauMdtIterHits[300];   //[StauMdtIterations]
   Int_t           StauMdtIterSegs[300];   //[StauMdtIterations]
   Float_t         StauMdtIterPosX[300];   //[StauMdtIterations]
   Float_t         StauMdtIterPosY[300];   //[StauMdtIterations]
   Float_t         StauMdtIterPosZ[300];   //[StauMdtIterations]
   Float_t         StauMdtIterDirX[300];   //[StauMdtIterations]
   Float_t         StauMdtIterDirY[300];   //[StauMdtIterations]
   Float_t         StauMdtIterDirZ[300];   //[StauMdtIterations]
   Int_t           StauMdtIterInSeg[300];   //[StauMdtIterations]
   Float_t         TruthPdgId;
   Float_t         TruthPdgMother;
   Float_t         TruthPt;
   Float_t         TruthQOverP;
   Float_t         TruthEta;
   Float_t         TruthPhi;
   Float_t         TruthTheta;
   Float_t         Trutha0;
   Float_t         Truthz0;
   Float_t         TruthCharge;
   Float_t         TruthDPt;
   Float_t         TruthDEta;
   Float_t         TruthDPhi;
   Float_t         RefittedTrkPt;
   Float_t         RefittedTrkEta;
   Float_t         RefittedTrkPhi;
   Float_t         RefittedTrkTheta;
   Float_t         RefittedTrka0;
   Float_t         RefittedTrkz0;
   Float_t         RefittedTrkQOverP;
   Float_t         RefittedTrkCharge;
   Float_t         RefittedTrkChi2;
   Float_t         RefittedTrkNDofFr;
   Float_t         RefittedTrkChi2pr;
   Float_t         RefittedTrkCov11;
   Float_t         RefittedTrkCov22;
   Float_t         RefittedTrkCov33;
   Float_t         RefittedTrkCov44;
   Float_t         RefittedTrkCov55;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_CandidateNumber;   //!
   TBranch        *b_IDTrkPt;   //!
   TBranch        *b_IDTrkEta;   //!
   TBranch        *b_IDTrkPhi;   //!
   TBranch        *b_IDTrkTheta;   //!
   TBranch        *b_IDTrka0;   //!
   TBranch        *b_IDTrkz0;   //!
   TBranch        *b_IDTrkQOverP;   //!
   TBranch        *b_IDTrkCharge;   //!
   TBranch        *b_IDTrkChi2;   //!
   TBranch        *b_IDTrkNDofFr;   //!
   TBranch        *b_IDTrkChi2pr;   //!
   TBranch        *b_IDTrkCov11;   //!
   TBranch        *b_IDTrkCov22;   //!
   TBranch        *b_IDTrkCov33;   //!
   TBranch        *b_IDTrkCov44;   //!
   TBranch        *b_IDTrkCov55;   //!
   TBranch        *b_QOverP;   //!
   TBranch        *b_passNN;   //!
   TBranch        *b_passStau;   //!
   TBranch        *b_NNBarrel;   //!
   TBranch        *b_NNEndCap;   //!
   TBranch        *b_InnerAngle;   //!
   TBranch        *b_InnerDelta;   //!
   TBranch        *b_MiddleAngle;   //!
   TBranch        *b_MiddleDelta;   //!
   TBranch        *b_LHR;   //!
   TBranch        *b_Isolation;   //!
   TBranch        *b_NumChambers;   //!
   TBranch        *b_ChamberID;   //!
   TBranch        *b_ChamberStation;   //!
   TBranch        *b_ChamberRegion;   //!
   TBranch        *b_ChamberDistance;   //!
   TBranch        *b_ChamberTechnology;   //!
   TBranch        *b_ChamberFineIsect;   //!
   TBranch        *b_ChamberMdtIsect;   //!
   TBranch        *b_ChamberRpcIsect;   //!
   TBranch        *b_ChamberTgcIsect;   //!
   TBranch        *b_ChamberCscIsect;   //!
   TBranch        *b_ChamberCombIsect;   //!
   TBranch        *b_ChamberMdtIsect1;   //!
   TBranch        *b_ChamberRpcIsect1;   //!
   TBranch        *b_ChamberTgcIsect1;   //!
   TBranch        *b_ChamberCscIsect1;   //!
   TBranch        *b_ChamberCombIsect1;   //!
   TBranch        *b_ChamberRZ;   //!
   TBranch        *b_ChamberPhi;   //!
   TBranch        *b_NumIsections;   //!
   TBranch        *b_IsectType;   //!
   TBranch        *b_IsectOrigTech;   //!
   TBranch        *b_IsectOrigDist;   //!
   TBranch        *b_IsectTargTech;   //!
   TBranch        *b_IsectTargDist;   //!
   TBranch        *b_IsectRegion;   //!
   TBranch        *b_IsectChamber;   //!
   TBranch        *b_IsectPosEta;   //!
   TBranch        *b_IsectPosPhi;   //!
   TBranch        *b_IsectPosR;   //!
   TBranch        *b_IsectPosZ;   //!
   TBranch        *b_IsectDirEta;   //!
   TBranch        *b_IsectDirPhi;   //!
   TBranch        *b_IsectDirZ;   //!
   TBranch        *b_IsectSegment;   //!
   TBranch        *b_NumSegments;   //!
   TBranch        *b_SegType;   //!
   TBranch        *b_SegDistance;   //!
   TBranch        *b_SegRegion;   //!
   TBranch        *b_SegStation;   //!
   TBranch        *b_SegHits;   //!
   TBranch        *b_SegPosEta;   //!
   TBranch        *b_SegPosPhi;   //!
   TBranch        *b_SegPosR;   //!
   TBranch        *b_SegPosZ;   //!
   TBranch        *b_SegDirEta;   //!
   TBranch        *b_SegDirPhi;   //!
   TBranch        *b_SegDirZ;   //!
   TBranch        *b_SegChi2;   //!
   TBranch        *b_SegNdf;   //!
   TBranch        *b_SegProb;   //!
   TBranch        *b_SegIsect;   //!
   TBranch        *b_NumHits;   //!
   TBranch        *b_HitChamber;   //!
   TBranch        *b_HitChamberID;   //!
   TBranch        *b_HitDeltaEta;   //!
   TBranch        *b_HitDeltaPhi;   //!
   TBranch        *b_HitEta;   //!
   TBranch        *b_HitPhi;   //!
   TBranch        *b_HitZ;   //!
   TBranch        *b_HitR;   //!
   TBranch        *b_HitIsEta;   //!
   TBranch        *b_HitDistance;   //!
   TBranch        *b_HitTechnology;   //!
   TBranch        *b_HitRegion;   //!
   TBranch        *b_HitStation;   //!
   TBranch        *b_HitFineIsect;   //!
   TBranch        *b_HitMdtIsect;   //!
   TBranch        *b_HitRpcIsect;   //!
   TBranch        *b_HitTgcIsect;   //!
   TBranch        *b_HitCscIsect;   //!
   TBranch        *b_HitCombIsect;   //!
   TBranch        *b_HitMdtIsect1;   //!
   TBranch        *b_HitRpcIsect1;   //!
   TBranch        *b_HitTgcIsect1;   //!
   TBranch        *b_HitCscIsect1;   //!
   TBranch        *b_HitCombIsect1;   //!
   TBranch        *b_HitInSegment;   //!
   TBranch        *b_HitDriftRadius;   //!
   TBranch        *b_HitError;   //!
   TBranch        *b_NumHitSegMaps;   //!
   TBranch        *b_HitSegMapHit;   //!
   TBranch        *b_HitSegMapSeg;   //!
   TBranch        *b_StauBeta;   //!
   TBranch        *b_StauMdtBeta;   //!
   TBranch        *b_StauTrigBeta;   //!
   TBranch        *b_StauMass;   //!
   TBranch        *b_StauTZero;   //!
   TBranch        *b_StauMdtIterations;   //!
   TBranch        *b_StauMdtIterBeta;   //!
   TBranch        *b_StauMdtIterChi2;   //!
   TBranch        *b_StauMdtIterHits;   //!
   TBranch        *b_StauMdtIterSegs;   //!
   TBranch        *b_StauMdtIterPosX;   //!
   TBranch        *b_StauMdtIterPosY;   //!
   TBranch        *b_StauMdtIterPosZ;   //!
   TBranch        *b_StauMdtIterDirX;   //!
   TBranch        *b_StauMdtIterDirY;   //!
   TBranch        *b_StauMdtIterDirZ;   //!
   TBranch        *b_StauMdtIterInSeg;   //!
   TBranch        *b_TruthPdgId;   //!
   TBranch        *b_TruthPdgMother;   //!
   TBranch        *b_TruthPt;   //!
   TBranch        *b_TruthQOverP;   //!
   TBranch        *b_TruthEta;   //!
   TBranch        *b_TruthPhi;   //!
   TBranch        *b_TruthTheta;   //!
   TBranch        *b_Trutha0;   //!
   TBranch        *b_Truthz0;   //!
   TBranch        *b_TruthCharge;   //!
   TBranch        *b_TruthDPt;   //!
   TBranch        *b_TruthDEta;   //!
   TBranch        *b_TruthDPhi;   //!
   TBranch        *b_RefittedTrkPt;   //!
   TBranch        *b_RefittedTrkEta;   //!
   TBranch        *b_RefittedTrkPhi;   //!
   TBranch        *b_RefittedTrkTheta;   //!
   TBranch        *b_RefittedTrka0;   //!
   TBranch        *b_RefittedTrkz0;   //!
   TBranch        *b_RefittedTrkQOverP;   //!
   TBranch        *b_RefittedTrkCharge;   //!
   TBranch        *b_RefittedTrkChi2;   //!
   TBranch        *b_RefittedTrkNDofFr;   //!
   TBranch        *b_RefittedTrkChi2pr;   //!
   TBranch        *b_RefittedTrkCov11;   //!
   TBranch        *b_RefittedTrkCov22;   //!
   TBranch        *b_RefittedTrkCov33;   //!
   TBranch        *b_RefittedTrkCov44;   //!
   TBranch        *b_RefittedTrkCov55;   //!

   DataNew(TTree *tree=0);
   virtual ~DataNew();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef DataNew_cxx
DataNew::DataNew(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/svalleco/t1_2.mugirl.root");
      if (!f) {
         f = new TFile("/home/svalleco/t1_2.mugirl.root");
      }
      tree = (TTree*)gDirectory->Get("MUGIRL");

   }
   Init(tree);
}

DataNew::~DataNew()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t DataNew::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t DataNew::LoadTree(Long64_t entry)
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

void DataNew::Init(TTree *tree)
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
   fChain->SetBranchAddress("CandidateNumber", &CandidateNumber, &b_CandidateNumber);
   fChain->SetBranchAddress("IDTrkPt", &IDTrkPt, &b_IDTrkPt);
   fChain->SetBranchAddress("IDTrkEta", &IDTrkEta, &b_IDTrkEta);
   fChain->SetBranchAddress("IDTrkPhi", &IDTrkPhi, &b_IDTrkPhi);
   fChain->SetBranchAddress("IDTrkTheta", &IDTrkTheta, &b_IDTrkTheta);
   fChain->SetBranchAddress("IDTrka0", &IDTrka0, &b_IDTrka0);
   fChain->SetBranchAddress("IDTrkz0", &IDTrkz0, &b_IDTrkz0);
   fChain->SetBranchAddress("IDTrkQOverP", &IDTrkQOverP, &b_IDTrkQOverP);
   fChain->SetBranchAddress("IDTrkCharge", &IDTrkCharge, &b_IDTrkCharge);
   fChain->SetBranchAddress("IDTrkChi2", &IDTrkChi2, &b_IDTrkChi2);
   fChain->SetBranchAddress("IDTrkNDofFr", &IDTrkNDofFr, &b_IDTrkNDofFr);
   fChain->SetBranchAddress("IDTrkChi2pr", &IDTrkChi2pr, &b_IDTrkChi2pr);
   fChain->SetBranchAddress("IDTrkCov11", &IDTrkCov11, &b_IDTrkCov11);
   fChain->SetBranchAddress("IDTrkCov22", &IDTrkCov22, &b_IDTrkCov22);
   fChain->SetBranchAddress("IDTrkCov33", &IDTrkCov33, &b_IDTrkCov33);
   fChain->SetBranchAddress("IDTrkCov44", &IDTrkCov44, &b_IDTrkCov44);
   fChain->SetBranchAddress("IDTrkCov55", &IDTrkCov55, &b_IDTrkCov55);
   fChain->SetBranchAddress("QOverP", &QOverP, &b_QOverP);
   fChain->SetBranchAddress("passNN", &passNN, &b_passNN);
   fChain->SetBranchAddress("passStau", &passStau, &b_passStau);
   fChain->SetBranchAddress("NNBarrel", &NNBarrel, &b_NNBarrel);
   fChain->SetBranchAddress("NNEndCap", &NNEndCap, &b_NNEndCap);
   fChain->SetBranchAddress("InnerAngle", &InnerAngle, &b_InnerAngle);
   fChain->SetBranchAddress("InnerDelta", &InnerDelta, &b_InnerDelta);
   fChain->SetBranchAddress("MiddleAngle", &MiddleAngle, &b_MiddleAngle);
   fChain->SetBranchAddress("MiddleDelta", &MiddleDelta, &b_MiddleDelta);
   fChain->SetBranchAddress("LHR", &LHR, &b_LHR);
   fChain->SetBranchAddress("Isolation", &Isolation, &b_Isolation);
   fChain->SetBranchAddress("NumChambers", &NumChambers, &b_NumChambers);
   fChain->SetBranchAddress("ChamberID", ChamberID, &b_ChamberID);
   fChain->SetBranchAddress("ChamberStation", ChamberStation, &b_ChamberStation);
   fChain->SetBranchAddress("ChamberRegion", ChamberRegion, &b_ChamberRegion);
   fChain->SetBranchAddress("ChamberDistance", ChamberDistance, &b_ChamberDistance);
   fChain->SetBranchAddress("ChamberTechnology", ChamberTechnology, &b_ChamberTechnology);
   fChain->SetBranchAddress("ChamberFineIsect", ChamberFineIsect, &b_ChamberFineIsect);
   fChain->SetBranchAddress("ChamberMdtIsect", ChamberMdtIsect, &b_ChamberMdtIsect);
   fChain->SetBranchAddress("ChamberRpcIsect", ChamberRpcIsect, &b_ChamberRpcIsect);
   fChain->SetBranchAddress("ChamberTgcIsect", ChamberTgcIsect, &b_ChamberTgcIsect);
   fChain->SetBranchAddress("ChamberCscIsect", ChamberCscIsect, &b_ChamberCscIsect);
   fChain->SetBranchAddress("ChamberCombIsect", ChamberCombIsect, &b_ChamberCombIsect);
   fChain->SetBranchAddress("ChamberMdtIsect1", ChamberMdtIsect1, &b_ChamberMdtIsect1);
   fChain->SetBranchAddress("ChamberRpcIsect1", ChamberRpcIsect1, &b_ChamberRpcIsect1);
   fChain->SetBranchAddress("ChamberTgcIsect1", ChamberTgcIsect1, &b_ChamberTgcIsect1);
   fChain->SetBranchAddress("ChamberCscIsect1", ChamberCscIsect1, &b_ChamberCscIsect1);
   fChain->SetBranchAddress("ChamberCombIsect1", ChamberCombIsect1, &b_ChamberCombIsect1);
   fChain->SetBranchAddress("ChamberRZ", ChamberRZ, &b_ChamberRZ);
   fChain->SetBranchAddress("ChamberPhi", ChamberPhi, &b_ChamberPhi);
   fChain->SetBranchAddress("NumIsections", &NumIsections, &b_NumIsections);
   fChain->SetBranchAddress("IsectType", IsectType, &b_IsectType);
   fChain->SetBranchAddress("IsectOrigTech", IsectOrigTech, &b_IsectOrigTech);
   fChain->SetBranchAddress("IsectOrigDist", IsectOrigDist, &b_IsectOrigDist);
   fChain->SetBranchAddress("IsectTargTech", IsectTargTech, &b_IsectTargTech);
   fChain->SetBranchAddress("IsectTargDist", IsectTargDist, &b_IsectTargDist);
   fChain->SetBranchAddress("IsectRegion", IsectRegion, &b_IsectRegion);
   fChain->SetBranchAddress("IsectChamber", IsectChamber, &b_IsectChamber);
   fChain->SetBranchAddress("IsectPosEta", IsectPosEta, &b_IsectPosEta);
   fChain->SetBranchAddress("IsectPosPhi", IsectPosPhi, &b_IsectPosPhi);
   fChain->SetBranchAddress("IsectPosR", IsectPosR, &b_IsectPosR);
   fChain->SetBranchAddress("IsectPosZ", IsectPosZ, &b_IsectPosZ);
   fChain->SetBranchAddress("IsectDirEta", IsectDirEta, &b_IsectDirEta);
   fChain->SetBranchAddress("IsectDirPhi", IsectDirPhi, &b_IsectDirPhi);
   fChain->SetBranchAddress("IsectDirZ", IsectDirZ, &b_IsectDirZ);
   fChain->SetBranchAddress("IsectSegment", IsectSegment, &b_IsectSegment);
   fChain->SetBranchAddress("NumSegments", &NumSegments, &b_NumSegments);
   fChain->SetBranchAddress("SegType", SegType, &b_SegType);
   fChain->SetBranchAddress("SegDistance", SegDistance, &b_SegDistance);
   fChain->SetBranchAddress("SegRegion", SegRegion, &b_SegRegion);
   fChain->SetBranchAddress("SegStation", SegStation, &b_SegStation);
   fChain->SetBranchAddress("SegHits", SegHits, &b_SegHits);
   fChain->SetBranchAddress("SegPosEta", SegPosEta, &b_SegPosEta);
   fChain->SetBranchAddress("SegPosPhi", SegPosPhi, &b_SegPosPhi);
   fChain->SetBranchAddress("SegPosR", SegPosR, &b_SegPosR);
   fChain->SetBranchAddress("SegPosZ", SegPosZ, &b_SegPosZ);
   fChain->SetBranchAddress("SegDirEta", SegDirEta, &b_SegDirEta);
   fChain->SetBranchAddress("SegDirPhi", SegDirPhi, &b_SegDirPhi);
   fChain->SetBranchAddress("SegDirZ", SegDirZ, &b_SegDirZ);
   fChain->SetBranchAddress("SegChi2", SegChi2, &b_SegChi2);
   fChain->SetBranchAddress("SegNdf", SegNdf, &b_SegNdf);
   fChain->SetBranchAddress("SegProb", SegProb, &b_SegProb);
   fChain->SetBranchAddress("SegIsect", SegIsect, &b_SegIsect);
   fChain->SetBranchAddress("NumHits", &NumHits, &b_NumHits);
   fChain->SetBranchAddress("HitChamber", HitChamber, &b_HitChamber);
   fChain->SetBranchAddress("HitChamberID", HitChamberID, &b_HitChamberID);
   fChain->SetBranchAddress("HitDeltaEta", HitDeltaEta, &b_HitDeltaEta);
   fChain->SetBranchAddress("HitDeltaPhi", HitDeltaPhi, &b_HitDeltaPhi);
   fChain->SetBranchAddress("HitEta", HitEta, &b_HitEta);
   fChain->SetBranchAddress("HitPhi", HitPhi, &b_HitPhi);
   fChain->SetBranchAddress("HitZ", HitZ, &b_HitZ);
   fChain->SetBranchAddress("HitR", HitR, &b_HitR);
   fChain->SetBranchAddress("HitIsEta", HitIsEta, &b_HitIsEta);
   fChain->SetBranchAddress("HitDistance", HitDistance, &b_HitDistance);
   fChain->SetBranchAddress("HitTechnology", HitTechnology, &b_HitTechnology);
   fChain->SetBranchAddress("HitRegion", HitRegion, &b_HitRegion);
   fChain->SetBranchAddress("HitStation", HitStation, &b_HitStation);
   fChain->SetBranchAddress("HitFineIsect", HitFineIsect, &b_HitFineIsect);
   fChain->SetBranchAddress("HitMdtIsect", HitMdtIsect, &b_HitMdtIsect);
   fChain->SetBranchAddress("HitRpcIsect", HitRpcIsect, &b_HitRpcIsect);
   fChain->SetBranchAddress("HitTgcIsect", HitTgcIsect, &b_HitTgcIsect);
   fChain->SetBranchAddress("HitCscIsect", HitCscIsect, &b_HitCscIsect);
   fChain->SetBranchAddress("HitCombIsect", HitCombIsect, &b_HitCombIsect);
   fChain->SetBranchAddress("HitMdtIsect1", HitMdtIsect1, &b_HitMdtIsect1);
   fChain->SetBranchAddress("HitRpcIsect1", HitRpcIsect1, &b_HitRpcIsect1);
   fChain->SetBranchAddress("HitTgcIsect1", HitTgcIsect1, &b_HitTgcIsect1);
   fChain->SetBranchAddress("HitCscIsect1", HitCscIsect1, &b_HitCscIsect1);
   fChain->SetBranchAddress("HitCombIsect1", HitCombIsect1, &b_HitCombIsect1);
   fChain->SetBranchAddress("HitInSegment", HitInSegment, &b_HitInSegment);
   fChain->SetBranchAddress("HitDriftRadius", HitDriftRadius, &b_HitDriftRadius);
   fChain->SetBranchAddress("HitError", HitError, &b_HitError);
   fChain->SetBranchAddress("NumHitSegMaps", &NumHitSegMaps, &b_NumHitSegMaps);
   fChain->SetBranchAddress("HitSegMapHit", HitSegMapHit, &b_HitSegMapHit);
   fChain->SetBranchAddress("HitSegMapSeg", HitSegMapSeg, &b_HitSegMapSeg);
   fChain->SetBranchAddress("StauBeta", &StauBeta, &b_StauBeta);
   fChain->SetBranchAddress("StauMdtBeta", &StauMdtBeta, &b_StauMdtBeta);
   fChain->SetBranchAddress("StauTrigBeta", &StauTrigBeta, &b_StauTrigBeta);
   fChain->SetBranchAddress("StauMass", &StauMass, &b_StauMass);
   fChain->SetBranchAddress("StauTZero", &StauTZero, &b_StauTZero);
   fChain->SetBranchAddress("StauMdtIterations", &StauMdtIterations, &b_StauMdtIterations);
   fChain->SetBranchAddress("StauMdtIterBeta", StauMdtIterBeta, &b_StauMdtIterBeta);
   fChain->SetBranchAddress("StauMdtIterChi2", StauMdtIterChi2, &b_StauMdtIterChi2);
   fChain->SetBranchAddress("StauMdtIterHits", StauMdtIterHits, &b_StauMdtIterHits);
   fChain->SetBranchAddress("StauMdtIterSegs", StauMdtIterSegs, &b_StauMdtIterSegs);
   fChain->SetBranchAddress("StauMdtIterPosX", StauMdtIterPosX, &b_StauMdtIterPosX);
   fChain->SetBranchAddress("StauMdtIterPosY", StauMdtIterPosY, &b_StauMdtIterPosY);
   fChain->SetBranchAddress("StauMdtIterPosZ", StauMdtIterPosZ, &b_StauMdtIterPosZ);
   fChain->SetBranchAddress("StauMdtIterDirX", StauMdtIterDirX, &b_StauMdtIterDirX);
   fChain->SetBranchAddress("StauMdtIterDirY", StauMdtIterDirY, &b_StauMdtIterDirY);
   fChain->SetBranchAddress("StauMdtIterDirZ", StauMdtIterDirZ, &b_StauMdtIterDirZ);
   fChain->SetBranchAddress("StauMdtIterInSeg", StauMdtIterInSeg, &b_StauMdtIterInSeg);
   fChain->SetBranchAddress("TruthPdgId", &TruthPdgId, &b_TruthPdgId);
   fChain->SetBranchAddress("TruthPdgMother", &TruthPdgMother, &b_TruthPdgMother);
   fChain->SetBranchAddress("TruthPt", &TruthPt, &b_TruthPt);
   fChain->SetBranchAddress("TruthQOverP", &TruthQOverP, &b_TruthQOverP);
   fChain->SetBranchAddress("TruthEta", &TruthEta, &b_TruthEta);
   fChain->SetBranchAddress("TruthPhi", &TruthPhi, &b_TruthPhi);
   fChain->SetBranchAddress("TruthTheta", &TruthTheta, &b_TruthTheta);
   fChain->SetBranchAddress("Trutha0", &Trutha0, &b_Trutha0);
   fChain->SetBranchAddress("Truthz0", &Truthz0, &b_Truthz0);
   fChain->SetBranchAddress("TruthCharge", &TruthCharge, &b_TruthCharge);
   fChain->SetBranchAddress("TruthDPt", &TruthDPt, &b_TruthDPt);
   fChain->SetBranchAddress("TruthDEta", &TruthDEta, &b_TruthDEta);
   fChain->SetBranchAddress("TruthDPhi", &TruthDPhi, &b_TruthDPhi);
   fChain->SetBranchAddress("RefittedTrkPt", &RefittedTrkPt, &b_RefittedTrkPt);
   fChain->SetBranchAddress("RefittedTrkEta", &RefittedTrkEta, &b_RefittedTrkEta);
   fChain->SetBranchAddress("RefittedTrkPhi", &RefittedTrkPhi, &b_RefittedTrkPhi);
   fChain->SetBranchAddress("RefittedTrkTheta", &RefittedTrkTheta, &b_RefittedTrkTheta);
   fChain->SetBranchAddress("RefittedTrka0", &RefittedTrka0, &b_RefittedTrka0);
   fChain->SetBranchAddress("RefittedTrkz0", &RefittedTrkz0, &b_RefittedTrkz0);
   fChain->SetBranchAddress("RefittedTrkQOverP", &RefittedTrkQOverP, &b_RefittedTrkQOverP);
   fChain->SetBranchAddress("RefittedTrkCharge", &RefittedTrkCharge, &b_RefittedTrkCharge);
   fChain->SetBranchAddress("RefittedTrkChi2", &RefittedTrkChi2, &b_RefittedTrkChi2);
   fChain->SetBranchAddress("RefittedTrkNDofFr", &RefittedTrkNDofFr, &b_RefittedTrkNDofFr);
   fChain->SetBranchAddress("RefittedTrkChi2pr", &RefittedTrkChi2pr, &b_RefittedTrkChi2pr);
   fChain->SetBranchAddress("RefittedTrkCov11", &RefittedTrkCov11, &b_RefittedTrkCov11);
   fChain->SetBranchAddress("RefittedTrkCov22", &RefittedTrkCov22, &b_RefittedTrkCov22);
   fChain->SetBranchAddress("RefittedTrkCov33", &RefittedTrkCov33, &b_RefittedTrkCov33);
   fChain->SetBranchAddress("RefittedTrkCov44", &RefittedTrkCov44, &b_RefittedTrkCov44);
   fChain->SetBranchAddress("RefittedTrkCov55", &RefittedTrkCov55, &b_RefittedTrkCov55);
   Notify();
}

Bool_t DataNew::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void DataNew::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t DataNew::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef DataNew_cxx
