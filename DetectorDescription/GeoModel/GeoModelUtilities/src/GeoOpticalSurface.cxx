/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoOpticalSurface.h"

GeoOpticalSurface::GeoOpticalSurface(const std::string& name,
				     GeoOpticalSurfaceModel model,
				     GeoOpticalSurfaceFinish finish,
				     GeoSurfaceType type,
				     double parameter):
  m_name(name),
  m_model(model),
  m_finish(finish),
  m_type(type),
  m_parameter(parameter),
  m_materialPropertiesTable(0)
{
}

GeoOpticalSurface::~GeoOpticalSurface()
{
  if(m_materialPropertiesTable)
    m_materialPropertiesTable->unref();
}

