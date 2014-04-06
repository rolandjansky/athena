/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETEvent/MissingETClusterMap.h"

MissingETClusterMap* MissingETClusterMap::m_instance = 0;

MissingETClusterMap::MissingETClusterMap()
  : MissingEtMap<CaloCluster>()
{ }

MissingETClusterMap::~MissingETClusterMap()
{ 
 delete m_instance;
}

MissingETClusterMap* MissingETClusterMap::instance()
{
  if ( m_instance == 0 )
    {
      m_instance = new MissingETClusterMap();
      m_instance->setMapType("ClusterMap");
    }
  return m_instance;
}
