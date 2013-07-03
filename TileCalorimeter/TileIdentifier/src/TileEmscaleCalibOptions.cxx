/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileIdentifier/TileEmscaleCalibOptions.h"
#include <sstream>

//
//_____________________________________________________
void
TileEmscaleCalibOptions::setEmscaleCalibBit(CalBitPos idx, bool turnOn)
{
  if(turnOn) m_options |=  static_cast<uint32_t>(1<<idx);
  else       m_options &= ~static_cast<uint32_t>(1<<idx);
}

//
//_____________________________________________________
TileEmscaleCalibOptions::TileEmscaleCalibOptions() : m_options(0) 
{
  initBitToProb();
}

//
//_____________________________________________________
TileEmscaleCalibOptions::TileEmscaleCalibOptions(uint32_t options) 
{
  m_options = options;
  initBitToProb();
}

//
//_____________________________________________________
void
TileEmscaleCalibOptions::initBitToProb()
{
  m_bitToProb.clear();
  m_bitToProb.resize(32,"Undefined");
  m_bitToProb[OflCisFitLin]  = "OflCisFitLin"; 
  m_bitToProb[OflCisFitNln]  = "OflCisFitNln";  
  m_bitToProb[OflLasLin]     = "OflLasLin";	 
  m_bitToProb[OflLasNln]     = "OflLasNln";	 
  m_bitToProb[OflLasPart]    = "OflLasPart";    
  m_bitToProb[OflCes]        = "OflCes"; 	 
  m_bitToProb[OflEms]        = "OflEms";   	 
  m_bitToProb[OflLasLinPisa] = "OflLasLinPisa"; 
  m_bitToProb[OflLasNlnPisa] = "OflLasNlnPisa"; 
  m_bitToProb[OflMinbias]    = "OflMinbias";    
  m_bitToProb[OnlCisLin]     = "OnlCisLin";	 
  m_bitToProb[OnlLasLin]     = "OnlLasLin";	 
  m_bitToProb[OnlCes]        = "OnlCes"; 	 
  m_bitToProb[OnlEms]        = "OnlEms";        
}

//
//_____________________________________________________
void
TileEmscaleCalibOptions::dumpEmscaleCalibOptions(std::ostringstream& stream) const
{
  stream << std::endl << "---===### Emscale Calib Options ###===---" << std::endl;
  for(uint32_t idx = 0; idx < MaxBit; ++idx) {
    if(getEmscaleCalibBit(static_cast<CalBitPos>(idx))) {
      stream << m_bitToProb[idx] << ": ON" << std::endl;
    } else {
      stream << m_bitToProb[idx] << ": OFF" << std::endl;
    }
  }
  stream << "---===### END OF Emscale Calib Options ###===---" << std::endl << std::endl;
}











  
