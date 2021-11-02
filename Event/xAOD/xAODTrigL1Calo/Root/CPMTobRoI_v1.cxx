/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTobRoI_v1.cxx 687286 2015-08-04 11:20:22Z amazurov $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTobRoI_v1.h"

namespace xAOD{

  CPMTobRoI_v1::CPMTobRoI_v1() :
    SG::AuxElement()
  {
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CPMTobRoI_v1 , uint32_t , roiWord , setRoiWord )
  
  void CPMTobRoI_v1::initialize(int crate, int cpm, int chip, int location,
      						  int type, int energy, int isol) {
          uint32_t roiWord  =  static_cast<uint32_t>(s_wordIdVal) << s_wordIdBit;
	  roiWord |= (type       & s_typeMask)       << s_wordIdBit;
	  roiWord |= (crate      & s_crateMask)      << s_crateBit;
	  roiWord |= (cpm        & s_cpmMask)        << s_cpmBit;
	  roiWord |= (chip       & s_chipMask)       << s_chipBit;
	  roiWord |= (location   & s_locationMask)   << s_locationBit;
	  roiWord |= (isol       & s_isolMask)       << s_isolBit;
	  roiWord |= (energy     & s_energyMask)     << s_energyBit;

	  setRoiWord(roiWord);
  }


  /// Return crate number (0-1)
  int CPMTobRoI_v1::crate()    const
  {
    return (roiWord() >> s_crateBit) & s_crateMask;  
  }
  
  /// Return CPM number (1-14)
  int CPMTobRoI_v1::cpm()      const
  {
    return (roiWord() >> s_cpmBit) & s_cpmMask;
  }
  
  /// Return CP chip number (0-7)
  int CPMTobRoI_v1::chip()     const
  {
    return (roiWord() >> s_chipBit) & s_chipMask; 
  }
  
  /// Return location (RoI local coords) (0-7)
  int CPMTobRoI_v1::location() const
  {
    return (roiWord() >> s_locationBit) & s_locationMask;  
  }
  
  /// Return type em/tau (0/1)
  int CPMTobRoI_v1::type()     const
  {
    return (roiWord() >> s_wordIdBit) & s_typeMask;  
  }
  
  /// Return energy
  int CPMTobRoI_v1::energy()   const
  {
    return (roiWord() >> s_energyBit) & s_energyMask;  
  }
  
  /// Return isolation
  int CPMTobRoI_v1::isolation() const
  {
    return (roiWord() >> s_isolBit) & s_isolMask;  
  }

} // namespace xAOD

