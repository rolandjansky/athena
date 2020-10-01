/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Build simple stave support as a box
// This is built one time per layer. 

#include "GeoPixelSimpleStaveSupport.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"



GeoPixelSimpleStaveSupport::GeoPixelSimpleStaveSupport(InDetDD::PixelDetectorManager* ddmgr,
                                                       PixelGeometryManager* mgr)
  : GeoPixelStaveSupport(ddmgr, mgr),
    m_transform(GeoTrf::Transform3D::Identity())
{
  m_physVol = Build();
  m_physVol->ref();
}
GeoPixelSimpleStaveSupport::~GeoPixelSimpleStaveSupport(){
  m_physVol->unref();
}

GeoVPhysVol* GeoPixelSimpleStaveSupport::Build() {
  
  double thickness = m_gmt_mgr->PixelLadderSupportThickness();
  double width     = m_gmt_mgr->PixelLadderSupportWidth();
  double length    = m_gmt_mgr->PixelLadderSupportLength();
  double xOffset   = m_gmt_mgr->PixelLadderServicesX();
  double yOffset   = m_gmt_mgr->PixelLadderServicesY();
  int staveIndex   = m_gmt_mgr->PixelStaveIndex(m_gmt_mgr->GetLD());

  GeoBox * shape = new GeoBox(0.5*thickness, 0.5*width, 0.5*length);
  std::string matName = m_gmt_mgr->getMaterialName("StaveSupport", m_gmt_mgr->GetLD(), staveIndex);
  const GeoMaterial* material = m_mat_mgr->getMaterialForVolume(matName,shape->volume());
  GeoLogVol* logVol = new GeoLogVol("StaveSupport",shape,material);

  m_transform = GeoTrf::Translate3D(xOffset,yOffset,0);

  m_thicknessP =  xOffset + 0.5*thickness;
  m_thicknessN =  -xOffset + 0.5*thickness;

  return new GeoPhysVol(logVol);
}

