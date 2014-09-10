/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelECCable.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

GeoPixelECCable::GeoPixelECCable() 
{}


GeoVPhysVol* GeoPixelECCable::Build( ) {
  //   
  // Dimensions 
  //
  double rmin = gmt_mgr->PixelECCablesRMin();
  double rmax = gmt_mgr->PixelECCablesRMax();
  double thickness = gmt_mgr->PixelECCablesThickness();
  const GeoTube* cableTube = new GeoTube(rmin,rmax,thickness/2.);
  std::string matName = gmt_mgr->getMaterialName("DiskCable", gmt_mgr->GetLD());
  const GeoMaterial* cableMat = mat_mgr->getMaterialForVolume(matName, cableTube->volume());
  std::ostringstream ostr; ostr << gmt_mgr->GetLD();
  const GeoLogVol* theECCable = new GeoLogVol("ECCable"+ostr.str(),cableTube,cableMat);

  GeoPhysVol* cablePhys = new GeoPhysVol(theECCable);
  return cablePhys;
}

