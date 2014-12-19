/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_EndcapCode.h"
#include <string>
#include <sstream>

namespace InDetDD {

//
// Mask
//
const int TRT_EndcapCode::Z_MASK           = 0x00000001;    // lowest six bits
const int TRT_EndcapCode::WHEEL_MASK       = 0x0000001F;    // lowest five
const int TRT_EndcapCode::STRAWLAYER_MASK  = 0x0000001F;    // lowest five
const int TRT_EndcapCode::PHIPOS_MASK      = 0x0000001F; 

//
// Shifts
//
const int TRT_EndcapCode::Z_SHIFT           = 0;
const int TRT_EndcapCode::WHEEL_SHIFT       = 1;
const int TRT_EndcapCode::STRAWLAYER_SHIFT  = 6;
const int TRT_EndcapCode::PHIPOS_SHIFT      = 11;

TRT_EndcapCode::TRT_EndcapCode()
{
  m_key = 0;
}

TRT_EndcapCode::TRT_EndcapCode(unsigned isPos, unsigned wheelIndex, unsigned strawLayerIndex, unsigned int phiIndex)
  :
  m_key ((( isPos           & Z_MASK )          << Z_SHIFT)          | 
	(( wheelIndex      & WHEEL_MASK )      << WHEEL_SHIFT)      | 
	(( strawLayerIndex & STRAWLAYER_MASK ) << STRAWLAYER_SHIFT) |
	(( phiIndex        & PHIPOS_MASK )     << PHIPOS_SHIFT))
{
}

TRT_EndcapCode::TRT_EndcapCode(const TRT_EndcapCode &right)
{
  m_key = right.getKey();
}




TRT_EndcapCode::~TRT_EndcapCode()
{
}


TRT_EndcapCode & TRT_EndcapCode::operator=(const TRT_EndcapCode &right)
{
  if (this != &right){
    m_key = right.getKey();
  }
  return *this;
}



std::string TRT_EndcapCode::getName() const {
  std::ostringstream outputStream;
  outputStream << this;
  return outputStream.str();
}

}
