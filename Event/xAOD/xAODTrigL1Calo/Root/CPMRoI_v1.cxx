/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMRoI_v1.cxx 638578 2015-01-09 17:00:15Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMRoI_v1.h"

namespace xAOD{  
  
  CPMRoI_v1::CPMRoI_v1() :
    SG::AuxElement()
  {        
  }
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMRoI_v1 ,  uint32_t , roiWord , setRoiWord )   
  
  /// Return crate number (0-1)
  int CPMRoI_v1::crate() const
  {
    return (roiWord() >> s_crateBit) & s_crateMask;  
  }
  
  /// Return CPM number (1-14)
  int CPMRoI_v1::cpm() const
  {
    return (roiWord() >> s_cpmBit) & s_cpmMask;  
  }
  
  /// Return CP chip number (0-7)
  int CPMRoI_v1::chip() const
  {
    return (roiWord() >> s_chipBit) & s_chipMask;  
  }
  
  /// Return location (RoI local coords) (0-7)
  int CPMRoI_v1::location() const
  {
    return (roiWord() >> s_locationBit) & s_locationMask;  
  }
  
  /// Return hit thresholds map
  int CPMRoI_v1::hits() const
  {
    return (roiWord() >> s_hitsBit) & s_hitsMask; 
  }
  
  /// Return error flags (bit 0 Saturation, bit 1 Parity)
  int CPMRoI_v1::error() const
  {
    return (parity() << 1) | saturation();  
  }
  
  /// Return parity error flag (0/1)
  int CPMRoI_v1::parity() const
  {
    return (roiWord() >> s_parityBit) & s_parityMask;  
  }
  
  /// Return saturation flag (0/1)
  int CPMRoI_v1::saturation() const
  {
    return (roiWord() >> s_saturationBit) & s_saturationMask;  
  }
  
} // namespace xAOD
