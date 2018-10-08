/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelEnum_H
#define LArBadChannelEnum_H

class LArBadChannelEnum {
 public:

  //typedef unsigned int      BitPackedWord;
  typedef unsigned int      PosType;
  typedef unsigned int      BitWord;

  enum ProblemType {
    deadReadoutBit = 0, 
    deadCalibBit = 1, 
    deadPhysBit = 2, 
    almostDeadBit = 3, 
    shortBit = 4, 
    unstableBit = 5,
    distortedBit = 6,
    lowNoiseHGBit = 7,
    highNoiseHGBit = 8,
    unstableNoiseHGBit = 9,
    lowNoiseMGBit = 10,
    highNoiseMGBit = 11,
    unstableNoiseMGBit = 12,
    lowNoiseLGBit = 13,
    highNoiseLGBit = 14,
    unstableNoiseLGBit = 15,
    missingFEBBit = 16,
    peculiarCalibrationLineBit = 17,
    problematicForUnknownReasonBit = 18,
    sporadicBurstNoiseBit = 19,
    deadSCACellBit = 20,
    badFirstSampleBit = 21,
    unflaggedByLADIeSBit = 22
  };

  ///  const static BitWord HighGainMask;
  ///  const static BitWord MediumGainMask;
  ///  const static BitWord LowGainMask;

};

#endif

