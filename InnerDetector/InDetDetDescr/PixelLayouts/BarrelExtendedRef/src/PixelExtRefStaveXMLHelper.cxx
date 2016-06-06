/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "BarrelExtendedRef/PixelExtRefStaveXMLHelper.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

PixelExtRefStaveXMLHelper::PixelExtRefStaveXMLHelper(int layer, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics),
  m_layer(layer)
{

  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  std::string fileName="GenericExtRefStave.xml";
  if(const char* env_p = std::getenv("PIXEL_STAVESUPPORT_GEO_XML")) fileName = std::string(env_p);
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
      basics->msgStream()<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      InitializeXML();
      bParsed = ParseFile(file);
    }

  // No XML file was parsed    
  if(!bParsed){
    std::cout<<"XML file "<<fileName<<" not found"<<std::endl;
    return;
  }


  std::cout<<"PixelExtRefStaveXMLHelper for layer "<<m_layer<<std::endl;
  m_layerIndices = getChildValue_Indices("PixelStaveGeo","Layer",m_layer);

  std::cout<<"PixelExtRefStaveXMLHelper for layer "<<m_layer<<" "<<m_layerIndices.size()<<" "<<m_layerIndices[0]<<std::endl;
}

PixelExtRefStaveXMLHelper::~PixelExtRefStaveXMLHelper()
{
  TerminateXML();
}


double PixelExtRefStaveXMLHelper::getClearance() const
{
  return getDouble("PixelStaveGeo",m_layerIndices,"ClearanceX");
}

double PixelExtRefStaveXMLHelper::getStaggerDist() const
{
  return getDouble("PixelStaveGeo",m_layerIndices,"StaggerDist");
}

double PixelExtRefStaveXMLHelper::getStaggerSign() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "StaggerSign");
}

double PixelExtRefStaveXMLHelper::getCenterShift() const
{
  return getDouble("PixelStaveGeo",m_layerIndices,"ModuleCenterShift");
}

double PixelExtRefStaveXMLHelper::getBarrelModuleDZ() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "BarrelModuleDZ", 0, -1.);
}


double PixelExtRefStaveXMLHelper::getStaveSupportLength() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "EnvelopeLength");
}

double PixelExtRefStaveXMLHelper::getStaveSupportWidth() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "StaveSupportWidth",0, -1);
}

double PixelExtRefStaveXMLHelper::getStaveSupportThick() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "LadderThickness");
}

std::string PixelExtRefStaveXMLHelper::getStaveSupportMaterial() const
{
  return getString("PixelStaveGeo", m_layerIndices, "StaveSupportMaterialGeo");
}

double PixelExtRefStaveXMLHelper::getServiceOffsetX() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "ServiceOffsetX");
}

double PixelExtRefStaveXMLHelper::getServiceOffsetY() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "ServiceOffsetY");
}
