/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ShowerShapeBinning_h
#define ShowerShapeBinning_h

#include "TChain.h"
#include "TTree.h"
#include "TH1.h"


class ShowerShapeBinning
{
public:


   ShowerShapeBinning();

   ~ShowerShapeBinning();

   void CreateHitsAlphaDrNtuple(std::string);
   void RunBinning();
   bool CheckEmptyBin();
   void CreateNNinput();
   bool CheckGranularity(TTree *);
   bool CheckGradient();

   std::vector<float> Getxbins(TH1F *histo, int nbins);
   std::tuple<int, double> GetBinUpEdge(TH1F* histo);

   std::tuple<float, float> GetUnitsmm(float eta_hits, float deta, float dphi, float cellr, float cellz);

   void InitHitsTree(TTree *, TTree *);

   void BookHitsAlphaDrTree(TTree *);
   void InitHitsAlphaDrTree(TTree *, TTree *);
   void BookNNinputTree(TTree *, TTree *);



   // * setter methods
   void set_calolayer(int calolayer_) { m_calolayer = calolayer_; }
   void set_PCAbin(int PCAbin_) { m_PCAbin = PCAbin_; }
   void set_nbinsR(int nbinsR_) { m_nbinsR = nbinsR_; }
   void set_nbinsAlpha(int nbinsAlpha_) { m_nbinsAlpha = nbinsAlpha_; }
   void set_mincalosize(float mincalosize_) { m_mincalosize = mincalosize_; }
   void set_particle(string particle_) { m_particle = particle_; }
   void set_energy(float energy_) { m_particleEnergy = energy_; }
   void set_eta(float etamin_, float etamax_) { m_etamin = etamin_; m_etamax = etamax_; }
   void set_tolerance(float tolerance_) { m_tolerance = tolerance_; }

   void set_topDir(string topDir_) { m_topDir = topDir_; }
   void set_fileName(string fileName_) { m_fileName = fileName_; }


private:

   // * settings
   int m_calolayer;
   int m_PCAbin;
   int m_PCA; // ttree variable
   int m_nbinsR;
   int m_nbinsAlpha;
   float m_mincalosize;
   string m_particle;
   float m_particleEnergy; // in Gev
   float m_etamin;
   float m_etamax;
   float m_tolerance;

   int m_debug;


   std::string m_topDir;
   std::string m_fileName;


   // * for hits TTree
   int m_eventNumber;
   bool m_newEvent;
   float m_energy;
   float m_etaHit;
   float m_phiHit;
   float m_etaEntrance;
   float m_phiEntrance;
   float m_deta;
   float m_dphi;


   float m_etaCenter;
   float m_phiCenter;
   float m_etaShift;
   float m_phiShift;
   float m_cellr;
   float m_cellz;
   float m_celleta;
   float m_cellDeta;
   float m_cellDphi;

   // * hits TTree with alpha and dr

   float m_alpha;
   float m_dr;

   // * alpha and dr bin widths
   std::vector<float> xbinsAlpha;
   std::vector<float> ybinsR;


   // * histograms

   double nevent = 0.;

   TH2F *halphadrE;

   TH1F *h_alpha;
   TH1F *h_dr;


   TH2F *hHits;
   TH2F *hEnergyDensity;
   TH2F *hEnergy;
   TH1F *hDrE;
   TH1F *hDrEnergy;


   TH2F *hLnEnergy;
   TH2F *hLnEnergyDensity;

   TH1F *hGradient;

   TH2F *hValidation;

   // * NNinput TTree variables

   float m_alphaBinCenter;
   float m_rBinCenter;
   float m_Hits;
   float m_Energy;
   float m_EnergyDensity;
   float m_LnEnergy;
   float m_LnEnergyDensity;

   float m_alphaProb;
   float m_rProb;
   float m_alphaBin;
   float m_rBin;
   float m_LnrBin;
   float m_weight;


   ClassDef(ShowerShapeBinning, 1);
};

#if defined(__MAKECINT__)
#pragma link C++ class ShowerShapeBinning+;
#endif

#endif
