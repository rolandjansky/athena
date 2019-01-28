/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/DriftTube.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/MDT_Technology.h"
#include <iostream>
#include <string>

namespace MuonGM {

DriftTube::DriftTube(std::string n)
  : DetectorElement(n)
  , length(0.) // length is set in MultiLayer.cxx
{
  gasMaterial="muo::ArCO2";
  tubeMaterial="std::Aluminium";
  plugMaterial="std::Bakelite";
  wireMaterial="std::Aluminium";
  MYSQL *amdb=MYSQL::GetPointer();	
  MDT *md=(MDT *)amdb->GetTechnology(name.substr(0,5));
  gasRadius   = md->innerRadius;
  outerRadius = gasRadius+md->tubeWallThickness;
  plugLength  = md->tubeEndPlugLength;    
}
  
GeoVPhysVol *DriftTube::build()
{
  const GeoTube     *stube   = new GeoTube(0.0, outerRadius, length/2.0);
  const GeoMaterial *mtube   = matManager->getMaterial(tubeMaterial);
  const GeoLogVol   *ltube   = new GeoLogVol("MDTDriftWall", stube, mtube);
  GeoPhysVol  *ptube   = new GeoPhysVol(ltube);
  
  const GeoTube     *splug   = new GeoTube(0.0, outerRadius, plugLength/2.0);
  const GeoMaterial *mplug   = matManager->getMaterial(plugMaterial);
  const GeoLogVol   *lplug   = new GeoLogVol("Endplug",splug, mplug);
  GeoPhysVol  *pplug   = new GeoPhysVol(lplug);
  
  const GeoTube     *sgas    = new GeoTube(0, gasRadius, length/2.0-plugLength);
  const GeoMaterial *mgas    = matManager->getMaterial(gasMaterial);
  const GeoLogVol   *lgas    = new GeoLogVol("SensitiveGas",sgas,mgas);
  GeoPhysVol  *pgas    = new GeoPhysVol(lgas);
  
  GeoSerialDenominator *plugDenominator= new GeoSerialDenominator("Tube Endplug");
  GeoTransform *ec0X = new GeoTransform(GeoTrf::TranslateZ3D(+(length-plugLength)/2));
  GeoTransform *ec1X = new GeoTransform(GeoTrf::TranslateZ3D(-(length-plugLength)/2));
  std::string sGasName = "SensitiveGas";
  GeoNameTag           *gasDenominator = new GeoNameTag(sGasName);
  
  ptube->add(plugDenominator);
  ptube->add(ec0X);
  ptube->add(pplug);
  ptube->add(ec1X);
  ptube->add(pplug);
  ptube->add(gasDenominator);
  ptube->add(pgas);
  
  return ptube;
}

void DriftTube::print()
{
  std::cout << "Drift tube " << name.c_str() << " :" << std::endl;
  std::cout << "		Tube material 	: " << tubeMaterial.c_str() << std::endl;
  std::cout << "		Radius		: " << outerRadius << std::endl;
  std::cout << "		Length		: " << length;
  std::cout << "		Thickness	: " << outerRadius-gasRadius << " mm" << std::endl;
  std::cout << "		Gas material	: " << gasMaterial.c_str() << std::endl;
  std::cout << "		EP length	: " << plugLength << std::endl;
}
} // namespace MuonGM
