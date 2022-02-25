/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ReadoutGeometry/TRT_Conditions.h"

namespace InDetDD {

TRT_Conditions::TRT_Conditions()
  : m_dxContainer(nullptr)
{}

const TRTCond::StrawDxContainer* 
TRT_Conditions::dxContainer() const
{
  return m_dxContainer;
}

void 
TRT_Conditions::setDxContainer(const TRTCond::StrawDxContainer* container) const
{
  m_dxContainer = container;
}


} // end namespace
