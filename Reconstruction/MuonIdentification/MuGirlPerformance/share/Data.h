//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Apr 17 14:01:07 2007 by ROOT version 5.12/00e
// from TChain MUGIRL/
//////////////////////////////////////////////////////////

#ifndef Data_h
#define Data_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class Data {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Int_t           RunNumber;
   Int_t           EventNumber;
   Int_t           NumTruthMuon;
   Int_t           TruthMuType[100];   //[NumTruthMuon]
   Float_t         TruthMuPt[100];   //[NumTruthMuon]
   Float_t         TruthMuPhi[100];   //[NumTruthMuon]
   Float_t         TruthMuEta[100];   //[NumTruthMuon]
   Int_t           TruthMuPdg[100];   //[NumTruthMuon]
   Int_t           TruthMuParentPdg[100];   //[NumTruthMuon]
   Int_t           NumCandidates;
   Float_t         IDTrkPt[100];   //[NumCandidates]
   Float_t         IDTrkEta[100];   //[NumCandidates]
   Float_t         IDTrkPhi[100];   //[NumCandidates]
   Float_t         IDTrkCharge[100];   //[NumCandidates]
   Float_t         IDTrkQOverP[100];   //[NumCandidates]
   Float_t         QOverP[100];   //[NumCandidates]
   Float_t         TruthPt[100];   //[NumCandidates]
   Float_t         TruthEta[100];   //[NumCandidates]
   Float_t         TruthPhi[100];   //[NumCandidates]
   Float_t         TruthCharge[100];   //[NumCandidates]
   Float_t         TruthDPt[100];   //[NumCandidates]
   Float_t         TruthDEta[100];   //[NumCandidates]
   Float_t         TruthDPhi[100];   //[NumCandidates]
   Float_t         NNBarrel[100];   //[NumCandidates]
   Float_t         NNEndCap[100];   //[NumCandidates]
   Float_t         InnerAngle[100];   //[NumCandidates]
   Float_t         MiddleAngle[100];   //[NumCandidates]
   Float_t         LHR[100];   //[NumCandidates]
   Int_t           Isolation[100];   //[NumCandidates]
   Int_t           NumChambers[100];   //[NumCandidates]
   Int_t           ChamberStation[100][200];   //[NumCandidates]
   Int_t           ChamberRegion[100][200];   //[NumCandidates]
   Int_t           ChamberDistance[100][200];   //[NumCandidates]
   Int_t           ChamberTechnology[100][200];   //[NumCandidates]
   Int_t           ChamberFineIsect[100][200];   //[NumCandidates]
   Int_t           ChamberMdtIsect[100][200];   //[NumCandidates]
   Int_t           ChamberRpcIsect[100][200];   //[NumCandidates]
   Int_t           ChamberTgcIsect[100][200];   //[NumCandidates]
   Int_t           ChamberCscIsect[100][200];   //[NumCandidates]
   Int_t           ChamberCombIsect[100][200];   //[NumCandidates]
   Int_t           ChamberMdtIsect1[100][200];   //[NumCandidates]
   Int_t           ChamberRpcIsect1[100][200];   //[NumCandidates]
   Int_t           ChamberTgcIsect1[100][200];   //[NumCandidates]
   Int_t           ChamberCscIsect1[100][200];   //[NumCandidates]
   Int_t           ChamberCombIsect1[100][200];   //[NumCandidates]
   Float_t         ChamberRZ[100][200];   //[NumCandidates]
   Float_t         ChamberPhi[100][200];   //[NumCandidates]
   Int_t           NumIsections[100];   //[NumCandidates]
   Int_t           IsectType[100][300];   //[NumCandidates]
   Int_t           IsectOrigTech[100][300];   //[NumCandidates]
   Int_t           IsectOrigDist[100][300];   //[NumCandidates]
   Int_t           IsectTargTech[100][300];   //[NumCandidates]
   Int_t           IsectTargDist[100][300];   //[NumCandidates]
   Int_t           IsectRegion[100][300];   //[NumCandidates]
   Int_t           IsectChamber[100][300];   //[NumCandidates]
   Float_t         IsectPosEta[100][300];   //[NumCandidates]
   Float_t         IsectPosPhi[100][300];   //[NumCandidates]
   Float_t         IsectPosR[100][300];   //[NumCandidates]
   Float_t         IsectPosZ[100][300];   //[NumCandidates]
   Float_t         IsectDirEta[100][300];   //[NumCandidates]
   Float_t         IsectDirPhi[100][300];   //[NumCandidates]
   Float_t         IsectDirZ[100][300];   //[NumCandidates]
   Int_t           IsectSegment[100][300];   //[NumCandidates]
   Int_t           NumSegments[100];   //[NumCandidates]
   Int_t           SegType[100][100];   //[NumCandidates]
   Int_t           SegDistance[100][100];   //[NumCandidates]
   Int_t           SegRegion[100][100];   //[NumCandidates]
   Int_t           SegStation[100][100];   //[NumCandidates]
   Int_t           SegHits[100][100];   //[NumCandidates]
   Float_t         SegPosEta[100][100];   //[NumCandidates]
   Float_t         SegPosPhi[100][100];   //[NumCandidates]
   Float_t         SegPosR[100][100];   //[NumCandidates]
   Float_t         SegPosZ[100][100];   //[NumCandidates]
   Float_t         SegDirEta[100][100];   //[NumCandidates]
   Float_t         SegDirPhi[100][100];   //[NumCandidates]
   Float_t         SegDirZ[100][100];   //[NumCandidates]
   Float_t         SegChi2[100][100];   //[NumCandidates]
   Int_t           SegNdf[100][100];   //[NumCandidates]
   Float_t         SegProb[100][100];   //[NumCandidates]
   Int_t           SegIsect[100][100];   //[NumCandidates]
   Int_t           NumHits[100];   //[NumCandidates]
   Int_t           HitChamber[100][300];   //[NumCandidates]
   Float_t         HitDeltaEta[100][300];   //[NumCandidates]
   Float_t         HitDeltaPhi[100][300];   //[NumCandidates]
   Float_t         HitEta[100][300];   //[NumCandidates]
   Float_t         HitPhi[100][300];   //[NumCandidates]
   Float_t         HitZ[100][300];   //[NumCandidates]
   Float_t         HitR[100][300];   //[NumCandidates]
   Int_t           HitIsEta[100][300];   //[NumCandidates]
   Int_t           HitDistance[100][300];   //[NumCandidates]
   Int_t           HitTechnology[100][300];   //[NumCandidates]
   Int_t           HitRegion[100][300];   //[NumCandidates]
   Int_t           HitStation[100][300];   //[NumCandidates]
   Int_t           HitFineIsect[100][300];   //[NumCandidates]
   Int_t           HitMdtIsect[100][300];   //[NumCandidates]
   Int_t           HitRpcIsect[100][300];   //[NumCandidates]
   Int_t           HitTgcIsect[100][300];   //[NumCandidates]
   Int_t           HitCscIsect[100][300];   //[NumCandidates]
   Int_t           HitCombIsect[100][300];   //[NumCandidates]
   Int_t           HitMdtIsect1[100][300];   //[NumCandidates]
   Int_t           HitRpcIsect1[100][300];   //[NumCandidates]
   Int_t           HitTgcIsect1[100][300];   //[NumCandidates]
   Int_t           HitCscIsect1[100][300];   //[NumCandidates]
   Int_t           HitCombIsect1[100][300];   //[NumCandidates]
   Int_t           HitInSegment[100][300];   //[NumCandidates]
   Float_t         HitDriftRadius[100][300];   //[NumCandidates]
   Float_t         HitError[100][300];   //[NumCandidates]
   Int_t           NumHitSegMaps[100];   //[NumCandidates]
   Int_t           HitSegMapHit[100][300];   //[NumCandidates]
   Int_t           HitSegMapSeg[100][300];   //[NumCandidates]

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_NumTruthMuon;   //!
   TBranch        *b_TruthMuType;   //!
   TBranch        *b_TruthMuPt;   //!
   TBranch        *b_TruthMuPhi;   //!
   TBranch        *b_TruthMuEta;   //!
   TBranch        *b_TruthMuPdg;   //!
   TBranch        *b_TruthMuParentPdg;   //!
   TBranch        *b_NumCandidates;   //!
   TBranch        *b_IDTrkPt;   //!
   TBranch        *b_IDTrkEta;   //!
   TBranch        *b_IDTrkPhi;   //!
   TBranch        *b_IDTrkCharge;   //!
   TBranch        *b_IDTrkQOverP;   //!
   TBranch        *b_QOverP;   //!
   TBranch        *b_TruthPt;   //!
   TBranch        *b_TruthEta;   //!
   TBranch        *b_TruthPhi;   //!
   TBranch        *b_TruthCharge;   //!
   TBranch        *b_TruthDPt;   //!
   TBranch        *b_TruthDEta;   //!
   TBranch        *b_TruthDPhi;   //!
   TBranch        *b_NNBarrel;   //!
   TBranch        *b_NNEndCap;   //!
   TBranch        *b_InnerAngle;   //!
   TBranch        *b_MiddleAngle;   //!
   TBranch        *b_LHR;   //!
   TBranch        *b_Isolation;   //!
   TBranch        *b_NumChambers;   //!
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

   Data(TTree *tree=0);
   virtual ~Data();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Data_cxx
Data::Data(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("MuGirl/rel_3/bphys_mu6mu3_1_0.root");
      if (!f) {
         f = new TFile("bphys_mu6mu3_1_0.root");
         f->cd("Rint:/");
      }
      tree = (TTree*)gDirectory->Get("MUGIRL");

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("MUGIRL","");
      chain->Add("MuGirl/rel_3/bphys_mu6mu3_1.root/MUGIRL");
      
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

Data::~Data()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Data::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Data::LoadTree(Long64_t entry)
{
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

void Data::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normaly not necessary to make changes to the generated
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
   fChain->SetBranchAddress("NumTruthMuon", &NumTruthMuon, &b_NumTruthMuon);
   fChain->SetBranchAddress("TruthMuType", TruthMuType, &b_TruthMuType);
   fChain->SetBranchAddress("TruthMuPt", TruthMuPt, &b_TruthMuPt);
   fChain->SetBranchAddress("TruthMuPhi", TruthMuPhi, &b_TruthMuPhi);
   fChain->SetBranchAddress("TruthMuEta", TruthMuEta, &b_TruthMuEta);
   fChain->SetBranchAddress("TruthMuPdg", TruthMuPdg, &b_TruthMuPdg);
   fChain->SetBranchAddress("TruthMuParentPdg", TruthMuParentPdg, &b_TruthMuParentPdg);
   fChain->SetBranchAddress("NumCandidates", &NumCandidates, &b_NumCandidates);
   fChain->SetBranchAddress("IDTrkPt", IDTrkPt, &b_IDTrkPt);
   fChain->SetBranchAddress("IDTrkEta", IDTrkEta, &b_IDTrkEta);
   fChain->SetBranchAddress("IDTrkPhi", IDTrkPhi, &b_IDTrkPhi);
   fChain->SetBranchAddress("IDTrkCharge", IDTrkCharge, &b_IDTrkCharge);
   fChain->SetBranchAddress("IDTrkQOverP", IDTrkQOverP, &b_IDTrkQOverP);
   fChain->SetBranchAddress("QOverP", QOverP, &b_QOverP);
   fChain->SetBranchAddress("TruthPt", TruthPt, &b_TruthPt);
   fChain->SetBranchAddress("TruthEta", TruthEta, &b_TruthEta);
   fChain->SetBranchAddress("TruthPhi", TruthPhi, &b_TruthPhi);
   fChain->SetBranchAddress("TruthCharge", TruthCharge, &b_TruthCharge);
   fChain->SetBranchAddress("TruthDPt", TruthDPt, &b_TruthDPt);
   fChain->SetBranchAddress("TruthDEta", TruthDEta, &b_TruthDEta);
   fChain->SetBranchAddress("TruthDPhi", TruthDPhi, &b_TruthDPhi);
   fChain->SetBranchAddress("NNBarrel", NNBarrel, &b_NNBarrel);
   fChain->SetBranchAddress("NNEndCap", NNEndCap, &b_NNEndCap);
   fChain->SetBranchAddress("InnerAngle", InnerAngle, &b_InnerAngle);
   fChain->SetBranchAddress("MiddleAngle", MiddleAngle, &b_MiddleAngle);
   fChain->SetBranchAddress("LHR", LHR, &b_LHR);
   fChain->SetBranchAddress("Isolation", Isolation, &b_Isolation);
   fChain->SetBranchAddress("NumChambers", NumChambers, &b_NumChambers);
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
   fChain->SetBranchAddress("NumIsections", NumIsections, &b_NumIsections);
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
   fChain->SetBranchAddress("NumSegments", NumSegments, &b_NumSegments);
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
   fChain->SetBranchAddress("NumHits", NumHits, &b_NumHits);
   fChain->SetBranchAddress("HitChamber", HitChamber, &b_HitChamber);
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
   fChain->SetBranchAddress("NumHitSegMaps", NumHitSegMaps, &b_NumHitSegMaps);
   fChain->SetBranchAddress("HitSegMapHit", HitSegMapHit, &b_HitSegMapHit);
   fChain->SetBranchAddress("HitSegMapSeg", HitSegMapSeg, &b_HitSegMapSeg);
   Notify();
}

Bool_t Data::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Data::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Data::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Data_cxx
