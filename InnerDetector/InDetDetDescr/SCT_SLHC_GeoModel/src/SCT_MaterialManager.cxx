/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "SCT_SLHC_GeoModel/SCT_GeoModelAthenaComps.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "StoreGate/StoreGateSvc.h"

#include <iostream>

namespace InDetDDSLHC {


// Constructor 
SCT_MaterialManager::SCT_MaterialManager(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps) 
  : InDetMaterialManager("SCT_MaterialManager", athenaComps)
{
  addWeightTable(sctdb->weightTable(), "sct");
  loadMaterials();
  m_gasMaterial = getMaterial("std::Air");

}    

// Add materials not yet in the database 
void
SCT_MaterialManager::loadMaterials()
{
  //const GeoElement *copper    = getElement("Copper");

  //const GeoMaterial *kapton   = getMaterial("std::Kapton"); // 30th Aug 2005 D.Naito added.

  // CuKapton for Low Mass Tapes
  //GeoMaterial * matCuKapton   = new GeoMaterial("sct::CuKapton",2.94*CLHEP::gram/CLHEP::cm3);
  //matCuKapton->add(const_cast<GeoElement*>(copper),  0.6142);
  //matCuKapton->add(const_cast<GeoMaterial*>(kapton), 0.3858);
  //addMaterial(matCuKapton);

}


const GeoMaterial* 
SCT_MaterialManager::gasMaterial() const
{
  return m_gasMaterial;
}


void 
SCT_MaterialManager::setGasMaterial(const GeoMaterial* material)
{
  m_gasMaterial = material;
}
  
void 
SCT_MaterialManager::setGasMaterial(const std::string & materialName)
{
  // Make sure an instance is created in order to initialize the manager
  m_gasMaterial = getMaterial(materialName);
}  

}
