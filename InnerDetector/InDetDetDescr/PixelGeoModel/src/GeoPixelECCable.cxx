/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoPixelECCable.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

GeoPixelECCable::GeoPixelECCable(InDetDD::PixelDetectorManager* ddmgr,
                                 PixelGeometryManager* mgr)
  : GeoVPixelFactory (ddmgr, mgr)
{}


GeoVPhysVol* GeoPixelECCable::Build( ) {
  //   
  // Dimensions 
  //
  double rmin = m_gmt_mgr->PixelECCablesRMin();
  double rmax = m_gmt_mgr->PixelECCablesRMax();
  double thickness = m_gmt_mgr->PixelECCablesThickness();
  const GeoTube* cableTube = new GeoTube(rmin,rmax,thickness/2.);
  std::string matName = m_gmt_mgr->getMaterialName("DiskCable", m_gmt_mgr->GetLD());
  const GeoMaterial* cableMat = m_mat_mgr->getMaterialForVolume(matName, cableTube->volume());
  std::ostringstream ostr; ostr << m_gmt_mgr->GetLD();
  const GeoLogVol* theECCable = new GeoLogVol("ECCable"+ostr.str(),cableTube,cableMat);

  GeoPhysVol* cablePhys = new GeoPhysVol(theECCable);
  return cablePhys;
}

