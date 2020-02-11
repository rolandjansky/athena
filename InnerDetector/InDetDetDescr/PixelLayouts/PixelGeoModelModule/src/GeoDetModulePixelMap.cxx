/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModelModule/GeoDetModulePixelMap.h"

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelLayoutUtils/DBXMLUtils.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
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

GeoDetModulePixelMap::GeoDetModulePixelMap():
  GeoXMLUtils(),
  m_basics(nullptr),
  m_bModule(false)
{
  //try this to make compiler happy...
}

GeoDetModulePixelMap::GeoDetModulePixelMap(const PixelGeoBuilderBasics * basics,bool bModule):
  GeoXMLUtils(),
  m_basics(basics),
  m_bModule(bModule)
{
  preBuild();
}

void GeoDetModulePixelMap::preBuild()
{
  
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

  bool readXMLfromDB = m_basics->ReadInputDataFromDB();
  bool bParsed = false;

  if(readXMLfromDB)
    {
      m_basics->msgStream()<<MSG::DEBUG<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      DBXMLUtils dbUtils(m_basics);
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else{
    InitializeXML();
    m_basics->msgStream()<<MSG::DEBUG<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
    std::string file = PathResolver::find_file (fileName, "DATAPATH");
    bParsed = ParseFile(file);
  }
  
  if(!bParsed){
    m_basics->msgStream()<<MSG::DEBUG<<"XML file "<<fileName<<" not found"<<endreq;
    return;
  }

  int nbModule = getChildCount(itemGen.c_str(), 0, itemName.c_str());
  for(int iModule=0; iModule<nbModule; iModule++)
    {
      std::string moduleName = getChildValue(itemName.c_str(), iModule, "moduleName");
      moduleName.erase(std::remove(moduleName.begin(),moduleName.end(),' '),moduleName.end());

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
  m_basics->msgStream()<<MSG::ERROR<<"Unable to retrieve module of type "<<moduleType<<" from GeoDetModulePixelMap - please check that it exists!"<<endreq;
  return -1;
}


