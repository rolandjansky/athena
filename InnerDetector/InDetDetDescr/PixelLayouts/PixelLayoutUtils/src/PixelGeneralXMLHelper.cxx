/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"
#include "PathResolver/PathResolver.h"

PixelGeneralXMLHelper::PixelGeneralXMLHelper(std::string envFileName):
  GeoXMLUtils()
{

  std::cout<<"XML helper - PixelGeneralXMLHelper"<<std::endl;

  // Access XML file
  std::string fileName_brl="GenericPixelGeneral.xml";
  if(const char* env_p = std::getenv(envFileName.c_str())) fileName_brl = std::string(env_p);
  std::cout<<"XML file - PixelGeneral  "<<fileName_brl<<" / "<<envFileName<<std::endl;

  std::string file_brl = PathResolver::find_file (fileName_brl, "DATAPATH");
  InitializeXML();
  bool bParsed_brl = ParseFile(file_brl);

  if(!bParsed_brl){
    std::cout<<"XML file "<<fileName_brl<<" not found"<<std::endl;
    return;
  }


}

PixelGeneralXMLHelper::~PixelGeneralXMLHelper()
{
  TerminateXML();
}

double PixelGeneralXMLHelper::getEnvelopeRMin() const
{
  return getDouble("PixelEnvelope", 0, "RadiusMin");
}

double PixelGeneralXMLHelper::getEnvelopeRMax() const
{
  return getDouble("PixelEnvelope", 0, "RadiusMax");
}

double PixelGeneralXMLHelper::getEnvelopeHalfLength() const
{
  return getDouble("PixelEnvelope", 0, "HalfLength");
}

double PixelGeneralXMLHelper::getCommonHalfLength() const
{
  return getDouble("PixelCommon", 0, "HalfLength");
}


double PixelGeneralXMLHelper::getBarrelRMin() const
{
  return getDouble("PixelBarrelEnvelope", 0, "RadiusMin");
}

double PixelGeneralXMLHelper::getBarrelRMax() const
{
  return getDouble("PixelBarrelEnvelope", 0, "RadiusMax");
}

double PixelGeneralXMLHelper::getBarrelHalfLength() const
{
  return getDouble("PixelBarrelEnvelope", 0, "HalfLength");
}

bool PixelGeneralXMLHelper::isBarrelCylindrical() const
{
  std::vector<double> res = getBarrelRadiusList();
  std::cout<<"GEOPIXELSERVICES : # radii : "<<res.size()<<std::endl;
  return (res.size()==0);
}

std::vector<double> PixelGeneralXMLHelper::getBarrelRadiusList() const
{
  return getVectorDouble("PixelBarrelEnvelope", 0, "RadiusList");
}

std::vector<double> PixelGeneralXMLHelper::getBarrelHalfLengthList() const
{
  return getVectorDouble("PixelBarrelEnvelope", 0, "HalfLengthList");
}

double PixelGeneralXMLHelper::getEndcapRMin() const
{
  return getDouble("PixelEndcapEnvelope", 0, "RadiusMin");
}

double PixelGeneralXMLHelper::getEndcapRMax() const
{
  return getDouble("PixelEndcapEnvelope", 0, "RadiusMax");
}

double PixelGeneralXMLHelper::getEndcapZMin() const
{
  return getDouble("PixelEndcapEnvelope", 0, "ZMin");
}

double PixelGeneralXMLHelper::getEndcapZMax() const
{
  return getDouble("PixelEndcapEnvelope", 0, "ZMax");
}

bool PixelGeneralXMLHelper::isEndcapCylindrical() const
{
  std::vector<double> res = getEndcapRadiusList();
  return (res.size()>0);
}

std::vector<double> PixelGeneralXMLHelper::getEndcapRadiusList() const
{
  return getVectorDouble("PixelEndcapEnvelope", 0, "RadiusList");
}

std::vector<double> PixelGeneralXMLHelper::getEndcapZMinList() const
{
  return getVectorDouble("PixelEndcapEnvelope", 0, "ZMinList");
}

bool PixelGeneralXMLHelper::isBarrelPresent() const
{
  return getBoolean("PixelGeneral", 0, "Barrel");
}

bool PixelGeneralXMLHelper::isEndcapPresentA() const
{
  return getBoolean("PixelGeneral", 0, "EndcapA");
}

bool PixelGeneralXMLHelper::isEndcapPresentC() const
{
  return getBoolean("PixelGeneral", 0, "EndcapC");
}

double PixelGeneralXMLHelper::getLayerRMin(int ilayer) const
{
  std::ostringstream ostr; 
  ostr << "LayerMin" << ilayer;

  std::cout<<"Read pixel layer rmin "<<std::cout;
 
  return getDouble("PixelLayerEnvelope", 0, ostr.str().c_str(),0, 0.);
}

double PixelGeneralXMLHelper::getLayerRMax(int ilayer) const
{
  std::ostringstream ostr; 
  ostr << "LayerMax" << ilayer;
 
  return getDouble("PixelLayerEnvelope", 0, ostr.str().c_str(),0, 99999.);
}
