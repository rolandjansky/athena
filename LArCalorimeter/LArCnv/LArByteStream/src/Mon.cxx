/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "LArByteStream/Mon.h"

#define NSTREAMS 62
#define STREAMNUMBER 0
#define NBCS 32
#define NACTIVESCS 320

Mon::Mon()
  : headerMarker(0xFF1234FF), debugMarker(0xDEADBEEF), nStreams(NSTREAMS), streamNumber(STREAMNUMBER)
{}

void Mon::fillPacketInfo(uint32_t word) {
  nPackets = static_cast<int>((word & 0xff000000) >> 24);
  packetIndex = static_cast<int>((word & 0xff0000) >> 16);
  uint16_t sizeInBytes = static_cast<uint16_t>(word & 0xffff);
  if (sizeInBytes % 4) std::cout << "ERROR: Packet size written in the mon header is not multiple of 4 (cannot be converted from [bytes] to [32 bit words])\n";
  else packetSize = static_cast<int>(sizeInBytes / 4);
}

void Mon::fillRegion(uint32_t word) {
  region = static_cast<int>(word);
  switch (region) {
  case 0: return;
  case 1: return;
  case 2: return;
  case 3: return;
  case 4: return;
  case 5: return;
  default:
    std::cout << "Unknown calorimeter region word in mon header.\n";
  }
}

void Mon::fillNStreams(uint32_t word) {
  nStreams = static_cast<int>(word);
  if (nStreams != NSTREAMS) std::cout << "Number of streams word in mon header is not the standard one.\n";
}

void Mon::fillStreamNumber(uint32_t word) {
  streamNumber = static_cast<int>(word);
  if (streamNumber != STREAMNUMBER) std::cout << "Stream number word in mon header is not the standard one.\n";
}

void Mon::fillDataType(uint32_t word, int i) {
  dataType[i] = static_cast<int>(word);
  switch (dataType[i]) {
  case 0: return;
  case 1: return;
  case 2: return;
  case 3: return;
  case 0xff: return;
  default:
    std::cout << "Unknown calorimeter region word in mon header.\n";
  }
}

void Mon::fillNBCs(uint32_t word, int i) {
  nBCs[i] = static_cast<int>(word);
  if (nBCs[i] != NBCS) std::cout << "Number of BCs word in mon header is not the standard one.\n";
}

void Mon::fillTimeShift(uint32_t word, int i) {
  timeShift[i] = word;
}

void Mon::fillNActiveSCs(uint32_t word) {
  nActiveSCs = static_cast<int>(word);
  if (nActiveSCs != NACTIVESCS) std::cout << "Number of BCs word in mon header is not the standard one.\n";
}
