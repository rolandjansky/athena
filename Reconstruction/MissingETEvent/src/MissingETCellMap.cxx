/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETEvent/MissingETCellMap.h"

MissingETCellMap* MissingETCellMap::m_instance = 0;

MissingETCellMap::MissingETCellMap(size_t mapSize)
  : MissingEtMap<CaloCell>(mapSize,mapSize)
{ 
  // get map size from detector store
  
}

MissingETCellMap::~MissingETCellMap()
{ 
  delete m_instance;

}

MissingETCellMap* MissingETCellMap::instance(size_t mapSize)
{
  if ( m_instance == 0 )
    {
      m_instance = new MissingETCellMap(mapSize);
      m_instance->setMapType("CellMap");
    }
  return m_instance;
}
