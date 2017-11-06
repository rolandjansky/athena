/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/CellPartitioning.h"

CellPartitioning::CellPartitioning(unsigned int firstDivisionNumber):
  m_firstDivisionNumber(firstDivisionNumber)
{
}

CellPartitioning::CellPartitioning(const CellPartitioning& right):
  m_partition(right.m_partition),
  m_firstDivisionNumber(right.m_firstDivisionNumber)
{
}

CellPartitioning::~CellPartitioning()
{
}

CellPartitioning& CellPartitioning::operator=(const CellPartitioning& right)
{
  if (this!=&right) {
    m_partition=right.m_partition;
    m_firstDivisionNumber = right.m_firstDivisionNumber;
  }
  return *this;
}

void CellPartitioning::addValue(double xVal)
{
  m_partition.push_back(xVal);
}

