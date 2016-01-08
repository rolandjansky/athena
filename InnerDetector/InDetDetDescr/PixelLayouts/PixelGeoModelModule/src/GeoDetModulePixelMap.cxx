/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModelModule/GeoDetModulePixelMap.h"

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "PathResolver/PathResolver.h"

using std::max;


GeoDetModulePixelMap::GeoDetModulePixelMap(bool bModule):
  m_bModule(bModule)
{
  preBuild();
}

void GeoDetModulePixelMap::preBuild()
{
  //  std::cout<<" GeoDetModule* GeoDetModulePixelMap::build"<<std::endl;

  // -----------------------------------------------------------------------------
  // Read data from the XML file -------------------------------------------------

  
  std::string fileName="AlpineModules.xml";
  std::string itemGen = "PixelModules";
  std::string itemName = "Module";
  if(m_bModule){
    if(const char* env_p = std::getenv("PIXEL_SILICONMODULES_GEO_XML")) fileName = std::string(env_p);
  }
  else{
    if(const char* env_p = std::getenv("PIXEL_SILICONREADOUT_GEO_XML")) fileName = std::string(env_p);
    itemGen = "FrontEndChipReadout";
    itemName = "FrontEndChipGeo";
  }

  std::cout<<"XML file read to create map for modules / readout geos "<<fileName<<std::endl;

  InitializeXML();
  std::string file = PathResolver::find_file (fileName, "DATAPATH");
  bool bParsed = ParseFile(file);

  if(!bParsed){
    std::cout<<"XML file "<<fileName<<" not found"<<std::endl;
    return;
  }

  int nbModule = getChildCount(itemGen.c_str(), 0, itemName.c_str());
  std::cout<<"# module defined : "<<nbModule<<std::endl;
  for(int iModule=0; iModule<nbModule; iModule++)
    {
      std::string moduleName = getChildValue(itemName.c_str(), iModule, "moduleName");
      moduleName.erase(std::remove(moduleName.begin(),moduleName.end(),' '),moduleName.end());

      std::cout<<"   - "<<moduleName<<" "<<iModule<<std::endl;
      m_moduleMap.insert( std::pair<std::string,int>(moduleName,iModule) );
    }

  TerminateXML();
}

int GeoDetModulePixelMap::getModuleIndex(std::string moduleType) const
{
  moduleType.erase(std::remove(moduleType.begin(),moduleType.end(),' '),moduleType.end());

  for (auto it = m_moduleMap.begin(); it != m_moduleMap.end(); it++) {
    if(moduleType.compare(it->first)==0) return it->second;
  }
  return -1;
}


