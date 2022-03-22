/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EnergyLossComparisonTree_h
#define EnergyLossComparisonTree_h

#include <TProfile.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2D.h>
#include <vector>

class EnergyLossComparisonTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Int_t           Surfaces;
   Int_t           UpdateSurfaceType[50];
   Float_t         UpdateSurfaceR[50];
   Float_t         UpdateSurfaceZ[50];
   Float_t         TruePositionX[50];
   Float_t         TruePositionY[50];
   Float_t         TruePositionZ[50];
   Float_t         TruePositionR[50];

   Float_t         ReconPositionX[50];
   Float_t         ReconPositionY[50];
   Float_t         ReconPositionZ[50];
   Float_t         ReconPositionR[50];
   Float_t         OriginalP;

   Float_t         TrueNewP[50];
   Float_t         ElossError[50];
   Float_t         ReconNewP[50];
   Float_t         OriginalEta;
   Float_t         TrueNewEta[50];
   Float_t         ReconNewEta[50];

   std::vector<TH2D*>* m_twoDHistosTruth;
   std::vector<TH2D*>* m_twoDHistosReco;
   std::vector<TH2D*>* m_twoDHistosRecoSigma;
   // List of branches
   TBranch*        b_Surfaces;
   TBranch*        b_UpdateSurfaceType;
   TBranch*        b_UpdateSurfaceR;
   TBranch*        b_UpdateSurfaceZ;
   TBranch*        b_TruePositionX;
   TBranch*        b_TruePositionY;
   TBranch*        b_TruePositionZ;
   TBranch*        b_TruePositionR;

   TBranch*        b_ReconPositionX;
   TBranch*        b_ReconPositionY;
   TBranch*        b_ReconPositionZ;
   TBranch*        b_ReconPositionR;
   TBranch*        b_OriginalP;
   TBranch*        b_ElossError;

   TBranch*        b_TrueNewP;
   TBranch*        b_ReconNewP;
   TBranch*        b_OriginalEta;
   TBranch*        b_TrueNewEta;
   TBranch*        b_ReconNewEta;

   EnergyLossComparisonTree(TTree *tree=0);
   EnergyLossComparisonTree(const EnergyLossComparisonTree &) = delete;
   EnergyLossComparisonTree & operator=(const EnergyLossComparisonTree &) = delete;
   virtual ~EnergyLossComparisonTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual std::vector<TH1D*>* histograms(int bin, int mode = 0);
   virtual std::vector<TH2D*>* trueHistos(){return m_twoDHistosTruth;}
   virtual std::vector<TH2D*>* recoHistos(){return m_twoDHistosReco;}
   
   virtual TH2D* combinedTrueHistos();
   virtual std::vector<TH1D*>* doTrueFits(TH2D* true2D);

   virtual TH2D* combinedRecoHistoMPV();
   virtual TH2D* combinedRecoHistoSigma();

   virtual TH1D* doRecoFits(TH2D* reco2D, int l);

   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif
