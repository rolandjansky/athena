/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelParameters.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardParameters.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelParametersOld.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParametersOld.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardParametersOld.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardModuleParametersOld.h"
#include "SCT_SLHC_GeoModel/SCT_GeneralParameters.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"

namespace InDetDDSLHC {

SCT_GeometryManager::SCT_GeometryManager(const SCT_GeoModelAthenaComps *athenaComps)
  : SCT_ParametersBase(athenaComps),
    m_commonItems(0),
    m_barrelParameters(0),
    m_barrelModuleParameters(0),
    m_forwardParameters(0),
    m_forwardModuleParameters(0),
    m_generalParameters(0),
    m_materialManager(0),
    m_sctdb(0),
    m_old(false)
{
  // See if some parameter that only exists in the old text files exists. If so assume we are working with 
  // with a text file with the old format.
  if (db()->testField("","L0_RLAY")) {
    msg(MSG::INFO) << "Detected old format text file. Please migrate to new format." << endreq;
    m_old = true;
  } else {
    msg(MSG::DEBUG) << "New format text file." << endreq;
  }
}

SCT_GeometryManager::~SCT_GeometryManager()
{
  delete m_barrelParameters;
  delete m_barrelModuleParameters;
  delete m_forwardParameters;
  delete m_forwardModuleParameters;
  delete m_generalParameters;
  delete m_materialManager;
  delete m_sctdb;
}

//
// Access to run time options.
//
const SCT_Options & 
SCT_GeometryManager::options()
{
  return m_options;
}

void
SCT_GeometryManager::setOptions(const SCT_Options & options)
{
  m_options = options;
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
    if (m_old) {
      m_barrelParameters = new SCT_BarrelParametersOld(0,athenaComps()); 
    } else {
      m_barrelParameters = new SCT_BarrelParameters(sctdb(),athenaComps()); 
    }
  }
  return m_barrelParameters;
}

const SCT_BarrelModuleParameters * 
SCT_GeometryManager::barrelModuleParameters() const
{    
  if (!m_barrelModuleParameters) {
    if (m_old) {
      m_barrelModuleParameters = new SCT_BarrelModuleParametersOld(0,athenaComps());
    } else {
      m_barrelModuleParameters = new SCT_BarrelModuleParameters(sctdb(),athenaComps());
    }   
  }   
  return m_barrelModuleParameters;
}


const SCT_ForwardModuleParameters * 
SCT_GeometryManager::forwardModuleParameters() const
{    
  if (!m_forwardModuleParameters) {
    if (m_old) {
      m_forwardModuleParameters = new SCT_ForwardModuleParametersOld(0,athenaComps());
    } else {
      m_forwardModuleParameters = new SCT_ForwardModuleParameters(sctdb(),athenaComps());
    }
  }
  return m_forwardModuleParameters;
}
const SCT_ForwardParameters * 
SCT_GeometryManager::forwardParameters() const
{    
  if (!m_forwardParameters) {
    if (m_old) {
      m_forwardParameters = new SCT_ForwardParametersOld(0,athenaComps());
    } else {
      m_forwardParameters = new SCT_ForwardParameters(sctdb(),athenaComps());
    }
  }
  return m_forwardParameters;
}

const SCT_DataBase * 
SCT_GeometryManager::sctdb() const
{    
  if (!m_sctdb) {
    m_sctdb = new SCT_DataBase(athenaComps());
  }
  return m_sctdb;
}
      

const SCT_GeneralParameters * 
SCT_GeometryManager::generalParameters() const
{    
  if (!m_generalParameters) m_generalParameters = new SCT_GeneralParameters(sctdb(),athenaComps());
  return m_generalParameters;
}

SCT_MaterialManager * 
SCT_GeometryManager::materialManager() const
{
  if (!m_materialManager) {
    m_materialManager = new SCT_MaterialManager(sctdb(),athenaComps());
  }
  return m_materialManager;
}

}
