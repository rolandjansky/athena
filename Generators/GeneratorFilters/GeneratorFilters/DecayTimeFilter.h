/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef GENERATORFILTERS_DECAYTIMEFILTER_H
#define GENERATORFILTERS_DECAYTIMEFILTER_H

#include "GeneratorModules/GenFilter.h"

class DecayTimeFilter : public GenFilter {
public:

  DecayTimeFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:
   double tau(HepMC::ConstGenParticlePtr ptr) const;
   float m_lifetimeLow;
   float m_lifetimeHigh;
   std::vector<int> m_particleID;
};



#endif
