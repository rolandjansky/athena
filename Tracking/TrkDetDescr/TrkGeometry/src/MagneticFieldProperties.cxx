/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MagneticFieldProperties.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkGeometry/MagneticFieldProperties.h"

Trk::MagneticFieldProperties::MagneticFieldProperties(Trk::MagneticFieldMode mode) :
  m_magneticFieldMode(mode),
  m_magneticField(0., 0., 0.)
{}

Trk::MagneticFieldProperties::MagneticFieldProperties(const Amg::Vector3D& field) :
  m_magneticFieldMode(Trk::ConstantField),
  m_magneticField(field)
{}


Trk::MagneticFieldProperties::MagneticFieldProperties(const Trk::MagneticFieldProperties& magprop) :
  m_magneticFieldMode(magprop.m_magneticFieldMode),
  m_magneticField(magprop.m_magneticField)
{}  

Trk::MagneticFieldProperties& Trk::MagneticFieldProperties::operator=(const Trk::MagneticFieldProperties& magprop)
{
  if (this != &magprop){
     m_magneticFieldMode = magprop.m_magneticFieldMode;
     m_magneticField     = magprop.m_magneticField;
  }
  return(*this);
}


 
/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator<<( MsgStream& sl, const Trk::MagneticFieldProperties& mprop)
{
   sl << "Trk::MagneticFieldProperties, configuration: " << mprop.magneticFieldMode() << endreq; 
   return sl;
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::MagneticFieldProperties& mprop)
{
   sl << "Trk::MagneticFieldProperties, configuration: " << mprop.magneticFieldMode() << std::endl;
   return sl;
}
