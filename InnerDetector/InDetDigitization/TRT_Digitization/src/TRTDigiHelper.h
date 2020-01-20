/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DIGITIZATION_TRTDIGIHELPER_H
#define TRT_DIGITIZATION_TRTDIGIHELPER_H

class MsgStream;

namespace TRTDigiHelper {
  // The straw's gas mix: 1=Xe, 2=Kr, 3=Ar
  int StrawGasType(int statusHT, int useGasMix, MsgStream* msg=nullptr);

  unsigned int getRegion(int hitID);
}

#endif // TRT_DIGITIZATION_TRTDIGIHELPER_H
