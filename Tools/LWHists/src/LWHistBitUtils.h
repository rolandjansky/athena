/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWHistBitUtils                      //
//                                                            //
//  Description: Helper for various bit-operations.           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWHISTBITUTILS_H
#define LWHISTBITUTILS_H

#include <cassert>
#include <stdint.h>
#include <cstring>

class LWHistBitUtils {
public:

  template <class T> static T countSetBits(T data);
  template <class T> static T countSetBitsBefore(T data, T ibit);
  template <class T> static bool bitIsSet(T data, T ibit) { return data & (0x1<<ibit); }
  template <class T> static void setBit(T& data, T ibit) { data |= (0x1<<ibit); }

  template <uint8_t sizeofT> static uint8_t totalSummedOffsetInStages(uint32_t stagepattern );

  //Get positions of first and last set bits (1..32) (assumes i!=0):
  static uint32_t posMSB(uint32_t i);//position of most significant bit
  static uint32_t posLSB(uint32_t i);//position of least significant bit
  //8 bit versions:
  static uint8_t posMSB(uint8_t i);
  static uint8_t posLSB(uint8_t i);

  //Fast version of std::max<int>(0,x):
  // This relies on implementation and undefined behavior.
  // On x86_64, the naive way of writing this will anyway compile to a conditional
  // move, so this is likely no faster.  In any case, the only place this is used
  // is from within a nest of other conditionals, so i really doubt that this
  // will make any difference at all.  So go back to the simple
  // way of writing this.
  //static int minZero(int x) { assert(int(x-(x&x>>(sizeof(int)*8-1)))==std::max<int>(0,x)); return x-(x&x>>(sizeof(int)*8-1)); }
  static int minZero(int x) { return std::max<int>(0, x); }

private:
  template <uint8_t sizeofT> static uint8_t stageOffset8Bits(uint32_t stagepattern);

  LWHistBitUtils(){}
  ~LWHistBitUtils(){}
};

#include "LWHistBitUtils.icc"

#endif
