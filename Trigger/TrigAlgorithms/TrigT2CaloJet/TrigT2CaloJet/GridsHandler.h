/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_GRIDSHANDLER_H 
#define TRIGT2CALOJET_GRIDSHANDLER_H


/*!
  @brief Handles the separate grids for the different calorimeter compartments.

  April 2012
  davide.gerbaudo@gmail.com
*/

#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>

#include "TrigCaloEvent/Trig3Momentum.h"

class HashedTrig3Momentum;

typedef std::vector< HashedTrig3Momentum > Vmc;
typedef std::map< const CaloSampling::CaloSample, Vmc > CsVmcMap;
typedef std::pair<const CaloSampling::CaloSample, Vmc > CsVmcPair;
typedef std::vector< Trig3Momentum > Vt3m;

class GridsHandler {
 public:
  GridsHandler();
  Vmc& grid(const CaloSampling::CaloSample &sample);
  void clearAllGrids();
  void appendAllGrids(Vt3m &output) const;
 private:
  CsVmcMap m_grids;
  std::vector< CaloSampling::CaloSample > m_allSamples;
};

#endif

