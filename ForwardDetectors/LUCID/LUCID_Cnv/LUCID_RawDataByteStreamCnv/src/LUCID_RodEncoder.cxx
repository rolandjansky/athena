/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "LUCID_RawDataByteStreamCnv/LUCID_RodEncoder.h" 

LUCID_RodEncoder::LUCID_RodEncoder() {
  
  m_hitcounter0 = 0;
  m_hitcounter1 = 0;
  m_hitcounter2 = 0;
  m_hitcounter3 = 0;
}

LUCID_RodEncoder::~LUCID_RodEncoder() {}
    
void LUCID_RodEncoder::encode(std::vector<uint32_t>& data_block, MsgStream& log) {

  VDIGIT::iterator digit_it     = m_Digits.begin(); 
  VDIGIT::iterator digit_it_end = m_Digits.end();
  
  uint32_t data_word0 = 0;
  uint32_t data_word1 = 0;
  uint32_t data_word2 = 0;
  uint32_t data_word3 = 0;

  m_hitcounter0 = 0;
  m_hitcounter1 = 0;
  m_hitcounter2 = 0;
  m_hitcounter3 = 0;
  
  for(; digit_it != digit_it_end; ++digit_it) {
    
    unsigned short tubeID = (*digit_it)->getTubeID();
    bool           isHit  = (*digit_it)->isHit();
    
    if      (tubeID < 16) { data_word0 |= (isHit << (tubeID -  0)); m_hitcounter0 += isHit; }
    else if (tubeID < 20) { data_word2 |= (isHit << (tubeID - 16)); m_hitcounter2 += isHit; }
    else if (tubeID < 36) { data_word1 |= (isHit << (tubeID - 20)); m_hitcounter1 += isHit; }
    else if (tubeID < 40) { data_word3 |= (isHit << (tubeID - 36)); m_hitcounter3 += isHit; }
    else log << MSG::ERROR << " Unknown tubeID: " << tubeID << endmsg;
    
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG
      << " tubeID: " << std::setw(10) << tubeID
      << " npe: "    << std::setw(10) << (*digit_it)->getNpe() 
      << " isHit:  " << std::setw(10) << isHit 
      << endmsg;
  }

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG
    << " m_hitcounter0: " << std::setw(10) << m_hitcounter0 << endmsg
    << " m_hitcounter1: " << std::setw(10) << m_hitcounter1 << endmsg
    << " m_hitcounter2: " << std::setw(10) << m_hitcounter2 << endmsg
    << " m_hitcounter3: " << std::setw(10) << m_hitcounter3 << endmsg;
  
  data_word0 |= (m_hitcounter0 << 24);
  data_word1 |= (m_hitcounter1 << 24);
  data_word2 |= (m_hitcounter2 << 24);
  data_word3 |= (m_hitcounter3 << 24);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG
    << std::hex
    << " data_word0: " << data_word0 << endmsg
    << " data_word1: " << data_word1 << endmsg
    << " data_word2: " << data_word2 << endmsg
    << " data_word3: " << data_word3 << endmsg
    << std::dec;
  
  data_block.push_back(data_word0);
  data_block.push_back(data_word1);
  data_block.push_back(data_word2);
  data_block.push_back(data_word3);

  m_Digits.clear();
}
