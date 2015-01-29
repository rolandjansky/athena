/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMRoI_v1.cxx 638578 2015-01-09 17:00:15Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMRoI_v1.h"

namespace xAOD{  
    
  JEMRoI_v1::JEMRoI_v1() :
    SG::AuxElement()
  {        
  }
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JEMRoI_v1 , uint32_t , roiWord , setRoiWord )    
  
  /// Return crate number (0-1)
  int JEMRoI_v1::crate()    const
  {
    return (roiWord() >> s_crateBit) & s_crateMask;    
  }
  
  /// Return JEM number (0-15)
  int JEMRoI_v1::jem()      const
  {
    return (roiWord() >> s_jemBit) & s_jemMask;    
  }
  
  /// Return RoI frame number (0-7)
  int JEMRoI_v1::frame()    const
  {
    return (roiWord() >> s_frameBit) & s_frameMask;    
  }
  
  /// Return location (RoI local coords) (0-3)
  int JEMRoI_v1::location() const
  {
    return (roiWord() >> s_locationBit) & s_locationMask;    
  }
  
  /// Return forward jet flag (0/1)
  int JEMRoI_v1::forward()  const
  {
    return (roiWord() >> s_forwardBit) & s_forwardMask;       
  }
  
  /// Return Jet hit map (8 bits Main or 4 bits Forward)
  int JEMRoI_v1::hits()     const
  {
    int hits(0);
    if(forward()){
      hits = (roiWord() >> s_forwardHitsBit) & s_forwardHitsMask;
    }
    else{
      hits = (roiWord() >> s_mainHitsBit)    & s_mainHitsMask;
    }
    return hits;    
  }
  
  /// Return error flags (bit 0 Saturation, bit 1 Parity)
  int JEMRoI_v1::error()    const
  {
    return (parity() << 1) | saturation();    
  }
  
  /// Return parity error flag (0/1)
  int JEMRoI_v1::parity()     const
  {
    return (roiWord() >> s_parityBit) & s_parityMask;    
  }
  
  /// Return saturation flag (0/1)
  int JEMRoI_v1::saturation() const
  {
    return (roiWord() >> s_saturationBit) & s_saturationMask;    
  }
  
} // namespace xAOD
