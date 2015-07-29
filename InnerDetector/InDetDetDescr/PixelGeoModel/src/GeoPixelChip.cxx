/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds one Chip to be placed in one module
//
// The information on which layer to build is retrieved by the Geometry Manager
//
// Contained by: PixelModule_log
//
// Contains: nothing
//
#include "PixelGeoModel/GeoPixelChip.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

GeoVPhysVol* GeoPixelChip::Build() {
  //
  // Dimensions
  //
  double length = gmt_mgr->PixelChipLength(m_isModule3D);
  double thickness = gmt_mgr->PixelChipThickness(m_isModule3D);
  double width = gmt_mgr->PixelChipWidth(m_isModule3D);
  std::string LogName = "ChipBrl";
  if(gmt_mgr->isEndcap() ) LogName = "ChipEC";
  //std::cout << "Chip Thick = " << thickness << std::endl;
  const GeoBox* chipBox = new GeoBox(thickness/2.,width/2.,length/2.);

  std::string matName = gmt_mgr->getMaterialName("Chip",0,gmt_mgr->moduleType());
  if(m_isModule3D)matName = gmt_mgr->getMaterialName("Chip3D",0,gmt_mgr->moduleType());

  const GeoMaterial* chipMat = mat_mgr->getMaterialForVolume(matName,chipBox->volume());

  GeoLogVol* theChip = new GeoLogVol(LogName,chipBox,chipMat);
  GeoPhysVol* chipPhys = new GeoPhysVol(theChip);

  //std::cout<<"chip volume " << chipBox->volume() << " density " << chipMat->getDensity() << std::endl;
  return chipPhys;
}
