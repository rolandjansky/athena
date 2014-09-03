/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOSAMPLINGDATA_P1_H
#define CALOATHENAPOOL_CALOSAMPLINGDATA_P1_H

#include <vector>

class CaloSamplingData_p1 {
 public:
  unsigned int m_varTypePattern;
  std::vector<std::vector<float> > m_dataStore;

};

#endif
