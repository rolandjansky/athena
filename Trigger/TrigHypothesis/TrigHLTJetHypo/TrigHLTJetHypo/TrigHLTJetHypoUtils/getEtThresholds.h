/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_GETETTHRESHOLDS_H
#define TRIGHLTJETHYPO_GETETTHRESHOLDS_H

#include <vector>
std::vector<double> getEtThresholds(const std::vector<double>& dEtas,
                                    const std::vector<double>& etThresholds);
#endif
