/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisUtils/ParticleBaseSelector.h"

using CLHEP::GeV;
using namespace std;

ParticleBaseSelector::ParticleBaseSelector(const std::string& name,  ISvcLocator* pSvcLocator):
  SelectorBase<ParticleBaseSelector>(name, pSvcLocator)
{
  // these parameters just for example
  declareProperty("EMin", m_Emin = 15.0);
  declareProperty("EMax", m_Emax = 10000.0);
}

StatusCode ParticleBaseSelector::initialize()
{
  // print parameters in SelectorBase
  StatusCode sc = SelectorBase<ParticleBaseSelector>::initialize();

  // print parameters in ParticleBaseSelector
  (*mLog)<< MSG::INFO<<", PtMin="<<m_Emin;
  (*mLog)<< MSG::INFO<<", PtMax="<<m_Emax;
  return sc;
}

bool ParticleBaseSelector::accept(const ParticleBaseLink& link){
  const Contained* cpart=*link;
  Contained* part=const_cast<Contained*>(cpart);
  return accept(part);
}

bool ParticleBaseSelector::accept(const Contained* part)
{
  // kinematic cuts
  double pt=part->pt()/GeV;
  double eta=part->eta();
  if(!pt_eta_range(pt,eta)) return false;
  double E=part->e()/GeV;
  if(E>m_Emax || E<m_Emin) return false;

  return true;
}

