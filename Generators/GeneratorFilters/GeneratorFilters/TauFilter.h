/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef GENERATORFILTERS_TAUFILTER_H
#define GENERATORFILTERS_TAUFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "CLHEP/Vector/LorentzVector.h"

class IAtRndmGenSvc;

/// @author Michael Heldmann, Jan 2003
/// updated by Xin Chen, Nov. 2016
/// updated by Simon Arnling B????th, Nov. 2017

class TauFilter : public GenFilter {
public:

  TauFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

  CLHEP::HepLorentzVector sumDaughterNeutrinos( HepMC::ConstGenParticlePtr tau );

private:

  ServiceHandle<IAtRndmGenSvc> m_rand; // Random number generator

  int m_Ntau;
  double m_etaMaxe;
  double m_etaMaxmu;
  double m_etaMaxhad;

  double m_pTmine;
  double m_pTminmu;
  double m_pTminhad;

  // new option variables:
  bool m_NewOpt;
  int m_Nleptau;
  int m_Nhadtau;
  double m_etaMaxlep;
  double m_pTminlep;
  double m_pTminlep_lead;
  double m_pTminhad_lead;
  bool m_ReverseFilter;
  bool m_HasTightRegion;
  double m_LooseRejectionFactor;
  double m_pTminlep_tight;
  double m_pTminlep_tight_lead;
  double m_pTminhad_tight;
  double m_pTminhad_tight_lead;
  int m_filterEventNumber;
  
  // Maximum amount of Taus variables:
  bool m_useMaxNTaus;
  int m_maxNtau;
  int m_maxNhadtau;
  int m_maxNleptau;
  
  //double m_eventsaccepted;
  //double m_eventsrefused;
  
  double m_events[6];
  double m_events_sel[6];

  double m_eventse;
  double m_eventsmu;
  double m_eventshad;

  double m_eventseacc;
  double m_eventsmuacc;
  double m_eventshadacc;

};

#endif
