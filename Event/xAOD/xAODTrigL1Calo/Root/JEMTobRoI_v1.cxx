/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMTobRoI_v1.cxx 652818 2015-03-09 22:54:24Z morrisj $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMTobRoI_v1.h"

namespace xAOD{

  JEMTobRoI_v1::JEMTobRoI_v1() :
    SG::AuxElement()
  {
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JEMTobRoI_v1 , uint32_t , roiWord , setRoiWord )
  
  /// Return crate number (0-1)
  int JEMTobRoI_v1::crate()    const
  {
    return (roiWord() >> s_crateBit) & s_crateMask;  
  }
  
  /// Return JEM number (0-15)
  int JEMTobRoI_v1::jem()      const
  {
    return (roiWord() >> s_jemBit) & s_jemMask;  
  }
  
  /// Return RoI frame number (0-7)
  int JEMTobRoI_v1::frame()    const
  {
    return (roiWord() >> s_frameBit) & s_frameMask;  
  }
  
  /// Return location (RoI local coords) (0-3)
  int JEMTobRoI_v1::location() const
  {
    return (roiWord() >> s_locationBit) & s_locationMask; 
  }
  
  /// Return energy large window size
  int JEMTobRoI_v1::energyLarge() const
  {
    return (roiWord() >> s_energyLargeBit) & s_energyLargeMask;  
  }
  
  /// Return energy small window size
  int JEMTobRoI_v1::energySmall() const
  {
    return (roiWord() >> s_energySmallBit) & s_energySmallMask;  
  }

} // namespace xAOD

