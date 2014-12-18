/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelDiskSupports.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

#include <sstream>

GeoPixelDiskSupports::GeoPixelDiskSupports() {
  //
  // Initialize the vectors
  //
  //
  // zpos is not needed, but it's here just in case...
  //
  for (int ii =0; ii< gmt_mgr->PixelDiskNumSupports(); ii++) {
    double rmin = gmt_mgr->PixelDiskSupportRMin(ii);
    double rmax = gmt_mgr->PixelDiskSupportRMax(ii);
    double halflength = gmt_mgr->PixelDiskSupportThickness(ii)/2.;
    double zpos = 0.;
    int matTypeNum = gmt_mgr->PixelDiskSupportMaterialTypeNum(ii);
    _rmin.push_back(rmin);
    _rmax.push_back(rmax);
    _zpos.push_back(zpos);
    _halflength.push_back(halflength);
    _typeNum.push_back(matTypeNum);
  }
  _nframe = 0;
}


GeoVPhysVol* GeoPixelDiskSupports::Build( ) {
  //
  // Dimensions: (_nframe set with teh SetCylinder method)
  //
  double rmin = _rmin[_nframe];
  double rmax = _rmax[_nframe];
  double halflength = _halflength[_nframe];
  int typeNum = _typeNum[_nframe];
  const GeoTube* supportTube = new GeoTube(rmin,rmax,halflength);
  std::string matName = gmt_mgr->getMaterialName("DiskSupport", gmt_mgr->GetLD(), typeNum);
  const GeoMaterial* supportMat = mat_mgr->getMaterialForVolume(matName, supportTube->volume());
  std::string logName = "DiskSup";
  std::ostringstream o;
  o << _nframe;
  logName = logName+o.str();
  GeoLogVol* theSupport = new GeoLogVol(logName,supportTube,supportMat);
  GeoPhysVol* supportPhys = new GeoPhysVol(theSupport);
  return supportPhys;
}

