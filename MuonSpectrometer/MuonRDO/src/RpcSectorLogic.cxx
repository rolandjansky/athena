/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcSectorLogic.h"

void RpcSectorLogic::addCounter(uint16_t counter) { 
  m_counters.push_back(counter);
}

