/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_RodEncoder.cxx
//   Implementation file for class BCM_RodEncoder
///////////////////////////////////////////////////////////////////
// (c) ATLAS BCM Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//  Version 00-00-01 12/05/2008 Daniel Dobos
///////////////////////////////////////////////////////////////////

#include "BCM_RawDataByteStreamCnv/BCM_RodEncoder.h" 

////////////////////////
// constructor
////////////////////////
BCM_RodEncoder::BCM_RodEncoder()
{
}

////////////////////////
// destructor 
////////////////////////
BCM_RodEncoder::~BCM_RodEncoder() 
{
}

////////////////////////
//  fillROD() - convert BCM RDO to a vector of 32-bit words
////////////////////////
void BCM_RodEncoder::fillROD(std::vector<uint32_t>& v32rod, int /*BCs_per_LVL1ID*/) {

  // Loop over the hits in a RDO
  VRDO::iterator  rdo_it = m_RDOs.begin(); 
  VRDO::iterator  rdo_it_end = m_RDOs.end();

  if (rdo_it!=rdo_it_end) {

    const RDO * rdo_element;
    unsigned int offlineId = 99;
    unsigned int prev_offlineId = 99;
    unsigned int dataword_position;
    unsigned int Pulse1Position[8] = {0};
    unsigned int Pulse1Width[8] = {0};
    unsigned int Pulse2Position[8] = {0};
    unsigned int Pulse2Width[8] = {0};
    unsigned int BCID = 0;
    unsigned int Error = 0;

    for(; rdo_it!=rdo_it_end; ++rdo_it) {
      rdo_element = (*rdo_it);
      offlineId = rdo_element->getChannel();
      dataword_position = getDataword_position(offlineId);
      if (offlineId != prev_offlineId) {
        prev_offlineId = offlineId;
        Pulse1Position[dataword_position] = rdo_element->getPulse1Position();
        Pulse1Width[dataword_position]    = rdo_element->getPulse1Width();
        Pulse2Position[dataword_position] = rdo_element->getPulse2Position();
        Pulse2Width[dataword_position]    = rdo_element->getPulse2Width();
        BCID = rdo_element->getBCID();
      }
    }
    encode_data_block(v32rod, BCID, Pulse1Position, Pulse1Width, Pulse2Position, Pulse2Width, Error);
  }
  return; 
} 

////////////////////////
// encode_data_block() - convert eight RDOs into six data words
////////////////////////
unsigned int BCM_RodEncoder::encode_data_block(std::vector<uint32_t>& data_block,unsigned int BCID, unsigned int Pulse1Position[8], unsigned int Pulse1Width[8], unsigned int Pulse2Position[8], unsigned int Pulse2Width[8], unsigned int Error){
  uint32_t data_word = 0;
  data_word += (BCID              & 0xfff) << 20;
  data_word += (Pulse1Position[0] &  0x3f) << 14;
  data_word += (Pulse1Width[0]    &  0x1f) <<  9;
  data_word += (Pulse2Position[0] &  0x3f) <<  3;
  data_word += (Pulse2Width[0]    &  0x1c) >>  2;
  data_block.push_back(data_word);
  data_word = 0;
  data_word += (Pulse2Width[0]    &   0x3) << 30;
  data_word += (Pulse1Position[1] &  0x3f) << 24;
  data_word += (Pulse1Width[1]    &  0x1f) << 19;
  data_word += (Pulse2Position[1] &  0x3f) << 13;
  data_word += (Pulse2Width[1]    &  0x1f) <<  8;
  data_word += (Pulse1Position[2] &  0x3f) <<  2;
  data_word += (Pulse1Width[2]    &  0x18) >>  3;
  data_block.push_back(data_word);
  data_word = 0;
  data_word += (Pulse1Width[2]    &   0x7) << 29;
  data_word += (Pulse2Position[2] &  0x3f) << 23;
  data_word += (Pulse2Width[2]    &  0x1f) << 18;
  data_word += (Pulse1Position[3] &  0x3f) << 12;
  data_word += (Pulse1Width[3]    &  0x1f) <<  7;
  data_word += (Pulse2Position[3] &  0x3f) <<  1;
  data_word += (Pulse2Width[3]    &  0x10) >>  4;
  data_block.push_back(data_word);
  data_word = 0;
  data_word += (Pulse2Width[3]    &   0xf) << 28;
  data_word += (Pulse1Position[4] &  0x3f) << 22;
  data_word += (Pulse1Width[4]    &  0x1f) << 17;
  data_word += (Pulse2Position[4] &  0x3f) << 11;
  data_word += (Pulse2Width[4]    &  0x1f) <<  6;
  data_word += (Pulse1Position[5] &  0x3f) <<  0;
  data_block.push_back(data_word);
  data_word = 0;
  data_word += (Pulse1Width[5]    &  0x1f) << 27;
  data_word += (Pulse2Position[5] &  0x3f) << 21;
  data_word += (Pulse2Width[5]    &  0x1f) << 16;
  data_word += (Pulse1Position[6] &  0x3f) << 10;
  data_word += (Pulse1Width[6]    &  0x1f) <<  5;
  data_word += (Pulse2Position[6] &  0x3e) >>  1;
  data_block.push_back(data_word);
  data_word = 0;
  data_word += (Pulse2Position[6] &   0x1) << 31;
  data_word += (Pulse2Width[6]    &  0x1f) << 26;
  data_word += (Pulse1Position[7] &  0x3f) << 20;
  data_word += (Pulse1Width[7]    &  0x1f) << 15;
  data_word += (Pulse2Position[7] &  0x3f) <<  9;
  data_word += (Pulse2Width[7]    &  0x1f) <<  4;
  data_word += (Error             &   0xf) <<  0;
  data_block.push_back(data_word);
  return 6;
}

////////////////////////
// getDataword_position() - convert channelID into dataword_position
////////////////////////
unsigned int BCM_RodEncoder::getDataword_position(int ChannelID) {
  switch(ChannelID) {
  case 0:
    return 0;
  case 1:
    return 1;
  case 2:
    return 2;
  case 3:
    return 3;
  case 4:
    return 4;
  case 5:
    return 5;
  case 6:
    return 6;
  case 7:
    return 7;
  case 8:
    return 0;
  case 9:
    return 1;
  case 10:
    return 2;
  case 11:
    return 3;
  case 12:
    return 4;
  case 13:
    return 5;
  case 14:
    return 6;
  case 15:
    return 7;
  default:
    return 0xffffffff;
  }
}
