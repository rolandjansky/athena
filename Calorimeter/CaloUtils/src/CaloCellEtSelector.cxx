/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloCell.h"

#include "CaloUtils/CaloCellEtSelector.h"

CaloCellEtSelector::CaloCellEtSelector(double minEt,double maxEt)
  : ICaloCellSelector()
  , m_minEt(minEt), m_maxEt(maxEt)
{ }

CaloCellEtSelector::~CaloCellEtSelector()
{ }

bool CaloCellEtSelector::accept(const CaloCell* aCell) const
{
  if (!aCell) return false;
  return m_maxEt > m_minEt && aCell != 0 
    ? aCell->et() >= m_minEt && aCell->et() <= m_maxEt
    : aCell->et() >= m_minEt;
}
