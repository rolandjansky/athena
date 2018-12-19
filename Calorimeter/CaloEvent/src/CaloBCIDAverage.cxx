/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "CaloEvent/CaloBCIDAverage.h"


CaloBCIDAverage::CaloBCIDAverage(const LArMCSym* mcSym, std::unordered_map<unsigned, float>&& data):
  m_mcSym(mcSym),
  m_avg(data) {}

