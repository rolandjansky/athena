/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"
#include "PathResolver/PathResolver.h"
#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

PixelGeneralXMLHelper::PixelGeneralXMLHelper(std::string envFileName, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(), PixelGeoBuilder(basics)
{

  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  bool bParsed = false;
  std::string fileName_brl="GenericPixelGeneral.xml";
  if(readXMLfromDB)
    {
      if(const char* env_p = std::getenv(envFileName.c_str())) fileName_brl = std::string(env_p);
      getBasics()->msgStream()<<MSG::DEBUG<<"XML input : DB CLOB "<<fileName_brl<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      DBXMLUtils dbUtils(basics);
      std::string XMLtext = dbUtils.readXMLFromDB(fileName_brl);
      setSchemaVersion(dbUtils.getSchemaVersion(fileName_brl));
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      // Access XML file
      if(const char* env_p = std::getenv(envFileName.c_str())) fileName_brl = std::string(env_p);
      getBasics()->msgStream()<<MSG::DEBUG<<"XML input : from file "<<fileName_brl<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      std::string file_brl = PathResolver::find_file (fileName_brl, "DATAPATH");
      InitializeXML();
      bParsed = ParseFile(file_brl);
    }

  if(!bParsed){
    getBasics()->msgStream()<<MSG::ERROR<<"XML file "<<fileName_brl<<" not found"<<endreq;
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
  msg(MSG::DEBUG)<<"GEOPIXELSERVICES : # radii : "<<res.size()<<std::endl;
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

std::vector<double> PixelGeneralXMLHelper::getBarrelSupportZOffsetList() const
{
  if(getSchemaVersion() > 1) {
    return getVectorDouble("PixelBarrelSupport", 0, "ZOffsetList");
  }
  else msg(MSG::DEBUG)<<"XML: PixelBarrelSupport ZOffsetList not defined in old schema ("<<getSchemaVersion()<<") returning empty vector<double>..."<<endreq;
  std::vector<double> v;
  return v;
}

std::vector<double> PixelGeneralXMLHelper::getBarrelSupportThicknessList() const
{
  if(getSchemaVersion() > 1) {
    return getVectorDouble("PixelBarrelSupport", 0, "ThicknessList");
  }
  else msg(MSG::DEBUG)<<"XML: PixelBarrelSupport ThicknessList not defined in old schema ("<<getSchemaVersion()<<") returning empty vector<double>..."<<endreq;
  std::vector<double> v;
  return v;
}

std::vector<std::string> PixelGeneralXMLHelper::getBarrelSupportMaterialList() const
{
  if(getSchemaVersion() > 1) {
    return getVectorString("PixelBarrelSupport", 0, "MaterialList");
  }
  else msg(MSG::DEBUG)<<"XML: PixelBarrelSupport MaterialList not defined in old schema ("<<getSchemaVersion()<<") returning empty vector<string>..."<<endreq;
  std::vector<std::string> v;
  return v;
}

std::vector<double> PixelGeneralXMLHelper::getBarrelSupportRminInnerList() const
{
  if(getSchemaVersion() > 1) {
    return getVectorDouble("PixelBarrelSupport", 0, "RminInnerList");
  }
  else msg(MSG::DEBUG)<<"XML: PixelBarrelSupport RminInnerList not defined in old schema ("<<getSchemaVersion()<<") returning empty vector<double>..."<<endreq;
  std::vector<double> v;
  return v;
  
}

std::vector<double> PixelGeneralXMLHelper::getBarrelSupportRmaxInnerList() const
{
   if(getSchemaVersion() > 1) {
     return getVectorDouble("PixelBarrelSupport", 0, "RmaxInnerList");
   }
   else msg(MSG::DEBUG)<<"XML: PixelBarrelSupport RmaxInnerList not defined in old schema ("<<getSchemaVersion()<<") returning empty vector<double>..."<<endreq;
  std::vector<double> v;
  return v;
}

std::vector<double> PixelGeneralXMLHelper::getBarrelSupportRminOuterList() const
{
   if(getSchemaVersion() > 1) {
     return getVectorDouble("PixelBarrelSupport", 0, "RminOuterList");
   }
   else msg(MSG::DEBUG)<<"XML: PixelBarrelSupport RminOuterList not defined in old schema ("<<getSchemaVersion()<<") returning empty vector<double>..."<<endreq;
   std::vector<double> v;
   return v;
}

std::vector<double> PixelGeneralXMLHelper::getBarrelSupportRmaxOuterList() const
{
  if(getSchemaVersion() > 1) {
    return getVectorDouble("PixelBarrelSupport", 0, "RmaxOuterList");
  }
  else msg(MSG::DEBUG)<<"XML: PixelBarrelSupport RmaxOuterList not defined in old schema ("<<getSchemaVersion()<<") returning empty vector<double>..."<<endreq;
  std::vector<double> v;
   return v;
}

std::vector<int> PixelGeneralXMLHelper::getBarrelSupportNSectorsList() const
{
  if(getSchemaVersion() > 1) {
    return getVectorInt("PixelBarrelSupport", 0, "NSectorsList");
  }
  else msg(MSG::DEBUG)<<"XML: PixelBarrelSupport NSectorsList not defined in old schema ("<<getSchemaVersion()<<") returning empty vector<int>..."<<endreq;
  std::vector<int> v;
  return v;
}

std::vector<double> PixelGeneralXMLHelper::getBarrelSupportSPhiList() const
{
  if(getSchemaVersion() > 1) {
    return getVectorDouble("PixelBarrelSupport", 0, "SPhiList");
  }
  else msg(MSG::DEBUG)<<"XML: PixelBarrelSupport SPhiList not defined in old schema ("<<getSchemaVersion()<<") returning empty vector<double>..."<<endreq;
  std::vector<double> v;
  return v;
}

std::vector<double> PixelGeneralXMLHelper::getBarrelSupportDPhiList() const
{
  if(getSchemaVersion() > 1) {
    return getVectorDouble("PixelBarrelSupport", 0, "DPhiList");
  }
  else msg(MSG::DEBUG)<<"XML: PixelBarrelSupport DPhiList not defined in old schema ("<<getSchemaVersion()<<") returning empty vector<double>..."<<endreq;
  std::vector<double> v;
  return v;
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

bool PixelGeneralXMLHelper::isBCMPrimePresent() const
{
  if (getSchemaVersion() > 4) return getBoolean("PixelGeneral", 0, "BCMPrime");
  else  {
    msg(MSG::DEBUG)<<"XML: Old schema ("<<getSchemaVersion()<<"), disabling BCM'";
    return false;
  }
}

bool PixelGeneralXMLHelper::propagateDeadEdgeToSensorPosition() const
{
  if (getSchemaVersion() > 5) return getBoolean("PixelGeneral", 0, "HasSensorDeadEdge");
  else  {
    msg(MSG::DEBUG)<<"XML: Old schema ("<<getSchemaVersion()<<"), dead edges not propagated to sensor positions";
    return false;
  }
}

double PixelGeneralXMLHelper::getLayerRMin(int ilayer) const
{
  std::ostringstream ostr; 
  ostr << "LayerMin" << ilayer;
 
  return getDouble("PixelLayerEnvelope", 0, ostr.str().c_str(),0);
}

double PixelGeneralXMLHelper::getLayerRMax(int ilayer) const
{
  std::ostringstream ostr; 
  ostr << "LayerMax" << ilayer;
 
  return getDouble("PixelLayerEnvelope", 0, ostr.str().c_str(),0);
}
