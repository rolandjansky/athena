/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_BarrelCode.h"
#include <string>
#include <sstream>

namespace InDetDD {
//
// Mask
//
const int TRT_BarrelCode::Z_MASK           = 0x00000001;    // lowest six bits
const int TRT_BarrelCode::MODULE_MASK      = 0x00000003;    // lowest two
const int TRT_BarrelCode::PHI_MASK         = 0x0000001F;    // lowest five
const int TRT_BarrelCode::STRAWLAYER_MASK  = 0x0000001F;    // lowest five

//
// Shifts
//
const int TRT_BarrelCode::Z_SHIFT           = 0;
const int TRT_BarrelCode::MODULE_SHIFT      = 1;
const int TRT_BarrelCode::PHI_SHIFT         = 3;
const int TRT_BarrelCode::STRAWLAYER_SHIFT  = 8;





TRT_BarrelCode::TRT_BarrelCode()
{
  m_key = 0;
}

TRT_BarrelCode::TRT_BarrelCode(unsigned isPos, unsigned moduleIndex, unsigned phiIndex, unsigned strawLayerIndex)
  :
  m_key ((( isPos        & Z_MASK )             << Z_SHIFT)      | 
	(( moduleIndex     & MODULE_MASK )     << MODULE_SHIFT) | 
	(( phiIndex        & PHI_MASK )        << PHI_SHIFT)    | 
	(( strawLayerIndex & STRAWLAYER_MASK ) << STRAWLAYER_SHIFT))
{
}

TRT_BarrelCode::TRT_BarrelCode(const TRT_BarrelCode &right)
{
  m_key = right.getKey();
}




TRT_BarrelCode::~TRT_BarrelCode()
{
}


TRT_BarrelCode & TRT_BarrelCode::operator=(const TRT_BarrelCode &right)
{ 
  if (this != &right){
    m_key = right.getKey();
  }
  return *this;
}



std::string TRT_BarrelCode::getName() const {
  std::ostringstream outputStream;
  outputStream << this;
  return outputStream.str();
}

}
