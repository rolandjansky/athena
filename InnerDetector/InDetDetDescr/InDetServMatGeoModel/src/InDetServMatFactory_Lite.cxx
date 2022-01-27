/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatFactory_Lite.h"

#include "GeoModelKernel/GeoVolumeCursor.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoDefinitions.h"  



InDetServMatFactory_Lite::InDetServMatFactory_Lite()
  :m_manager(nullptr)
{}


void InDetServMatFactory_Lite::create(GeoPhysVol *world)
{
  m_manager=new InDetDD::InDetServMatManager();
  GeoVolumeCursor cursor(world);
  while (!cursor.atEnd()) {
    const GeoVPhysVol *physVol=cursor.getVolume().operator->();
    if (cursor.getName()=="InDetServMat") m_manager->addTreeTop(physVol);
    cursor.next();
  }
}

const InDetDD::InDetServMatManager * InDetServMatFactory_Lite::getDetectorManager() const
{
  return m_manager;
}

