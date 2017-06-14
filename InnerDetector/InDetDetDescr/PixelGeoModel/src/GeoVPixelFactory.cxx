/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoVPixelFactory.h"

using InDetDD::PixelDetectorManager;

PixelDetectorManager * GeoVPixelFactory::m_DDmgr = 0;
PixelGeometryManager * GeoVPixelFactory::s_geometryManager = 0;

GeoVPixelFactory::GeoVPixelFactory() :   
  m_epsilon(0.0001)
{
   m_gmt_mgr = s_geometryManager;
   m_mat_mgr = m_gmt_mgr->getMaterialManager();
}

GeoVPixelFactory::~GeoVPixelFactory()
{}


void GeoVPixelFactory::SetDDMgr(PixelDetectorManager* mgr) 
{
  m_DDmgr = mgr;
}

void GeoVPixelFactory::setGeometryManager(PixelGeometryManager* mgr) 
{
  s_geometryManager = mgr;
}
