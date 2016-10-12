/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_TAUFILTER_H
#define GENERATORFILTERS_TAUFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "CLHEP/Vector/LorentzVector.h"


/// @author Michael Heldmann, Jan 2003
class TauFilter : public GenFilter {
public:

  TauFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

  CLHEP::HepLorentzVector sumDaughterNeutrinos( HepMC::GenParticle *tau );

private:

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
  double m_events[6];
  double m_events_sel[6];

  //double m_eventsaccepted;
  //double m_eventsrefused;

  double m_eventse;
  double m_eventsmu;
  double m_eventshad;

  double m_eventseacc;
  double m_eventsmuacc;
  double m_eventshadacc;

};

#endif
