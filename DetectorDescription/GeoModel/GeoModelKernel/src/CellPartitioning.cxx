/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/CellPartitioning.h"

//##ModelId=47A79C30000C
CellPartitioning::CellPartitioning(unsigned int firstDivisionNumber):
  firstDivisionNumber(firstDivisionNumber)
{
}


//##ModelId=47A79C30001C
CellPartitioning::CellPartitioning(const CellPartitioning& right):
  partition(right.partition),
  firstDivisionNumber(right.firstDivisionNumber)
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
    partition=right.partition;
    firstDivisionNumber = right.firstDivisionNumber;
  }
  return *this;
}

//##ModelId=47A79D8C032E
void CellPartitioning::addValue(double xVal)
{
  partition.push_back(xVal);
}

