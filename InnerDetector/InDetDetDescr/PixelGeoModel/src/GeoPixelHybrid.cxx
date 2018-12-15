/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "GeoPixelHybrid.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

GeoVPhysVol* GeoPixelHybrid::Build() {
  //
  // Dimensions
  //
  double length = m_gmt_mgr->PixelHybridLength(m_isModule3D);
  double thickness = m_gmt_mgr->PixelHybridThickness(m_isModule3D);
  double width = m_gmt_mgr->PixelHybridWidth(m_isModule3D);

  const GeoBox* hybridBox = new GeoBox(thickness/2.,width/2.,length/2.);
  std::string matName = m_gmt_mgr->getMaterialName("Hybrid",0,m_gmt_mgr->moduleType());
  if(m_isModule3D)matName = m_gmt_mgr->getMaterialName("Hybrid3D",0,m_gmt_mgr->moduleType());

  const GeoMaterial* hybridMat = m_mat_mgr->getMaterialForVolume(matName,hybridBox->volume());
  std::string logName = "HybridEC";
  if(m_gmt_mgr->isBarrel() ) logName = "HybridBrl";
  GeoLogVol* theHybrid = new GeoLogVol(logName,hybridBox,hybridMat);
  GeoPhysVol* hybridPhys = new GeoPhysVol(theHybrid);

  return hybridPhys;
}
