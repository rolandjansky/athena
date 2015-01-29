/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTobRoI_v2.h"

namespace xAOD{

  CPMTobRoI_v2::CPMTobRoI_v2() :
    SG::AuxElement()
  {
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTobRoI_v2 , uint32_t , roiWord , setRoiWord )
  
  /// Return crate number (0-1)
  int CPMTobRoI_v2::crate()    const
  {
    return (roiWord() >> s_crateBit) & s_crateMask;  
  }
  
  /// Return CPM number (1-14)
  int CPMTobRoI_v2::cpm()      const
  {
    return (roiWord() >> s_cpmBit) & s_cpmMask;
  }
  
  /// Return CP chip number (0-7)
  int CPMTobRoI_v2::chip()     const
  {
    return (roiWord() >> s_chipBit) & s_chipMask; 
  }
  
  /// Return location (RoI local coords) (0-7)
  int CPMTobRoI_v2::location() const
  {
    return (roiWord() >> s_locationBit) & s_locationMask;  
  }
  
  /// Return type em/tau (0/1)
  int CPMTobRoI_v2::type()     const
  {
    return (roiWord() >> s_wordIdBit) & s_typeMask;  
  }
  
  /// Return energy
  int CPMTobRoI_v2::energy()   const
  {
    return (roiWord() >> s_energyBit) & s_energyMask;  
  }
  
  /// Return isolation
  int CPMTobRoI_v2::isolation() const
  {
    return (roiWord() >> s_isolBit) & s_isolMask;  
  }

} // namespace xAOD

