/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMTobRoI_v2.h"

namespace xAOD{

  JEMTobRoI_v2::JEMTobRoI_v2() :
    SG::AuxElement()
  {
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JEMTobRoI_v2 , uint32_t , roiWord , setRoiWord )
  
  /// Return crate number (0-1)
  int JEMTobRoI_v2::crate()    const
  {
    return (roiWord() >> s_crateBit) & s_crateMask;  
  }
  
  /// Return JEM number (0-15)
  int JEMTobRoI_v2::jem()      const
  {
    return (roiWord() >> s_jemBit) & s_jemMask;  
  }
  
  /// Return RoI frame number (0-7)
  int JEMTobRoI_v2::frame()    const
  {
    return (roiWord() >> s_frameBit) & s_frameMask;  
  }
  
  /// Return location (RoI local coords) (0-3)
  int JEMTobRoI_v2::location() const
  {
    return (roiWord() >> s_locationBit) & s_locationMask; 
  }
  
  /// Return energy large window size
  int JEMTobRoI_v2::energyLarge() const
  {
    return (roiWord() >> s_energyLargeBit) & s_energyLargeMask;  
  }
  
  /// Return energy small window size
  int JEMTobRoI_v2::energySmall() const
  {
    return (roiWord() >> s_energySmallBit) & s_energySmallMask;  
  }

} // namespace xAOD

