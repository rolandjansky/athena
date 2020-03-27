/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef GENERATORFILTERS_LEADINGCHARGEDPARTICLEFILTER_H
#define GENERATORFILTERS_LEADINGCHARGEDPARTICLEFILTER_H

#include "GeneratorModules/GenFilter.h"


/// @brief Filter events based on the pT of the hardest charged particle within a given |eta|
class LeadingChargedParticleFilter : public GenFilter {
 public:

  LeadingChargedParticleFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterEvent();

 private:

  double m_Ptmin;
  double m_Ptmax;
  double m_EtaRange;

};


#endif
