/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOCELLLINKCONTAINER_P2_H
#define CALOATHENAPOOL_CALOCELLLINKCONTAINER_P2_H

#include <vector>
#include <string> 
#include "DataModelAthenaPool/NavigableCnv_p1.h"

class CaloCellLinkContainer_p2 {
 public:
  std::vector<unsigned> m_linkI;
  std::vector<float> m_linkW;
  std::vector<unsigned> m_vISizes; // contains number of indices written in each cluster
  std::vector<unsigned> m_vWSizes; // contains number of weights written in each cluster
  unsigned m_nClusters;
  std::string m_contName;
};

#endif
