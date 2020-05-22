/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_DecayPositionFilter_H
#define GENERATORFILTERS_DecayPositionFilter_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "AtlasHepMC/GenEvent.h"

#include "GeneratorModules/GenFilter.h"


class DecayPositionFilter : public GenFilter {
public:

  DecayPositionFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  double m_RCut;
  int    m_PDGID;
  int    m_MinPass;
};

#endif
