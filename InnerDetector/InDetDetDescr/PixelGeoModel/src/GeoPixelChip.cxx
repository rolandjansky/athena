/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "GeoPixelChip.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

GeoVPhysVol* GeoPixelChip::Build() {
  //
  // Dimensions
  //
  double length = m_gmt_mgr->PixelChipLength(m_isModule3D);
  double thickness = m_gmt_mgr->PixelChipThickness(m_isModule3D);
  double width = m_gmt_mgr->PixelChipWidth(m_isModule3D);
  std::string LogName = "ChipBrl";
  if(m_gmt_mgr->isEndcap() ) LogName = "ChipEC";
  //std::cout << "Chip Thick = " << thickness << std::endl;
  const GeoBox* chipBox = new GeoBox(thickness/2.,width/2.,length/2.);

  std::string matName = m_gmt_mgr->getMaterialName("Chip",0,m_gmt_mgr->moduleType());
  if(m_isModule3D)matName = m_gmt_mgr->getMaterialName("Chip3D",0,m_gmt_mgr->moduleType());

  const GeoMaterial* chipMat = m_mat_mgr->getMaterialForVolume(matName,chipBox->volume());

  GeoLogVol* theChip = new GeoLogVol(LogName,chipBox,chipMat);
  GeoPhysVol* chipPhys = new GeoPhysVol(theChip);

  //std::cout<<"chip volume " << chipBox->volume() << " density " << chipMat->getDensity() << std::endl;
  return chipPhys;
}
