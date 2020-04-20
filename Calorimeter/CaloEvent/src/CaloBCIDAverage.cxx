/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef GENERATIONBASE
#include "CaloEvent/CaloBCIDAverage.h"


CaloBCIDAverage::CaloBCIDAverage(const LArMCSym* mcSym,
                                 CxxUtils::vec_aligned_vector<float>&& data):
  m_mcSym(mcSym),
  m_avg(std::move(data))
{
}
#endif
