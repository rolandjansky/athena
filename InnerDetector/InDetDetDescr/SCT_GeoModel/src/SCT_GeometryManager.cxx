/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_GeometryManager.h"

#include "SCT_GeoModel/SCT_BarrelParameters.h"
#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"
#include "SCT_GeoModel/SCT_ForwardModuleParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"
#include "InDetGeoModelUtils/DistortedMaterialManager.h"
#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h"


SCT_GeometryManager::SCT_GeometryManager()
  : m_athenaComps(0),
    m_commonItems(0),
    m_barrelParameters(0),
    m_barrelModuleParameters(0),
    m_forwardParameters(0),
    m_forwardModuleParameters(0),
    m_generalParameters(0),
    m_distortedMatManager(0)
{}

SCT_GeometryManager::~SCT_GeometryManager()
{
  delete m_barrelParameters;
  delete m_barrelModuleParameters;
  delete m_forwardParameters;
  delete m_forwardModuleParameters;
  delete m_generalParameters;
  delete m_distortedMatManager;
}

//
// Access to run time options.
//
const SCT_Options & 
SCT_GeometryManager::options() const
{
  return m_options;
}

void
SCT_GeometryManager::setOptions(const SCT_Options & options)
{
  m_options = options;
}

const SCT_GeoModelAthenaComps *
SCT_GeometryManager::athenaComps() const 
{
  return m_athenaComps;
}
  
void 
SCT_GeometryManager::setAthenaComps(const SCT_GeoModelAthenaComps * athenaComps)
{
  m_athenaComps = athenaComps;
}


//
// SiCommonItems which are passed to SiDetectorElements.
//

InDetDD::SiCommonItems * 
SCT_GeometryManager::commonItems() const
{
  return m_commonItems;
}


void 
SCT_GeometryManager::setCommonItems(InDetDD::SiCommonItems * commonItems)
{
  m_commonItems = commonItems;
}

const SCT_BarrelParameters * 
SCT_GeometryManager::barrelParameters() const
{    
  if (!m_barrelParameters) {
    m_barrelParameters = new SCT_BarrelParameters; 
  }
  return m_barrelParameters;
}

const SCT_BarrelModuleParameters * 
SCT_GeometryManager::barrelModuleParameters() const
{    
  if (!m_barrelModuleParameters) {
    m_barrelModuleParameters = new SCT_BarrelModuleParameters;
  }      
  return m_barrelModuleParameters;
}


const SCT_ForwardModuleParameters * 
SCT_GeometryManager::forwardModuleParameters() const
{    
  if (!m_forwardModuleParameters) {
    m_forwardModuleParameters = new SCT_ForwardModuleParameters;
  }    
  return m_forwardModuleParameters;
}


const SCT_ForwardParameters * 
SCT_GeometryManager::forwardParameters() const
{    
  if (!m_forwardParameters) {
    m_forwardParameters = new SCT_ForwardParameters;
  }
  return m_forwardParameters;
}


const SCT_GeneralParameters * 
SCT_GeometryManager::generalParameters() const
{    
  if (!m_generalParameters) m_generalParameters = new SCT_GeneralParameters;
  return m_generalParameters;
}

const InDetDD::DistortedMaterialManager * 
SCT_GeometryManager::distortedMatManager() const
{    
  if (!m_distortedMatManager) m_distortedMatManager = new InDetDD::DistortedMaterialManager;
  return m_distortedMatManager;
}

