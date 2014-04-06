/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENT_MISSINGETCELLMAP_H
#define MISSINGETEVENT_MISSINGETCELLMAP_H

#include "MissingETEvent/MissingEtMap.h"
#include "CaloEvent/CaloCell.h"

class MissingETCellMap : public MissingEtMap<CaloCell>
{
public:

  ~MissingETCellMap();

  static MissingETCellMap* instance(size_t mapSize=200000);

private:

  MissingETCellMap(size_t mapSize=200000);

  static MissingETCellMap* m_instance;
};
#endif
