/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
#include "PixelServicesTool/PixelSimpleServiceXMLHelper.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "PathResolver/PathResolver.h"

PixelSimpleServiceXMLHelper::PixelSimpleServiceXMLHelper(IRDBRecordset_ptr table, const InDetDD::ServiceVolumeSchema & schema):
  GeoXMLUtils(),
  m_schema(schema),
  m_bXMLdefined(true)
{
  std::string nodeName = table->nodeName();
  for_each(nodeName.begin(), nodeName.end(), [](char& in){ in = ::toupper(in); });
  std::string envName = "PIXEL_"+nodeName+"_GEO_XML";

  std::string fileName;
  if(const char* env_p = std::getenv(envName.c_str())) fileName = std::string(env_p);
  std::string file = PathResolver::find_file (fileName, "DATAPATH");
  InitializeXML();
  bool bParsed = ParseFile(file);
  
  if(!bParsed){
    m_bXMLdefined = false;
    std::cout<<"XML file "<<fileName<<" not found"<<std::endl;
    return;
    }
}

PixelSimpleServiceXMLHelper::PixelSimpleServiceXMLHelper(std::string envFileName):
  GeoXMLUtils(),
  m_bXMLdefined(true)
{

  std::cout<<"XML helper - PixelSimpleServiceXMLHelper"<<std::endl;
    
  std::string envName = envFileName;
  std::cout<<"SimpleServiceVolumeMakerMgr : env name "<<envName<<std::endl;
  
  std::string fileName;
  if(const char* env_p = std::getenv(envName.c_str())) fileName = std::string(env_p);
  std::string file = PathResolver::find_file (fileName, "DATAPATH");
  std::cout<<" PixelServices : "<<file<<std::endl;
  InitializeXML();
  bool bParsed = ParseFile(file);
  
  if(!bParsed){
    m_bXMLdefined = false;
    std::cout<<"XML file "<<fileName<<" not found"<<std::endl;
    return;
    }
}

PixelSimpleServiceXMLHelper::~PixelSimpleServiceXMLHelper()
{
  TerminateXML();
}

double PixelSimpleServiceXMLHelper::rmin(int index) const
{
  return getDouble("SimpleService", index, m_schema.rmin().c_str());
}


double PixelSimpleServiceXMLHelper::rmax(int index) const
{
  return getDouble("SimpleService", index, m_schema.rmax().c_str());
}


double PixelSimpleServiceXMLHelper::rmin2(int index) const
{
  return getDouble("SimpleService", index, m_schema.rmin2().c_str());
}

double PixelSimpleServiceXMLHelper::rmax2(int index) const
{
  return getDouble("SimpleService", index, m_schema.rmax2().c_str());
}

double PixelSimpleServiceXMLHelper::zmin(int index) const
{
  return getDouble("SimpleService", index, m_schema.zmin().c_str());
}

double PixelSimpleServiceXMLHelper::zmax(int index) const
{
  return getDouble("SimpleService", index, m_schema.zmax().c_str());
}

double PixelSimpleServiceXMLHelper::phiDelta(int index) const
{
  return getDouble("SimpleService", index, m_schema.phiDelta().c_str());
}

double PixelSimpleServiceXMLHelper::width(int index) const
{
  if (m_schema.has_width()) {
    return getDouble("SimpleService", index, m_schema.width().c_str());
  }
  return 0;
}

double PixelSimpleServiceXMLHelper::phiStart(int index) const
{
  return getDouble("SimpleService", index, m_schema.phiStart().c_str());
}

double PixelSimpleServiceXMLHelper::phiStep(int index) const
{
  if (m_schema.has_phiStep()) {
    return getDouble("SimpleService", index, m_schema.phiStep().c_str());
  } 
  return 0;
}

bool PixelSimpleServiceXMLHelper::zsymm(int index) const
{
  return getBoolean("SimpleService", index, m_schema.zsymm().c_str());
}


int PixelSimpleServiceXMLHelper::repeat(int index) const
{
  return getInt("SimpleService", index, m_schema.repeat().c_str());
}

int PixelSimpleServiceXMLHelper::radialDiv(int index) const
{
  if (m_schema.has_radial()) {  
    return getInt("SimpleService", index, m_schema.radialDiv().c_str());
  } else { 
    return 0;
  }
}

std::string PixelSimpleServiceXMLHelper::shapeType(int index) const
{
  if (m_schema.has_shapeType()) {  
    std::string tmp=getString("SimpleService", index, m_schema.shapeType().c_str(),0,"TUBE");
    tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
    return tmp;
    }
  return "UNKNOWN";
}

std::string PixelSimpleServiceXMLHelper::volName(int index) const
{
  std::string tmp=getString("SimpleService", index, m_schema.volName().c_str(),0,"");
  tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
  return tmp;
}

std::string PixelSimpleServiceXMLHelper::materialName(int index) const
{
  std::string tmp=getString("SimpleService", index, m_schema.materialName().c_str());
  tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
  return tmp;

}

int PixelSimpleServiceXMLHelper::volId(int index) const
{
  int volNumber= getInt("SimpleService", index, m_schema.volId().c_str(),0,-1);

  if(volNumber<0)return index;
  return volNumber;
}

unsigned int PixelSimpleServiceXMLHelper::numElements() const
{
  if (!m_bXMLdefined) return 0;
  return getChildCount("PixelServices", 0, "SimpleService");
}


int PixelSimpleServiceXMLHelper::getServiceIndex( std::string srvName) const
{
  int srvIndex = getChildValue_Index("SimpleService",
				     m_schema.volName().c_str(),
				     -1,
				     srvName);
  return srvIndex;
}



bool PixelSimpleServiceXMLHelper::SupportTubeExists(std::string srvName) const
{
  int index=getServiceIndex(srvName);
  if(index<0) return false;

  return true;
}

double PixelSimpleServiceXMLHelper::SupportTubeRMin(std::string srvName) const
{

  int index=getServiceIndex(srvName);
  if(index<0) return -1;

  return rmin(index);
}

double PixelSimpleServiceXMLHelper::SupportTubeRMax(std::string srvName) const
{

  int index=getServiceIndex(srvName);
  if(index<0) return -1;

  return rmax(index);
}

double PixelSimpleServiceXMLHelper::SupportTubeZMin(std::string srvName) const
{

  int index=getServiceIndex(srvName);
  if(index<0) return -1;

  return zmin(index);
}

double PixelSimpleServiceXMLHelper::SupportTubeZMax(std::string srvName) const
{

  int index=getServiceIndex(srvName);
  if(index<0) return -1;

  return zmax(index);
}


