/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ShowerShapeStudy_h
#define ShowerShapeStudy_h

#include "TTree.h"



class ShowerShapeStudy
{
public:
   ShowerShapeStudy();
   ~ShowerShapeStudy();

   std::tuple<float, float> GetShift(int);

   void EachParticleShower();
   void InvesitageShowerCenter(std::string inputHits);


   std::tuple<float, float> GetUnitsmm(float eta_hits, float deta, float dphi, float cellr, float cellz);

   void InitHitsTree(TTree *, TTree *);

   //void BookHitsAlphaDrTree(TTree *);
   float DeltaPhi(float, float);

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
   void set_hitsNtupleName(string hitsNtupleName_) { m_hitsNtupleName = hitsNtupleName_; }



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
   string m_hitsNtupleName;

   int m_debug;


   std::string m_topDir;
   std::string m_fileName;



   // * cell geometry

   float cellDeta;
   float cellDphi;
   float cellr;
   float cellz;
   float celleta;

   // * for hits TTree
   int m_eventNumber;
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



   ClassDef(ShowerShapeStudy, 1);
};

#if defined(__MAKECINT__)
#pragma link C++ class ShowerShapeThinning+;
#endif

#endif
