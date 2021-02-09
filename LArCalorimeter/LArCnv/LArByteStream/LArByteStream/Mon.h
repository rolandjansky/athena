/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file Mon.h
 * @author Gen Tateno
 * @date November 2019
 * @brief Tool to store LATOME mon header and footer data.
 */

#ifndef MON_H
#define MON_H
#include <map>
#include "LArByteStream/LArLATOMEDecoder.h"

#define MON_HEADER_SIZE 18
/** @brief Tool to store LATOME mon header and footer data.*/
class Mon
{
 public:
  Mon();
  ~Mon();
  void fillPacketInfo(uint32_t word);
  void fillHeaderSize(uint32_t word);
  void fillRegion(uint32_t word);
  void fillNStreams(uint32_t word);
  void fillStreamNumber(uint32_t word);
  void fillDataType(uint32_t word, int i);
  void fillNBCs(uint32_t word, int i);
  void fillTimeShift(uint32_t word, int i);
  void fillNActiveSCs(uint32_t word);
  
  uint32_t LATOMEID;
  uint32_t extendedL1ID;
  uint32_t headerMarker;
  int nPackets;
  int packetIndex;
  int packetSize;
  uint32_t debugMarker;
  int headerSize = MON_HEADER_SIZE;
  int region;
  int nStreams;
  int streamNumber;
  std::array<int, 2> dataType;
  std::array<int, 2> nBCs;
  std::array<uint32_t, 2> timeShift;
  int nActiveSCs;
  int nthMONHWordPacketDetermined = 3;
  std::array<uint32_t, MON_HEADER_SIZE> headerRef = {0, 0, 0xFF1234FF, 0, 0xDEADBEEF, MON_HEADER_SIZE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  std::array<uint32_t, 2> trailerRef = {0xC0FFEE00, 0xAAAAAAAA};
};

#endif // MON_H
