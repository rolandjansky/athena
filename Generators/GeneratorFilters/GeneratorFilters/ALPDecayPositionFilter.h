/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_ALPDecayPositionFilter_H
#define GENERATORFILTERS_ALPDecayPositionFilter_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "HepMC/GenEvent.h"

#include "GeneratorModules/GenFilter.h"


class ALPDecayPositionFilter : public GenFilter {
 public:

  ALPDecayPositionFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

 private:

  double m_RCutMax;
  double m_RCutMin;
  int    m_PDGID;
  int    m_MinPass;
};

#endif
