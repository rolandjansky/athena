/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_GeometryManager.h"

#include "InDetGeoModelUtils/DistortedMaterialManager.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiCommonItems.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"
#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_GeoModel/SCT_DataBase.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"
#include "SCT_GeoModel/SCT_ForwardModuleParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"
#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h"

SCT_GeometryManager::SCT_GeometryManager(SCT_DataBase* rdb)
  : m_athenaComps{rdb->athenaComps()},
    m_rdb{rdb}
{
  // This class uses reference counting. Should not be delete'd in destructor.
  m_commonItems = new InDetDD::SiCommonItems(m_athenaComps->getIdHelper());

  m_barrelParameters = std::make_unique<SCT_BarrelParameters>(m_rdb);
  m_barrelModuleParameters = std::make_unique<SCT_BarrelModuleParameters>(m_rdb);
  m_forwardParameters = std::make_unique<SCT_ForwardParameters>(m_rdb);
  m_forwardModuleParameters = std::make_unique<SCT_ForwardModuleParameters>(m_rdb);
  m_generalParameters = std::make_unique<SCT_GeneralParameters>(m_rdb);
  m_distortedMatManager = std::make_unique<InDetDD::DistortedMaterialManager>();
}

SCT_GeometryManager::~SCT_GeometryManager()
{
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
  
//
// SiCommonItems which are passed to SiDetectorElements.
//

const InDetDD::SiCommonItems *
SCT_GeometryManager::commonItems() const
{
  return m_commonItems;
}

const SCT_BarrelParameters * 
SCT_GeometryManager::barrelParameters() const
{    
  return m_barrelParameters.get();
}

const SCT_BarrelModuleParameters * 
SCT_GeometryManager::barrelModuleParameters() const
{    
  return m_barrelModuleParameters.get();
}


const SCT_ForwardModuleParameters * 
SCT_GeometryManager::forwardModuleParameters() const
{    
  return m_forwardModuleParameters.get();
}


const SCT_ForwardParameters * 
SCT_GeometryManager::forwardParameters() const
{    
  return m_forwardParameters.get();
}


const SCT_GeneralParameters * 
SCT_GeometryManager::generalParameters() const
{    
  return m_generalParameters.get();
}

const InDetDD::DistortedMaterialManager * 
SCT_GeometryManager::distortedMatManager() const
{    
  return m_distortedMatManager.get();
}

SCT_GeometryManager&
SCT_GeometryManager::operator=(const SCT_GeometryManager& right) {
  if (this != &right) {
    m_options = right.m_options;
    m_athenaComps = right.m_athenaComps;
    m_commonItems = right.m_commonItems;
    m_rdb = right.m_rdb;
    m_barrelParameters.reset(new SCT_BarrelParameters(m_rdb));
    m_barrelModuleParameters.reset(new SCT_BarrelModuleParameters(m_rdb));
    m_forwardParameters.reset(new SCT_ForwardParameters(m_rdb));
    m_forwardModuleParameters.reset(new SCT_ForwardModuleParameters(m_rdb));
    m_generalParameters.reset(new SCT_GeneralParameters(m_rdb));
    m_distortedMatManager.reset(new InDetDD::DistortedMaterialManager());
  }
  return *this;
}

SCT_GeometryManager::SCT_GeometryManager(const SCT_GeometryManager& right) {
  m_options = right.m_options;
  m_athenaComps = right.m_athenaComps;
  m_commonItems = right.m_commonItems;
  m_rdb = right.m_rdb;
  m_barrelParameters.reset(new SCT_BarrelParameters(m_rdb));
  m_barrelModuleParameters.reset(new SCT_BarrelModuleParameters(m_rdb));
  m_forwardParameters.reset(new SCT_ForwardParameters(m_rdb));
  m_forwardModuleParameters.reset(new SCT_ForwardModuleParameters(m_rdb));
  m_generalParameters.reset(new SCT_GeneralParameters(m_rdb));
  m_distortedMatManager.reset(new InDetDD::DistortedMaterialManager());
}
