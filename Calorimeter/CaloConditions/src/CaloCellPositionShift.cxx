/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloConditions/CaloCellPositionShift.h"

namespace CaloRec {

CaloCellPositionShift::CaloCellPositionShift(const size_type& size) : m_deltaX(size,0.),m_deltaY(size,0.),m_deltaZ(size,0.)
{} 

void CaloCellPositionShift::resize(const size_type& size) {
 m_deltaX.resize(size,0.0);
 m_deltaY.resize(size,0.0);
 m_deltaZ.resize(size,0.0);
 return;
}

}//end namespace
