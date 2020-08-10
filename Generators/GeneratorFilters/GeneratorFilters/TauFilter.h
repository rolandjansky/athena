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

  CLHEP::HepLorentzVector sumDaughterNeutrinos( HepMC::ConstGenParticlePtr tau );

private:

  int m_Ntau;
  double m_etaMaxe;
  double m_etaMaxmu;
  double m_etaMaxhad;

  double m_pTmine;
  double m_pTminmu;
  double m_pTminhad;

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
