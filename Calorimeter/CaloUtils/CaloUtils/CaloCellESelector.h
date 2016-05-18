/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOCELLESELECTOR_H
#define CALOUTILS_CALOCELLESELECTOR_H

#include "CaloInterface/ICaloCellSelector.h"

class CaloCell;

class CaloCellESelector : public ICaloCellSelector
{
 public:

  CaloCellESelector();
  CaloCellESelector(double minE, double maxE=-1.);

  virtual ~CaloCellESelector();

  virtual bool accept(const CaloCell* aCell) const;

 private:

  double m_minE, m_maxE;
};
#endif
