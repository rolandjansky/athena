/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArLATOMEHeader.h"

//Constructor
LArLATOMEHeader::LArLATOMEHeader(const uint32_t id):m_sourceId(id),m_latomeId(0),m_nChannels(0),m_BCId(0),m_l1Id(0) {
} 

LArLATOMEHeader::LArLATOMEHeader(const uint32_t id, const uint32_t latomeid, const uint16_t nchan, const uint16_t bcid, const uint32_t l1id):m_sourceId(id),m_latomeId(latomeid),m_nChannels(nchan),m_BCId(bcid),m_l1Id(l1id) {
} 
