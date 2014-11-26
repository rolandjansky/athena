/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TrigSteeringEvent/TrigPassFlags.h"

TrigPassFlags::TrigPassFlags()
   : m_container_ptr(0) {}

TrigPassFlags::TrigPassFlags(const unsigned int size,const unsigned int flagSize, const void* cont)
   : m_container_ptr(cont) {
   std::vector<bool> temp(flagSize,false);
   m_flagsPerObject.resize(size,temp);
}


void TrigPassFlags::setFlagBit(const unsigned int position,const unsigned int bitPosition,const bool bitValue, const void *cont) {
   if (m_container_ptr && cont != m_container_ptr)
      throw std::runtime_error("Passed container does not match the container that created this TrigPassFlags. No flag bit set.");

   if ( position >= size() )
      throw std::runtime_error("Specified flag position is larger than the size of the TrigPassFlags object. No flag bit set.");
   
   if ( bitPosition >= flagSize() )
      throw std::runtime_error("Specified bitPosition is larger than the flag size. No flag bit set.");
   
   m_flagsPerObject[position][bitPosition] = bitValue;
}

void TrigPassFlags::setFlag(const unsigned int position, const std::vector<bool>& flag, const void *cont) {
   if (m_container_ptr && cont != m_container_ptr)
      throw std::runtime_error("Passed container does not match the container that created this TrigPassFlags. No flag set.");

   if ( position >= size() )
      throw std::runtime_error("Specified flag position is larger than the size of the TrigPassFlags object. No flag set.");
   
   if ( flag.size() != flagSize() )
      throw std::runtime_error("Size of specified flag is different from the flag size of the TrigPassFlags object. No flag set.");

   m_flagsPerObject[position] = flag;
}

bool TrigPassFlags::getFlagBit(const unsigned int position, const unsigned int bitPosition) const {
   if ( position >= size() )
      throw std::runtime_error("Specified object position is larger than the size of the TrigPassFlags objects.");
   
   if ( bitPosition >= flagSize() )
      throw std::runtime_error("Specified bitPosition is larger than the flag size.");
   
   return m_flagsPerObject[position][bitPosition];
}

const std::vector<bool>& TrigPassFlags::getFlag(const unsigned int position) const {
   if ( position >= size() )
      throw std::runtime_error("Specified object position is larger than the size of the TrigPassFlags objects.");
   
   return m_flagsPerObject[position];
}


