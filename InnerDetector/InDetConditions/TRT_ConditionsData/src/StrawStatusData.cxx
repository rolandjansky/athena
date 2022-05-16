/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ConditionsData/StrawStatusData.h"

namespace TRTCond
{

StrawStatusData::StrawStatusData(size_t nstraws)
{
  m_status.reserve(nstraws);
}

void StrawStatusData::setStatus(const IdentifierHash &hashId, unsigned int status)
{
  m_status[hashId] = status;
}

unsigned int StrawStatusData::findStatus(const IdentifierHash &hashId) const
{
  return m_status[hashId];
}

} // namespace TRTCond
