/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_SOFTLEPTONINJETFILTER_H
#define GENERATORFILTERS_SOFTLEPTONINJETFILTER_H

#include "GeneratorModules/GenFilter.h"


/// @author A. Kaczmarska, F. Derue
class SoftLeptonInJetFilter : public GenFilter {
public:

  SoftLeptonInJetFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

protected:

  bool isElectron(HepMC::ConstGenParticlePtr p) const;
  bool isParton(HepMC::ConstGenParticlePtr p) const;

private:

  double m_Ptmin;
  double m_EtaRange;
  int m_NLeptons;
  int m_NPartons;
  double m_part_Ptmin;
  double m_part_EtaRange;
  int m_part_ID;
  double m_jet_cone;

};

#endif
