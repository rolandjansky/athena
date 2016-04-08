/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOCELLLINKCONTAINER_P1_H
#define CALOATHENAPOOL_CALOCELLLINKCONTAINER_P1_H

#include <vector>
#include "DataModelAthenaPool/NavigableCnv_p1.h"
#include "DataModelAthenaPool/Navigable_p1.h"

class CaloCellLinkContainer_p1 {
 public:
  
  typedef Navigable_p1<unsigned int,float> CellLink;
  std::vector<CellLink> m_links;
};


#endif
