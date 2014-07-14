/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JemJetElement.h"

namespace LVL1BS {

// Constant definitions

const int      JemJetElement::s_emDataBit;
const int      JemJetElement::s_emParityBit;
const int      JemJetElement::s_hadDataBit;
const int      JemJetElement::s_hadParityBit;
const int      JemJetElement::s_linkErrorBit;
const int      JemJetElement::s_pairBit;
const int      JemJetElement::s_pinBit;
const int      JemJetElement::s_wordIdBit;
const int      JemJetElement::s_jeWordId;
const int      JemJetElement::s_pairsPerPin;
const int      JemJetElement::s_pairOffset;
const uint32_t JemJetElement::s_emDataMask;
const uint32_t JemJetElement::s_emParityMask;
const uint32_t JemJetElement::s_hadDataMask;
const uint32_t JemJetElement::s_hadParityMask;
const uint32_t JemJetElement::s_linkErrorMask;
const uint32_t JemJetElement::s_pairMask;
const uint32_t JemJetElement::s_pinMask;
const uint32_t JemJetElement::s_wordIdMask;

JemJetElement::JemJetElement(uint32_t word) : m_data(word)
{
}

JemJetElement::JemJetElement(const int chan, const int emDat, const int hadDat,
                             const int emParErr, const int hadParErr,
			     const int linkErr)
{
  uint32_t word = 0;
  word |= (emDat      & s_emDataMask)    << s_emDataBit;
  word |= (emParErr   & s_emParityMask)  << s_emParityBit;
  word |= (hadDat     & s_hadDataMask)   << s_hadDataBit;
  word |= (hadParErr  & s_hadParityMask) << s_hadParityBit;
  word |= (linkErr    & s_linkErrorMask) << s_linkErrorBit;
  if (word) {
    word |= ((chan % s_pairsPerPin + s_pairOffset) & s_pairMask) << s_pairBit;
    word |= ((chan / s_pairsPerPin)                & s_pinMask)  << s_pinBit;
    word |= (s_jeWordId & s_wordIdMask)    << s_wordIdBit;
  }
  m_data = word;
}

} // end namespace
