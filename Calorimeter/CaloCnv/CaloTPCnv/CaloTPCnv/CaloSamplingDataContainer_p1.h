/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOSAMPLINGDATACONTAINER_P1_H
#define CALOATHENAPOOL_CALOSAMPLINGDATACONTAINER_P1_H

#include <vector>

class CaloSamplingDataContainer_p1 {
 public:
  //unsigned int m_varTypePattern; Move back to individual cluster
  std::vector<unsigned int> m_varTypePatterns;
  std::vector<float> m_dataStore;
  typedef std::vector<float>::const_iterator const_iterator;
};

#endif
