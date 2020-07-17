/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/



///////////////////////////////////////////////////////////////////
// TRT_LoLumRawData.cxx
//   Implementation file for class TRT_LoLumRawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
// Implementation provided by A. Zalite, February 2003
///////////////////////////////////////////////////////////////////

#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetRawData/TRT_RDORawData.h"

// static member variables
const double TRT_LoLumRawData::m_driftTimeBinWidth = 3.125;
const unsigned int TRT_LoLumRawData::m_maskFourLastBits=0xFFFFFF0;  // 1 1 11111111 1 11111111 1 11110000
const unsigned int TRT_LoLumRawData::m_maskThreeLastBits=0xFFFFFF8;  // 1 1 11111111 1 11111111 1 11111000

// default constructor
TRT_LoLumRawData::TRT_LoLumRawData() :
  TRT_RDORawData(Identifier(), 0) //call base-class constructor
{}

// Constructor with parameters:
TRT_LoLumRawData::TRT_LoLumRawData(const Identifier rdoId,
                           const unsigned int word) :
  TRT_RDORawData( rdoId, word) //call base-class constructor
{}

// Destructor:
//should I be destructing something here?
TRT_LoLumRawData::~TRT_LoLumRawData()
{}

unsigned int TRT_LoLumRawData::driftTimeBin(unsigned int word) {
  unsigned int leadingEdge=0, trailingEdge=0;
  findLargestIsland(word, leadingEdge, trailingEdge);
  return leadingEdge;
}

unsigned int TRT_LoLumRawData::trailingEdge(unsigned int word) {
  unsigned int leadingEdge=0, trailingEdge=0;
  findLargestIsland(word, leadingEdge, trailingEdge);
  return trailingEdge;
}

double TRT_LoLumRawData::timeOverThreshold(unsigned int word) {
  unsigned int leadingEdge=0, trailingEdge=0;
  if (findLargestIsland(word, leadingEdge, trailingEdge)) {
    return (trailingEdge - leadingEdge + 1) * m_driftTimeBinWidth;
  };
  return 0.;
}

bool TRT_LoLumRawData::findLargestIsland(unsigned int word, unsigned int& leadingEdge, unsigned int& trailingEdge) {
  unsigned long mask = 0x02000000;  // 0 0 10000000 0 00000000 0 00000000
  unsigned int bestLength = 0;
  unsigned int currentLength = 0;

  // set 4 last bits to zero (to match data and MC bitmasks)
  unsigned int wordLE = word & m_maskFourLastBits;

  mask >>=1;  // 0 0 01000000 0 00000000 0 00000000
  bool SawZero = false;
  unsigned int k = 1;
  leadingEdge=0, trailingEdge=0;
  unsigned int currentLeadingEdge=0, currentTrailingEdge=0;

  // shift bitmask to the right until end
  while (true) {
    if (!(wordLE & mask) && !SawZero) SawZero = true; // search for the first 0 to 1 transition
    if (SawZero) {
      if (wordLE & mask){
        if (currentLength==0) currentLeadingEdge=k;
        currentTrailingEdge=k;
        ++currentLength;
      } else { // remember longest island, ignore islands of length 1 which are very likely noise
        if (currentLength >= bestLength && currentLeadingEdge<18 && currentLength > 1) {
          bestLength = currentLength;
          leadingEdge = currentLeadingEdge;
          trailingEdge = currentTrailingEdge;
        }
        currentLength = 0;
      }
    }
    mask >>= 1;
    if (!(mask & m_maskThreeLastBits)) break; // stop after checking 20 LT bits
    if (k == 7 || k == 15) mask >>= 1; // ignore HT bits
    assert(k < 20);
    ++k;
  }
  assert(k == 20);
  // avoid very early TE, most likely from previous BX. Hit will still be used for tracking if it has a valid LE
  if (trailingEdge < 8) trailingEdge = 0;
  return leadingEdge && trailingEdge;
}
