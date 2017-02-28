/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/SiCommonItems.h"

#include "InDetCondServices/ISiLorentzAngleSvc.h"

namespace InDetDD {

SiCommonItems::SiCommonItems(const AtlasDetectorID* const idHelper)
  :  m_msg("SiDetectorElement"),
     m_idHelper(idHelper), 
     m_lorentzAngleSvcHandle("", "SiDetectorElement"),
     m_lorentzAngleSvc(0),
     m_lorentzAngleSvcInit(false)
{}

void   
SiCommonItems::setSolenoidFrame(const HepGeom::Transform3D & transform) const
{
  m_solenoidFrame = transform;
}

void 
SiCommonItems::setLorentzAngleSvc(const ServiceHandle<ISiLorentzAngleSvc> & lorentzAngleSvc)
{
  m_lorentzAngleSvcHandle = lorentzAngleSvc;
}
  
ISiLorentzAngleSvc * 
SiCommonItems::lorentzAngleSvc() const 
{
  if (!m_lorentzAngleSvcInit) {
    if (!m_lorentzAngleSvcHandle.empty()) {
      StatusCode sc = m_lorentzAngleSvcHandle.retrieve(); 
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "InDetReadoutGeometry ERROR: Could not locate Lorentz angle service: " << m_lorentzAngleSvcHandle.name() << endmsg;
      }
    }
    m_lorentzAngleSvc =  m_lorentzAngleSvcHandle.get();
    m_lorentzAngleSvcInit = true;
  }
  return m_lorentzAngleSvc;
}


} // End namespace InDetDD
