/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXCompression - Energy encoder/decoder for gFEX
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************
#ifndef gFEXCompression_H
#define gFEXCompression_H

namespace LVL1 {

/**
LAr supercell data are received by the gFEX in a 10-bit multi-linear encoded form.
This simple utility class contains 3 functions:
  - Compress:  encodes a signed integer MeV value
  - Expand:    decodes a 10 bit unsigned int into a signed integer MeV value
  - Threshold: applies a threshold (in MeV) to the compressed code, zeroing if below
  - Linearize: decodes a 10 bit unsigned int into the unsigned 16 bit gFEX ET with
               least count of 25 MeV. A user-defined threshold (in MeV) can be
               applied, but as the gFEX ET is positive a negative threshold is
               equivalent to a threshold of 0.
  */
class gFEXCompression {

public:
  /** Compress data */
  static unsigned int compress(int Et);
  /** Uncompress data */
  static int expand(unsigned int code);
  /** Apply threshold to compressed data */
  static unsigned int threshold(unsigned int code, int threshold = -101200);
  /** Linearize LAr code to eFEX internal format */
  static unsigned int linearize(unsigned int code, int threshold = 0);

private:
  /** Maximum ET value that can be encoded */
  static const int s_maxET = 1019200;
  /** Number of ranges */
  static const unsigned int s_nRanges = 6;
  /** Step sizes in each range, MeV */
  static const int s_steps[s_nRanges];
  /** Minimum ET values in each range, MeV */
  static const int s_minET[s_nRanges];
  /** Minimum code value in each range */
  static const int s_minCode[s_nRanges];
  /** Indicates no data present */
  static const int s_NoData = 0;
  /** LAr underflow code */
  static const unsigned int s_LArUnderflow = 1;
  /** LAr overflow code */
  static const unsigned int s_LArOverflow  = 4062;
  /** Reserved code min value */
  static const unsigned int s_LArReserved_min  = 4063;
  /** Reserved code max value */
  static const unsigned int s_LArReserved_max  = 4094;
  /** Invalid code value */
  static const unsigned int s_LArInvalid   = 4095;
  /** LAr saturated code */
  //static const unsigned int s_LArSaturated = 1023;
  /** Maximum code value */
  static const unsigned int s_LArMaxCode   = 4095;
  /** L1Calo ET digit step */
  static const unsigned int s_gFEXstep     = 200;
  /** L1Calo saturated/overflow */
  static const unsigned int s_gFEXOverflow = 0xffff;
  /** Error return value */
  static const int s_error = -999;
};


}//end of ns

#endif
