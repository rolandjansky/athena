/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef GENERATORFILTERS_XAODDECAYTIMEFILTER_H
#define GENERATORFILTERS_XAODDECAYTIMEFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

class xAODDecayTimeFilter : public GenFilter {
public:

  xAODDecayTimeFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:
   double tau(const xAOD::TruthParticle* ptr) const;
   float m_lifetimeLow;
   float m_lifetimeHigh;
   std::vector<int> m_particleID;
};



#endif