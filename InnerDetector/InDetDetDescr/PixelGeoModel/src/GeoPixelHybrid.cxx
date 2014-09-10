/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds one Hybrid to be placed in one module
//
// The information on which layer to build is retrieved by the Geometry Manager
//
// Contained by: PixelModule_log
//
// Contains: nothing
//
#include "PixelGeoModel/GeoPixelHybrid.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

GeoVPhysVol* GeoPixelHybrid::Build() {
  //
  // Dimensions
  //
  double length = gmt_mgr->PixelHybridLength(m_isModule3D);
  double thickness = gmt_mgr->PixelHybridThickness(m_isModule3D);
  double width = gmt_mgr->PixelHybridWidth(m_isModule3D);

  const GeoBox* hybridBox = new GeoBox(thickness/2.,width/2.,length/2.);
  std::string matName = gmt_mgr->getMaterialName("Hybrid",0,gmt_mgr->moduleType());
  if(m_isModule3D)matName = gmt_mgr->getMaterialName("Hybrid3D",0,gmt_mgr->moduleType());

  const GeoMaterial* hybridMat = mat_mgr->getMaterialForVolume(matName,hybridBox->volume());
  std::string logName = "HybridEC";
  if(gmt_mgr->isBarrel() ) logName = "HybridBrl";
  GeoLogVol* theHybrid = new GeoLogVol(logName,hybridBox,hybridMat);
  GeoPhysVol* hybridPhys = new GeoPhysVol(theHybrid);

  return hybridPhys;
}
