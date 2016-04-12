/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloCell.h"

#include "CaloUtils/CaloCellESelector.h"

CaloCellESelector::CaloCellESelector(double minE,double maxE)
  : ICaloCellSelector()
  , m_minE(minE), m_maxE(maxE)
{ }

CaloCellESelector::~CaloCellESelector()
{ }

bool CaloCellESelector::accept(const CaloCell* aCell) const
{
  if (!aCell) return false;
  return m_maxE > m_minE
    ? aCell->e() >= m_minE && aCell->e() <= m_maxE
    : aCell->e() >= m_minE;
}
