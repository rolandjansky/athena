/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelPigtail.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"

#include<math.h>
#include <sstream>

GeoPixelPigtail::GeoPixelPigtail() {
  const GeoMaterial* air  = mat_mgr->getMaterial("std::Air");
  //

  double length = gmt_mgr->PixelPigtailEnvelopeLength()+m_epsilon;
  m_rmin = gmt_mgr->PixelPigtailBendRMin();
  m_rmax = gmt_mgr->PixelPigtailBendRMax();
  m_phi  = gmt_mgr->PixelPigtailBendPhiMin();
  m_dphi = gmt_mgr->PixelPigtailBendPhiMax() -  m_phi;
  m_width = gmt_mgr->PixelPigtailWidthZ();
  m_posBendX = gmt_mgr->PixelPigtailBendX();
  m_posBendY = gmt_mgr->PixelPigtailBendY();
  m_posZ = gmt_mgr->PixelPigtailPosZ();

  double rmin = m_rmin-m_epsilon;
  double rmax = m_rmax+m_epsilon;
  const GeoTubs* tubs = new GeoTubs(rmin,rmax,0.5*length,m_phi,m_dphi);
  thePigtail = new GeoLogVol("PigtailCylEnv",tubs,air);
}

GeoVPhysVol* GeoPixelPigtail::Build() {
  GeoPhysVol* pigtailPhys = new GeoPhysVol(thePigtail);
  for (int iModule = 0; iModule<gmt_mgr->PixelNModule(); iModule++) {
    int moduleEta =  gmt_mgr->PixelModuleEtaFromIndex(iModule);
    double zShift = gmt_mgr->PixelModuleZPosition(moduleEta);
    std::string logName = "PigtailCyl";
    const GeoTubs* tubs  = new GeoTubs(m_rmin,m_rmax,0.5*m_width,m_phi,m_dphi);
    std::string matName = gmt_mgr->getMaterialName("PigtailCyl", gmt_mgr->GetLD());
    const GeoMaterial* cyliMat = mat_mgr->getMaterialForVolume(matName,tubs->volume());
    GeoLogVol* theTubs   = new GeoLogVol(logName,tubs,cyliMat);
    GeoPhysVol* pigtailPart = new GeoPhysVol(theTubs);

    double zpos = m_posZ + zShift;
    GeoTransform* xform = new GeoTransform(HepGeom::TranslateZ3D(zpos));
    pigtailPhys->add(xform);
    pigtailPhys->add(pigtailPart);
  }
  return pigtailPhys;
}
