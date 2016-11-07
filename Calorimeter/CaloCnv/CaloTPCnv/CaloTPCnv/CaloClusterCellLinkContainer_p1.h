/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOCLUSTERCELLLINKCONTAINER_P1_H
#define CALOATHENAPOOL_CALOCLUSTERCELLLINKCONTAINER_P1_H

#include <vector>
#include <string> 
#include "DataModelAthenaPool/DataLink_p2.h"

class CaloClusterCellLinkContainer_p1 {
 public:
  std::vector<unsigned> m_indices;
  std::vector<float> m_weights;
  std::vector<unsigned> m_nCellsPerCluster; //Number of cells in each cluster
  DataLink_p2 m_cellCont;
};

#endif
