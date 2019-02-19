/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMTobRoI_v1.cxx 685662 2015-07-27 10:30:15Z amazurov $

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


  void JEMTobRoI_v1::initialize(const int crate, const int jem, const int frame,
        const int location, const int energyLarge, const int energySmall ) {
      uint32_t roiWord  =  s_wordIdVal             << s_wordIdBit;
      roiWord |= (crate       & s_crateMask)       << s_crateBit;
      roiWord |= (jem         & s_jemMask)         << s_jemBit;
      roiWord |= (frame       & s_frameMask)       << s_frameBit;
      roiWord |= (location    & s_locationMask)    << s_locationBit;
      roiWord |= (energySmall & s_energySmallMask) << s_energySmallBit;
      roiWord |= (energyLarge & s_energyLargeMask) << s_energyLargeBit;

      setRoiWord(roiWord);
  }
  
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


  std::ostream &operator<<(std::ostream &os, const xAOD::JEMTobRoI_v1 &el)
  {
    os << "xAOD::JEMTobRoI crate=" << int(el.crate())
       << " jem=" << el.jem()
       << " frame=" << el.frame()
       << " location=" << el.location()
       << " energyLarge=" << el.energyLarge()
       << " energySmall=" << el.energySmall();
    return os;
  }


} // namespace xAOD

