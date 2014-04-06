/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENT_MISSINGETCLUSTERMAP_H
#define MISSINGETEVENT_MISSINGETCLUSTERMAP_H

#include "MissingETEvent/MissingEtMap.h"

#include "CaloEvent/CaloCluster.h"

class MissingETClusterMap : public MissingEtMap<CaloCluster>
{
public:

  ~MissingETClusterMap();

  static MissingETClusterMap* instance();

protected:
  MissingETClusterMap();
  static MissingETClusterMap* m_instance;
};
#endif
