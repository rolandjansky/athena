/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/CellPartitioning.h"

//##ModelId=47A79C30000C
CellPartitioning::CellPartitioning(unsigned int firstDivisionNumber):
  m_firstDivisionNumber(firstDivisionNumber)
{
}


//##ModelId=47A79C30001C
CellPartitioning::CellPartitioning(const CellPartitioning& right):
  m_partition(right.m_partition),
  m_firstDivisionNumber(right.m_firstDivisionNumber)
{
}

//##ModelId=47A79C30005D
CellPartitioning::~CellPartitioning()
{
}

//##ModelId=47A79C5902EC
CellPartitioning& CellPartitioning::operator=(const CellPartitioning& right)
{
  if (this!=&right) {
    m_partition=right.m_partition;
    m_firstDivisionNumber = right.m_firstDivisionNumber;
  }
  return *this;
}

//##ModelId=47A79D8C032E
void CellPartitioning::addValue(double xVal)
{
  m_partition.push_back(xVal);
}

