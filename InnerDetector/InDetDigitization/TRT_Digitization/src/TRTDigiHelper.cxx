/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTDigiHelper.h"
#include <iostream>
#include "GaudiKernel/MsgStream.h"

namespace TRTDigiHelper {
  // The straw's gas mix: 1=Xe, 2=Kr, 3=Ar
  int StrawGasType(int statusHT, int useGasMix, MsgStream* log) {

    // TRT/Cond/StatusHT provides:
    // enum { Undefined, Dead(Ar), Good(Xe), Xenon(Xe), Argon(Ar), Krypton(Kr) , EmulatedArgon(Xe, see below) }
    // The useGasMix default behaviour (0) is to use TRT/Cond/StatusHT, other values can be set to force
    // the whole detector to (1)Xenon, (2)Krypton, (3)Argon:

    int strawGasType=99;

    if (useGasMix==0) { // use StatusHT

      if      ( statusHT==2 || statusHT==3 ) { strawGasType = 0; } // Xe
      else if ( statusHT==5 )                { strawGasType = 1; } // Kr
      else if ( statusHT==1 || statusHT==4 ) { strawGasType = 2; } // Ar
      else if ( statusHT==6 )                { strawGasType = 0; } // Xe
      else if ( statusHT==7 )                { strawGasType = 0; } // Xe
      // statusHT==6 is emulate argon, make it xenon here,
      // and emulate argon later with reduced TR eff.
      else {
        if (log) { *log << MSG::FATAL << "StrawGasType: StrawStatus = " << statusHT
                        << ", must be 'Good(2)||Xenon(3)' or 'Dead(1)||Argon(4)' or 'Krypton(5)!'"
                        << ", or 'EmulateArgon(6)' or 'EmulateKrypton(7)'" << endmsg; }
      }
    }
    else if (useGasMix==1) { strawGasType = 0; } // force whole detector to Xe
    else if (useGasMix==2) { strawGasType = 1; } // force whole detector to Kr
    else if (useGasMix==3) { strawGasType = 2; } // force whole detector to Ar

    if ( strawGasType<0 || strawGasType>2 ) {
      if (log) { *log << MSG::FATAL << "StrawGasType: strawGasType value (" << strawGasType << ") must be 0(Xe), 1(Kr) or 2(Ar)!" << endmsg; }
      throw std::exception();
    }

    return strawGasType;

  }

  unsigned int getRegion(int hitID) {

    // 1=barrelShort, 2=barrelLong, 3=ECA, 4=ECB
    const int mask(0x0000001F);
    const int word_shift(5);
    int layerID, ringID, wheelID;
    unsigned int region(0);

    if ( !(hitID & 0x00200000) ) { // barrel

      hitID >>= word_shift;
      layerID = hitID & mask;
      hitID >>= word_shift;
      hitID >>= word_shift;
      ringID = hitID & mask;
      region = ( (layerID < 9) && (ringID == 0) ) ? 1 : 2;

    } else { // endcap

      hitID >>= word_shift;
      hitID >>= word_shift;
      hitID >>= word_shift;
      wheelID = hitID & mask;
      region = wheelID < 8 ?  3 : 4;

    }

    return region;

  }
}
