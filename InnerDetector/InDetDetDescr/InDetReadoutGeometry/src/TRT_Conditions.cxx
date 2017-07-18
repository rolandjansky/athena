/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_Conditions.h"

namespace InDetDD {

TRT_Conditions::TRT_Conditions()
  : m_dxContainer(0)
{}

const TRTCond::StrawDxContainer* 
TRT_Conditions::dxContainer() const
{
  return m_dxContainer;
}

void 
TRT_Conditions::setDxContainer(const TRTCond::StrawDxContainer* container)
{
  m_dxContainer = container;
}

const HepGeom::Transform3D &
TRT_Conditions::solenoidFrame() const
{
  return m_solenoidFrame;
}
 
void 
TRT_Conditions::setSolenoidFrame(const HepGeom::Transform3D & frame)
{
  m_solenoidFrame = frame;
}

} // end namespace
