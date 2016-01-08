/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EndcapRingRef/PixelRingSupportXMLHelper.h"
#include "PathResolver/PathResolver.h"

PixelRingSupportXMLHelper::PixelRingSupportXMLHelper():
  GeoXMLUtils(),
  m_bXMLfileExist(false)
{

  std::string fileName="GenericRingSupport.xml";
  if(const char* env_p = std::getenv("PIXEL_PIXELDISCSUPPORT_GEO_XML")) fileName = std::string(env_p);
  //  std::cout<<"XML disc support : "<<fileName<<std::endl;

  std::string file = PathResolver::find_file (fileName, "DATAPATH");
  InitializeXML();
  bool bParsed = ParseFile(file);

  if(!bParsed){
    //    std::cout<<"XML file "<<fileName<<" not found"<<std::endl;
    return;
  }

  m_bXMLfileExist = true;

}

PixelRingSupportXMLHelper::~PixelRingSupportXMLHelper()
{
  TerminateXML();
}

int PixelRingSupportXMLHelper::getNbSupport(int layer)
{
  if(!m_bXMLfileExist) return 0;

  int layerIndex = getChildValue_Index("PixelRingSupport", "Layer", layer);
  std::string ringGeoName = getString("PixelRingSupport", layerIndex, "RingSupportGeo");
  m_ringGeoIndex = (ringGeoName!="None")? getChildValue_Index("PixelRingSupportGeo", "name", -1, ringGeoName) : -1;
  //  std::cout<<"RING support : "<<ringGeoName<<" "<<m_ringGeoIndex<<std::endl;  

  if(m_ringGeoIndex<0) return 0;
  std::vector<double> v = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"rmin");
  return (int)v.size();
}

double PixelRingSupportXMLHelper::getRingSupportRMin(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"rmin");
  return v[iSupport];
}

double PixelRingSupportXMLHelper::getRingSupportRMax(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"rmax");
  return v[iSupport];
}

double PixelRingSupportXMLHelper::getRingSupportThickness(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"thickness");
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}


std::string PixelRingSupportXMLHelper::getRingSupportMaterial(int iSupport) const
{
  std::vector<std::string> v = getVectorString("PixelRingSupportGeo",m_ringGeoIndex,"material");
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}



