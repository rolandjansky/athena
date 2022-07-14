/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

  Gaudi::Property<double> m_RCut{this, "RCut", 400.};
  Gaudi::Property<int>    m_PDGID{this, "PDG", 3000001};
  Gaudi::Property<int>    m_MinPass{this, "MinPass", 2};
  Gaudi::Property<int>    m_simBarcodeOffset{this, "SimBarcodeOffset",200000};
};

#endif
