/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/RODHeader.h"

namespace LVL1 {

RODHeader::RODHeader() : m_version(0), m_sourceId(0), m_run(0), m_lvl1Id(0),
                         m_bcId(0), m_trigType(0), m_detType(0),
			 m_statusWords(0), m_payloadSize(0)
{
}

RODHeader::RODHeader(uint32_t version, uint32_t sourceId, uint32_t run,
                     uint32_t lvl1Id,  uint32_t bcId,     uint32_t trigType,
		     uint32_t detType, const std::vector<uint32_t>& statusWords,
		     uint32_t nData)
		    : m_version(version), m_sourceId(sourceId),
		      m_run(run), m_lvl1Id(lvl1Id), m_bcId(bcId),
		      m_trigType(trigType), m_detType(detType),
		      m_statusWords(statusWords), m_payloadSize(nData)
{
}

RODHeader::~RODHeader()
{
}

} // end namespace
