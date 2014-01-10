/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_TRIGMINBIASNTUPLEALG_H
#define TRIGMINBIASNTUPLE_TRIGMINBIASNTUPLEALG_H

#include "TrigNtupleBase/NtupleAlgorithm.h"

class TrigMinBiasNtupleAlg: public NtupleAlgorithm {
  
 public:
  TrigMinBiasNtupleAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode    initialize();

};

#endif
