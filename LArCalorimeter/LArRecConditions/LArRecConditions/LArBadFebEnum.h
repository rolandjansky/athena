/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadFebEnum_H
#define LArBadFebEnum_H

class LArBadFebEnum {
 public:

  //typedef unsigned int      BitPackedWord;
  typedef unsigned int      PosType;
  typedef unsigned int      BitWord;

  enum ProblemType {
    deadAllBit = 0, 
    deadReadoutBit = 1, 
    inErrorBit = 2, 
    deactivatedInOKSBit = 3,   //From LArFebErrorSummary.h:
    maskParity         = 16,   //Parity,		 
    maskBCID            =17,   //BCID,		 
    maskSampleHeader   = 18,   //SampleHeader,	 
    maskEVTID          = 19,   //EVTID,		 
    maskScacStatus     = 20,   //ScacStatus,	 
    maskScaOutOfRange  = 21,   //ScaOutOfRange,	 
    maskGainMismatch   = 22,   //GainMismatch,	 
    maskTypeMismatch   = 23,   //TypeMismatch,	 
    maskNumOfSamples   = 24,   //NumOfSamples,	 
    maskEmptyDataBlock = 25,   //EmptyDataBlock,	 
    maskDspBlockSize   = 26,   //DspBlockSize,	 
    maskCheckSum       = 27,   //CheckSum, 	 
    maskMissingHeader  = 28,    //MissingHeader, 	 
    maskBadGain        = 29    //BadGain, 	 
                               //N_LArFebErrorType
  };			       
			       
};

#endif

