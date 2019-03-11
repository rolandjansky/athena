/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_Conditions.h"

namespace InDetDD {

TRT_Conditions::TRT_Conditions()
  : m_dxContainer(0),
    m_mutex{}
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
  std::lock_guard<std::mutex> lock{m_mutex};
  return m_solenoidFrame;
  // This reference might be changed by setSolenoidFrame.
  // However, it occurrs very rarely.
}
 
void 
TRT_Conditions::setSolenoidFrame(const HepGeom::Transform3D & frame)
{
  std::lock_guard<std::mutex> lock{m_mutex};
  m_solenoidFrame = frame;
}

} // end namespace
