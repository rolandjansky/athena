/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamPipeDetectorFactory_Lite.h"

#include "GeoModelKernel/GeoVolumeCursor.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoDefinitions.h"  



BeamPipeDetectorFactory_Lite::BeamPipeDetectorFactory_Lite()
  :m_manager(nullptr)
{}


void BeamPipeDetectorFactory_Lite::create(GeoPhysVol *world)
{
  m_manager=new BeamPipeDetectorManager();
  GeoVolumeCursor cursor(world);
  while (!cursor.atEnd()) {
    GeoVPhysVol *physVol=const_cast<GeoVPhysVol *> (cursor.getVolume().operator->());
    if (cursor.getName()=="BeamPipe") m_manager->addTreeTop(physVol);
    cursor.next();
  }
}

const BeamPipeDetectorManager * BeamPipeDetectorFactory_Lite::getDetectorManager() const
{
  return m_manager;
}

