/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelLayoutLoI/PixelLoIStaveXMLHelper.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

PixelLoIStaveXMLHelper::PixelLoIStaveXMLHelper(int layer, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics),
  m_layer(layer)
{

  std::string fileName="GenericLoIStave.xml";
  if(const char* env_p = std::getenv("PIXEL_STAVESUPPORT_GEO_XML")) fileName = std::string(env_p);
  std::cout<<"SES - XML "<<fileName<<std::endl;

  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  bool bParsed=false;
  if(readXMLfromDB)
    {
      basics->msgStream()<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      DBXMLUtils dbUtils(getBasics());
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      InitializeXML();
      bParsed = ParseFile(file);
    }

  if(!bParsed){
    std::cout<<"XML file "<<fileName<<" not found"<<std::endl;
    return;
  }

  m_layerIndices = getChildValue_Indices("PixelStaveGeo","Layer",m_layer);

  std::cout<<"PixelLoIStaveXMLHelper for layer "<<m_layer<<" "<<m_layerIndices[0]<<" "<<m_layerIndices[1]<<std::endl;

}

PixelLoIStaveXMLHelper::~PixelLoIStaveXMLHelper()
{
  TerminateXML();
}


double PixelLoIStaveXMLHelper::getClearance() const
{
  return getDouble("PixelStaveGeo",m_layerIndices,"ClearanceX");
}

double PixelLoIStaveXMLHelper::getStaggerDist() const
{
  return getDouble("PixelStaveGeo",m_layerIndices,"StaggerDist");
}

double PixelLoIStaveXMLHelper::getStaggerSign() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "StaggerSign");
}

double PixelLoIStaveXMLHelper::getCenterShift() const
{
  return getDouble("PixelStaveGeo",m_layerIndices,"ModuleCenterShift");
}

double PixelLoIStaveXMLHelper::getBarrelModuleDZ() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "BarrelModuleDZ", 0, -1.);
}


double PixelLoIStaveXMLHelper::getStaveSupportLength() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "EnvelopeLength");
}

double PixelLoIStaveXMLHelper::getStaveSupportWidth() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "StaveSupportWidth",0, -1);
}

double PixelLoIStaveXMLHelper::getStaveSupportThick() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "LadderThickness");
}

std::string PixelLoIStaveXMLHelper::getStaveSupportMaterial() const
{
  return getString("PixelStaveGeo", m_layerIndices, "StaveSupportMaterialGeo");
}

double PixelLoIStaveXMLHelper::getServiceOffsetX() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "ServiceOffsetX");
}

double PixelLoIStaveXMLHelper::getServiceOffsetY() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "ServiceOffsetY");
}
