/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelLayoutLoI/PixelDiscSupportXMLHelper.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

PixelDiscSupportXMLHelper::PixelDiscSupportXMLHelper( const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics),
  m_bXMLfileExist(false)
{

  std::string fileName="GenericRingSupport.xml";
  if(const char* env_p = std::getenv("PIXEL_PIXELDISCSUPPORT_GEO_XML")) fileName = std::string(env_p);
  std::cout<<"XML disc support : "<<fileName<<std::endl;

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

  m_bXMLfileExist = true;

}

PixelDiscSupportXMLHelper::~PixelDiscSupportXMLHelper()
{
  TerminateXML();
}

int PixelDiscSupportXMLHelper::getNbSupport(int layer)
{
  if(!m_bXMLfileExist) return 0;

  int layerIndex = getChildValue_Index("PixelDiscSupport", "Layer", layer);

  //  std::cout<<"Disc support "<<layer<<" "<<layerIndex<<std::endl;

  std::string ringGeoName = getString("PixelDiscSupport", layerIndex, "RingSupportGeo");
  m_ringGeoIndex = (ringGeoName!="None")? getChildValue_Index("PixelDiscSupportGeo", "name", -1, ringGeoName) : -1;
  //  std::cout<<"RING support : "<<ringGeoName<<" "<<m_ringGeoIndex<<std::endl;  

  if(m_ringGeoIndex<0) return 0;
  std::vector<double> v = getVectorDouble("PixelDiscSupportGeo",m_ringGeoIndex,"rmin");
  return (int)v.size();
}

double PixelDiscSupportXMLHelper::getRingSupportRMin(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelDiscSupportGeo",m_ringGeoIndex,"rmin");
  return v[iSupport];
}

double PixelDiscSupportXMLHelper::getRingSupportRMax(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelDiscSupportGeo",m_ringGeoIndex,"rmax");
  return v[iSupport];
}

double PixelDiscSupportXMLHelper::getRingSupportThickness(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelDiscSupportGeo",m_ringGeoIndex,"thickness");
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}


std::string PixelDiscSupportXMLHelper::getRingSupportMaterial(int iSupport) const
{
  std::vector<std::string> v = getVectorString("PixelDiscSupportGeo",m_ringGeoIndex,"material");
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}



