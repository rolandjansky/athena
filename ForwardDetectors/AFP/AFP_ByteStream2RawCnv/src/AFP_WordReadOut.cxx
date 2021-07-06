/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_ByteStream2RawCnv/AFP_WordReadOut.h"

AFP_WordReadOut::AFP_WordReadOut(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

AFP_WordReadOut::~AFP_WordReadOut()
{
}

StatusCode AFP_WordReadOut::initialize()
{
  if (m_linkNumTrans.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve service " << m_linkNumTrans );
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Retrieved service " << m_linkNumTrans );
  }
  return StatusCode::SUCCESS;
}

StatusCode AFP_WordReadOut::finalize()
{
  return StatusCode::SUCCESS;
}

uint32_t AFP_WordReadOut::getBits(uint32_t the_word, const uint16_t start, const uint16_t stop) const
{
  uint32_t word = 0;
  for (uint16_t i = stop; i < start; i++) {
    word = ((word | 1) << 1);
  }
  uint32_t result = (the_word >> stop) & (word | 1);
  return result;
}
