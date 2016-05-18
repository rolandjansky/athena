/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOCELLETSELECTOR_H
#define CALOUTILS_CALOCELLETSELECTOR_H

#include "CaloInterface/ICaloCellSelector.h"

class CaloCell;

class CaloCellEtSelector : public ICaloCellSelector
{
 public:

  CaloCellEtSelector();
  CaloCellEtSelector(double minEt, double maxEt=-1.);

  virtual ~CaloCellEtSelector();

  virtual bool accept(const CaloCell* aCell) const;

 private:

  double m_minEt, m_maxEt;
};
#endif
