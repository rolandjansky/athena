/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERSVBFMJJINTERVALFILTER_H
#define GENERATORFILTERSVBFMJJINTERVALFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODJet/JetContainer.h"

class IAtRndmGenSvc;


class VBFMjjIntervalFilter : public GenFilter {
public:

  VBFMjjIntervalFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  double m_olapPt;
  double m_yMax;                           // Rapidity acceptance
  double m_pTavgMin;                       // Required average dijet pT
  std::string m_TruthJetContainerName;     // Name of the truth jet container

  ServiceHandle<IAtRndmGenSvc> m_rand;     // Random number generator

  //long m_total;                            // Total number of events tested
  //long m_passed;                           // Number of events passing all cuts
  //long m_outsideAcceptance;                // Number of events failing rapidity acceptance cuts

  double m_norm;                           // Normalization for weights
  //double m_high;                           // High-side function level
  //bool m_doShape;                          // Attempt to flatten the dY distribution
  double m_prob0;
  double m_prob1;
  double m_prob2low;
  double m_prob2high;
  double m_mjjlow;
  bool m_truncatelowmjj;
  double m_mjjhigh;
  bool m_truncatehighmjj;
  bool m_photonjetoverlap;
  bool m_electronjetoverlap;
  bool m_taujetoverlap;
  double m_alpha;
  bool m_ApplyNjet; 
  unsigned int m_NJetsMin; 
  unsigned int m_NJetsMax; 
  bool m_ApplyWeighting; 
  bool m_applyDphi; 
  double m_dphijj; 


  bool checkOverlap(double, double, const std::vector<HepMC::ConstGenParticlePtr>&);
  bool checkOverlap(double, double, const std::vector<TLorentzVector>&);
  TLorentzVector sumDaughterNeutrinos( HepMC::ConstGenParticlePtr );

public:

  bool ApplyMassDphi(xAOD::JetContainer *jets);
  double getEventWeight(xAOD::JetContainer *jets);
};

#endif
