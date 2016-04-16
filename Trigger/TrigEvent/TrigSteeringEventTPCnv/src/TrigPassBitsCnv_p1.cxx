/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigPassBits.h"
#include <cmath>
#include <iostream>
#include "TrigSteeringEventTPCnv/TrigPassBits_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCnv_p1.h"



void  store_bits(const std::vector<bool>& bits, std::vector<uint32_t>& store ){
  const unsigned word_count = store.size();
  const unsigned bits_count = bits.size();

  for ( unsigned word = 0; word < word_count; ++word) {    
    uint32_t& temp(store[word]);

    for ( unsigned bit = 0; bit < 32; ++bit) {
      const unsigned position = word*32+bit;
      if (position >= bits_count )
	return;
      if (bits[position])
	temp |= (1 << bit);
    }
  }
}

void restore_bits(std::vector<bool>& bits, const std::vector<uint32_t>& store) {
  const unsigned word_count = store.size();
  const unsigned bits_count = bits.size();


  for ( unsigned word = 0; word < word_count; ++word) {    
    const uint32_t& temp(store[word]);

    for ( unsigned bit = 0; bit < 32; ++bit) {
      const unsigned position = word*32+bit;
      if (position >= bits_count )
	return;

      if (temp & (1 << bit))
	bits[position] = true;
      else
	bits[position] = false;      
    }
  }
}


unsigned necessary_words(unsigned size) {
  return  (size ? 1 : 0) + size/32;
}



void TrigPassBitsCnv_p1::persToTrans(const TrigPassBits_p1* persObj, 
				     TrigPassBits* transObj, 
				     MsgStream &log)
{
   log << MSG::DEBUG << "TrigPassBitsCnv_p1::persToTrans called " << endreq;
   
   transObj->m_decisions.resize(persObj->m_size, false); // reserve space and set defaults to 0

   restore_bits(transObj->m_decisions, persObj->m_serialized); // get back the bits

}


void TrigPassBitsCnv_p1::transToPers(const TrigPassBits* transObj, 
				     TrigPassBits_p1* persObj, 
				     MsgStream &log)
{
   log << MSG::DEBUG << "TrigPassBitsCnv_p1::transToPers called " << endreq;
   persObj->m_size = transObj->m_decisions.size();   

   persObj->m_serialized.resize(necessary_words(persObj->m_size), 0); // reserve the space and set the content to 0

   store_bits(transObj->m_decisions, persObj->m_serialized); // pack the bits
}
