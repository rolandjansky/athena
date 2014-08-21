/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICALOCELLSELECTOR_H
#define CALOINTERFACE_ICALOCELLSELECTOR_H

class CaloCell;

class ICaloCellSelector
{
 public:

  virtual ~ICaloCellSelector() { };

  virtual bool accept(const CaloCell* aCell) const = 0;
};
#endif
