/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ConditionsData/StrawStatusSummary.h"

namespace TRTCond
{

StrawStatusSummary::StrawStatusSummary(size_t nstraws)
{
  m_status.reserve(nstraws);
}

void StrawStatusSummary::setStatus(const IdentifierHash &hashId, bool status)
{
  m_status[hashId] = status;
}

bool StrawStatusSummary::findStatus(const IdentifierHash &hashId) const
{
  return m_status[hashId];
}

} // namespace TRTCond
