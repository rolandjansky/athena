/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoPixelDiskSupports.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

#include <sstream>

GeoPixelDiskSupports::GeoPixelDiskSupports(InDetDD::PixelDetectorManager* ddmgr,
                                           PixelGeometryManager* mgr)
  : GeoVPixelFactory (ddmgr, mgr)
{
  //
  // Initialize the vectors
  //
  //
  // zpos is not needed, but it's here just in case...
  //
  for (int ii =0; ii< m_gmt_mgr->PixelDiskNumSupports(); ii++) {
    double rmin = m_gmt_mgr->PixelDiskSupportRMin(ii);
    double rmax = m_gmt_mgr->PixelDiskSupportRMax(ii);
    double halflength = m_gmt_mgr->PixelDiskSupportThickness(ii)/2.;
    double zpos = 0.;
    int matTypeNum = m_gmt_mgr->PixelDiskSupportMaterialTypeNum(ii);
    m_rmin.push_back(rmin);
    m_rmax.push_back(rmax);
    m_zpos.push_back(zpos);
    m_halflength.push_back(halflength);
    m_typeNum.push_back(matTypeNum);
  }
  m_nframe = 0;
}


GeoVPhysVol* GeoPixelDiskSupports::Build( ) {
  //
  // Dimensions: (_nframe set with the SetCylinder method)
  //
  double rmin = m_rmin[m_nframe];
  double rmax = m_rmax[m_nframe];
  double halflength = m_halflength[m_nframe];
  int typeNum = m_typeNum[m_nframe];

  const GeoTube* supportTube = new GeoTube(rmin,rmax,halflength);
  std::string matName = m_gmt_mgr->getMaterialName("DiskSupport", m_gmt_mgr->GetLD(), typeNum);
  const GeoMaterial* supportMat = m_mat_mgr->getMaterialForVolume(matName, supportTube->volume());
  std::string logName = "DiskSup";
  std::ostringstream o;
  o << m_nframe;
  logName = logName+o.str();
  GeoLogVol* theSupport = new GeoLogVol(logName,supportTube,supportMat);
  GeoPhysVol* supportPhys = new GeoPhysVol(theSupport);
  return supportPhys;
}

