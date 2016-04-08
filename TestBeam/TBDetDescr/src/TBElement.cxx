/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//========================================
// file TBElement.cxx
// 
//========================================

#include "TBDetDescr/TBElement.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#define MAKE_STRING( msg )  ( ((std::ostringstream&)(std::ostringstream() << msg)).str() )

TBElement::TBElement ()
  : m_id(TBElementID::Unknown)
{
}

TBElement::TBElement ( TBElementID::TBElementID id, std::string name,
                       Amg::Vector3D position, Amg::RotationMatrix3D rotation )
  : m_id(id)
  , m_name(name)
  , m_position(position)
  , m_rotation(rotation)
{
}

TBElement::operator std::string() const
{
  std::ostringstream buf;

  buf << "TBElement"
      << "  id = " << m_id
      << "  name = " << m_name
      << "  position = " << m_position
      << "  Rotation = " << m_rotation ;

  return buf.str();
}
