/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#include "Pythia_i/Pythia.h"
#include "GeneratorModules/GenModule.h"

class PythiaTau3Mu:public Pythia {
 public:
  PythiaTau3Mu(const std::string& name, ISvcLocator* pSvcLocator);
  ~PythiaTau3Mu();
};
