/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelLayoutAlpine/PixelStaveSupportXMLHelper.h"
#include "PathResolver/PathResolver.h"

PixelStaveSupportXMLHelper::PixelStaveSupportXMLHelper(int layer):
  GeoXMLUtils(),
  m_layer(layer)
{

  std::string fileName="GenericStaveSupport.xml";
  if(const char* env_p = std::getenv("PIXEL_STAVESUPPORT_GEO_XML")) fileName = std::string(env_p);

  std::string file = PathResolver::find_file (fileName, "DATAPATH");
  InitializeXML();
  bool bParsed = ParseFile(file);

  if(!bParsed){
    std::cout<<"XML file "<<fileName<<" not found"<<std::endl;
    return;
  }

  std::string staveGeoName = getString("PixelStaveSupport", m_layer, "StaveSupportGeo");
  m_staveGeoIndex=getChildValue_Index("PixelStaveSupportGeo", "name", -1, staveGeoName);
  std::string staveGeoNameEC = getString("PixelStaveSupport", m_layer, "StaveSupportGeoEC");
  m_staveGeoIndexEC = (staveGeoNameEC=="") ? -1 : getChildValue_Index("PixelStaveSupportGeo", "name", -1, staveGeoNameEC);

}

PixelStaveSupportXMLHelper::~PixelStaveSupportXMLHelper()
{
  TerminateXML();
}

double PixelStaveSupportXMLHelper::getMechanicalStaveLength() const
{
  return 2. * getDouble("PixelStaveSupport", m_layer, "Length");
}

double PixelStaveSupportXMLHelper::getStaveLength() const
{
  return 2. * getDouble("PixelStaveSupport", m_layer, "Length");
}

double PixelStaveSupportXMLHelper::getMechanicalStaveWidth() const
{
  return 2. * getDouble("PixelStaveSupport", m_layer, "Width");
}

double PixelStaveSupportXMLHelper::getTubeOuterDiam(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "TubeOuterDiam");
}

double PixelStaveSupportXMLHelper::getTubeInnerDiam(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "TubeInnerDiam");
}

double PixelStaveSupportXMLHelper::getTubeMiddlePos(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "TubeRadialPos");
}

double PixelStaveSupportXMLHelper::getTubeCentralPos(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "TubeCentralPos");
}

double PixelStaveSupportXMLHelper::getTubeGlueThick(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "TubeGlueThick");
}

double PixelStaveSupportXMLHelper::getFacePlateThick(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "FacePlateThick");
}
double PixelStaveSupportXMLHelper::getFacePlateWidth(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "FacePlateWidth");
}
double PixelStaveSupportXMLHelper::getFacePlateGlueThick(int geoIndex) const
{
 return  getDouble("PixelStaveSupportGeo", geoIndex, "FacePlateGlueThick");
}
double PixelStaveSupportXMLHelper::getFacePlateGreaseThick(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "FacePlateGreaseThick");
}

double PixelStaveSupportXMLHelper::getFoamBaseThick(int geoIndex) const
{
   return getDouble("PixelStaveSupportGeo", geoIndex, "FoamBaseThick");
}
double PixelStaveSupportXMLHelper::getFoamBaseThickTrans(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "FoamBaseThickTR");
}
double PixelStaveSupportXMLHelper::getFoamEdgeThick(int geoIndex) const
{
   return getDouble("PixelStaveSupportGeo", geoIndex, "FoamEdgeThick");
}
double PixelStaveSupportXMLHelper::getFoamEdgeThickTrans(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "FoamEdgeThickTR");
}
double PixelStaveSupportXMLHelper::getFoamThick(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "FoamThick");
}


double PixelStaveSupportXMLHelper::getOmegaThick(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "OmegaThick");
}
double PixelStaveSupportXMLHelper::getOmegaGlueThick(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "OmegaGlueThick");
}
double PixelStaveSupportXMLHelper::getOmegaMiddleThick(int geoIndex) const
{
  return getDouble("PixelStaveSupportGeo", geoIndex, "OmegaMiddleThick");
}
double PixelStaveSupportXMLHelper::getOmegaTopWidth(int geoIndex) const
{
 return getDouble("PixelStaveSupportGeo", geoIndex, "OmegaTopWidth");
}




int PixelStaveSupportXMLHelper::getStaveFlexIndex() const
{
  int flexIndex = getChildValue_Index("StaveFlexGeo", "Layer", m_layer);
  return flexIndex;
}

int PixelStaveSupportXMLHelper::getStaveFlexSectionNumber(int flexIndex) const
{
  if(flexIndex<0) return 0;
  std::vector<double >v = getVectorDouble("StaveFlexGeo", flexIndex, "zmin");
  return (int)v.size();
}

std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionZmin(int flexIndex) const
{
  std::vector<double >v = getVectorDouble("StaveFlexGeo", flexIndex, "zmin");
  return v;
}

std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionZmax(int flexIndex) const
{
  std::vector<double >v = getVectorDouble("StaveFlexGeo", flexIndex, "zmax");
  return v;
}

std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionLength(int flexIndex) const
{
  std::vector<double >v = getVectorDouble("StaveFlexGeo", flexIndex, "delta");
  return v;
}

std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionThick(int flexIndex) const
{
  std::vector<double >v = getVectorDouble("StaveFlexGeo", flexIndex, "thick");
  return v;
}

double PixelStaveSupportXMLHelper::getStaveCopperThick(int flexIndex) const
{
  double v = getDouble("StaveFlexGeo", flexIndex, "thickCu");
  return v;
}

double PixelStaveSupportXMLHelper::getStaveFlexWingWidth(int flexIndex) const
{
  double v = getDouble("StaveFlexGeo", flexIndex, "wingWidth");
  return v;
}

std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionCopperVolume(int flexIndex) const
{
  std::vector<double >vCu = getVectorDouble("StaveFlexGeo", flexIndex, "volCu");
  return vCu;
}

std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionAdhesiveVolume(int flexIndex) const
{
  std::vector<double >vCu = getVectorDouble("StaveFlexGeo", flexIndex, "volAd");
  return vCu;
}


std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionPolyimideVolume(int flexIndex) const
{
  std::vector<double >vCu = getVectorDouble("StaveFlexGeo", flexIndex, "volP");
  return vCu;
}

std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionCopperWeight(int flexIndex) const
{
  std::vector<double >vCu = getVectorDouble("StaveFlexGeo", flexIndex, "wgCu");
  return vCu;
}

std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionAdhesiveWeight(int flexIndex) const
{
  std::vector<double >vCu = getVectorDouble("StaveFlexGeo", flexIndex, "wgAd");
  return vCu;
}


std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionPolyimideWeight(int flexIndex) const
{
  std::vector<double >vCu = getVectorDouble("StaveFlexGeo", flexIndex, "wgP");
  return vCu;
}


std::vector<double> PixelStaveSupportXMLHelper::getStaveFlexSectionVolume(int flexIndex) const
{
  std::vector<double >vPol = getVectorDouble("StaveFlexGeo", flexIndex, "volP");
  std::vector<double >vAdh = getVectorDouble("StaveFlexGeo", flexIndex, "volAd");
  std::vector<double >vCu = getVectorDouble("StaveFlexGeo", flexIndex, "volCu");

  std::vector<double> v;
  for(int i=0; i<(int)vPol.size(); i++) v.push_back(vPol[i]+vAdh[i]+vCu[i]);
  return v;
}


std::vector<int> PixelStaveSupportXMLHelper::getStaveFlexSectionTransition(int flexIndex) const
{
  std::vector<int >vTrans = getVectorInt("StaveFlexGeo", flexIndex, "ztransition");
  return vTrans;
}


std::vector<std::string> PixelStaveSupportXMLHelper::getStaveFlexSectionMaterial(int flexIndex) const
{
  std::vector<std::string >vTrans = getVectorString("StaveFlexGeo", flexIndex, "matName");
  return vTrans;
}

std::vector<std::string> PixelStaveSupportXMLHelper::getStaveFlexSectionMaterialTrans(int flexIndex) const
{
  std::vector<std::string >vTrans = getVectorString("StaveFlexGeo", flexIndex, "matNameTrans");
  return vTrans;
}
