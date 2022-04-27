/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////
// TRTStrawStatusData.cxx
////////////////////////////

#include "TRT_ConditionsData/TRTStrawStatusData.h"
#include "Identifier/Identifier.h"
#include <iostream>

//constructor
TRTStrawStatusData::TRTStrawStatusData()
{
  const size_t nstraws{350848};
  m_status.reserve(nstraws);
}

//destructor
TRTStrawStatusData::~TRTStrawStatusData()
{ }

//add map entries
void TRTStrawStatusData::setStatus(const IdentifierHash& hashId, const int stat)
{
  m_status[hashId]=stat;
}

int TRTStrawStatusData::findStatus(const IdentifierHash& hashId) const
{
  int stat = m_status[hashId];
  return stat;
}

