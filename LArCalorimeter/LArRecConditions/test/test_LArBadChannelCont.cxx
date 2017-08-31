/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArBadChannelCont.h"
#include "LArRecConditions/LArBadChannelEnum.h"


int main (int /*argc*/, char** /*argv[]*/) {

  LArBadChannelCont cont;

  HWIdentifier id1(0x39569400);
  LArBadChannel bc1(0x1 << LArBadChannelEnum::sporadicBurstNoiseBit |
		    0x1 << LArBadChannelEnum::highNoiseHGBit);
  cont.add(id1,bc1);

  HWIdentifier id2(0x39568f00);
  LArBadChannel bc2(0x1 << LArBadChannelEnum::deadReadoutBit);
  cont.add(id2,bc2);

  HWIdentifier id3(0x3956be00);
  LArBadChannel bc3(0x1 << LArBadChannelEnum::deadPhysBit);
  cont.add(id3,bc3);

  LArBadChannel bc2a(0x1 << LArBadChannelEnum::deadCalibBit);
  cont.add(id2,bc2a);
  
  
  cont.sort();
  
  const LArBadChanBitPacking packing;
  std::cout << "Status= " << packing.stringStatus(cont.status(id1)) << std::endl;
 
  if (!(cont.status(id1).sporadicBurstNoise() && cont.status(id1).highNoiseHG())) return 1;
  if (!(cont.status(id2).deadReadout() && cont.status(id2).deadCalib())) return 1;

  if (!cont.status(id3).deadPhys()) return 1;
      
  HWIdentifier id4(0x38c81700);
  if (!cont.status(id4).good()) return 1;



 return 0;
}
