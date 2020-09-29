/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "EndcapRingRef/PixelRingSupportXMLHelper.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

PixelRingSupportXMLHelper::PixelRingSupportXMLHelper(const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics),
  m_bXMLfileExist(false)
{

  std::string fileName="GenericRingSupport.xml";
  if(const char* env_p = std::getenv("PIXEL_PIXELDISCSUPPORT_GEO_XML")) fileName = std::string(env_p);
  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  bool bParsed=false;
  if(readXMLfromDB)
    {
      basics->msgStream()<<MSG::DEBUG<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      DBXMLUtils dbUtils(getBasics());
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      setSchemaVersion(dbUtils.getSchemaVersion(fileName)); //my 
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      basics->msgStream()<<MSG::DEBUG<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      InitializeXML();
      bParsed = ParseFile(file);
    } 

  if(!bParsed){
    basics->msgStream()<<MSG::ERROR<<"XML file "<<fileName<<" not found"<<endreq;
    return;
  }

  m_bXMLfileExist = true;

}

PixelRingSupportXMLHelper::~PixelRingSupportXMLHelper()
{
  TerminateXML();
}

bool PixelRingSupportXMLHelper::putLHBeforeRHrings(){
  bool m_LHBeforeRHrings = 0;
  if(getSchemaVersion() > 6) {
    return getBoolean("LHBeforeRHRings",m_LHBeforeRHrings, "putLHBeforeRHRings");
  }
  else return true;
}

bool PixelRingSupportXMLHelper::swapFrontBackModulePhiPosition(){
  bool m_swapFrontBack = 0;
  if(getSchemaVersion() > 6) return getBoolean("ModulePhiPosition",m_swapFrontBack, "swapFrontBack");
  else return false;
}

int PixelRingSupportXMLHelper::getNbSupport(int layer, int ring) 
{
  if(!m_bXMLfileExist) return 0;

  int layerIndex = getChildValue_Index("PixelRingSupport", "Layer", layer);
  std::vector<std::string> v_ringGeoName = getVectorString("PixelRingSupport", layerIndex, "RingSupportGeo");
  std::string ringGeoName = int(v_ringGeoName.size())>ring ? v_ringGeoName[ring] : v_ringGeoName[0];
  m_ringGeoIndex = (ringGeoName!="None")? getChildValue_Index("PixelRingSupportGeo", "name", -1, ringGeoName) : -1;

  if(m_ringGeoIndex<0) return 0;
  std::vector<double> vt = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"thickness");
  return (int)vt.size();
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

int PixelRingSupportXMLHelper::getRingSupportNSectors(int iSupport) const
{
  std::vector<int> v = getVectorInt("PixelRingSupportGeo",m_ringGeoIndex,"nsectors");
  if (v.size()<1) return 1;
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}

double PixelRingSupportXMLHelper::getRingSupportSPhi(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"sphi");
  if (v.size()<1) return 0.;
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}

double PixelRingSupportXMLHelper::getRingSupportDPhi(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"dphi");
  if (v.size()<1) return 360.;
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}

std::string PixelRingSupportXMLHelper::getRingSupportMaterial(int iSupport) const
{
  std::vector<std::string> v = getVectorString("PixelRingSupportGeo",m_ringGeoIndex,"material");
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

int PixelRingSupportXMLHelper::getNbLayerSupport(int layer)
{
  if(!m_bXMLfileExist) return 0;

  int layerIndex = getChildValue_Index("PixelLayerSupport", "Layer", layer);
  if ( layerIndex < 0 ) return 0;

  std::string ringGeoName = getString("PixelLayerSupport", layerIndex, "LayerSupportGeo");
  m_ringGeoIndex = (ringGeoName!="None")? getChildValue_Index("PixelLayerSupportGeo", "name", -1, ringGeoName) : -1;

  if(m_ringGeoIndex<0) return 0;
  return 1;
}

std::vector<int> PixelRingSupportXMLHelper::getNbLayerSupportIndex(int layer)
{
  std::vector<int> layers;
  
  if(!m_bXMLfileExist) return layers;

  int layerIndex = getChildValue_Index("PixelLayerSupport", "Layer", layer);
  if ( layerIndex < 0 ) return layers;
  std::string ringGeoName = getString("PixelLayerSupport", layerIndex, "LayerSupportGeo");
   
  // using the first name support to get the layer index
  std::stringstream ss(ringGeoName);
  std::string item;
  
  while (ss>>item) {
    if(item.size()==0) continue;
       
    m_ringGeoIndex = (ringGeoName!="None")? getChildValue_Index("PixelLayerSupportGeo", "name", -1, item) : -1;
    layers.push_back(m_ringGeoIndex);
  }
  return layers;
}

std::vector<int> PixelRingSupportXMLHelper::getNbShellSupportIndex(int layer)
{
  std::vector<int> shells;
  
  if(!m_bXMLfileExist) return shells;

  int layerIndex = getChildValue_Index("PixelLayerSupportShell", "Layer", layer);
  if ( layerIndex < 0 ) return shells;
  std::string ringGeoName = getString("PixelLayerSupport", layerIndex, "LayerSupportShell");
   
  // using the first name support to get the layer index
  std::stringstream ss(ringGeoName);
  std::string item;
  
  while (ss>>item) {
    if(item.size()==0) continue;
       
    m_ringGeoIndex = (ringGeoName!="None")? getChildValue_Index("PixelLayerSupportShell", "name", -1, item) : -1;
    shells.push_back(m_ringGeoIndex);
  }

   return shells;
}

std::vector<double> PixelRingSupportXMLHelper::getLayerSupportRadius(int /*iSupport*/) const
{
  std::vector<double> v = getVectorDouble("PixelLayerSupportGeo",m_ringGeoIndex,"r");
  return v;
}

std::vector<double> PixelRingSupportXMLHelper::getLayerSupportZ(int /*iSupport*/) const
{
  std::vector<double> v = getVectorDouble("PixelLayerSupportGeo",m_ringGeoIndex,"z");
  return v;
}

std::string PixelRingSupportXMLHelper::getLayerSupportMaterial(int iSupport) const
{
  std::vector<std::string> v = getVectorString("PixelLayerSupportGeo",m_ringGeoIndex,"material");
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}

std::vector<double> PixelRingSupportXMLHelper::getLayerSupportRadiusAtIndex(int index) const
{
  std::vector<double> v = getVectorDouble("PixelLayerSupportGeo",index,"r");
  return v;
}

std::vector<double> PixelRingSupportXMLHelper::getLayerSupportZAtIndex(int index) const
{
  std::vector<double> v = getVectorDouble("PixelLayerSupportGeo",index,"z");
  return v;
}

std::string PixelRingSupportXMLHelper::getLayerSupportMaterialAtIndex(int index) const
{
  std::vector<std::string> v = getVectorString("PixelLayerSupportGeo",index,"material");
  int myIndex = (v.size()==1)? 0 : index;
  return v[myIndex];
}

//============== SHELL DESCRIPTION ==================

std::vector<double>  PixelRingSupportXMLHelper::getLayerShellRadius(int layer) const
{
  std::vector<double> v; 
  int layerIndex = getChildValue_Index("PixelLayerSupportShell", "Layer", layer);
  if (layerIndex<0) return v; 
  return getVectorDouble("PixelLayerSupportShell",layerIndex,"r");

}

std::vector<double>   PixelRingSupportXMLHelper::getLayerShellZBounds(int layer) const
{
  std::vector<double> v; 
  int layerIndex = getChildValue_Index("PixelLayerSupportShell", "Layer", layer);
  if (layerIndex<0) return v; 
  return getVectorDouble("PixelLayerSupportShell",layerIndex,"z");
}

std::vector<double> PixelRingSupportXMLHelper::getCorrugatedShellZClearance(int layer) const
{
  std::vector<double> v; 
  int layerIndex = getChildValue_Index("PixelLayerSupportShell", "Layer", layer);
  if (layerIndex<0) return v; 
  return getVectorDouble("PixelLayerSupportShell",layerIndex,"CorrugatedZClearance");
}

double PixelRingSupportXMLHelper::getCorrugatedCableClearance(int layer) const
{
  int layerIndex = getChildValue_Index("PixelLayerSupportShell", "Layer", layer);
  if (layerIndex<0) return -1.; 
  return getDouble("PixelLayerSupportShell",layerIndex,"CorrugatedCableClearance");
}

double PixelRingSupportXMLHelper::getCorrugatedShellDepth(int layer) const
{
  int layerIndex = getChildValue_Index("PixelLayerSupportShell", "Layer", layer);
  if (layerIndex<0) return -1.; 
  return getDouble("PixelLayerSupportShell",layerIndex,"CorrugatedShellDepth");
}

double PixelRingSupportXMLHelper::getLayerShellThickness(int layer) const
{
  int layerIndex = getChildValue_Index("PixelLayerSupportShell", "Layer", layer);
  if (layerIndex<0) return -1.; 
  return getDouble("PixelLayerSupportShell",layerIndex,"thickness");
}

std::string PixelRingSupportXMLHelper::getLayerShellMaterial(int layer) const
{
  int layerIndex = getChildValue_Index("PixelLayerSupportShell", "Layer", layer);
  if (layerIndex<0) return ""; 
  return getString("PixelLayerSupportShell",layerIndex,"material");
}

