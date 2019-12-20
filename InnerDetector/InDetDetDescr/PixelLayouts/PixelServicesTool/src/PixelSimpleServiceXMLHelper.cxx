/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
#include "PixelServicesTool/PixelSimpleServiceXMLHelper.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

PixelSimpleServiceXMLHelper::PixelSimpleServiceXMLHelper(IRDBRecordset_ptr table, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics),
  m_bXMLdefined(true)
{
  std::string nodeName = table->nodeName();
  for_each(nodeName.begin(), nodeName.end(), [](char& in){ in = ::toupper(in); });
  std::string envName = "PIXEL_"+nodeName+"_GEO_XML";

  this->Setup(envName);
}

PixelSimpleServiceXMLHelper::PixelSimpleServiceXMLHelper(const std::string& envName, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics),
  m_bXMLdefined(true)
{
  this->Setup(envName);
}

// Both constructors call this piece of code to initiliase the class
void PixelSimpleServiceXMLHelper::Setup(const std::string& envName)
{
  msg(MSG::DEBUG) << "XML helper - PixelSimpleServiceXMLHelper" << endmsg;   
  msg(MSG::DEBUG) << "SimpleServiceVolumeMakerMgr : env name " << envName<<endmsg;
  
  std::string fileName;
  if (const char* env_p = std::getenv(envName.c_str())) fileName = std::string(env_p);
  if (fileName.size()==0){
    m_bXMLdefined = false;
    return;
  }

  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  msg(MSG::DEBUG)<<"Build material table from XML  (DB XML file : "<<readXMLfromDB<<" )"<<endmsg;

  bool bParsed=false;
  if(readXMLfromDB) {
    msg(MSG::DEBUG)<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
    DBXMLUtils dbUtils(getBasics());
    std::string XMLtext = dbUtils.readXMLFromDB(fileName);
    setSchemaVersion(dbUtils.getSchemaVersion(fileName));
    InitializeXML();
    bParsed = ParseBuffer(XMLtext,std::string(""));
  } else {
    msg(MSG::DEBUG)<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
    std::string file = PathResolver::find_file (fileName, "DATAPATH");
    msg(MSG::DEBUG)<< " PixelServices : "<<file<<endmsg;
    InitializeXML();
    bParsed = ParseFile(file);
  }
  
  if(!bParsed){
    m_bXMLdefined = false;
    msg(MSG::WARNING)<<"XML file "<<fileName<<" not found"<<endmsg;
    return;
  }
}



PixelSimpleServiceXMLHelper::~PixelSimpleServiceXMLHelper()
{
  TerminateXML();
}

double PixelSimpleServiceXMLHelper::rmin(int index) const
{
  return getDouble("SimpleService", index, "RIN");
}


double PixelSimpleServiceXMLHelper::rmax(int index) const
{
  return getDouble("SimpleService", index, "ROUT");
}


double PixelSimpleServiceXMLHelper::rmin2(int index) const
{
  if(getSchemaVersion() > 4){
    return getDouble("SimpleService", index, "RIN2");
  }
  
  msg(MSG::DEBUG)<<"XML: SimpleService rmin2 not fully defined in old schema ("<<getSchemaVersion()<<") returning 0 except for CONE shapes..."<<endreq;
  if (shapeType(index)=="CONE") return getDouble("SimpleService", index, "RIN2");
  return 0.0;
}

double PixelSimpleServiceXMLHelper::rmax2(int index) const
{
  if(getSchemaVersion() > 4){
    return getDouble("SimpleService", index, "ROUT2");
  }

  msg(MSG::DEBUG)<<"XML: SimpleService rmax2 not fully defined in old schema ("<<getSchemaVersion()<<") returning 0 except for CONE shapes..."<<endreq;
  if (shapeType(index)=="CONE") return getDouble("SimpleService", index, "ROUT2");
  return 0.0;
}

double PixelSimpleServiceXMLHelper::zmin(int index) const
{
  return getDouble("SimpleService", index, "ZIN");
}

double PixelSimpleServiceXMLHelper::zmax(int index) const
{
  return getDouble("SimpleService", index, "ZOUT");
}

// TODO: change xml tag from WIDTH to PHIDELTA
double PixelSimpleServiceXMLHelper::phiDelta(int index) const
{
  if(getSchemaVersion() > 4){
    return getDouble("SimpleService", index, "WIDTH");
  }
  
  msg(MSG::DEBUG)<<"XML: SimpleService phiDelta not fully defined in old schema ("<<getSchemaVersion()<<") returning 0 except for TUBS shapes..."<<endreq;
  if (shapeType(index)=="TUBS") return getDouble("SimpleService", index, "WIDTH");
  return 0.0;
}

double PixelSimpleServiceXMLHelper::width(int index) const
{
  if(getSchemaVersion() > 4){
    return getDouble("SimpleService", index, "WIDTH");
  }
  
  msg(MSG::DEBUG)<<"XML: SimpleService width not fully defined in old schema ("<<getSchemaVersion()<<") returning 0 except for TUBS shapes..."<<endreq;
  if (shapeType(index)=="TUBS") return getDouble("SimpleService", index, "WIDTH");
  return 0.0;
}

// TODO: change ml tag from Phi to PhiStart
double PixelSimpleServiceXMLHelper::phiStart(int index) const
{
   if(getSchemaVersion() > 4){
     return getDouble("SimpleService", index, "PHI");
   }
   
   msg(MSG::DEBUG)<<"XML: SimpleService phiStart not fully defined in old schema ("<<getSchemaVersion()<<") returning 0 except for TUBS shapes..."<<endreq;
   if (shapeType(index)=="TUBS") return getDouble("SimpleService", index, "PHI");
   return 0.0;
}

// Not implemented, saving for future reference
//double PixelSimpleServiceXMLHelper::phiStep(int index) const
//{
//  return 0;
//}

bool PixelSimpleServiceXMLHelper::zsymm(int index) const
{
  return getBoolean("SimpleService", index, "ZSYMM");
}


int PixelSimpleServiceXMLHelper::repeat(int index) const
{
  if(getSchemaVersion() > 4){
    return getInt("SimpleService", index, "REPEAT");
  }
  else msg(MSG::DEBUG)<<"XML: SimpleService repeat not fully defined in old schema ("<<getSchemaVersion()<<") returning 0 except for TUBS shapes..."<<endreq;
  if (shapeType(index)=="TUBS") return getDouble("SimpleService", index, "REPEAT");
  return 0;
}

// Not implemented, saving for future reference
//int PixelSimpleServiceXMLHelper::radialDiv(int index) const
//{
//  return 0;
//}

std::string PixelSimpleServiceXMLHelper::shapeType(int index) const
{
  std::string tmp=getString("SimpleService", index, "SHAPE", 0);
  tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
  return tmp;
}

std::string PixelSimpleServiceXMLHelper::volName(int index) const
{
  std::string tmp=getString("SimpleService", index, "VOLNAME", 0);
  tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
  return tmp;
}

std::string PixelSimpleServiceXMLHelper::materialName(int index) const
{
  std::string tmp=getString("SimpleService", index, "MATERIALNAME");
  tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
  return tmp;
}

unsigned int PixelSimpleServiceXMLHelper::numElements() const
{
  if (!m_bXMLdefined) return 0;
  return getChildCount("PixelServices", 0, "SimpleService");
}


int PixelSimpleServiceXMLHelper::getServiceIndex( const std::string& srvName) const
{
  int srvIndex = getChildValue_Index("SimpleService", "VOLNAME", -1, srvName);
  return srvIndex;
}


// FIXME: too specific, returns info on any named volume, not just support tubes
bool PixelSimpleServiceXMLHelper::SupportTubeExists(const std::string& srvName) const
{
  int index=getServiceIndex(srvName);
  if(index<0) return false;
  return true;
}

double PixelSimpleServiceXMLHelper::SupportTubeRMin(const std::string& srvName) const
{
  int index=getServiceIndex(srvName);
  if(index<0) return -1;
  return rmin(index);
}

double PixelSimpleServiceXMLHelper::SupportTubeRMax(const std::string& srvName) const
{

  int index=getServiceIndex(srvName);
  if(index<0) return -1;

  return rmax(index);
}

double PixelSimpleServiceXMLHelper::SupportTubeZMin(const std::string& srvName) const
{

  int index=getServiceIndex(srvName);
  if(index<0) return -1;

  return zmin(index);
}

double PixelSimpleServiceXMLHelper::SupportTubeZMax(const std::string& srvName) const
{
  int index=getServiceIndex(srvName);
  if(index<0) return -1;
  return zmax(index);
}
