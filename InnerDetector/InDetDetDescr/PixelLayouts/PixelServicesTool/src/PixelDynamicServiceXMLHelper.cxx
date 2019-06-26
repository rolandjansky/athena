/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
#include "PixelServicesTool/PixelDynamicServiceXMLHelper.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

PixelDynamicServiceXMLHelper::PixelDynamicServiceXMLHelper(std::string envFileName, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics)
{
    
  std::string envName = envFileName;
  std::string fileName;
  if(const char* env_p = std::getenv(envName.c_str())) fileName = std::string(env_p);

  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  if(readXMLfromDB)
    {
      msg(MSG::INFO)<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      DBXMLUtils dbUtils(getBasics());
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      m_bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      msg(MSG::DEBUG)<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      InitializeXML();
      m_bParsed = ParseFile(file);
    }
  
  if(!m_bParsed) {
    msg(MSG::WARNING) << "XML file "<<fileName<<"("<<envName<<")"<<" not found"<<endmsg;
    return;
  }
}

PixelDynamicServiceXMLHelper::~PixelDynamicServiceXMLHelper()
{
  TerminateXML();
}


// =================================================================
//  Standard routing
// =================================================================
bool PixelDynamicServiceXMLHelper::routeBarrelStandard() const
{
  return getDouble("StandardRoute", 0, "barrel");
}

bool PixelDynamicServiceXMLHelper::routeEndcapStandard() const
{
  return getDouble("StandardRoute", 0, "endcap");
}

// =================================================================
//  Barrel services
// =================================================================
double PixelDynamicServiceXMLHelper::BarrelEOSLength(int index) const
{
  std::vector<int> srvIndex = getChildValue_Indices("PixelBarrelEOS","Layer", index);
  return getDouble("PixelBarrelEOS", srvIndex, "EosLength");
}

double PixelDynamicServiceXMLHelper::BarrelEOSHalfThick(int index) const
{
  std::vector<int> srvIndex = getChildValue_Indices("PixelBarrelEOS","Layer", index);
  return getDouble("PixelBarrelEOS", srvIndex, "EosHalfThick");
}

double PixelDynamicServiceXMLHelper::BarrelEOSTolerance(int index) const
{
  std::vector<int> srvIndex = getChildValue_Indices("PixelBarrelEOS","Layer", index);
  return getDouble("PixelBarrelEOS", srvIndex, "EosTolerance");
}


// =================================================================
//  Endcap services
// =================================================================
double PixelDynamicServiceXMLHelper::EndcapEOSLength(int index) const
{
  std::vector<int> srvIndex = getChildValue_Indices("PixelEndcapEOS","Disc", index);
  return getDouble("PixelEndcapEOS", srvIndex, "EosLength");
}

double PixelDynamicServiceXMLHelper::EndcapEOSRMin(int index) const
{
  std::vector<int> srvIndex = getChildValue_Indices("PixelEndcapEOS","Disc", index);
  return getDouble("PixelEndcapEOS", srvIndex, "EosRMin");
}


// =================================================================
//  Endcap service offset
// =================================================================
double PixelDynamicServiceXMLHelper::EndcapEOSOffset(int index) const
{
  std::vector<int> srvIndex = getChildValue_Indices("PixelEndcapOffset","Disc", index);
  if(srvIndex.size()<1) return -1.;
  return getDouble("PixelEndcapOffset", srvIndex, "EosOffset");
}


std::string PixelDynamicServiceXMLHelper::EndcapDiscRoute(int discRoute) const
{
  std::vector<std::string>nodeList = getNodeList("PixelEndcapRoute");
  
  if(nodeList.size()<1) return "StdRoute";

  for(auto& it : nodeList){
    std::vector<int> discIndices = getVectorInt("PixelEndcapRoute",0,it.c_str());
    for(auto& it2 : discIndices) if(discRoute==it2) return it;
  }
  
  return "StdRoute";

}


//**********************************************************************************
