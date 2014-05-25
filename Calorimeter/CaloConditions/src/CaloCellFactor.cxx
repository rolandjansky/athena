/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloConditions/CaloCellFactor.h"

namespace CaloRec {

CaloCellFactor::CaloCellFactor(const size_type& size) : m_cellFactor(size,1.0) 
{} 

void CaloCellFactor::resize(const size_type& size) {
 m_cellFactor.resize(size,1.0);
 return;
}

}//end namespace
