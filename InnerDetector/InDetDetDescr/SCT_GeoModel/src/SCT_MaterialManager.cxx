/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_MaterialManager.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include "SCT_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <iostream>

InDetMaterialManager * SCT_MaterialManager::s_materialManager = 0;
const GeoMaterial * SCT_MaterialManager::s_gasMaterial = 0;

// Constructor 
SCT_MaterialManager::SCT_MaterialManager() 
{
  // Get my material manager if we haven't already done so.
  if (!s_materialManager) {

    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
    StoreGateSvc* detStore;
    StatusCode sc = svcLocator->service("DetectorStore", detStore );
    if (sc.isFailure()) {
      std::cout << "Could not locate DetectorStore" << std::endl;
      return;
    }

    const SCT_DataBase * rdb = SCT_DataBase::instance();
    //s_materialManager =  new InDetMaterialManager("SCT_MaterialManager", detStore, rdb->weightTable(), "sct")
    s_materialManager =  new InDetMaterialManager("SCT_MaterialManager", rdb->athenaComps());
    s_materialManager->addWeightTable(rdb->weightTable(), "sct");
    s_materialManager->addScalingTable(rdb->scalingTable());

    loadMaterials();
  }

  if (!s_gasMaterial) {
    s_gasMaterial = s_materialManager->getMaterial("std::Air");
  }

}    

void 
SCT_MaterialManager::reinit()
{
  s_gasMaterial = 0;
  delete s_materialManager;
  s_materialManager = 0;
}


// Add materials not yet in the database 
void
SCT_MaterialManager::loadMaterials()
{
  //const GeoElement *copper    = getElement("Copper");

  //const GeoMaterial *kapton   = getMaterial("std::Kapton"); // 30th Aug 2005 D.Naito added.

  // CuKapton for Low Mass Tapes
  //GeoMaterial * matCuKapton   = new GeoMaterial("sct::CuKapton",2.94*gram/Gaudi::Units::cm3);
  //matCuKapton->add(const_cast<GeoElement*>(copper),  0.6142);
  //matCuKapton->add(const_cast<GeoMaterial*>(kapton), 0.3858);
  //addMaterial(matCuKapton);
}

const GeoElement* 
SCT_MaterialManager::getElement(const std::string & elementName) const
{
  return s_materialManager->getElement(elementName);
}

const GeoMaterial* 
SCT_MaterialManager::getMaterial(const std::string & materialName) const
{
  return s_materialManager->getMaterial(materialName);
}

void
SCT_MaterialManager::addMaterial(GeoMaterial* material)
{
  return s_materialManager->addMaterial(material);
}

const GeoMaterial* 
SCT_MaterialManager::getMaterial(const std::string & originalMaterial, 
                                 double density,  
                                 const std::string & newName)
{
  
  return s_materialManager->getMaterial(originalMaterial, density, newName);
}

const GeoMaterial *
SCT_MaterialManager::getMaterialForVolume(const std::string & materialName, double volume)
{
  return s_materialManager->getMaterialForVolume(materialName, volume);
}



const GeoMaterial* 
SCT_MaterialManager::gasMaterial() const
{
  return s_gasMaterial;
}


void 
SCT_MaterialManager::setGasMaterial(const GeoMaterial* material)
{
  s_gasMaterial = material;
}
  
void 
SCT_MaterialManager::setGasMaterial(const std::string & materialName)
{
  // Make sure an instance is created in order to initialize the manager
  SCT_MaterialManager manager;
  s_gasMaterial = manager.getMaterial(materialName);
}  

